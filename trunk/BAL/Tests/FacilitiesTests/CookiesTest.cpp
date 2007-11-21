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

#include <assert.h>
#include "BALConfiguration.h"
#include "BICookieJar.h"
#include "KURL.h"
#include "PlatformString.h"
#include "TestManager/TestManager.h"

using WebCore::String;
using WebCore::KURL;

String map;

class TestsCookiesSecurity
{
public:
    static void createLotsOfCookies()
    {
        TestManager::LogMessage("  ->createLotsOfCookies()\n ");
        respectsSecurityFromRFC();
        lotsOfCookiesForOneMap();
        moreThan300Cookies();
    }
    
private:
    static void lotsOfCookiesForOneMap()
    {
        KURL tmpUrl("http://www.foo.com/iLoveCookies.html");
        String cookieToSend;
        
        for (int i=0; i<=20; i++) {
            cookieToSend = "fooCookies" + String::number(i) + "=value";
            getBICookieJar()->setCookies(tmpUrl,tmpUrl,cookieToSend);
        }
        
        String result = getBICookieJar()->cookies(tmpUrl);
        TestManager::AssertTrue("Cookie value", result == "fooCookies4=value; fooCookies8=value; fooCookies11=value; fooCookies15=value; fooCookies9=value; fooCookies5=value; fooCookies14=value; fooCookies10=value; fooCookies0=value; fooCookies2=value; fooCookies1=value; fooCookies7=value; fooCookies12=value; fooCookies18=value; fooCookies17=value; fooCookies13=value; fooCookies6=value; fooCookies19=value; fooCookies3=value; fooCookies20=value; ");
    }
    
    static void moreThan300Cookies()
    {
        KURL* url;
        String cookieToSend, tmpUrl;
        
        for (int i=0; i<=299; i++) {
            tmpUrl = "http://" + String::number(i) + ".foo.com/iLoveCookies.html";
            url = new KURL(tmpUrl.deprecatedString());
            cookieToSend = "fooCookies" + String::number(i) + "=value; domain=." + String::number(i) + ".foo.com";
            getBICookieJar()->setCookies(*url,*url,cookieToSend);
            delete url;
        }
        
        KURL tmpUrl2("http://www.lastCookie.com/roomMissing.html");
        String lastCookie("noMoreSpace=forMe");
        getBICookieJar()->setCookies(tmpUrl2, tmpUrl2, lastCookie);
        
        TestManager::AssertTrue("cookie is empty", getBICookieJar()->cookies(tmpUrl2).isEmpty());
        
        for (int i=0; i<=19; i++) {
            tmpUrl = "http://" + String::number(i) + ".foo.com/iLoveCookies.html";
            url = new KURL(tmpUrl.deprecatedString());
            cookieToSend = "fooCookies" + String::number(i) + "=value; expires=Sun, 17-Jan-1970 19:14:07 GMT ;domain=." + String::number(i) + ".foo.com";
            getBICookieJar()->setCookies(*url,*url,cookieToSend);
            delete url;
        }
    }
    
    static void respectsSecurityFromRFC()
    {
        //the difference between the url domain and the cookie domain name should not contain a dot
        KURL url("http://x.y.foo.com/rfc.html");
        String cookie("toBeRejected=true; domain=.foo.com");
        getBICookieJar()->setCookies(url, url, cookie);
        TestManager::AssertTrue("cookie is empty", getBICookieJar()->cookies(url).isEmpty());
        
        //cookie's domain should match the present URL domain.
        url = "http://www.oneUrl.com/rfc.html";
        cookie = "toBeRejected=true; domain=www.differentUrl.com";
        getBICookieJar()->setCookies(url, url, cookie);
        TestManager::AssertTrue("cookie is empty", getBICookieJar()->cookies(url).isEmpty());
        
        //cookie's path should match the present URL path.
        url = "http://www.oneUrl.com/rfc/rfc.html";
        cookie = "toBeRejected=true; path=/test/";
        getBICookieJar()->setCookies(url, url, cookie);
        TestManager::AssertTrue("cookie is empty", getBICookieJar()->cookies(url).isEmpty());
    }
};


class TestsForAccessingCookies
{
public:
    static void accessCookies()
    {
        TestManager::LogMessage("  ->accessCookies()\n ");
        String result1, result2;
        
        result1 = getCookiesFromOneMap();
        TestManager::AssertTrue("accessCookies", result1=="test2=content2; test1=content1; ");
        
        result2 = getCookiesOneUrlForTwoMaps();
        TestManager::AssertTrue("accesCookies", result2=="test4=content4; test3=content3; ");
    }
    
private:
    static String getCookiesFromOneMap()
    {
        //url1 = http://www.testOWB.com/CookiesTest.html
        KURL url1("http://www.testOWB.com/CookiesTest.html");
        return getBICookieJar()->cookies(url1);
    }
    
    static String getCookiesOneUrlForTwoMaps()
    {
        //url3 = http://test.OWB.com/CookiesTest.html
        KURL url3("http://test.OWB.com/CookiesTest.html");
        return getBICookieJar()->cookies(url3);
    }
};

class TestsForCreatingCookies
{
private:
    
    //creates a standard cookie with all fields (but the secure one) filled in
    static void createOneCookie()
    {
        //url1 = http://www.testOWB.com/CookiesTest.html
        KURL url1("http://www.testOWB.com/CookiesTest.html");
        String content1("test1=content1; expires=Sun, 17-Jan-2038 19:14:07 GMT; path=/; domain=www.testOWB.com;");
        
        getBICookieJar()->setCookies(url1,url1,content1);
    }
    
    static void createSessionCookie()
    {
        //url1 = http://www.testOWB.com/CookiesTest.html
        KURL url1("http://www.testOWB.com/CookiesTest.html");
        String sessionCookie("test2=content2; path=/; domain=www.testOWB.com;");
        
        getBICookieJar()->setCookies(url1,url1,sessionCookie);
    }
    
    static void createCookiesForTwoMaps()
    {
        //url3 = http://test.OWB.com/CookiesTest.html
        KURL url3("http://test.OWB.com/CookiesTest.html");
        String noPath("test3=content3; expires=Sun, 17-Jan-2038 19:14:07 GMT; domain=.OWB.com;");
        String noDomain("test4=content4; expires=Sun, 17-Jan-2038 19:14:07 GMT; path=/;");
        
        getBICookieJar()->setCookies(url3,url3,noPath);
        getBICookieJar()->setCookies(url3,url3,noDomain);
    }
    
    static void deleteOneCookie()
    {
        KURL tmpUrl("http://www.fakeUrl.com/testForRemove.html");
        
        String willBeRemoved("toRemove=true; expires=Sun, 17-Jan-2038 19:14:07 GMT; path=/;");
        getBICookieJar()->setCookies(tmpUrl,tmpUrl,willBeRemoved);
        TestManager::AssertTrue("cookie value", getBICookieJar()->cookies(tmpUrl) == "toRemove=true; ");
        
        willBeRemoved = "toRemove=true; expires=Sun, 17-Jan-1970 19:14:07 GMT; path=/;";
        getBICookieJar()->setCookies(tmpUrl,tmpUrl,willBeRemoved);
        TestManager::AssertTrue("cookie string is empty", getBICookieJar()->cookies(tmpUrl) == "");
    }

public:
    static void createCookies() {
        TestManager::LogMessage("  ->createCookies()\n ");
        createOneCookie();
        createSessionCookie();
        createCookiesForTwoMaps();
        
        deleteOneCookie();
    }
    
};

static TestNode gCreateCookies = { "TestSetCookies", "TestSetCookies",
    TestNode::AUTO, TestsForCreatingCookies::createCookies, NULL };

static TestNode gAccessCookies = { "TestGetCookies", "TestGetCookies",
    TestNode::AUTO, TestsForAccessingCookies::accessCookies, NULL };

static TestNode gCreateLotsOfCookies = { "TestCookiesSecurity", "TestCookiesSecurity",
    TestNode::AUTO, TestsCookiesSecurity::createLotsOfCookies, NULL };

TestNode* gCookiesTestNodeList[] = {
    &gCreateCookies,
    &gAccessCookies,
    &gCreateLotsOfCookies,
	NULL
};

TestNode gTestSuiteFacilities = {
    "TestCookies",
    "test cookies",
    TestNode::TEST_SUITE,
    NULL, /* no function, it's a test suite */
    gCookiesTestNodeList
};
