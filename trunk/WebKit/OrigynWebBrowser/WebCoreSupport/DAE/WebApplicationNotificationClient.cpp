/*
 * Copyright (C) 2009 Pleyo.  All rights reserved.
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
#include "WebApplicationNotificationClient.h"

#include "Application.h"
#include "BALBase.h"
#include "CString.h" // For utf8()
#include "KURL.h"
#include "JSActionDelegate.h"
#include "Node.h"
#include "WebEventSender.h"
#include "WebEventSenderPrivate.h"
#include "WebFrame.h"
#include "WebFrameLoadDelegate.h"
#include "WebServiceManager.h"
#include "WebView.h"

using namespace WebCore;

WebApplicationNotificationClient::WebApplicationNotificationClient()
{
}

WebApplicationNotificationClient::~WebApplicationNotificationClient()
{
}

void WebApplicationNotificationClient::didCreateApplication(Application* application)
{
#if ENABLE(DAE_TUNER)
    WebServiceManager::serviceManager().setCurrentService(0);
#endif

    // Application created from JavaScript have no API component, so create it now.
    if (!application->page()) {
        WebView* webView = new WebView();
        application->setPage(core(webView));
        webView->setApplication(application);
        webView->setViewWindow(WebEventSender::getEventSender()->webWindow());
        // FIXME: We need to do this explicit conversion.
        BalRectangle rect = application->rect();
        webView->initWithFrame(rect, NULL, NULL);
        // The only application that can have a NULL parent is the root application.
        if (application->parent()) {
            WebView* parentView = kit(application->parent()->page());
            ASSERT(parentView);
            webView->setWebFrameLoadDelegate(parentView->webFrameLoadDelegate());
            webView->setJSActionDelegate(parentView->jsActionDelegate());
        }
        webView->setTransparent(true);

        webView->mainFrame()->loadURL(application->url().string().utf8().data());
    }

    // We should have page by now.
    ASSERT(application->page());

    // FIXME: Temporary - this should be handled by the Service Manager
    application->activate();
    if (application->broadcastRelation() == Application::BroadcastRelatedAutostart)
        application->show();
    
#if ENABLE(DAE_VIDEO)
    // Invoking the Service Manager is required to sartup listening for AIT notifications
    // FIXME: This should be moved to VideoBroadcastElement::setChannel() once the Service
    // Manager gets moved down to the BAL
    WebServiceManager::serviceManager();
#endif
}

void WebApplicationNotificationClient::didDeleteApplication(Application* application)
{
    WebView* webView = kit(application->page());
    ASSERT(webView);
    delete webView;
}

void WebApplicationNotificationClient::didShowApplication(Application* application)
{
    WebEventSender::getEventSender()->setPrimaryReceiver(application);
    application->dispatchApplicationEvent(eventNames().ApplicationShownEvent);
    WebEventSender::getEventSender()->showApplication(application);
}

void WebApplicationNotificationClient::didHideApplication(Application* application)
{
    application->dispatchApplicationEvent(eventNames().ApplicationHiddenEvent);
    WebEventSender::getEventSender()->hideApplication(application);
}

void WebApplicationNotificationClient::didActivateApplication(Application* application)
{
    WebEventSender::getEventSender()->priv()->addApplication(application);
}

void WebApplicationNotificationClient::didDeactivateApplication(Application* application)
{
    WebEventSender::getEventSender()->priv()->removeApplication(application);
}

void WebApplicationNotificationClient::willDestroyApplication(Application* application)
{
    WebEventSender::getEventSender()->priv()->scheduleApplicationForDestruction(application);
    application->hide();
    application->dispatchApplicationEvent(eventNames().ApplicationUnloadedEvent);
}

bool WebApplicationNotificationClient::isPrimaryReceiver(const Application* application) const
{
    return WebEventSender::getEventSender()->isPrimaryReceiver(application);
}

void WebApplicationNotificationClient::setPrimaryReceiver(Application* application)
{
    WebEventSender::getEventSender()->setPrimaryReceiver(application);
}

void WebApplicationNotificationClient::dispatchSystemEvent(Application* application, Event* event, Node* node)
{
    WebEventSender::getEventSender()->dispatchSystemEvent(application, event, node);
}
