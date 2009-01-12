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

#include "config.h"

#include "CookieManager.h"

#include "Logging.h"
#include "CurrentTime.h"

#if ENABLE(DATABASE)
#include "SQLiteStatement.h"
#endif

namespace WKAL {

static CookieManager* sharedManager = 0;

CookieManager* CookieManager::getCookieManager()
{
    if (!sharedManager)
        sharedManager = new CookieManager();
    return sharedManager;
}

CookieManager::CookieManager()
    : m_count(0)
{
    // Keep our instance as we may call CookieManager::getCookieManager later.
    if (!sharedManager)
        sharedManager = this;

    m_cookieJarFileName = String("cookieCollection.db");

#if ENABLE(DATABASE)
    getDatabaseCookies();
#endif
}

CookieManager::~CookieManager()
{
    // Destroy all CookieMaps.
    for (HashMap<String, CookieMap*>::iterator it = m_managerMap.begin(); it != m_managerMap.end(); ++it)
        delete it->second;
}

bool CookieManager::is_cookie_sep(UChar c)
{
    return (c == '\r' || c =='\n');
}

void CookieManager::setCookies(const KURL& url, const KURL& policyURL, const String& value)
{
    unsigned cookieStart, cookieEnd = 0;

    double curTime = currentTime();

    // Iterate over the header to parse all the cookies.
    while (cookieEnd <= value.length()) {
        cookieStart = cookieEnd;

        // Find a cookie separator.
        while (cookieEnd <= value.length() && !is_cookie_sep(value[cookieEnd]))
            cookieEnd++;

        Cookie* cookie = Cookie::parse(url, value, cookieStart, cookieEnd - 1, curTime);

        if (!cookie) {
            LOG_ERROR("Invalid cookie string");
            continue;
        }

        // Security checks.
        if (!shouldReject(cookie, url))
            checkAndTreatCookie(cookie);
        else {
            LOG_ERROR("Cookie %s was rejected for security reason", value.ascii().data());
            delete cookie;
        }
    }
}

bool CookieManager::shouldReject(const Cookie* cookie, const KURL& url)
{
    // Check if path attribute is a prefix of the request URI.
    if (!cookie->path().endsWith(url.path()) == -1)
        return true;

    // Check if domain start with a dot and contains an embedded dot.
    if (cookie->domain()[0] != '.' || cookie->domain().find(".", 1) == -1 || cookie->domain().find(".", 1) == cookie->domain().length())
        return true;

    // The request host should domain match the Domain attribute.
    int diffPos = url.host().endsWith(cookie->domain());
    if (diffPos == -1)
        return true;
    // We should check for an embedded dot in the portion of string in the host not in the domain
    // but to match firefox behaviour we do not.

    return false;
}

String CookieManager::getCookie(const String& url)
{
    KURL kurl = KURL(url);
    return getCookie(kurl);
}

String CookieManager::getCookie(const KURL& url)
{
    bool isConnectionSecure = false;

    if (url.string().startsWith(String("https:", false)))
        isConnectionSecure = true;

    String res = String();
    double curTime = currentTime();

    for (HashMap<String, CookieMap*>::iterator it = m_managerMap.begin(); it != m_managerMap.end(); ++it) {

        // Handle sub-domain by only looking at the end of the host.
        if (it->first.endsWith(url.host()) || (it->first.startsWith(".", false) && ("." + url.host()).endsWith(it->first, false))) {
            // Get CookieMap to check for expired cookies.
            HashMap<String, Cookie*> curMap = it->second->getCookieMap();
            for (HashMap<String, Cookie*>::iterator itCookie = curMap.begin(); itCookie != curMap.end(); ++itCookie) {
                Cookie* curCookie = itCookie->second;
                // Check for non session cookie expired.
                if (!curCookie->isSession() && curCookie->expiry() < curTime) {
                    LOG(Network, "Cookie name: %s value: %s path: %s  expired", curCookie->name().ascii().data(), curCookie->value().ascii().data(), curCookie->path().ascii().data());
                    curMap.take(itCookie->first);
#if ENABLE(DATABASE)
                    removeCookieFromDatabase(curCookie);
#endif
                    removedCookie();
                    delete curCookie;
                } else {
                    // Update last accessed time.
                    it->second->updateTime(curCookie, curTime);

                    if (url.path().startsWith(curCookie->path(), false) && (isConnectionSecure || !curCookie->isSecure()))
                        res += curCookie->name() + "=" + curCookie->value() + ";";
                }
            }
        }
    }

    return res;
}

void CookieManager::removeAllCookies(bool shouldRemoveFromDatabase)
{
    for (HashMap<String, CookieMap*>::iterator it = m_managerMap.begin(); it != m_managerMap.end(); ++it) {
        HashMap<String, Cookie*> curMap = it->second->getCookieMap();
        for (HashMap<String, Cookie*>::iterator itCookie = curMap.begin(); itCookie != curMap.end(); ++itCookie) {
#if ENABLE(DATABASE)
            if (shouldRemoveFromDatabase && !itCookie->second->isSession())
                removeCookieFromDatabase(itCookie->second);
#endif
            delete itCookie->second;
        }
        delete it->second;
    }

    m_count = 0;
}

void CookieManager::setCookieJar(const char* fileName)
{
    m_cookieJarFileName = String(fileName);
}

void CookieManager::checkAndTreatCookie(Cookie* cookie)
{
    ASSERT(cookie->domain().length());

    CookieMap* curMap = m_managerMap.get(cookie->domain());

    // Check for cookie to remove in case it is not a session cookie and it has expired.
    if (!cookie->isSession() && cookie->expiry() < currentTime()) {
        // The cookie has expired so check we have a valid HashMap so try delete it.
        if (curMap) {
            // Check if we have a cookie to remove and update information accordingly.
            Cookie* prevCookie = curMap->takePrevious(cookie);
            if (prevCookie) {
#if ENABLE(DATABASE)
                removeCookieFromDatabase(cookie);
#endif
                removedCookie();
                delete cookie;
            }
        }
    } else {
        if (!curMap) {
            curMap = new CookieMap();
            m_managerMap.add(cookie->domain(), curMap);
            addCookieToMap(curMap, cookie);
        } else {
            // Check if there is a previous cookie.
            Cookie* prevCookie = curMap->takePrevious(cookie);

            if (prevCookie) {
                update(curMap, prevCookie, cookie);
                delete prevCookie;
            } else
                addCookieToMap(curMap, cookie);
        }
    }
}

void CookieManager::addCookieToMap(CookieMap* map, Cookie* cookie)
{
    // Check if we do not have reached the cookie's threshold.
    // FIXME : should split the case and remove one cookie among all the other if m_count >= max_count
    if (!map->canInsertCookie() || m_count >= max_count) {
        Cookie* rmCookie = map->removeOldestCookie();
#if ENABLE(DATABASE)
        removeCookieFromDatabase(rmCookie);
#endif
        removedCookie();
        delete rmCookie;
    }

    map->add(cookie);
#if ENABLE(DATABASE)
    // Only add non session cookie to the database.
    if (!cookie->isSession())
        insertCookieIntoDatabase(cookie);
#endif
    m_count++;
}

void CookieManager::update(CookieMap* map, Cookie* prevCookie, Cookie* newCookie)
{
    ASSERT(!map->takePrevious(prevCookie));
    map->add(newCookie);
#if ENABLE(DATABASE)
    updateDatabaseEntry(newCookie);
#endif
}

#if ENABLE(DATABASE)
void CookieManager::getDatabaseCookies()
{
    if (!m_db.isOpen() && !m_db.open(m_cookieJarFileName)) {
        LOG_ERROR("Cannot open the cookie database");
        return;
    }

    // Check that the database is correctly initialized.
    if (!m_db.tableExists(String("cookies"))) {
        m_db.close();
        return;
    }

    // Problem if the database is not connected
    SQLiteStatement select(m_db, "SELECT name, value, host, path, expiry, lastAccessed, isSecure FROM cookies;");

    if (select.prepare()) {
        LOG_ERROR("Cannot retrieved cookies in the database");
        return;
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

        Cookie* newCookie = new Cookie(name, value, domain, path, expiry, lastAccessed, isSecure);

        if (expiry > currentTime()) {

            CookieMap* curMap = m_managerMap.get(newCookie->domain());
            if (!curMap) {
                curMap = new CookieMap();
                m_managerMap.add(newCookie->domain(), curMap);
            }
            // Use the straightforward add
            curMap->add(newCookie);
            m_count++;
        } else {
#if ENABLE(DATABASE)
            removeCookieFromDatabase(newCookie);
#endif
            delete newCookie;
        }
    }
}

// We have to be careful as we do not know if the entry already existed, which could induce m_count leaks
void CookieManager::insertCookieIntoDatabase(const Cookie* cookie)
{
    if (!m_db.isOpen() && !m_db.open(m_cookieJarFileName)) {
        LOG_ERROR("Cannot open the cookie database");
        return;
    }

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

void CookieManager::updateDatabaseEntry(const Cookie* cookie)
{
    if (!m_db.isOpen() && !m_db.open(m_cookieJarFileName)) {
        LOG_ERROR("Cannot open the cookie database");
        return;
    }

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

void CookieManager::removeCookieFromDatabase(const Cookie* cookie)
{
    if (!m_db.isOpen() && !m_db.open(m_cookieJarFileName)) {
        LOG_ERROR("Cannot open the cookie database");
        return;
    }

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

#endif // ENABLE(DATABASE)

} // namespace WKAL
