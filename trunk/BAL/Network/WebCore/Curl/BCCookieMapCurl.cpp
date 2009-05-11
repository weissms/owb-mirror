/*
 * Copyright (C) 2008, 2009 Julien Chaffraix <julien.chaffraix@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
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
#include "CookieMap.h"

#include "Cookie.h"
#include "CookieBackingStore.h"
#include "CookieManager.h"
#include "CurrentTime.h"
#include "Logging.h"

namespace WebCore {

CookieMap::CookieMap()
    : m_oldestCookie(0)
{
}

CookieMap::~CookieMap()
{
    HashMap<String, Cookie*>::iterator first = m_cookieMap.begin();
    HashMap<String, Cookie*>::iterator end = m_cookieMap.end();
    for (HashMap<String, Cookie*>::iterator cookieIterator = first; cookieIterator != end; ++cookieIterator)
        delete cookieIterator->second;
}

Cookie* CookieMap::takePrevious(const Cookie* cookie)
{
    String key = cookie->name() + cookie->path();

#ifndef NDEBUG
    LOG(Network, "Key : %s\n", key.ascii().data());
#endif

    return m_cookieMap.take(key);
}

void CookieMap::add(Cookie* cookie)
{
    String key = cookie->name() + cookie->path();
    ASSERT(!m_cookieMap.get(key));
    m_cookieMap.add(key, cookie);
    if (!m_oldestCookie || m_oldestCookie->lastAccessed() > cookie->lastAccessed())
        m_oldestCookie = cookie;
}

void CookieMap::remove(const Cookie* cookie)
{
    String key = cookie->name() + cookie->path();

    // Find a previous entry for deletion
    Cookie* prevCookie = m_cookieMap.take(key);

    if (prevCookie == m_oldestCookie)
        updateOldestCookie();

    ASSERT(prevCookie);
    delete prevCookie;
}

Vector<Cookie*> CookieMap::getCookies()
{
    Vector<Cookie*> cookies;

    double now = currentTime();

    HashMap<String, Cookie*>::iterator first = m_cookieMap.begin();
    HashMap<String, Cookie*>::iterator end = m_cookieMap.end();
    for (HashMap<String, Cookie*>::iterator cookieIterator = first; cookieIterator != end; ++cookieIterator) {
        Cookie* cookie = cookieIterator->second;
        // Check for non session cookie expired.
        if (!cookie->isSession() && cookie->expiry() < currentTime()) {
            LOG(Network, "Cookie name: %s value: %s path: %s  expired", cookie->name().ascii().data(), cookie->value().ascii().data(), cookie->path().ascii().data());
            m_cookieMap.take(cookieIterator->first);
            cookieBackingStore().remove(cookie);
            cookieManager().removedCookie();
            delete cookie;
        } else {
            updateTime(cookie, now);
            cookies.append(cookie);
        }
    }

    return cookies;
}

Cookie* CookieMap::removeOldestCookie()
{
    String key = m_oldestCookie->name() + m_oldestCookie->path();

    Cookie* result = m_cookieMap.take(key);

    updateOldestCookie();

    return result;
}

void CookieMap::updateTime(Cookie* cookie, double newTime)
{
    cookie->setLastAccessed(newTime);

    if (cookie == m_oldestCookie)
        updateOldestCookie();
}

void CookieMap::updateOldestCookie()
{
    if (m_cookieMap.size() == 0)
        m_oldestCookie = 0;
    else {
        HashMap<String, Cookie*>::iterator it = m_cookieMap.begin();
        m_oldestCookie = it->second;
        ++it;
        for (; it != m_cookieMap.end(); ++it)
            if (m_oldestCookie->lastAccessed() > it->second->lastAccessed())
                m_oldestCookie = it->second;
    }
}

} // Namespace WebCore
