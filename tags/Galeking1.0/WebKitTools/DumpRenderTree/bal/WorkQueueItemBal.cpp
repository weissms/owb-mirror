/*
 * Copyright (C) 2007 Alp Toker <alp@atoker.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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
    if (!strlen(targetString))
        targetFrame = getWebView()->mainFrame();
    else
        targetFrame = getWebView()->mainFrame()->findFrameNamed(targetString);
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
    getWebView()->mainFrame()->reload();
    return true;
}

bool ScriptItem::invoke() const
{
    char* scriptString = JSStringCopyUTF8CString(m_script.get());
    getWebView()->executeScript(scriptString);
    free(scriptString);
    return true;
}

bool BackForwardItem::invoke() const
{
    getWebView()->goBackOrForward(m_howFar);
    return true;
}
