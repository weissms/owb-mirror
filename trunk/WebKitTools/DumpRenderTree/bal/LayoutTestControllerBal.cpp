/*
 * Copyright (C) 2007 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2008 Nuanti Ltd.
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
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//#include "config.h"
#include "LayoutTestController.h"

#include "DumpRenderTree.h"
#include "WorkQueue.h"
#include "WorkQueueItem.h"
#include <JavaScriptCore/JSRetainPtr.h>
#include <JavaScriptCore/JSStringRef.h>

#include <stdio.h>
#include <WebKit.h>

LayoutTestController::~LayoutTestController()
{
    // FIXME: implement
}

void LayoutTestController::addDisallowedURL(JSStringRef url)
{
    // FIXME: implement
}

void LayoutTestController::clearBackForwardList()
{
    WebView* webView = getWebView();

    WebBackForwardList* backForwardList = webView->backForwardList();
    if (!backForwardList)
        return;

    WebHistoryItem* item = backForwardList->currentItem();
    if (item)
        return;

    // We clear the history by setting the back/forward list's capacity to 0
    // then restoring it back and adding back the current item.
    int capacity = backForwardList->capacity();

    backForwardList->setCapacity(0);
    backForwardList->setCapacity(capacity);
    backForwardList->addItem(item);
    backForwardList->goToItem(item);
}

JSStringRef LayoutTestController::copyDecodedHostName(JSStringRef name)
{
    // FIXME: implement
    return 0;
}

JSStringRef LayoutTestController::copyEncodedHostName(JSStringRef name)
{
    // FIXME: implement
    return 0;
}

void LayoutTestController::display()
{
    displayWebView();
}

void LayoutTestController::dispatchPendingLoadRequests()
{
    // FIXME: Implement for testing fix for 6727495
}

void LayoutTestController::keepWebHistory()
{
    // FIXME: implement
}

size_t LayoutTestController::webHistoryItemCount()
{
    // FIXME: implement
    return 0;
}

void LayoutTestController::notifyDone()
{
    if (m_waitToDump && !topLoadingFrame && !WorkQueue::shared()->count())
        dump();
    
    m_waitToDump = false;
}

JSStringRef LayoutTestController::pathToLocalResource(JSContextRef context, JSStringRef url)
{
    // Function introduced in r28690. This may need special-casing on Windows.
    return JSStringRetain(url); // Do nothing on Unix.
}

void LayoutTestController::queueLoad(JSStringRef url, JSStringRef target)
{
    // FIXME: We need to resolve relative URLs here
    WorkQueue::shared()->queue(new LoadItem(url, target));
}

void LayoutTestController::setAcceptsEditing(bool acceptsEditing)
{
    getWebView()->mainFrame()->setEditable(acceptsEditing);
}

void LayoutTestController::setCustomPolicyDelegate(bool setDelegate, bool permissive)
{
    // FIXME: implement
}

void LayoutTestController::waitForPolicyDelegate()
{
    setWaitToDump(true);
    //[policyDelegate setControllerToNotifyDone:this];
    //[[mainFrame webView] setPolicyDelegate:policyDelegate];
    // FIXME: implement
}

void LayoutTestController::setMainFrameIsFirstResponder(bool flag)
{
    // FIXME: implement
}

void LayoutTestController::setTabKeyCyclesThroughElements(bool cycles)
{
    // FIXME: implement
}



















void LayoutTestController::setUseDashboardCompatibilityMode(bool flag)
{
    // FIXME: implement
}

static char* userStyleSheet = NULL;
static bool userStyleSheetEnabled = true;

void LayoutTestController::setUserStyleSheetEnabled(bool flag)
{
    userStyleSheetEnabled = flag;

    if (flag && userStyleSheet)
        getWebView()->setCustomUserAgent(userStyleSheet);
    else
        getWebView()->setCustomUserAgent("");
}

void LayoutTestController::setUserStyleSheetLocation(JSStringRef path)
{
    free(userStyleSheet);
    userStyleSheet = JSStringCopyUTF8CString(path);
    if (userStyleSheetEnabled)
        setUserStyleSheetEnabled(true);
}

void LayoutTestController::setWindowIsKey(bool windowIsKey)
{
    // FIXME: implement
}

void LayoutTestController::setSmartInsertDeleteEnabled(bool flag)
{
    // FIXME: implement
}

void LayoutTestController::setWaitToDump(bool waitUntilDone)
{
    static const int timeoutSeconds = 10;

    m_waitToDump = waitUntilDone;
    if (m_waitToDump && !waitToDumpWatchdog)
        addTimetoDump(timeoutSeconds);
}

int LayoutTestController::windowCount()
{
    // FIXME: implement
    return 1;
}

void LayoutTestController::setPrivateBrowsingEnabled(bool privateBrowsingEnabled)
{
    // FIXME: implement
}

void LayoutTestController::setXSSAuditorEnabled(bool enabled)
{
    // FIXME: implement
}

void LayoutTestController::setAuthorAndUserStylesEnabled(bool flag)
{
    // FIXME: implement
}

void LayoutTestController::setIconDatabaseEnabled(bool flag)
{
    // FIXME: implement
}

void LayoutTestController::setJavaScriptProfilingEnabled(bool flag)
{
    // FIXME: implement
}

void LayoutTestController::setSelectTrailingWhitespaceEnabled(bool flag)
{
    // FIXME: implement
}

void LayoutTestController::setPopupBlockingEnabled(bool popupBlockingEnabled)
{
    // FIXME: implement
}

bool LayoutTestController::elementDoesAutoCompleteForElementWithId(JSStringRef id) 
{
    // FIXME: implement
    return false;
}

void LayoutTestController::execCommand(JSStringRef name, JSStringRef value)
{
    // FIXME: implement
}

void LayoutTestController::setCacheModel(int)
{
    // FIXME: implement
}

bool LayoutTestController::isCommandEnabled(JSStringRef /*name*/)
{
    // FIXME: implement
    return false;
}

void LayoutTestController::setPersistentUserStyleSheetLocation(JSStringRef jsURL)
{
    // FIXME: implement
}

void LayoutTestController::clearPersistentUserStyleSheet()
{
    // FIXME: implement
}

void LayoutTestController::clearAllDatabases()
{
    // FIXME: implement
}
 
void LayoutTestController::setDatabaseQuota(unsigned long long quota)
{    
    // FIXME: implement
}

bool LayoutTestController::pauseAnimationAtTimeOnElementWithId(JSStringRef animationName, double time, JSStringRef elementId)
{    
    char* name = JSStringCopyUTF8CString(animationName);
    char* element = JSStringCopyUTF8CString(elementId);
    return getWebView()->mainFrame()->pauseAnimation(name, time, element);
}

bool LayoutTestController::pauseTransitionAtTimeOnElementWithId(JSStringRef propertyName, double time, JSStringRef elementId)
{    
    char* name = JSStringCopyUTF8CString(propertyName);
    char* element = JSStringCopyUTF8CString(elementId);
    return getWebView()->mainFrame()->pauseTransition(name, time, element);
}

unsigned LayoutTestController::numberOfActiveAnimations() const
{
    return getWebView()->mainFrame()->numberOfActiveAnimations();
}
/*
void LayoutTestController::setAppCacheMaximumSize(unsigned long long size)
{
    printf("ERROR: LayoutTestController::setAppCacheMaximumSize() not implemented\n");
}
*/