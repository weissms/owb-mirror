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
#include "CookieBackingStore.h"

#include "Cookie.h"
#include "CookieManager.h"
#include "SQLiteStatement.h"

namespace WebCore {

// This is chosen to match Mozilla's table name.
static String tableName = "cookies";

CookieBackingStore& cookieBackingStore()
{
    static CookieBackingStore backingStore;
    return backingStore;
}

CookieBackingStore::CookieBackingStore()
{
}

CookieBackingStore::~CookieBackingStore()
{
}

bool CookieBackingStore::tableExists()
{
    return m_db.isOpen() && m_db.tableExists(tableName);
}

void CookieBackingStore::open(const String& cookieJar)
{
    if (m_db.isOpen())
        close();

    if (!m_db.open(cookieJar)) {
        LOG_ERROR("Could not open the cookie database. No cookie will be stored!");
        return;
    }

    String createTableQuery("CREATE TABLE ");
    createTableQuery += tableName;
    // FIXME: Mozilla uses isHttpOnly which is not included here but should be to have a similar table. 
    createTableQuery +=" (name TEXT, value TEXT, host TEXT, path TEXT, expiry DOUBLE, lastAccessed DOUBLE, isSecure INTEGER);";

    if (!tableExists() &&  m_db.executeCommand(createTableQuery)) {
        LOG_ERROR("Could not create the table to store the cookies into. No cookie will be stored!");
        close();
    }
}

void CookieBackingStore::close()
{
    m_db.close();
}

void CookieBackingStore::insert(const Cookie* cookie)
{
    if (!tableExists())
        return;
 
    String insertQuery("INSERT INTO ");
    insertQuery += tableName;
    insertQuery += " (name, value, host, path, expiry, lastAccessed, isSecure) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7);";
    SQLiteStatement insertStatement(m_db, insertQuery);

    if (insertStatement.prepare()) {
        LOG_ERROR("Cannot save cookie");
        return;
    }

    // Binds all the values
    if (insertStatement.bindText(1, cookie->name()) || insertStatement.bindText(2, cookie->value())
        || insertStatement.bindText(3, cookie->domain()) || insertStatement.bindText(4, cookie->path())
        || insertStatement.bindDouble(5, cookie->expiry()) || insertStatement.bindDouble(6, cookie->lastAccessed())
        || insertStatement.bindInt64(7, cookie->isSecure())) {
        LOG_ERROR("Cannot save cookie");                                                                                                                                                          
        return;                                                                                                                                                                                   
    }

    if (!insertStatement.executeCommand()) {                                                                                                                                                               
        LOG_ERROR("Cannot save cookie");                                                                                                                                                          
        return;
    }
}

void CookieBackingStore::update(const Cookie* cookie)
{
    if (!tableExists())
        return;

    String updateQuery("UPDATE ");
    updateQuery += tableName;
    // The where statement is chosen to match CookieMap key.
    updateQuery += " SET name = ?1, value = ?2, host = ?3, path = ?4, expiry = ?5, lastAccessed = ?6, isSecure = ?7 where name = ?1 and host = ?3 and path = ?4;";
    SQLiteStatement updateStatement(m_db, updateQuery);

    if (updateStatement.prepare()) {
        LOG_ERROR("Cannot update cookie");
        return;
    }

    // Binds all the values
    if (updateStatement.bindText(1, cookie->name()) || updateStatement.bindText(2, cookie->value())
        || updateStatement.bindText(3, cookie->path()) || updateStatement.bindText(4, cookie->domain())
        || updateStatement.bindDouble(5, cookie->expiry()) || updateStatement.bindDouble(6, cookie->lastAccessed())
        || updateStatement.bindInt64(7, cookie->isSecure())) {
        LOG_ERROR("Cannot update cookie");
        return;
    }

    if (!updateStatement.executeCommand()) {
        LOG_ERROR("Cannot update cookie");
        return;
    }
 
}

void CookieBackingStore::remove(const Cookie* cookie)
{
    if (!tableExists())
        return;

    String deleteQuery("DELETE FROM ");
    deleteQuery += tableName;
    // The where statement is chosen to match CookieMap key.
    deleteQuery += " WHERE name=?1 and host=?2 and path=?3;";
    SQLiteStatement deleteStatement(m_db, deleteQuery);

    if (deleteStatement.prepare()) {
        LOG_ERROR("Cannot delete cookie");
        return;
    }

    // Binds all the values
    if (deleteStatement.bindText(1, cookie->name()) || deleteStatement.bindText(2, cookie->domain())
        || deleteStatement.bindText(3, cookie->path())) {
        LOG_ERROR("Cannot delete cookie");
        return;
    }

    if (!deleteStatement.executeCommand()) {
        LOG_ERROR("Cannot delete cookie from database");
        return;
    }
}

void CookieBackingStore::removeAll()
{
    if (!tableExists())
        return;

    String deleteQuery("DELETE * from ");
    deleteQuery += tableName;
    deleteQuery += ";";

    SQLiteStatement deleteStatement(m_db, deleteQuery);
    if (deleteStatement.prepare()) {
        LOG_ERROR("Could not prepare DELETE * statement");
        return;
    }

    if (!deleteStatement.executeCommand()) {
        LOG_ERROR("Cannot delete cookie from database");
        return;
    }
}

Vector<Cookie*> CookieBackingStore::getAllCookies()
{
    Vector<Cookie*, 8> cookies;

    // Check that the table exists to avoid doing an unnecessary request.
    if (!tableExists())
        return cookies;

    String selectQuery("SELECT name, value, host, path, expiry, lastAccessed, isSecure FROM ");
    selectQuery += tableName;

    SQLiteStatement selectStatement(m_db, selectQuery);

    if (selectStatement.prepare()) {
        LOG_ERROR("Cannot retrieved cookies from the database");
        return cookies;
    }

    while (selectStatement.step() == SQLResultRow) {
        // There is a row to fetch
        String name, value, domain, path;                                                                                                                                                         
        double expiry, lastAccessed;
        bool isSecure;

        name = selectStatement.getColumnText(0);
        value = selectStatement.getColumnText(1);
        domain = selectStatement.getColumnText(2);
        path = selectStatement.getColumnText(3);
        expiry = selectStatement.getColumnDouble(4);
        lastAccessed = selectStatement.getColumnDouble(5);
        isSecure = (selectStatement.getColumnInt(6) != 0);

        cookies.append(new Cookie(name, value, domain, path, expiry, lastAccessed, isSecure));
    }

    return cookies;
}

} // namespace WebCore
