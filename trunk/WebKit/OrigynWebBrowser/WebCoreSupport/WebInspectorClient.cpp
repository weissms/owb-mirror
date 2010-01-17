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
#include "NotImplemented.h"
#include "Page.h"
#include "PlatformString.h"
#include "WebFrameLoadDelegate.h"
#include "JSActionDelegate.h"
#include "WebPreferences.h"
#include "WebView.h"

#include <stdlib.h>

#if ENABLE(DAE)
#include "Application.h"
#endif

using namespace WebCore;

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

Page* WebInspectorClient::createPage()
{
    
    const char* inspectorURL = getenv("INSPECTOR_URL");
    if (!inspectorURL)
        return 0;
#if ENABLE(DAE)
    IntRect frameRect(m_webView->frameRect());
    frameRect.setY((frameRect.height()*2)/3);
    frameRect.setHeight(frameRect.height()/3);
    RefPtr<Application> app = m_webView->application()->createApplication(inspectorURL, true, frameRect);
    if (!app->page())
        return 0;

    WebView* view = kit(app->page());
    if (!view)
        return 0;

    view->preferences()->setDeveloperExtrasEnabled(false); 
    view->setWebFrameLoadDelegate(0);
    view->setJSActionDelegate(0);

    return app->page();
#else
    notImplemented();
    return 0;
#endif
}

String WebInspectorClient::localizedStringsURL()
{
    notImplemented();
    return String();
}

String WebInspectorClient::hiddenPanels()
{
    // FIXME: implement this
    return String();
}

void WebInspectorClient::showWindow()
{
    if (m_webView->page())
        m_webView->page()->inspectorController()->setWindowVisible(true, true);
}

void WebInspectorClient::closeWindow()
{
    if (m_webView->page())
        m_webView->page()->inspectorController()->setWindowVisible(false);
}

void WebInspectorClient::attachWindow()
{
    notImplemented();
}

void WebInspectorClient::detachWindow()
{
    notImplemented();
}

void WebInspectorClient::setAttachedWindowHeight(unsigned height)
{
    notImplemented();
}

void WebInspectorClient::highlight(Node* node)
{
    notImplemented();
}

void WebInspectorClient::hideHighlight()
{
    notImplemented();
}

void WebInspectorClient::inspectedURLChanged(const String& url)
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

void WebInspectorClient::inspectorWindowObjectCleared()
{
    notImplemented();
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

