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

//#include "config.h"
#include "WorkQueueItem.h"

#include "DumpRenderTree.h"

#include <JavaScriptCore/JSStringRef.h>
#include <WebKit.h>
#include <string.h>

// Returns a newly allocated UTF-8 character buffer which must be freed with g_free()
char* JSStringCopyUTF8CString(JSStringRef jsString)
{
    size_t dataSize = JSStringGetMaximumUTF8CStringSize(jsString);
    char* utf8 = (char*)malloc(dataSize);
    JSStringGetUTF8CString(jsString, utf8, dataSize);

    return utf8;
}

bool LoadItem::invoke() const
{
    char* targetString = JSStringCopyUTF8CString(m_target.get());

    WebFrame* targetFrame;
    WebView* webView = getWebView();
    if (!strlen(targetString))
        targetFrame = webView->mainFrame();
    else
        targetFrame = webView->mainFrame()->findFrameNamed(targetString);
    free(targetString);

    char* urlString = JSStringCopyUTF8CString(m_url.get());
    targetFrame->loadURL(urlString);
//    WebKitNetworkRequest* request = webkit_network_request_new(urlString);
    free(urlString);
//    webkit_web_frame_load_request(targetFrame, request);
//    g_object_unref(request);
    return true;
}

bool ReloadItem::invoke() const
{
    WebView* webView = getWebView();
    webView->mainFrame()->reload();
    return true;
}

bool ScriptItem::invoke() const
{
    char* scriptString = JSStringCopyUTF8CString(m_script.get());
    WebView* webView = getWebView();
    webView->executeScript(scriptString);
    free(scriptString);
    return true;
}

bool BackForwardItem::invoke() const
{
    WebView* webView = getWebView();
    webView->goBackOrForward(m_howFar);
    return true;
}
