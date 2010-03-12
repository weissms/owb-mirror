/*
 * Copyright (C) 2004, 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2005, 2006 Michael Emmel mike.emmel@gmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ResourceHandle.h"

#include "CookieManager.h"
#include "CString.h"
#include "DocLoader.h"
#include "NotImplemented.h"
#include "ResourceHandleInternal.h"
#include "ResourceHandleManager.h"

#if PLATFORM(WIN) && PLATFORM(CF)
#include <wtf/RetainPtr.h>
#endif

namespace WebCore {

class WebCoreSynchronousLoader : public ResourceHandleClient {
public:
    WebCoreSynchronousLoader();

    virtual void didReceiveResponse(ResourceHandle*, const ResourceResponse&);
    virtual void didReceiveData(ResourceHandle*, const char*, int, int lengthReceived);
    virtual void didFinishLoading(ResourceHandle*);
    virtual void didFail(ResourceHandle*, const ResourceError&);

    ResourceResponse resourceResponse() const { return m_response; }
    ResourceError resourceError() const { return m_error; }
    Vector<char> data() const { return m_data; }

private:
    ResourceResponse m_response;
    ResourceError m_error;
    Vector<char> m_data;
};

WebCoreSynchronousLoader::WebCoreSynchronousLoader()
{
}

void WebCoreSynchronousLoader::didReceiveResponse(ResourceHandle*, const ResourceResponse& response)
{
    m_response = response;
}

void WebCoreSynchronousLoader::didReceiveData(ResourceHandle*, const char* data, int length, int)
{
    m_data.append(data, length);
}

void WebCoreSynchronousLoader::didFinishLoading(ResourceHandle*)
{
}

void WebCoreSynchronousLoader::didFail(ResourceHandle*, const ResourceError& error)
{
    m_error = error;
}


ResourceHandleInternal::~ResourceHandleInternal()
{
    fastFree(m_url);
    if (m_customHeaders)
        curl_slist_free_all(m_customHeaders);
}

ResourceHandle::~ResourceHandle()
{
    cancel();
}

bool ResourceHandle::start(Frame* frame)
{
    // The frame could be null if the ResourceHandle is not associated to any
    // Frame, e.g. if we are downloading a file.
    // If the frame is not null but the page is null this must be an attempted
    // load from an onUnload handler, so let's just block it.
    if (frame && !frame->page())
        return false;

    ResourceHandleManager::sharedInstance()->add(this);
    return true;
}

void ResourceHandle::cancel()
{
    ResourceHandleManager::sharedInstance()->cancel(this);
}

PassRefPtr<SharedBuffer> ResourceHandle::bufferedData()
{
    return 0;
}

bool ResourceHandle::supportsBufferedData()
{
    return false;
}

#if PLATFORM(WIN) && PLATFORM(CF)
void ResourceHandle::setHostAllowsAnyHTTPSCertificate(const String& host)
{
    allowsAnyHTTPSCertificateHosts().add(host.lower());
}
#endif

#if PLATFORM(WIN) && PLATFORM(CF)
// FIXME:  The CFDataRef will need to be something else when
// building without 
static HashMap<String, RetainPtr<CFDataRef> >& clientCerts()
{
    static HashMap<String, RetainPtr<CFDataRef> > certs;
    return certs;
}

void ResourceHandle::setClientCertificate(const String& host, CFDataRef cert)
{
    clientCerts().set(host.lower(), cert);
}
#endif

void ResourceHandle::setDefersLoading(bool defers)
{
    if (d->m_defersLoading == defers)
        return;

#if LIBCURL_VERSION_NUM > 0x071200
    if (!d->m_handle)
        d->m_defersLoading = defers;
    else if (defers) {
        CURLcode error = curl_easy_pause(d->m_handle, CURLPAUSE_ALL);
        // If we could not defer the handle, so don't do it.
        if (error != CURLE_OK)
            return;

        d->m_defersLoading = defers;
    } else {
        // We need to set defersLoading before restarting a connection
        // or libcURL will call the callbacks in curl_easy_pause and
        // we would ASSERT.
        d->m_defersLoading = defers;

        CURLcode error = curl_easy_pause(d->m_handle, CURLPAUSE_CONT);
        if (error != CURLE_OK)
            // Restarting the handle has failed so just cancel it.
            cancel();
    }
#else
    d->m_defersLoading = defers;
    LOG_ERROR("Deferred loading is implemented if libcURL version is above 7.18.0");
#endif
}

bool ResourceHandle::willLoadFromCache(ResourceRequest&, Frame*)
{
    notImplemented();
    return false;
}

void ResourceHandle::setCookies()
{
    KURL url = getInternal()->m_request.url();
    cookieManager().setCookies(url, getInternal()->m_response.httpHeaderField("Set-Cookie"));
    checkAndSendCookies(url);
}

void ResourceHandle::checkAndSendCookies(KURL& url)
{
    // Cookies are a part of the http protocol only
    if (!String(d->m_url).startsWith("http"))
        return;

    if (url.isEmpty())
        url = KURL(ParsedURLString, d->m_url);

    // Prepare a cookie header if there are cookies related to this url.
    String cookiePairs = cookieManager().getCookie(url, WithHttpOnlyCookies);
    if (!cookiePairs.isEmpty() && d->m_handle) {
        Vector<char> cookieChar = cookiePairs.impl()->ascii();
        LOG(Network, "CURL POST Cookie : %s \n", cookieChar.data());
        curl_easy_setopt(d->m_handle, CURLOPT_COOKIE, cookieChar.data());
    }
}

bool ResourceHandle::loadsBlocked()
{
    notImplemented();
    return false;
}

void ResourceHandle::loadResourceSynchronously(const ResourceRequest& request, StoredCredentials storedCredentials, ResourceError& error, ResourceResponse& response, Vector<char>& data, Frame*)
{
    WebCoreSynchronousLoader syncLoader;
    ResourceHandle handle(request, &syncLoader, true, false);

    ResourceHandleManager* manager = ResourceHandleManager::sharedInstance();

    manager->dispatchSynchronousJob(&handle);

    error = syncLoader.resourceError();
    data = syncLoader.data();
    response = syncLoader.resourceResponse();
}

void ResourceHandle::didReceiveAuthenticationChallenge(const AuthenticationChallenge& challenge) 
{
    // If the currentWebChallenge is not null, it means that we have already been called, so avoid an infinite loop by bailing out.
    // We call didReceiveAuthenticationChallenge so that the user can input his credentials if we have a client.
    if (!d->m_currentWebChallenge.isNull()) {
        if (client())
            client()->didReceiveAuthenticationChallenge(this, challenge);
        return;
    }

    if (!d->m_user.isEmpty() && !d->m_pass.isEmpty()) {
        Credential urlCredential(d->m_user, d->m_pass, CredentialPersistenceNone);
        receivedCredential(challenge, urlCredential);
        d->m_user = String();
        d->m_pass = String();
        return;
    }

    if (client())
        client()->didReceiveAuthenticationChallenge(this, challenge);
}

void ResourceHandle::receivedCredential(const AuthenticationChallenge& challenge, const Credential& credential)
{
    // FIXME: We should assert somehow that challenge is the current challenge.
    //
    // Mac uses this method to store the credential. We should do that too in the database.
    
    // Clone our ResourceHandle and add it so that it is send with the credential.
    // FIXME: We should have a way of cloning an handle.
    RefPtr<ResourceHandle> newHandle = ResourceHandle::create(request(), client(), 0, d->m_defersLoading, shouldContentSniff());
    setClient(0); // Clear the client to avoid it being cleared by WebCore.
    AuthenticationChallenge newAuthenticationChallenge(challenge.protectionSpace(), credential, challenge.previousFailureCount() + 1, challenge.failureResponse(), challenge.error());

    // Store the new authentication challenge.
    newHandle->getInternal()->m_currentWebChallenge = newAuthenticationChallenge;

    d->m_cancelled = true;
}

void ResourceHandle::receivedRequestToContinueWithoutCredential(const AuthenticationChallenge&) 
{
    // Do nothing as curl will automatically continue its transfer.
}

void ResourceHandle::receivedCancellation(const AuthenticationChallenge&)
{
    d->m_cancelled = true;
}

} // namespace WebCore
