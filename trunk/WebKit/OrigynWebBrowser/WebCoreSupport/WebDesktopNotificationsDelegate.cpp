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

#include "config.h"
#include <Document.h>
#include <KURL.h>
#include "WebDesktopNotificationsDelegate.h"
#include "WebSecurityOrigin.h"
#include "WebView.h"

#if ENABLE(NOTIFICATIONS)

using namespace WebCore;

WebDesktopNotificationsDelegate::WebDesktopNotificationsDelegate(WebView* webView)
    : m_webView(webView)
{
}

bool WebDesktopNotificationsDelegate::show(Notification* object)
{
    /*if (hasNotificationDelegate())
        notificationDelegate()->showDesktopNotification(NotificationCOMWrapper::create(object));*/
    return true;
}

void WebDesktopNotificationsDelegate::cancel(Notification* object)
{
    /*if (hasNotificationDelegate())
        notificationDelegate()->cancelDesktopNotification(NotificationCOMWrapper::create(object));*/
}

void WebDesktopNotificationsDelegate::notificationObjectDestroyed(Notification* object)
{
    /*if (hasNotificationDelegate())
        notificationDelegate()->notificationDestroyed(NotificationCOMWrapper::create(object));*/
}

void WebDesktopNotificationsDelegate::requestPermission(SecurityOrigin* origin, PassRefPtr<VoidCallback> callback)
{
    /*BString org(origin->toString());
    if (hasNotificationDelegate())
        notificationDelegate()->requestNotificationPermission(org);*/
}

NotificationPresenter::Permission WebDesktopNotificationsDelegate::checkPermission(const KURL& url, Document*)
{
    int out = 0;
    /*BString org(SecurityOrigin::create(url)->toString());
    if (hasNotificationDelegate())
        notificationDelegate()->checkNotificationPermission(org, &out);*/
    return (NotificationPresenter::Permission) out;
}

#endif  // ENABLE(NOTIFICATIONS)
