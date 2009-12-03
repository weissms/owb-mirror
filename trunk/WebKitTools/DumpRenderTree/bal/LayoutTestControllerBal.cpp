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
#include "LayoutTestController.h"

#include "DumpRenderTree.h"
#include "EditingDelegate.h"
#include "WorkQueue.h"
#include "WorkQueueItem.h"
#include "PolicyDelegate.h"
#include <JavaScriptCore/JSRetainPtr.h>
#include <JavaScriptCore/JSStringRef.h>

#include <stdio.h>
#include <string.h>
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
    char* relativeURL = JSStringCopyUTF8CString(url);
    if (!relativeURL)
        return;
    if (strncmp("http://", relativeURL, 7) != 0 
     && strncmp("https://", relativeURL, 8) != 0
     && strncmp("data:", relativeURL, 5) != 0 
     && strncmp("file://", relativeURL, 7) != 0) {
        WebView* webView = getWebView();
        string mainUrl = webView->mainFrameURL();
        size_t pos = mainUrl.find_last_of("/");
        string newUrl = mainUrl.substr(0, pos + 1);
        newUrl += relativeURL;
        size_t pos2 = newUrl.find_last_of("?");
        if (pos2 == newUrl.size() - 1)
            newUrl = newUrl.substr(0, pos2);
        
        WorkQueue::shared()->queue(new LoadItem(JSStringCreateWithUTF8CString(newUrl.c_str()), target));
    } else 
        WorkQueue::shared()->queue(new LoadItem(url, target));
}

void LayoutTestController::setAcceptsEditing(bool acceptsEditing)
{
    WebView* webView = getWebView();
    webView->mainFrame()->setEditable(acceptsEditing);
    EditingDelegate* editing = static_cast<EditingDelegate*>(webView->webEditingDelegate().get());
    editing->setAcceptsEditing(acceptsEditing);
}

void LayoutTestController::setCustomPolicyDelegate(bool setDelegate, bool permissive)
{
    WebView* webView = getWebView();
    if (!webView)
        return;

    if (setDelegate) {
        policyDelegate->setPermissive(permissive);
        webView->setPolicyDelegate(policyDelegate);
    } else
        webView->setPolicyDelegate(0);
}

void LayoutTestController::waitForPolicyDelegate()
{
    setWaitToDump(true);
    WebView* webView = getWebView();
    if (!webView)
        return;

    policyDelegate->setControllerToNotifyDone(this);
    webView->setPolicyDelegate(policyDelegate);
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

    WebView* webView = getWebView();
    if (flag && userStyleSheet)
        webView->setCustomUserAgent(userStyleSheet);
    else
        webView->setCustomUserAgent("");
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
    WebView* webView = getWebView();
    WebInspector *inspector = webView->inspector();
    if (inspector)
        inspector->setJavaScriptProfilingEnabled(flag);
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

JSRetainPtr<JSStringRef> LayoutTestController::counterValueForElementById(JSStringRef id)
{
    // FIXME: implement
    return 0;
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

void LayoutTestController::overridePreference(JSStringRef key, JSStringRef value)
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
    WebView* webView = getWebView();
    return webView->mainFrame()->pauseAnimation(name, time, element);
}

bool LayoutTestController::pauseTransitionAtTimeOnElementWithId(JSStringRef propertyName, double time, JSStringRef elementId)
{    
    char* name = JSStringCopyUTF8CString(propertyName);
    char* element = JSStringCopyUTF8CString(elementId);
    WebView* webView = getWebView();
    return webView->mainFrame()->pauseTransition(name, time, element);
}

unsigned LayoutTestController::numberOfActiveAnimations() const
{
    WebView* webView = getWebView();
    return webView->mainFrame()->numberOfActiveAnimations();
}

void LayoutTestController::setAppCacheMaximumSize(unsigned long long size)
{
//     printf("ERROR: LayoutTestController::setAppCacheMaximumSize() not implemented\n");
}

void LayoutTestController::disableImageLoading()
{
  // FIXME: implement
}

unsigned LayoutTestController::workerThreadCount() const
{
  // FIXME: implement
    return 0;
}

void LayoutTestController::whiteListAccessFromOrigin(JSStringRef sourceOrigin, JSStringRef destinationProtocol, JSStringRef destinationHost, bool allowDestinationSubdomains)
{
    // FIXME: Implement.
}

void LayoutTestController::setAlwaysAcceptCookies(bool alwaysAcceptCookies)
{
    // FIXME: Implement this (and restore the default value before running each test in DumpRenderTree.cpp).
}

void LayoutTestController::addUserScript(JSStringRef source, bool runAtStart)
{
    // FIXME: Implement.
}

void LayoutTestController::addUserStyleSheet(JSStringRef source)
{
    // FIXME: Implement.
}

void LayoutTestController::setMockGeolocationPosition(double latitude, double longitude, double accuracy)
{
    // FIXME: Implement.
}

void LayoutTestController::setMockGeolocationError(int code, JSStringRef message)
{
    // FIXME: Implement.
}

void LayoutTestController::showWebInspector()
{
    //getWebView()->inspector()->show()
}

void LayoutTestController::closeWebInspector()
{
    //getWebView()->inspector()->close()
}

void LayoutTestController::evaluateInWebInspector(long callId, JSStringRef script)
{
    //getWebView()->inspector()->evaluateInFrontend(callId, char*);
}

void LayoutTestController::removeAllVisitedLinks()
{
    // FIXME: Implement this.
}

void LayoutTestController::evaluateScriptInIsolatedWorld(unsigned worldID, JSObjectRef globalObject, JSStringRef script)
{
    // FIXME: Implement this.
}

void LayoutTestController::setTimelineProfilingEnabled(bool flag)
{
}

void LayoutTestController::setAllowUniversalAccessFromFileURLs(bool flag)
{
    // FIXME: implement
}
