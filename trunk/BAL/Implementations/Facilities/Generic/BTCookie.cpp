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
#include "BALConfiguration.h"
#include "BICookieJar.h"
#include "SystemTime.h"
#include "BTCookie.h"
#include "BTLogHelper.h"
#include "DeprecatedString.h"
#include "PlatformString.h"

//
// BTCookie implementation
//

void BTCookie::initFromString(const String& cookie)
{
    if (cookie.length() > 4096) {
        DBGML(MODULE_FACILITIES, LEVEL_WARNING, "cookies are limited to 4KB\n");
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
    
    //the expiration date is read in a temporary string
    String tmpExpires;
    tmpExpires = extractValueIn("expires=", cookie);
    
    if (!tmpExpires || tmpExpires.length() < 20) //then it is a session cookie
        m_expires = -1;
    else
        //then we convert it in seconds since 1/1/1970
        m_expires = timeFromString(tmpExpires);
    
    m_path = extractValueIn("path=", cookie);
    if (!m_path) 
        m_path = "/";
    
    m_domain = extractValueIn("domain=", cookie);
    if (!m_domain.isEmpty() && !m_domain.startsWith(".",false))
        m_domain = "." + m_domain;
    
    if (cookie.contains("secure",false))
        m_secure = true;

    DBGM(MODULE_FACILITIES, "The following cookie was initialized :\n"
            "(%s)=(%s)\n%s=%lf\n%s=%s\n%s=%s\n%s=%d \n",
        m_name.deprecatedString().ascii(), m_value.deprecatedString().ascii(),
        "expires", m_expires ,
        "path", m_path.deprecatedString().ascii(),
        "domain", m_domain.deprecatedString().ascii(),
        "secure", m_secure);

}

String BTCookie::createBackupStringFromCookie()
{
    String result;
    
    result = m_name + "=" + m_value + ";";
    result += "path=" + m_path + ";" ;
    String tmp = String::number(m_expires);
    result += "expires=" +tmp + ";secure=" ;
    if (m_secure)
        result += "true;" ;
    else
        result += "false;" ;
    
    return result;
}

/**
 * Initiate the cookie from a backup string
 */
void BTCookie::initCookieFromBackupString(String cookie, String domain)
{
    int first, second;
    String tmp;
    
    // find name
    first = cookie.find("=");
    m_name = cookie.left(first);
    
    // find value
    second = cookie.find(";");
    m_value = cookie.substring(first + 1, second - first - 1);
    
    //find path
    m_path = extractValueIn("path=", cookie);
    
    //find expiration date
    tmp = extractValueIn("expires=", cookie);
    m_expires = tmp.toDouble();
    
    //the domain is the cookieMap name
    m_domain = domain;
    
    //set secure state
    tmp = extractValueIn("secure=", cookie);
    if (tmp.contains("false",true))
        m_secure = false;
    else
        m_secure = true;
    
    DBGM(MODULE_FACILITIES, "The following cookie was read from file :\n" 
                                         "(%s)=(%s)\n%s=%lf\n%s=%s\n%s=%s\n%s=%d \n",
                                         m_name.deprecatedString().ascii(), m_value.deprecatedString().ascii(),
                                         "expires", m_expires ,
                                         "path", m_path.deprecatedString().ascii(),
                                         "domain", m_domain.deprecatedString().ascii(),
                                         "secure", m_secure);
}

String BTCookie::extractValueIn(const char* name, const String& cookie)
{
    int first, second;
    // find name
    first = cookie.find(name,0,false);
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

int BTCookieMap::add(const BTCookie& cookie)
{
    int numberAdded = 0;
    
    //check if the cookie already exists to replace it by a new one
    if (m_cookieMap.contains(cookie.name())) {
        DBGML(MODULE_FACILITIES, LEVEL_INFO, "Cookie named %s is replaced by a new one\n",
                                                      cookie.name().deprecatedString().ascii());
        m_cookieMap.remove(cookie.name());
        numberAdded--;
    }
    
    //check if the cookie hasn't expired
    if (cookie.expireDate() != -1 && cookie.expireDate() < WebCore::currentTime()) {
        DBGML(MODULE_FACILITIES, LEVEL_INFO, "COOKIE Removed : %s : his expires date (%lf) is lower than systeme date (%lf) \n",
                                                       cookie.name().deprecatedString().ascii(), cookie.expireDate(), WebCore::currentTime());
        
        return numberAdded;
    }
    
    //check if there's room for the new cookie in this map, if there is not we make some for it
    if (m_count >= 20) {
        DBGML(MODULE_FACILITIES, LEVEL_WARNING, "Cookies are limited to 20 per domain...a cookie from this domain will be REMOVED TO MAKE ROOM for %s.\n", 
                                                         cookie.name().deprecatedString().ascii());
        double min = WebCore::currentTime();
        HashMap<String, BTCookie>::iterator itBak = m_cookieMap.begin();
        for (HashMap<String, BTCookie>::iterator it = m_cookieMap.begin();it != m_cookieMap.end(); ++it) {
            if (it->second.expireDate() < min) {
                min = it->second.expireDate();
                itBak = it;
            }
        }
        m_cookieMap.remove(itBak);
        m_count--;
        numberAdded--;
    }
    
    if ((getBICookieJar()->cookiesCount()) >= 300) {
        DBGML(MODULE_FACILITIES, LEVEL_WARNING, "Cookies' count is limited to 300\n");
        return numberAdded;
    }
    
    //We add the cookie in the map
    m_cookieMap.set(cookie.name(), cookie);
    
    // NOTE can't test correctly return state in .second
    DBGML(MODULE_FACILITIES, LEVEL_INFO, "*\n* COOKIE ADDED : %s\n***********\n",cookie.name().deprecatedString().ascii());
    m_count++;
    numberAdded++;
    
    return numberAdded;
}

String BTCookieMap::createBackupStringFromCookies()
{
    String result;
    for (HashMap<String, BTCookie>::iterator it = m_cookieMap.begin(); it != m_cookieMap.end(); ++it) {
        if (it->second.expireDate() != -1 && it->second.expireDate() > WebCore::currentTime()) {
            result += it->second.createBackupStringFromCookie();
            result += "\n";
        }
        else
            DBGML(MODULE_FACILITIES, LEVEL_INFO, "\n   Cookie %s --> removed\n", it->first.deprecatedString().ascii());
    }
    
    return result;
}


// can't be const because of m_cookieMap.begin() and .end()
String BTCookieMap::getNameValuePairs()
{
    String result;
    for (HashMap<String, BTCookie>::iterator it = m_cookieMap.begin();
        it != m_cookieMap.end(); ++it) {
        result += it->first + "=" + it->second.value() + "; ";
            DBGML(MODULE_FACILITIES, LEVEL_INFO, "first=%s second=%s \n",
                it->first.deprecatedString().ascii(),
                it->second.value().deprecatedString().ascii());
    }
    return result;
}

// can't be const because of m_cookieMap.begin() and .end()
String BTCookieMap::getNameValuePairs(String path, bool siteIsSecure)
{
    String result = "";
    for (HashMap<String, BTCookie>::iterator it = m_cookieMap.begin(); it != m_cookieMap.end(); ++it)
        if (path.startsWith(it->second.path(),false) && (!it->second.isSecure() || siteIsSecure))
            result += it->first + "=" + it->second.value() + "; ";

    return result;
}

