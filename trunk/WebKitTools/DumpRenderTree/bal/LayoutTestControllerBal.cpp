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
    char* URL = JSStringCopyUTF8CString(url);
    disallowedURLs.push_back(URL);
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
    char* decode = JSStringCopyUTF8CString(name);
    return JSStringCreateWithUTF8CString(getWebView()->decodeHostName(decode));
}

JSStringRef LayoutTestController::copyEncodedHostName(JSStringRef name)
{
    char* encode = JSStringCopyUTF8CString(name);
    return JSStringCreateWithUTF8CString(getWebView()->encodeHostName(encode));
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

int LayoutTestController::pageNumberForElementById(JSStringRef, float, float)
{
    // FIXME: implement
    return -1;
}

int LayoutTestController::numberOfPages(float, float)
{
    // FIXME: implement
    return -1;
}

size_t LayoutTestController::webHistoryItemCount()
{
    WebView* webView = getWebView();

    WebBackForwardList* backForwardList = webView->backForwardList();
    if (!backForwardList)
        return 0;

    int forwardListCount = backForwardList->forwardListCount();
    int backListCount = backForwardList->backListCount();
    return forwardListCount + backListCount;
}

void LayoutTestController::notifyDone()
{
    if (m_waitToDump && !topLoadingFrame && !WorkQueue::shared()->count())
        dump();
    
    m_waitToDump = false;
}

JSStringRef LayoutTestController::pathToLocalResource(JSContextRef context, JSStringRef url)
{
    // FIX the path because url begin by /tmp/LayoutTests and for us LayoutTest can be anywhere on disk.
    char* URL = JSStringCopyUTF8CString(url);
    string resourceURL = URL;
    size_t pos = resourceURL.find("LayoutTests");
    if (pos == string::npos)
        return JSStringRetain(url);

    resourceURL = resourceURL.substr(pos);

    WebView* webView = getWebView();
    string mainUrl = webView->mainFrameURL();
    pos = mainUrl.find("LayoutTests");
    string base = mainUrl.substr(0, pos);

    resourceURL = base + resourceURL;
    return JSStringCreateWithUTF8CString(resourceURL.c_str());
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
    getWebView()->setTabKeyCyclesThroughElements(cycles);
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
    WebView* webView = getWebView();
    if (windowIsKey)
        webView->setFocus();
    else
        webView->clearFocus();
}

void LayoutTestController::setSmartInsertDeleteEnabled(bool flag)
{
    WebView* webView = getWebView();
    webView->setSmartInsertDeleteEnabled(flag);
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
    WebView* webView = getWebView();
    webView->preferences()->setPrivateBrowsingEnabled(privateBrowsingEnabled);
}

void LayoutTestController::setXSSAuditorEnabled(bool enabled)
{
    WebView* webView = getWebView();
    webView->preferences()->setXSSAuditorEnabled(enabled);
}

void LayoutTestController::setAuthorAndUserStylesEnabled(bool flag)
{
    WebView* webView = getWebView();
    webView->preferences()->setAuthorAndUserStylesEnabled(flag);
}

void LayoutTestController::setIconDatabaseEnabled(bool flag)
{
#if ENABLE(ICONDATABASE)
    WebIconDatabase* tmpIconDatabase = WebIconDatabase::createInstance();
    WebIconDatabase* iconDatabase = tmpIconDatabase->sharedIconDatabase();

    iconDatabase->setEnabled(flag);
#endif
}

void LayoutTestController::setJavaScriptProfilingEnabled(bool flag)
{
    WebView* webView = getWebView();
    WebInspector *inspector = webView->inspector();
    if (inspector) {
        webView->preferences()->setDeveloperExtrasEnabled(flag);
        inspector->setJavaScriptProfilingEnabled(flag);
    }
}

void LayoutTestController::setSelectTrailingWhitespaceEnabled(bool flag)
{
    getWebView()->setSelectTrailingWhitespaceEnabled(flag);
}

void LayoutTestController::setPopupBlockingEnabled(bool popupBlockingEnabled)
{
    WebView* webView = getWebView();
    webView->preferences()->setJavaScriptCanOpenWindowsAutomatically(!popupBlockingEnabled);
}

bool LayoutTestController::elementDoesAutoCompleteForElementWithId(JSStringRef id) 
{
    DOMDocument* document = getWebView()->mainFrame()->domDocument();
    if (!document)
        return false;

    char* ident = JSStringCopyUTF8CString(id);
    DOMElement* element = document->getElementById(ident);
    if (!element)
        return false;

    return getWebView()->mainFrame()->elementDoesAutoComplete(element);
}

void LayoutTestController::execCommand(JSStringRef name, JSStringRef value)
{
    char* n = JSStringCopyUTF8CString(name);
    char* val = JSStringCopyUTF8CString(value);
    getWebView()->executeCoreCommandByName(n, val);
}

void LayoutTestController::setCacheModel(int)
{
    // FIXME: implement
}

JSRetainPtr<JSStringRef> LayoutTestController::counterValueForElementById(JSStringRef id)
{
    char* elementID = JSStringCopyUTF8CString(id);
    const char* count = getWebView()->mainFrame()->counterValueForElementById(elementID);
    JSRetainPtr<JSStringRef> counterValue(Adopt, JSStringCreateWithUTF8CString(count));
    return counterValue;
}

bool LayoutTestController::isCommandEnabled(JSStringRef name)
{
    char* command = JSStringCopyUTF8CString(name);
    return getWebView()->commandEnabled(command);
}

void LayoutTestController::setPersistentUserStyleSheetLocation(JSStringRef jsURL)
{
    char* url = JSStringCopyUTF8CString(jsURL);
    ::setPersistentUserStyleSheetLocation(url);
}

void LayoutTestController::clearPersistentUserStyleSheet()
{
    ::setPersistentUserStyleSheetLocation(0);
}

void LayoutTestController::clearAllDatabases()
{
#if ENABLE(DATABASE)
    WebDatabaseManager* tmpDatabaseManager = WebDatabaseManager::createInstance();
    WebDatabaseManager* databaseManager = tmpDatabaseManager->sharedWebDatabaseManager();

    databaseManager->deleteAllDatabases();
#endif
}

void LayoutTestController::overridePreference(JSStringRef key, JSStringRef value)
{
    char* name = JSStringCopyUTF8CString(key);
    char* val = JSStringCopyUTF8CString(value);

    getWebView()->preferences()->setPreferenceForTest(name, val);
}
 
void LayoutTestController::setDatabaseQuota(unsigned long long quota)
{
#if ENABLE(DATABASE)
    WebDatabaseManager* tmpDatabaseManager = WebDatabaseManager::createInstance();
    WebDatabaseManager* databaseManager = tmpDatabaseManager->sharedWebDatabaseManager();

    databaseManager->setQuota("file:///", quota);
#endif
}

void LayoutTestController::setDomainRelaxationForbiddenForURLScheme(bool, JSStringRef)
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
    getWebView()->preferences()->setLoadsImagesAutomatically(false);
}

unsigned LayoutTestController::workerThreadCount() const
{
    WebWorkersPrivate* worker = WebWorkersPrivate::createInstance();
    unsigned count =  worker->workerThreadCount();
    return count;
}

void LayoutTestController::whiteListAccessFromOrigin(JSStringRef sourceOrigin, JSStringRef destinationProtocol, JSStringRef destinationHost, bool allowDestinationSubdomains)
{
    char* source = JSStringCopyUTF8CString(sourceOrigin);
    char* destination = JSStringCopyUTF8CString(destinationProtocol);
    char* host = JSStringCopyUTF8CString(destinationHost);
    getWebView()->whiteListAccessFromOrigin(source, destination, host, allowDestinationSubdomains);
}

void LayoutTestController::setAlwaysAcceptCookies(bool alwaysAcceptCookies)
{
    if (alwaysAcceptCookies == m_alwaysAcceptCookies)
        return;
    if (alwaysAcceptCookies)
        getWebView()->preferences()->setCookieStorageAcceptPolicy(WebKitCookieStorageAcceptPolicyAlways);
    else
        getWebView()->preferences()->setCookieStorageAcceptPolicy(WebKitCookieStorageAcceptPolicyOnlyFromMainDocumentDomain);
    m_alwaysAcceptCookies = alwaysAcceptCookies;
}

void LayoutTestController::addUserScript(JSStringRef source, bool runAtStart)
{
    WebScriptWorld* script = WebScriptWorld::createInstance();
    char* src = JSStringCopyUTF8CString(source);
    getWebView()->addUserScriptToGroup("org.webkit.DumpRenderTree", script, src, 0, 0, 0, 0, 0, runAtStart ? WebView::WebUserScriptInjectAtDocumentStart : WebView::WebUserScriptInjectAtDocumentEnd);
}

void LayoutTestController::addUserStyleSheet(JSStringRef source)
{
    WebScriptWorld* script = WebScriptWorld::createInstance();
    char* src = JSStringCopyUTF8CString(source);
    getWebView()->addUserStyleSheetToGroup("org.webkit.DumpRenderTree", script, src, 0, 0, 0, 0, 0);
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
    WebView* webView = getWebView();
    WebInspector *inspector = webView->inspector();
    if (inspector) 
        inspector->show();
}

void LayoutTestController::closeWebInspector()
{
    WebView* webView = getWebView();
    WebInspector *inspector = webView->inspector();
    if (inspector)
        inspector->close();
    webView->preferences()->setDeveloperExtrasEnabled(false);

}

void LayoutTestController::evaluateInWebInspector(long callId, JSStringRef script)
{
    char* source = JSStringCopyUTF8CString(script);
    WebView* webView = getWebView();
    WebInspector *inspector = webView->inspector();
    if (inspector)
        inspector->evaluateInFrontend(callId, source);
}

void LayoutTestController::removeAllVisitedLinks()
{
    // FIXME: Implement this.
}

void LayoutTestController::evaluateScriptInIsolatedWorld(unsigned worldID, JSObjectRef globalObject, JSStringRef script)
{
    char* source = JSStringCopyUTF8CString(script);
    const char *result;
    WebScriptWorld* scriptWorld = WebScriptWorld::createInstance();
    getWebView()->mainFrame()->stringByEvaluatingJavaScriptInScriptWorld(scriptWorld, globalObject, source, &result);
}

void LayoutTestController::setTimelineProfilingEnabled(bool flag)
{
    WebView* webView = getWebView();
    WebInspector *inspector = webView->inspector();
    if (inspector)
        inspector->setTimelineProfilingEnabled(flag);
}

void LayoutTestController::setAllowUniversalAccessFromFileURLs(bool flag)
{
    getWebView()->preferences()->setAllowUniversalAccessFromFileURLs(flag);
}

bool LayoutTestController::sampleSVGAnimationForElementAtTime(JSStringRef animationId, double time, JSStringRef elementId)
{
    char* name = JSStringCopyUTF8CString(animationId);
    char* element = JSStringCopyUTF8CString(elementId);
    WebView* webView = getWebView();
    bool returnValue =  webView->mainFrame()->pauseSVGAnimation(name, time, element);
    free(name);
    free(element);
    return returnValue;
}

void LayoutTestController::setFrameSetFlatteningEnabled(bool flag)
{
    // FIXME: implement
}

void LayoutTestController::setAllowFileAccessFromFileURLs(bool enabled)
{
    // FIXME: implement
}

void LayoutTestController::apiTestNewWindowDataLoadBaseURL(JSStringRef utf8Data, JSStringRef baseURL)
{
    // FIXME: implement
}

void LayoutTestController::apiTestGoToCurrentBackForwardItem()
{
    // FIXME: implement
}

void LayoutTestController::setSpatialNavigationEnabled(bool enabled)
{
    // FIXME: implement
}
