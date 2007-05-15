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
 * @file  BTCookie.cpp
 *
 * Implementation file for BTCookie.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#include "config.h"
#include "BTCookie.h"
#include "BTLogHelper.h"
#include "DeprecatedString.h"

//
// BTCookie implementation
//

void BTCookie::initFromString(const String& cookie)
{
    if (cookie.length() > 4096) {
        logml(MODULE_BRIDGE, LEVEL_WARNING, "cookies are limited to 4KB");
        return;
    }

    int first, second;
    // find name
    first = cookie.find("=");
    m_name = cookie.left(first);
    // find value
    second = cookie.find(";");
    m_value = cookie.substring(first + 1, second - first - 1);
    // name is the first arg before =
    // m_expires = extractValueIn("expires=", cookie); // convert to date
    m_path = extractValueIn("path=", cookie);
    m_domain = extractValueIn("domain=", cookie);

    logm(MODULE_BRIDGE, make_message("The following cookie was initialized : "
            "(%s)=(%s)\n%s=%d\n%s=%s\n%s=%s\n%s=%d",
        m_name.deprecatedString().ascii(), m_value.deprecatedString().ascii(),
        "expires", m_expires,
        "path", m_path.deprecatedString().ascii(),
        "domain", m_domain.deprecatedString().ascii(),
        "secure", m_secure));
}

String BTCookie::extractValueIn(const char* name, const String& cookie)
{
    int first, second;
    // find name
    first = cookie.find(name);
    if (first == -1)
        return String(); // null string
    // find value
    second = cookie.find(";", first);
    if (second == -1 && cookie.find(" ", first) != -1)
        // no ";" and no " ", so we're and the end of line
        second = cookie.length();
    return cookie.substring(first + strlen(name), second - first - strlen(name));
}

//
// BTCookieMap implementation
//

void BTCookieMap::add(const BTCookie& cookie)
{
    if (m_count >= 20) {
        logml(MODULE_BRIDGE, LEVEL_WARNING, "cookies are limited to 20 per domain");
        return;
    }
    // NOTE can't test correctly return state in .second
    m_cookieMap.set(cookie.getName(), cookie);
    m_count++;
}

// can't be const because of m_cookieMap.begin() and .end()
String BTCookieMap::getNameValuePairs()
{
    String result;
    for (HashMap<String, BTCookie>::iterator it = m_cookieMap.begin();
        it != m_cookieMap.end(); ++it) {
        result += it->first + "=" + it->second.getValue() + "; ";
/*            log(make_message("first=%s second=%s",
                it->first.deprecatedString().ascii(),
                it->second.getValue().deprecatedString().ascii()));*/
    }
    return result;
}
