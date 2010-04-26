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
#include "WebInspectorClient.h"

#include "CString.h"
#include "IntRect.h"
#include "NotImplemented.h"
#include "Page.h"
#include "PlatformString.h"
#include "WebFrameLoadDelegate.h"
#include "JSActionDelegate.h"
#include "WebPreferences.h"
#include "WebUtils.h"
#include "WebView.h"

#include <stdlib.h>
#include <string>

#if ENABLE(DAE)
#include "WebApplication.h"
#include "WebApplicationManager.h"
#endif

using namespace WebCore;
using namespace std;

WebInspectorClient::WebInspectorClient(WebView *view)
    :m_webView(view)
{
}

WebInspectorClient::~WebInspectorClient()
{
}

void WebInspectorClient::inspectorDestroyed()
{
    delete this;
}

void WebInspectorClient::openInspectorFrontend(InspectorController* inspectorController)
{
#if ENABLE(DAE)
    if (m_application)
        return;
#endif

    const char* inspectorURL = getenv("INSPECTOR_URL");
    if (!inspectorURL)
        return;

    string url = inspectorURL;
    if (isAbsolute(inspectorURL)) {
        url = "file://";
        url += inspectorURL;
    }
#if ENABLE(DAE)
    IntRect frameRect(m_webView->frameRect());
    frameRect.setY((frameRect.height()*2)/3);
    frameRect.setHeight(frameRect.height()/3);
    m_application = webAppMgr().createApplication(0, frameRect, url.c_str(), 0, 0);

    WebView* view = m_application->webView();
    if (!view)
        return;

    view->preferences()->setDeveloperExtrasEnabled(false); 
    view->setWebFrameLoadDelegate(0);
    view->setJSActionDelegate(0);
    
    Page* page = core(view);
    page->inspectorController()->setInspectorFrontendClient(new WebInspectorFrontendClient(m_webView, view, this));
#else
    notImplemented();
#endif
}

void WebInspectorClient::highlight(Node*)
{
    notImplemented();
}

void WebInspectorClient::hideHighlight()
{
    notImplemented();
}

void WebInspectorClient::updateHighlight()
{
    notImplemented();
}

void WebInspectorClient::populateSetting(const String& key, String* value)
{
    loadSettings();
    if (m_settings->contains(key))
        *value = m_settings->get(key);
}

void WebInspectorClient::storeSetting(const String& key, const String& value)
{
    loadSettings();
    m_settings->set(key, value);
    saveSettings();
}

void WebInspectorClient::loadSettings()
{
    if (m_settings)
        return;

    m_settings.set(new SettingsMap);
    String data = m_webView->inspectorSettings();
    if (data.isEmpty())
        return;

    Vector<String> entries;
    data.split("\n", entries);
    for (Vector<String>::iterator it = entries.begin(); it != entries.end(); ++it) {
        Vector<String> tokens;
        it->split(":", tokens);
        if (tokens.size() < 3)
            continue;

        String name = decodeURLEscapeSequences(tokens[0]);
        String type = tokens[1];
        String value = tokens[2];
        for (size_t i = 3; i < tokens.size(); ++i)
            value += ":" + tokens[i];

        if (type == "string")
            value = decodeURLEscapeSequences(value);

        m_settings->set(name, value);
    }
}

void WebInspectorClient::saveSettings()
{
    String data;
    for (SettingsMap::iterator it = m_settings->begin(); it != m_settings->end(); ++it) {
        String name = encodeWithURLEscapeSequences(it->first);
        String value = it->second;
        String entry = String::format(
            "%s:string:%s",
            name.utf8().data(),
            encodeWithURLEscapeSequences(value).utf8().data());
        data.append(entry);
        data.append("\n");
    }
    m_webView->setInspectorSettings(data.utf8().data());
}

WebInspectorFrontendClient::WebInspectorFrontendClient(WebView* inspectedWebView, WebView* frontendWebView, WebInspectorClient* inspectorClient)
    : InspectorFrontendClientLocal(inspectedWebView->page()->inspectorController(),  core(frontendWebView))
    , m_inspectedWebView(inspectedWebView)
    , m_inspectorClient(inspectorClient)
    , m_frontendWebView(frontendWebView)
    , m_attached(false)
    , m_destroyingInspectorView(false)
#if ENABLE(DAE)
    , m_application(webAppMgr().application(frontendWebView))
#endif
{
}

WebInspectorFrontendClient::~WebInspectorFrontendClient()
{
    destroyInspectorView();
}

void WebInspectorFrontendClient::frontendLoaded()
{
    InspectorFrontendClientLocal::frontendLoaded();

    setAttachedWindow(m_attached);
}

String WebInspectorFrontendClient::localizedStringsURL()
{
    return String();
}

String WebInspectorFrontendClient::hiddenPanels()
{
    // FIXME: implement this
    return String();
}

void WebInspectorFrontendClient::bringToFront()
{
    // If no preference is set - default to an attached window. This is important for inspector LayoutTests.
    String shouldAttach = m_inspectedWebView->page()->inspectorController()->setting(InspectorController::inspectorStartsAttachedSettingName());

    /*if (m_inspectedWebView->page())
        m_inspectedWebView->page()->inspectorController()->setWindowVisible(true, true);*/
#if ENABLE(DAE)
    if (m_application)
        m_application->show();
#endif
}

void WebInspectorFrontendClient::closeWindow()
{
    /*if (m_webView->page())
        m_webView->page()->inspectorController()->setWindowVisible(false);*/
#if ENABLE(DAE)
    if (m_application)
        m_application->hide();
#endif
}

void WebInspectorFrontendClient::attachWindow()
{
    if (m_attached)
        return;

    m_inspectedWebView->page()->inspectorController()->setSetting(InspectorController::inspectorStartsAttachedSettingName(), "true");
}

void WebInspectorFrontendClient::detachWindow()
{
    if (!m_attached)
        return;

    m_inspectedWebView->page()->inspectorController()->setSetting(InspectorController::inspectorStartsAttachedSettingName(), "false");
}

void WebInspectorFrontendClient::setAttachedWindowHeight(unsigned height)
{
    notImplemented();
}

void WebInspectorFrontendClient::inspectedURLChanged(const String& newURL)
{
    notImplemented();
}

void WebInspectorFrontendClient::destroyInspectorView()
{
    if (m_destroyingInspectorView)
        return;
    m_destroyingInspectorView = true;

    m_inspectedWebView->page()->inspectorController()->disconnectFrontend();

    //m_inspectorClient->frontendClosing();
}
