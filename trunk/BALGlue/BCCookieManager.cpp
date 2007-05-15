/*
 * Copyright (C) 2007 Pleyo.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Pleyo nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PLEYO AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PLEYO OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/**
 * @file  BCCookieManager.cpp
 *
 * Implementation file for BCCookieManager.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#include "config.h"
#include "BALConfiguration.h"
#include "BCCookieManager.h"
#include "BIObserverService.h"
#include "BTCookie.h"
#include "BTLogHelper.h"
#include "DeprecatedString.h"
#include "KURL.h"

using WebCore::String;


static BICookieJar* __BCCookieManager = 0;

BICookieJar* getBICookieJar()
{
    if (__BCCookieManager)
        return __BCCookieManager;
    else
        return __BCCookieManager = new BCCookieManager();
}

void deleteBICookieJar(BICookieJar*)
{
}

BCCookieManager::BCCookieManager() : m_count(0) 
{
    BAL::getBIObserverService()->registerObserver(String("Set-Cookie"), this);
}

BCCookieManager::~BCCookieManager()
{
    // destroy all allocated all CookieMap
    for (HashMap<String, BTCookieMap*>::iterator it = m_managerMap.begin();
        it != m_managerMap.end(); ++it) {
            logm(MODULE_BRIDGE,make_message("map for '%s' was '%s'!\n",
                 it->first.deprecatedString().ascii(),
                 it->second->getNameValuePairs().deprecatedString().ascii()));
            delete it->second;
        }
    BAL::getBIObserverService()->removeObserver(String("Set-Cookies"), this);
}

void BCCookieManager::setCookies(const KURL& url, const KURL& /*policyURL*/, const String& value)
{
    BTCookie cookie;
    cookie.initFromString(value);
    String host(url.host());
    add(host, cookie);
}

String BCCookieManager::cookies(const KURL& url)
{
    BTCookieMap* map = getMapForHost(String(url.host()));
    if (map)
        return map->getNameValuePairs();
    else
        return String();
}

void BCCookieManager::add(const String& host, const BTCookie& cookie)
{
    if (m_count >= 300) {
        logml(MODULE_BRIDGE, LEVEL_WARNING, "cookies' count is limited to 300");
        return;
    }

    BTCookieMap* map = getMapForHost(host);
    if (!map) {
        map = new BTCookieMap();
        m_managerMap.set(host, map);
    }
    map->add(cookie);
    m_count++;
}

// can't return BTCookieMap& because of map.get(host) which returns temporary val
BTCookieMap* BCCookieManager::getMapForHost(const String& host) const
{
    if (m_managerMap.find(host) == m_managerMap.end())
        return 0; // do not create map for each site visited
    else
        return m_managerMap.get(host);
}

void BCCookieManager::observe(const String& topic, const String& data)
{
    // for Cookie Observer, add URL= into cookie to know which 'url' parameter to pass to setCookies method.
    // this is the reason why data has to be manipulated before calling setCookies().
    int index = data.find(" URL=", 0, true);
    String urlStr = data.copy();
    urlStr.remove(0, index + 5);
    const KURL url = KURL(urlStr.deprecatedString());
    const KURL policy = KURL();

    logml(MODULE_BRIDGE, LEVEL_INFO, make_message("observe cookie %s for URL %s\n", data.left(index).deprecatedString().ascii(), urlStr.deprecatedString().ascii()));
    setCookies(url, policy, data.left(index));
}
