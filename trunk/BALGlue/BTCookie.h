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
 * @file  BTCookie.h
 *
 * Header file for BTCookie.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef BTCOOKIE_H
#define BTCOOKIE_H

#include "StringHash.h"
#include "wtf/HashMap.h"
#include "PlatformString.h"

using WebCore::String;

/**
 * A Cookie is basically name/value pair stored on host by web pages.
 * domain and path are stored but not used.
 * expires and secure are unsupported.
 */
class BTCookie {
public:
    BTCookie() : m_expires(0), m_secure(false) {}
    BTCookie(String& name, String& value, String& /*expires*/, String& /*secure*/) :
        m_name(name), m_value(value), m_expires(0), m_secure(false)
    {}
    ~BTCookie() {}

    /**
     * Get the cookie name.
     */
    const String& getName() const { return m_name; }
    /**
     * Get the cookie value.
     */
    const String& getValue() const { return m_value; }
    /**
     * Returns true if cookie is set for all domain.
     */
    bool isDomain() const { return !m_domain.isEmpty(); }
    /**
     * The cookie is secured only if the transfer was secured (https).
     */
    bool isSecure() const { return m_secure; }
    /**
     * Construct the cookie from the values stored in parameter.
     * @param(in) string containing at least name=value
     */
    void initFromString(const String& cookie);

private:
    String extractValueIn(const char* name, const String& cookie);
    String m_name;
    String m_value;
    String m_domain;
    String m_path;
    int m_expires;
    bool m_secure;
};

/**
 * A CookieMap is design to hold cookies for one host.
 * All cookies for one host are stored in a map.
 */
class BTCookieMap {
public:
    BTCookieMap() : m_count(0) {}
    ~BTCookieMap() {}
    /**
     * Store a cookie in map.
     * If it already exists, it is updated.
     * @param(in) the cookie
     */
    void add(const BTCookie& cookie);
    /**
     * Returns "name=value; ..." sequences in one line for all cookies.
     */
    String getNameValuePairs();

private:
    /// map of cookie names and the BTCookie
    HashMap<String, BTCookie> m_cookieMap;

    /// cookies are limited to 20 per domain
    unsigned short m_count;
};

#endif // BTCOOKIE_H


