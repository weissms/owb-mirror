/*
 * Copyright (C) Julien Chaffraix <julien.chaffraix@gmail.com>
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

#ifndef CookieManager_h
#define CookieManager_h

#include "config.h"

#include "Cookie.h"
#include "CookieMap.h"
#include "KURL.h"
#include "PlatformString.h"
#include <wtf/HashMap.h>

#if ENABLE(DATABASE)
#include "SQLiteDatabase.h"
#endif

namespace WebCore {

class CookieManager {
public:
    static CookieManager* getCookieManager();

    void setCookies(const KURL& url, const KURL& policyURL, const String& value);

    String getCookie(const KURL& url);
    String getCookie(const String& url);

    void removeAllCookies(bool shouldRemoveFromDatabase = true);

    unsigned short cookiesCount() { return m_count; }

    void setCookieJar(const char *);

    ~CookieManager();

private:
    CookieManager();

    inline bool is_cookie_sep(UChar c);

    void checkAndTreatCookie(Cookie* cookie);

    bool shouldReject(const Cookie* cookie, const KURL& url);

    void addCookieToMap(CookieMap* map, Cookie* cookie);
    void update(CookieMap* map, Cookie* prevCookie, Cookie* newCookie);

    // Count update method
    inline void removedCookie() { ASSERT(m_count > 0); m_count--; }

    HashMap<String, CookieMap*> m_managerMap;

    // Count all cookies, cookies are limited by max_count
    unsigned short m_count;

    String m_cookieJarFileName;

#if ENABLE(DATABASE)
    // Internal database manipulation methods
    void insertCookieIntoDatabase(const Cookie* cookie);
    void updateDatabaseEntry(const Cookie* cookie);
    void removeCookieFromDatabase(const Cookie* cookie);

    void getDatabaseCookies();

    SQLiteDatabase m_db;
#endif

    // Constants
    static const int max_count = 300;
};

} // namespace WebCore

#endif
