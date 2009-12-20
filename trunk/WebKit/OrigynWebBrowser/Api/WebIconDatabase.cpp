/*
 * Copyright (C) 2008 Pleyo.  All rights reserved.
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
#include "Platform.h"
#if ENABLE(ICONDATABASE)
#include "config.h"
#include "WebIconDatabase.h"

#include "WebPreferences.h"

#include <BitmapImage.h>
#include <CString.h>
#include <IconDatabaseClient.h>
#include <PlatformString.h>
#include <FileSystem.h>
#include <IconDatabase.h>
#include <Image.h>
#include <PlatformString.h>
#include <wtf/MainThread.h>
#include "ObserverServiceData.h"

using namespace WebCore;
using namespace WTF;
using namespace std;

class WebIconDatabaseClient : public IconDatabaseClient {
public:
    WebIconDatabaseClient(WebIconDatabase* icon)
    : m_webIconDatabase(icon)
    {}

    virtual ~WebIconDatabaseClient() { m_webIconDatabase = 0; }
    virtual void dispatchDidRemoveAllIcons() { m_webIconDatabase->dispatchDidRemoveAllIcons(); }
    virtual void dispatchDidAddIconForPageURL(const String& pageURL) { m_webIconDatabase->dispatchDidAddIconForPageURL(strdup(pageURL.utf8().data())); }

    Mutex m_notificationMutex;
private:
    WebIconDatabase* m_webIconDatabase;
};


WebIconDatabase* WebIconDatabase::m_sharedWebIconDatabase = 0;

WebIconDatabase::WebIconDatabase()
: m_deliveryRequested(false)
, m_webIconDatabaseClient(new WebIconDatabaseClient(this))
{
}

WebIconDatabase::~WebIconDatabase()
{
}

void WebIconDatabase::init()
{
    WebPreferences* standardPrefs = WebPreferences::sharedStandardPreferences();
    bool enabled = standardPrefs->iconDatabaseEnabled();
//     if (!enabled) {
//         LOG_ERROR("Unable to get icon database enabled preference");
//     }
    iconDatabase()->setEnabled(!!enabled);

    startUpIconDatabase();
}

void WebIconDatabase::startUpIconDatabase()
{
    WebPreferences* standardPrefs = WebPreferences::sharedStandardPreferences();
    iconDatabase()->setClient(m_webIconDatabaseClient);

    String databasePath = standardPrefs->iconDatabaseLocation();

    if (databasePath.isEmpty())
        databasePath = homeDirectoryPath();

    bool ret = iconDatabase()->open(databasePath);
    if (!ret)
        LOG_ERROR("Failed to open icon database path");
}

void WebIconDatabase::shutDownIconDatabase()
{
}

WebIconDatabase* WebIconDatabase::createInstance()
{
    WebIconDatabase* instance = new WebIconDatabase();
    return instance;
}

WebIconDatabase* WebIconDatabase::sharedWebIconDatabase()
{
    if (m_sharedWebIconDatabase) {
        return m_sharedWebIconDatabase;
    }
    m_sharedWebIconDatabase = createInstance();
    m_sharedWebIconDatabase->init();
    return m_sharedWebIconDatabase;
}

WebIconDatabase *WebIconDatabase::sharedIconDatabase()
{
    return sharedWebIconDatabase();
}

BalSurface* WebIconDatabase::iconForURL(const char* url, BalPoint size, bool /*cache*/)
{
    IntPoint p(size);
    IntSize intSize(p.x(), p.y());

    Image* icon = iconDatabase()->iconForPageURL(url, intSize);
    if (!icon)
        return 0;
    BalSurface* surf = icon->nativeImageForCurrentFrame();
    return surf;
}

void WebIconDatabase::retainIconForURL(const char* url)
{
    iconDatabase()->retainIconForPageURL(url);
}

void WebIconDatabase::releaseIconForURL(const char* url)
{
    iconDatabase()->releaseIconForPageURL(url);
}

void WebIconDatabase::removeAllIcons()
{
    iconDatabase()->removeAllIcons();
}

void WebIconDatabase::delayDatabaseCleanup()
{
    IconDatabase::delayDatabaseCleanup();
}

void WebIconDatabase::allowDatabaseCleanup()
{
    IconDatabase::allowDatabaseCleanup();
}

const char* WebIconDatabase::iconURLForURL(const char* url)
{
    return strdup(iconDatabase()->iconURLForPageURL(url).utf8().data());
}


BalSurface* WebIconDatabase::getOrCreateSharedBitmap(BalPoint s)
{
    BalSurface* result = m_sharedIconMap[s];
    if (result)
        return result;
    RefPtr<BitmapImage> image = BitmapImage::create();
    IntPoint p(s);
    IntSize size(p.x(), p.y());
    image->setContainerSize(size);
    result = image->nativeImageForCurrentFrame();
    m_sharedIconMap[s] = result;
    return result;
}

BalSurface* WebIconDatabase::getOrCreateDefaultIconBitmap(BalPoint s)
{
    BalSurface* result = m_defaultIconMap[s];
    if (result)
        return result;

    RefPtr<BitmapImage> image = BitmapImage::create();
    IntPoint p(s);
    IntSize size(p.x(), p.y());
    image->setContainerSize(size);

    result = image->nativeImageForCurrentFrame();
    m_defaultIconMap[s] = result;
    return result;
}

bool WebIconDatabase::isEnabled()
{
    return iconDatabase()->isEnabled();
}

void WebIconDatabase::setEnabled(bool flag)
{
    bool currentlyEnabled = isEnabled();
    if (currentlyEnabled && !flag) {
        iconDatabase()->setEnabled(false);
        shutDownIconDatabase();
    } else if (!currentlyEnabled && flag) {
        iconDatabase()->setEnabled(true);
        startUpIconDatabase();
    }
}

void WebIconDatabase::dispatchDidRemoveAllIcons()
{
    // Queueing the empty string is a special way of saying "this queued notification is the didRemoveAllIcons notification"
    MutexLocker locker(m_webIconDatabaseClient->m_notificationMutex);
    m_notificationQueue.push_back("");
    scheduleNotificationDelivery();
}

void WebIconDatabase::dispatchDidAddIconForPageURL(const char* pageURL)
{   
    MutexLocker locker(m_webIconDatabaseClient->m_notificationMutex);
    m_notificationQueue.push_back(pageURL);
    scheduleNotificationDelivery();
}

void WebIconDatabase::scheduleNotificationDelivery()
{
    // Caller of this method must hold the m_notificationQueue lock
    ASSERT(!m_webIconDatabaseClient->m_notificationMutex.tryLock());

    if (!m_deliveryRequested) {
        m_deliveryRequested = true;
        callOnMainThread(deliverNotifications, 0);
    }
}

const char* WebIconDatabase::iconDatabaseDidAddIconNotification()
{
    static const char* didAddIconName = WebIconDatabaseDidAddIconNotification;
    return didAddIconName;
}

const char* WebIconDatabase::iconDatabaseNotificationUserInfoURLKey()
{
    static const char* iconUserInfoURLKey = WebIconNotificationUserInfoURLKey;
    return iconUserInfoURLKey;
}

const char* WebIconDatabase::iconDatabaseDidRemoveAllIconsNotification()
{
    static const char* didRemoveAllIconsName = WebIconDatabaseDidRemoveAllIconsNotification;
    return didRemoveAllIconsName;
}

static void postDidRemoveAllIconsNotification(WebIconDatabase* iconDB)
{
    WebCore::ObserverServiceData::createObserverService()->notifyObserver(WebIconDatabase::iconDatabaseDidRemoveAllIconsNotification(), "", iconDB);
}

static void postDidAddIconNotification(const char* pageURL, WebIconDatabase* iconDB)
{
    WebCore::ObserverServiceData::createObserverService()->notifyObserver(WebIconDatabase::iconDatabaseDidAddIconNotification(), pageURL, iconDB);
}

void WebIconDatabase::deliverNotifications(void*)
{
    ASSERT(m_sharedWebIconDatabase);
    if (!m_sharedWebIconDatabase)
        return;

    ASSERT(m_sharedWebIconDatabase->m_deliveryRequested);

    vector<const char*> queue;
    {
        MutexLocker locker(m_sharedWebIconDatabase->m_webIconDatabaseClient->m_notificationMutex);
        queue.swap(m_sharedWebIconDatabase->m_notificationQueue);
        m_sharedWebIconDatabase->m_deliveryRequested = false;
    }

    for (unsigned i = 0; i < queue.size(); ++i) {
        if (!queue[i])
            postDidRemoveAllIconsNotification(m_sharedWebIconDatabase);
        else
            postDidAddIconNotification(queue[i], m_sharedWebIconDatabase);
    }
}

bool operator<(BalPoint p1, BalPoint p2)
{
    IntPoint point1(p1);
    IntPoint point2(p2);
    return point1 < point2;
}

#endif
