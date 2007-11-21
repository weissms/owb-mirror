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
#include "BIFile.h"


static BICookieJar* __BCCookieManager = 0;
char *filePath = "cookieCollection.txt";

BICookieJar* getBICookieJar()
{
    if (__BCCookieManager)
        return __BCCookieManager;
    else
        return __BCCookieManager = new BCCookieManager();
}

void deleteBICookieJar(BICookieJar*)
{
    delete __BCCookieManager;
}

BCCookieManager::BCCookieManager() : m_count(0) 
{
    //We do this to prevent the constructor code to call getBICookieJar() recursively
    if (__BCCookieManager == NULL)
        __BCCookieManager = this;
    BAL::getBIObserverService()->registerObserver(String("Set-Cookie"), this);
    getCookiesFromFile();
}

BCCookieManager::~BCCookieManager()
{
    saveCookiesToFile();
    
    // destroy all allocated all CookieMap
    for (HashMap<String, BTCookieMap*>::iterator it = m_managerMap.begin();
        it != m_managerMap.end(); ++it) {
            DBGM(MODULE_FACILITIES, "map for '%s' was '%s'!\n",
                 it->first.deprecatedString().ascii(),
                 it->second->getNameValuePairs().deprecatedString().ascii());
            delete it->second;
        }
    BAL::getBIObserverService()->removeObserver(String("Set-Cookies"), this);
}

void BCCookieManager::setCookies(const KURL& url, const KURL& /*policyURL*/, const String& value)
{
    int cookiesAdded = 0;
    String host = url.host();

    BTCookie cookie;
    cookie.initFromString(value);

    if (respectsSecurityStandards(url, cookie))
        cookiesAdded = add(host, cookie);
    else {
        DBGML(MODULE_FACILITIES, LEVEL_WARNING, "COOKIE %s is considered as dangerous and will be rejected\n",
                                                       cookie.name().deprecatedString().ascii());
        return;
    }

    if (cookiesAdded >= 0)
        BAL::getBIObserverService()->notifyObserver(String("SendCookies"), url.url());
}

String BCCookieManager::cookies(const KURL& url)
{
    DBGML(MODULE_FACILITIES, LEVEL_INFO, "Looking for cookies for URL : %s\n",url.url().ascii());

    HashMap<WebCore::String, BTCookieMap*> mapList = getMapListForHost(String(url.host()));
    if (!mapList.isEmpty()) {
        String result;
        if (url.url().startsWith("http:"))
            for (HashMap<WebCore::String, BTCookieMap*>::iterator it = mapList.begin(); it != mapList.end() ; ++it)
                result += it->second->getNameValuePairs(String(url.url().remove(0,url.host().length()+7)), false);
        else //https:
            for (HashMap<WebCore::String, BTCookieMap*>::iterator it = mapList.begin(); it != mapList.end() ; ++it)
                result += it->second->getNameValuePairs(String(url.url().remove(0,url.host().length()+8)), true);
        return result;
    }
    return String();
}

bool BCCookieManager::respectsSecurityStandards(const KURL& url, BTCookie& cookie)
{
    if (!cookie.domain().isEmpty()) {
        String domain = cookie.domain();
        domain = domain.right(domain.length()-1);
        if (!domain.contains(".",false) || !url.host().endsWith(domain.deprecatedString())) {
            DBGML(MODULE_FACILITIES, LEVEL_WARNING, "Illegal Cookie : Bad domain name\n");
            return false;
        }
        /*
         * A Set-Cookie from request-host y.x.foo.com for Domain=.foo.com
         * would be rejected, because y.x and contains a dot.
         */
        if (url.host()!=domain && url.host().left(url.host().length()-domain.length()-1).contains(".",false)) {
            DBGML(MODULE_FACILITIES, LEVEL_WARNING, "Illegal Cookie : y.x.foo.com for Domain=.foo.com\n");
            return false;
        }
    }

    if (!url.path().startsWith(cookie.path().deprecatedString())) {
        DBGML(MODULE_FACILITIES, LEVEL_WARNING, "Illegal Cookie : cookie's path doesn't match the present URL\n");
        return false;
    }

    return true;
}

int BCCookieManager::add(const String& host, const BTCookie& cookie)
{
    int addedCookies = 0;
    
    //If the cookie specifies a domain name, the map should have this name else it'll be the host name. 
    String domain = host;
    if (!cookie.domain().isEmpty())
        domain = cookie.domain();
    
    //Need to check first if the cookie map already exists to eventually create one
    if (!thisMapExists(domain)) {
        BTCookieMap* newMap = new BTCookieMap();
        
        m_managerMap.set(domain, newMap);
        DBGML(MODULE_FACILITIES, LEVEL_INFO, "NEW BTCookieMap created : %s \n", domain.deprecatedString().ascii());
        
        addedCookies = newMap->add(cookie);
        m_count += addedCookies;
        return addedCookies;
    }
    
    //getting the map related to this host
    HashMap<WebCore::String, BTCookieMap*> existingMap = getMapListForHost(domain, true);
    
    DBGML(MODULE_FACILITIES, LEVEL_INFO, "Trying to add a cookie to : %s \n", domain.deprecatedString().ascii());
    addedCookies = existingMap.begin()->second->add(cookie);
    m_count += addedCookies;
    return addedCookies;
}

bool BCCookieManager::thisMapExists(String mapName)
{
    for (HashMap<WebCore::String, BTCookieMap*>::iterator it = m_managerMap.begin(); it != m_managerMap.end() ; ++it) {
        if (it->first == mapName)
            return true;
    }

    return false;
}

// can't return BTCookieMap& because of map.get(host) which returns temporary val
HashMap<WebCore::String, BTCookieMap*> BCCookieManager::getMapListForHost(const String& host, bool onlyOneMap) const
{
    HashMap<WebCore::String, BTCookieMap*> tmp;
    for (HashMap<WebCore::String, BTCookieMap*>::const_iterator it = m_managerMap.begin(); it != m_managerMap.end(); ++it) {
        if (onlyOneMap && host == it->first) {
            tmp.add(it->first,it->second);
            return tmp;
        }

        /* To handle the sub domains, we only check the end of the host to return the map */
        if (!onlyOneMap && host.endsWith(it->first,false) || (it->first.startsWith(".",false) && ("."+host).endsWith(it->first,false)) )
            tmp.add(it->first,it->second);
    }

    if (tmp.isEmpty())
        DBGML(MODULE_FACILITIES, LEVEL_INFO, "Get MAP for host : %s \n NO MAP FOUND \n", host.deprecatedString().ascii());
    return tmp; // do not create map for each site visited
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

    DBGML(MODULE_FACILITIES, LEVEL_INFO, "*\n* OBSERVE Cookie %s \n* \nfor URL %s\n", data.left(index).deprecatedString().ascii(), urlStr.deprecatedString().ascii());
    setCookies(url, policy, data.left(index));
}

void BCCookieManager::saveCookiesToFile() 
{
    BAL::BIFile* cookieFile = new BAL::BIFile(filePath);
    int openedFile = cookieFile->open('w');
    
    if (openedFile == -1)
        DBGML(MODULE_FACILITIES, LEVEL_WARNING, "Cookie file can't be created\n");
    
    String tmp, tmpCookieList;
    for (HashMap<String, BTCookieMap*>::iterator it = m_managerMap.begin(); it != m_managerMap.end(); ++it) {
        DBGML(MODULE_FACILITIES, LEVEL_INFO, "Saving CookieMap : %s \n", it->first.deprecatedString().ascii());
        tmpCookieList = it->second->createBackupStringFromCookies();
        if (!tmpCookieList.isEmpty()) {
            tmp = it->first.deprecatedString().ascii();
            tmp.append('\n');
            
            tmp.append(tmpCookieList);
            tmp.append("*\n");
            
            cookieFile->write(tmp.deprecatedString().ascii());
        }
    }
    
    cookieFile->close();
    delete cookieFile;
}

void BCCookieManager::getCookiesFromFile()
{
    char* buffer = NULL;
    BAL::BIFile* cookieFile = new BAL::BIFile(filePath);
    
    if (cookieFile->open('r') == -1) {
        DBGML(MODULE_FACILITIES, LEVEL_WARNING, "Cookie file can't be opened\n");
        delete cookieFile;
        return;
    }
    
    int size = cookieFile->getSize();
    if (size > 0) {
        buffer = cookieFile->read(size);
        
        String cookieCollection(buffer, size), map, cookie;
        int startOfLine = 0, endOfLine = 0, addedCookies = 0;
        bool readCookie;
        BTCookieMap* newMap;
        BTCookie* newCookie;
        
        //read the entire file content
        while (endOfLine < (int)cookieCollection.length()) {
            //start a new BTCookieMap after each '*' character
            newMap = new BTCookieMap();
            
            //getting the name of the map (which is the domain)
            endOfLine = cookieCollection.find("\n",startOfLine);
            map = cookieCollection.substring(startOfLine, endOfLine - startOfLine);
            
            //adding the map to the cookieManager
            m_managerMap.set(map, newMap);
            
            readCookie = true;
            
            //reading all the cookies from this map
            while (readCookie) {
                startOfLine = endOfLine + 1;
                endOfLine = cookieCollection.find("\n",startOfLine);
                cookie = cookieCollection.substring(startOfLine, endOfLine - startOfLine);
                if (cookie == "*") {
                    readCookie = false;
                    startOfLine = endOfLine + 1;
                    endOfLine++;
                }
                else {
                    newCookie = new BTCookie;
                    newCookie->initCookieFromBackupString(cookie, map);
                    addedCookies = newMap->add(*newCookie);
                    m_count += addedCookies;
                    delete newCookie;
                }
            } //end reading cookies from this map
        } //end reading map from the file content

    } //end analysing the file

    cookieFile->close();

    delete buffer;
    delete cookieFile;
}

unsigned short BCCookieManager::cookiesCount()
{
    return m_count;
}
