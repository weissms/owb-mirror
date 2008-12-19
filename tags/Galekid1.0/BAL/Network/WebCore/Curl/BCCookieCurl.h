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

#ifndef Cookie_h
#define Cookie_h

#include "KURL.h"
#include "PlatformString.h"

namespace WKAL {

class Cookie {
public:
    // Default cookie : empty domain, non secure and session
    Cookie(double currentTime)
        : m_domain(String())
        , m_expiry(0)
        , m_isSecure(false)
        , m_isSession(true)
        , m_lastAccessed(currentTime)
    {
    }

    // For database cookies
    Cookie(const String& name, const String& value, const String& domain, const String& path, double& expiry, double& lastAccessed, bool& isSecure)
        : m_name(name)
        , m_value(value)
        , m_domain(domain)
        , m_path(path)
        , m_expiry(expiry)
        , m_isSecure(isSecure)
        , m_isSession(false)
        , m_lastAccessed(lastAccessed)
    {
    }

    ~Cookie()
    {
    }

    const String& name() const { return m_name; }
    void setName(const String& name) { m_name = name; }

    const String& value() const { return m_value; }
    void setValue(const String& value) { m_value = value; }

    const String& path() const { return m_path; }
    void setPath(const String& path) { m_path = path; }

    const String& domain() const { return m_domain; }
    void setDomain(const String& domain) { m_domain = domain; }

    double expiry() const { return m_expiry; }
    void setExpiry(const String& expiry);
    void setMaxAge(const String& maxAge);

    double lastAccessed() const { return m_lastAccessed; }
    void setLastAccessed(double lastAccessed) { m_lastAccessed = lastAccessed;}

    bool isSecure() const { return m_isSecure; }
    void setSecureFlag(bool secure) { m_isSecure = secure; }

    bool isSession() const { return m_isSession; }

    static Cookie* parse(const KURL& url, const String& cookie, unsigned start, unsigned end, double curTime);

private:
    String m_name;
    String m_value;
    String m_domain;
    String m_path;
    double m_expiry;
    bool m_isSecure;

    bool m_isSession;

    static inline bool is_lws(UChar c);

    // to manage LRU replacement policy
    double m_lastAccessed;

    // Constants
    static const int max_length = 4096; // Cookie size limit 4kB per RFC2109
};

} // Namespace WKAL

#endif
