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
 * @file  BCCookieManager.h
 *
 * Header file for BCCookieManager.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef BTCOOKIEMANAGER_H
#define BTCOOKIEMANAGER_H

#include "BICookieJar.h"
#include "BIObserver.h"
#include "PlatformString.h"
#include "StringHash.h"
#include "wtf/HashMap.h"

class BTCookie;
class BTCookieMap;

using BAL::BIObserver;

/**
 * The cookie manager is responsible of the cookie storage.
 * It owns a cookie map for each host which owns cookies.
 */
class BCCookieManager : public BICookieJar, BIObserver {
public:
    BCCookieManager();
    ~BCCookieManager();

    IMPLEMENT_BICOOKIEJAR;

    /**
     * Add a cookie for a host in map.
     * @param(in) the owner host
     * @param(in) the cookie to add
     */
    int add(const WebCore::String& host, const BTCookie& cookie);

    bool respectsSecurityStandards(const KURL& url, BTCookie& cookie);
    
    bool thisMapExists(String mapName);
        
    /**
     * Ask for the cookie map related to host.
     * @param(in) the host concerned
     * @return a list of cookie maps containing cookies for host
     * @return 0 if there's no cookie for host
     */
    HashMap<WebCore::String, BTCookieMap*> getMapListForHost(const WebCore::String& host, bool onlyOneMap = false) const;

    /**
     * Link cookie manager to an observer.
     * @param(in) the topic to register
     * @param(in) the cookie
     */
    void observe(const String&, const String&);
    
    /**
     * Save all the cookies in a file for backup
     * We use a text file for this.
     */
    void saveCookiesToFile();
    
    /**
     * Load all the cookies from a file
     * Expired cookies and previous session cookies are left behind
     */
    void getCookiesFromFile();

private:
    // can't put reference BTCookieMap& in map
    HashMap<WebCore::String, BTCookieMap*> m_managerMap;

    /// count all cookies. Cookies number is limited to 300
    unsigned short m_count;
};


#endif // BTCOOKIEMANAGER_H
