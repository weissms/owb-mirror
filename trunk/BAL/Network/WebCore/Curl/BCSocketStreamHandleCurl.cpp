/*
 * Copyright (C) 2009 Brent Fulgham.  All rights reserved.
 * Copyright (C) 2009 Google Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "SocketStreamHandle.h"

#include "KURL.h"
#include "Logging.h"
#include "NotImplemented.h"
#include "SocketStreamError.h"
#include "SocketStreamHandleClient.h"

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

const double pollInterval = 0.5; // Half a second.

namespace WebCore {

SocketStreamHandle::SocketStreamHandle(const KURL& url, SocketStreamHandleClient* client)
    : SocketStreamHandleBase(url, client)
    , m_pollTimer(this, &SocketStreamHandle::pollCallback)
    , m_socketFD(0)
{
    LOG(Network, "SocketStreamHandle %p new client %p", this, m_client);

    createAndConnectSocket();

    if (m_socketFD <= 0)
        m_client->didFail(this, SocketStreamError(errno));
    else
        m_pollTimer.startRepeating(pollInterval);
}

SocketStreamHandle::~SocketStreamHandle()
{
    LOG(Network, "SocketStreamHandle %p delete", this);
    closeSocket();
    setClient(0);
}

int SocketStreamHandle::platformSend(const char* data, int length)
{
    LOG(Network, "SocketStreamHandle %p platformSend", this);
    if (m_socketFD <= 0)
        return 0;

    ssize_t lengthSend = ::send(m_socketFD, data, length, 0);
    if (lengthSend < 0) {
        platformClose();
        m_client->didFail(this, SocketStreamError(errno));
        return 0;
    }

    // FIXME: Should we check for overflow here?
    return lengthSend;
}

void SocketStreamHandle::platformClose()
{
    LOG(Network, "SocketStreamHandle %p platformClose", this);
    bool shouldCallDidClose = m_socketFD > 0;
    closeSocket();
    if (shouldCallDidClose)
        m_client->didClose(this);
}

void SocketStreamHandle::didReceiveAuthenticationChallenge(const AuthenticationChallenge& authentificationChallenge)
{
    notImplemented();
    m_client->didReceiveAuthenticationChallenge(this, authentificationChallenge);
}

void SocketStreamHandle::receivedCredential(const AuthenticationChallenge&, const Credential&)
{
    notImplemented();
}

void SocketStreamHandle::receivedRequestToContinueWithoutCredential(const AuthenticationChallenge&)
{
    notImplemented();
}

void SocketStreamHandle::receivedCancellation(const AuthenticationChallenge& authentificationChallenge)
{
    notImplemented();
    m_client->didCancelAuthenticationChallenge(this, authentificationChallenge);
}

void SocketStreamHandle::closeSocket()
{
    if (m_socketFD > 0) {
        ASSERT(m_pollTimer.isActive());
        m_pollTimer.stop();
        ::close(m_socketFD);
        m_socketFD = 0;
    }
}

void SocketStreamHandle::createAndConnectSocket()
{
    ASSERT(m_state == Connecting);

    struct addrinfo hints;
    struct addrinfo* result = NULL;

    // Resolve the host name for the socket.
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV; // The port is a numeric value.
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    // FIXME: Avoid using an ascii conversion here!
    ASSERT(!m_url.host().isNull());
    Vector<char> vectorHost = m_url.host().impl()->ascii();
    char* host = strdup(vectorHost.data());

    // We do not support wss so we have hardcoded this value to match the specification.
    String port("80");
    if (m_url.hasPort())
        port = String::number(m_url.port());

    Vector<char> vectorPort = port.impl()->ascii();
    char* cPort = strdup(vectorPort.data());
    if (getaddrinfo(host, cPort, &hints, &result))
        return;


    // Open our socket.
    for (struct addrinfo* currentResult = result; currentResult != NULL; currentResult = currentResult->ai_next) {
        m_socketFD = socket(currentResult->ai_family, currentResult->ai_socktype, currentResult->ai_protocol);
        if (m_socketFD == -1)
            continue;

        if (connect(m_socketFD, currentResult->ai_addr, currentResult->ai_addrlen) != -1)
            break;

        // Connect did not work, just try the new version.
        ::close(m_socketFD);
        m_socketFD = 0;
    }

    freeaddrinfo(result);
    free(host);
    free(cPort);
    // We are connected now.
}

void SocketStreamHandle::pollCallback(Timer<SocketStreamHandle>* timer)
{
    ASSERT(m_socketFD > 0);
    if (m_socketFD <= 0)
        m_pollTimer.stop();

    // We cannot do this call synchronously when creating the socket or we would ASSERT, so we do it here.
    // FIXME: We should add a new oneshot timer.
    if (m_state == Connecting) {
        m_state = Open;
        m_client->didOpen(this);
    }

    fd_set read;
    FD_ZERO(&read);
    FD_SET(m_socketFD, &read);

    static struct timeval timeout = { 0, 50 }; // 50 ms
    // FIXME: We can block in this code if the server is sending a lot of data.
    for (;;) {
        int nbRead = ::select(m_socketFD + 1, &read, NULL, NULL, &timeout);
        if (nbRead == -1) {
            platformClose();
            m_client->didFail(this, SocketStreamError(errno));
            return;
        } else if (nbRead == 0)
            // Nothing to read.
            break;

        char buffer[1024];
        ssize_t length = ::read(m_socketFD, buffer, 1024);
        if (length < 0) {
            platformClose();
            m_client->didFail(this, SocketStreamError(errno));
            return;
        }
        m_client->didReceiveData(this, buffer, length);
    }
}

}  // namespace WebCore
