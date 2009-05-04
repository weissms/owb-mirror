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

#ifndef CookieMap_h
#define CookieMap_h

#include "config.h"

#include "StringHash.h"
#include "PlatformString.h"

namespace WebCore {

    class Cookie;

    class CookieMap {

    public:
        CookieMap();
        ~CookieMap();

        int count() { return m_cookieMap.size(); }

        void add(Cookie* cookie);
        void remove(const Cookie* cookie);

        Vector<Cookie*> getCookies();

        // Will take the cookie that match the paramater
        Cookie* takePrevious(const Cookie* cookie);

        bool canInsertCookie() { return (m_cookieMap.size() < max_count); }

        void updateTime(Cookie* cookie, double newTime);

        // Return Cookie to remove it from the database in the CookieManager
        Cookie* removeOldestCookie();

    private:
        void updateOldestCookie();

        // The key is the tuple (name, path)
        // The spec asks to have also domain, which is implied by choosing the CookieMap relevant to the domain
        HashMap<String, Cookie*> m_cookieMap;

        // Store the oldest cookie to speed up LRU checks
        Cookie* m_oldestCookie;

        // Constants
        // The number of cookie is limited to max_count (ie 20)
        static const int max_count = 20;

        // FIXME : should have a m_shouldUpdate flag to update the network layer only when the map has changed
    };

} // namespace WebCore

#endif // CookieMap_h
