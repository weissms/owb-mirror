/*
 * Copyright (C) 2009 Julien Chaffraix <jchaffraix@pleyo.com>
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

#include <config.h>
#include "CookieDatabaseBackingStore.h"

#if ENABLE(DATABASE)

#include "Cookie.h"
#include "CookieManager.h"
#include "SQLiteStatement.h"

namespace WebCore {

CookieDatabaseBackingStore& cookieDatabaseBackingStore()
{
    static CookieDatabaseBackingStore backingStore;
    return backingStore;
}

CookieDatabaseBackingStore::CookieDatabaseBackingStore()
{
}

CookieDatabaseBackingStore::~CookieDatabaseBackingStore()
{
}

void CookieDatabaseBackingStore::open(const String& cookieJar)
{
    if (m_db.isOpen())
        close();

    if (!m_db.open(cookieJar))
        LOG_ERROR("Could not open the cookie database");
}

void CookieDatabaseBackingStore::close()
{
    m_db.close();
}

void CookieDatabaseBackingStore::insert(const Cookie* cookie)
{
    ASSERT(m_db.isOpen());
    
    if (!m_db.tableExists(String("cookies")))
        // Mozilla uses isHttpOnly which is not included as we do not use it
        m_db.executeCommand(String("CREATE TABLE cookies (name TEXT, value TEXT, host TEXT, path TEXT, expiry DOUBLE, lastAccessed DOUBLE, isSecure INTEGER);"));
 
    SQLiteStatement insert(m_db, String("INSERT INTO cookies (name, value, host, path, expiry, lastAccessed, isSecure) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7);"));

    if (insert.prepare()) {
        LOG_ERROR("Cannot save cookie");
        return;
    }

    // Binds all the values
    if (insert.bindText(1, cookie->name()) || insert.bindText(2, cookie->value()) || insert.bindText(3, cookie->domain()) || insert.bindText(4, cookie->path()) || insert.bindDouble(5, cookie->expiry()) || insert.bindDouble(6, cookie->lastAccessed()) || insert.bindInt64(7, cookie->isSecure())) {
        LOG_ERROR("Cannot save cookie");                                                                                                                                                          
        return;                                                                                                                                                                                   
    }

    if (!insert.executeCommand()) {                                                                                                                                                               
        LOG_ERROR("Cannot save cookie");                                                                                                                                                          
        return;
    }
}

void CookieDatabaseBackingStore::update(const Cookie* cookie)
{
    ASSERT(m_db.isOpen());
    ASSERT(m_db.tableExists(String("cookies")));

    // the where statement is chosen to match CookieMap key
    SQLiteStatement update(m_db, String("UPDATE cookies SET name = ?1, value = ?2, host = ?3, path = ?4, expiry = ?5, lastAccessed = ?6, isSecure = ?7 where name = ?1 and host = ?3 and path = ?4;"));

    if (update.prepare()) {
        LOG_ERROR("Cannot update cookie");
        return;
    }

    // Binds all the values
    if (update.bindText(1, cookie->name()) || update.bindText(2, cookie->value()) || update.bindText(3, cookie->path()) || update.bindText(4, cookie->domain()) || update.bindDouble(5, cookie->expiry()) || update.bindDouble(6, cookie->lastAccessed()) || update.bindInt64(7, cookie->isSecure())) {
        LOG_ERROR("Cannot update cookie");
        return;
    }

    if (!update.executeCommand()) {
        LOG_ERROR("Cannot update cookie");
        return;
    }
 
}

void CookieDatabaseBackingStore::remove(const Cookie* cookie)
{
    ASSERT(m_db.isOpen());

    if(!m_db.tableExists(String("cookies")))
        return;

    // the where statement is chosen to match CookieMap key
    SQLiteStatement deleteStmt(m_db, String("DELETE FROM cookies WHERE name=?1 and host=?2 and path=?3;"));

    if (deleteStmt.prepare()) {
        LOG_ERROR("Cannot delete cookie");
        return;
    }

    // Binds all the values
    if (deleteStmt.bindText(1, cookie->name()) || deleteStmt.bindText(2, cookie->domain()) || deleteStmt.bindText(3, cookie->path())) {
        LOG_ERROR("Cannot delete cookie");
        return;
    }

    if (!deleteStmt.executeCommand()) {
        LOG_ERROR("Cannot delete cookie from database");
        return;
    }
}

Vector<Cookie*> CookieDatabaseBackingStore::getAllCookies()
{
    ASSERT(m_db.isOpen());

    Vector<Cookie*, 16> cookies;

    // Check that the database is correctly initialized.
    if (!m_db.tableExists(String("cookies")))
        return cookies;

    // Problem if the database is not connected
    SQLiteStatement select(m_db, "SELECT name, value, host, path, expiry, lastAccessed, isSecure FROM cookies;");

    if (select.prepare()) {
        LOG_ERROR("Cannot retrieved cookies in the database");
        return cookies;
    }

    while (select.step() == SQLResultRow) {
        // There is a row to fetch
        String name, value, domain, path;                                                                                                                                                         
        double expiry, lastAccessed;
        bool isSecure;

        name = select.getColumnText(0);
        value = select.getColumnText(1);
        domain = select.getColumnText(2);
        path = select.getColumnText(3);
        expiry = select.getColumnDouble(4);
        lastAccessed = select.getColumnDouble(5);
        isSecure = (select.getColumnInt(6) != 0);

        cookies.append(new Cookie(name, value, domain, path, expiry, lastAccessed, isSecure));
    }

    return cookies;
}

} // namespace WebCore

#endif // ENABLE(DATABASE)
