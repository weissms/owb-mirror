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
#include "CookieManager.h"

#include "Cookie.h"
#include "CookieBackingStore.h"
#include "CookieParser.h"
#include "CString.h"
#include "CurrentTime.h"
#include "Logging.h"

namespace WebCore {

CookieManager& cookieManager()
{
    static CookieManager cookieManager;
    return cookieManager;
} 

CookieManager::CookieManager()
    : m_count(0)
{
    m_cookieJarFileName = String("cookieCollection.db");

    // We force the cookie backing store to be open with the cookie jar to avoid
    // calling cookieManager() again and recursively calling this constructor.
    cookieBackingStore().open(cookieJar());
    getBackingStoreCookies();
}

CookieManager::~CookieManager()
{
    removeAllCookies(DoNotRemoveFromBackingStore);
    cookieBackingStore().close();
}

void CookieManager::setCookies(const KURL& url, const KURL& policyURL, const String& value)
{
    CookieParser parser(url);
    Vector<Cookie*> cookies = parser.parse(value);

    for (size_t i = 0; i < cookies.size(); ++i) {
        Cookie* cookie = cookies[i];
        if (!shouldRejectForSecurityReason(cookie, url))
            checkAndTreatCookie(cookie);
        else
            delete cookie;
    }
}

bool CookieManager::shouldRejectForSecurityReason(const Cookie* cookie, const KURL& url)
{
    // Check if path attribute is a prefix of the request URI.
    if (!cookie->path().endsWith(url.path()) == -1) {
        LOG_ERROR("Cookie %s is rejected because its path does not math the URL %s\n", cookie->toString().utf8().data(), url.string().utf8().data());
        return true;
    }

    // Check if the domain starts with a dot.
    if (cookie->domain()[0] != '.') {
        LOG_ERROR("Cookie %s is rejected because its domain does not start with a dot.\n", cookie->toString().utf8().data());
        return true;
    }

    // Check if the domain contains an embedded dot.
    if (cookie->domain().find(".", 1) == -1 || cookie->domain().find(".", 1) == cookie->domain().length()) {
        LOG_ERROR("Cookie %s is rejected because its domain does not contain an embedded dot.\n", cookie->toString().utf8().data());
        return true;
    }

    // The request host should domain match the Domain attribute.
    int diffPos = url.host().endsWith(cookie->domain());
    if (diffPos == -1) {
        LOG_ERROR("Cookie %s is rejected because its domain does not domain match the URL %s\n", cookie->toString().utf8().data(), url.string().utf8().data());
        return true;
    }
    // We should check for an embedded dot in the portion of string in the host not in the domain
    // but to match firefox behaviour we do not.

    return false;
}

String CookieManager::getCookie(const KURL& url, HttpOnlyCookieFiltering filter)
{
    bool isConnectionSecure = false;
    if (url.string().startsWith(String("https:", false)))
        isConnectionSecure = true;

    String res = String();
    for (HashMap<String, CookieMap*>::iterator it = m_managerMap.begin(); it != m_managerMap.end(); ++it) {

        // Handle sub-domain by only looking at the end of the host.
        if (it->first.endsWith(url.host()) || (it->first.startsWith(".", false) && ("." + url.host()).endsWith(it->first, false))) {
            // Get CookieMap to check for expired cookies.
            Vector<Cookie*> cookies = it->second->getCookies();

            for (size_t i = 0; i < cookies.size(); ++i) {
                Cookie* cookie = cookies[i];
                // Get the cookies filtering out the secure cookies on an unsecure connection and HttpOnly cookies if requested.
                if (url.path().startsWith(cookie->path(), false) && (isConnectionSecure || !cookie->isSecure()) && (filter == WithHttpOnlyCookies || !cookie->isHttpOnly()))
                    res += cookie->name() + "=" + cookie->value() + ";";
            }
        }
    }

    return res;
}

void CookieManager::removeAllCookies(BackingStoreRemoval backingStoreRemoval)
{
    HashMap<String, CookieMap*>::iterator first = m_managerMap.begin();
    HashMap<String, CookieMap*>::iterator end = m_managerMap.end();
    for (HashMap<String, CookieMap*>::iterator it = first; it != end; ++it)
        delete it->second;

    if (backingStoreRemoval == RemoveFromBackingStore)
        cookieBackingStore().removeAll();
    m_count = 0;
}

void CookieManager::setCookieJar(const char* fileName)
{
    m_cookieJarFileName = String(fileName);
    cookieBackingStore().open(m_cookieJarFileName);
}

void CookieManager::checkAndTreatCookie(Cookie* cookie)
{
    ASSERT(cookie->domain().length());

    CookieMap* curMap = m_managerMap.get(cookie->domain());

    // Check for cookie to remove in case it is not a session cookie and it has expired.
    if (cookie->hasExpired()) {
        // The cookie has expired so check we have a valid HashMap so try delete it.
        if (curMap) {
            // Check if we have a cookie to remove and update information accordingly.
            Cookie* prevCookie = curMap->takePrevious(cookie);
            if (prevCookie) {
                cookieBackingStore().remove(cookie);
                removedCookie();
                delete cookie;
            }
        }
    } else {
        if (!curMap) {
            curMap = new CookieMap();
            m_managerMap.add(cookie->domain(), curMap);
            addCookieToMap(curMap, cookie);
        } else {
            // Check if there is a previous cookie.
            Cookie* prevCookie = curMap->takePrevious(cookie);

            if (prevCookie) {
                update(curMap, prevCookie, cookie);
                delete prevCookie;
            } else
                addCookieToMap(curMap, cookie);
        }
    }
}

void CookieManager::addCookieToMap(CookieMap* map, Cookie* cookie)
{
    // Check if we do not have reached the cookie's threshold.
    // FIXME : should split the case and remove one cookie among all the other if m_count >= max_count
    if (map->count() > s_maxCookieCountPerHost || m_count >= s_globalMaxCookieCount) {
        Cookie* rmCookie = map->removeOldestCookie();
        cookieBackingStore().remove(rmCookie);
        removedCookie();
        delete rmCookie;
    }

    map->add(cookie);

    // Only add non session cookie to the backing store.
    if (!cookie->isSession())
        cookieBackingStore().insert(cookie);

    m_count++;
}

void CookieManager::update(CookieMap* map, Cookie* prevCookie, Cookie* newCookie)
{
    ASSERT(!map->takePrevious(prevCookie));
    map->add(newCookie);
    cookieBackingStore().update(newCookie);
}

void CookieManager::getBackingStoreCookies()
{
    Vector<Cookie*> cookies = cookieBackingStore().getAllCookies();

    for (size_t i = 0; i < cookies.size(); ++i) {
        Cookie* newCookie = cookies[i];

        if (newCookie->hasExpired()) {
            cookieBackingStore().remove(newCookie);
            delete newCookie;
        } else {
            CookieMap* curMap = m_managerMap.get(newCookie->domain());
            if (!curMap) {
                curMap = new CookieMap();
                m_managerMap.add(newCookie->domain(), curMap);
            }
            // Use the straightforward add
            curMap->add(newCookie);
            m_count++;
        }
    }
}

} // namespace WebCore
