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
#include "DumpRenderTree.h"

#include "AccessibilityController.h"
#include "EditingDelegate.h"
#include "EventSender.h"
#include "GCController.h"
#include "HistoryDelegate.h"
#include "LayoutTestController.h"
#include "PolicyDelegate.h"
#include "ResourceLoadDelegate.h"
#include "WorkQueue.h"
#include "WorkQueueItem.h"

#include <JSBase.h>
#include <WebKit.h>
#include <string>

#include <string.h>
#include "signal.h"
#include <unistd.h>
#include <cassert>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#if ENABLE(DAE)
#include "ApplicationTestController.h"
#endif

#if defined(USE_FREETYPE)
#include <fontconfig/fontconfig.h>
#endif

using namespace std;


void startEventLoop(BalWidget *view);
void stopEventLoop();
BalRectangle clientRect(bool);
BalWidget* createWindow(WebView **webView, BalRectangle rect);

volatile bool done;
SharedPtr<WebView> gWebView = 0;
#if ENABLE(DAE)
ApplicationTestController* gApplicationTestController = 0;
#endif
WebFrame* topLoadingFrame = 0;
static WebHistoryItem* prevTestBFItem;  // current b/f item at the end of the previous test
LayoutTestController* gLayoutTestController = 0;
AccessibilityController* gAccessibilityController = 0;
EventSenderController* gEventSenderController = 0;

static bool printSeparators;
static int dumpPixels;
static int dumpTree = 1;
SharedPtr<PolicyDelegate> policyDelegate;
SharedPtr<ResourceLoadDelegate> resourceLoadDelegate;
SharedPtr<EditingDelegate> sharedEditingDelegate;
SharedPtr<HistoryDelegate> sharedHistoryDelegate;
std::vector<std::string> disallowedURLs;
static const char* persistentUserStyleSheetLocation = 0;


WebView *getWebView()
{
    return gWebView.get();
}

void setWebView(WebView* w)
{
    gWebView = w;
}

bool getDone()
{
    return done;
}

void setDone(bool d)
{
    done = d;
}

static char* autocorrectURL(const char* url)
{
    if (strncmp("http://", url, 7) != 0 && strncmp("https://", url, 8) != 0 && strncmp("about:", url, 6)) {
        string s = "file://";
        s += url;
        return strdup(s.c_str());
    }

    return strdup(url);
}

void setPersistentUserStyleSheetLocation(const char* url)
{
    persistentUserStyleSheetLocation = url;
}

static bool processWork()
{
    // if we finish all the commands, we're ready to dump state
    if (WorkQueue::shared()->processWork() && !gLayoutTestController->waitToDump())
        dump();
    return false;
}

string descriptionSuitableForTestResult(WebFrame* webFrame)
{
    WebView* webView = webFrame->webView();
    if (!webView)
        return string();

    WebFrame* mainFrame = webView->mainFrame();
    if (!mainFrame)
        return string();

    string name = webFrame->name();
    if (name.empty())
        return (webFrame == mainFrame) ? "main frame" : string();

    string frameName = (webFrame == mainFrame) ? "main frame" : "frame";
    frameName += " \"" + name + "\"";

    return frameName;
}

class FrameLoadDelegate : public WebFrameLoadDelegate {

public:
    static TransferSharedPtr<FrameLoadDelegate> createInstance()
    {
        return new FrameLoadDelegate();
    }

    ~FrameLoadDelegate() 
    { 
        delete m_gcController;
    }

    virtual void didStartProvisionalLoad(WebFrame* frame)
    {
        if (!done && gLayoutTestController->dumpFrameLoadCallbacks())
            printf("%s - didStartProvisionalLoadForFrame\n", descriptionSuitableForTestResult(frame).c_str());

        if (!topLoadingFrame && !done) {
#if ENABLE(DAE)
            if (!getWebView())
                topLoadingFrame = frame;
#else                
            topLoadingFrame = frame;
#endif
        }

        if (!done && gLayoutTestController->stopProvisionalFrameLoads()) {
            printf ("%s - stopping load in didStartProvisionalLoadForFrame callback\n", descriptionSuitableForTestResult(frame).c_str());
            frame->stopLoading();
        }
    }

    virtual void didReceiveServerRedirectForProvisionalLoadForFrame(WebFrame* frame) 
    { 
        if (!done && gLayoutTestController->dumpFrameLoadCallbacks())
            printf("%s - didReceiveServerRedirectForProvisionalLoadForFrame\n", descriptionSuitableForTestResult(frame).c_str());
    }

    virtual void willPerformClientRedirectToURL(WebFrame* frame, const char* url, double delaySeconds, double fireDate) 
    { 
        if (!done && gLayoutTestController->dumpFrameLoadCallbacks())
            printf("%s - willPerformClientRedirectToURL: %s \n", descriptionSuitableForTestResult(frame).c_str(), url);
    }

    virtual void didCancelClientRedirectForFrame(WebFrame* frame) 
    { 
        if (!done && gLayoutTestController->dumpFrameLoadCallbacks())
            printf("%s - didCancelClientRedirectForFrame\n", descriptionSuitableForTestResult(frame).c_str());
    }

    virtual void didCommitLoad(WebFrame* frame) 
    { 
        if (!done && gLayoutTestController->dumpFrameLoadCallbacks())
            printf("%s - didCommitLoadForFrame\n", descriptionSuitableForTestResult(frame).c_str());
    }

    virtual void didFinishLoad(WebFrame* frame)
    {
        if (!done && gLayoutTestController->dumpFrameLoadCallbacks())
            printf("%s - didFinishLoadForFrame\n", descriptionSuitableForTestResult(frame).c_str());

        if (frame != topLoadingFrame)
            return;

        topLoadingFrame = 0;

        if (!gWebView)
            return;

        WorkQueue::shared()->setFrozen(true); // first complete load freezes the queue for the rest of this test
        if (gLayoutTestController->waitToDump())
            return;

        if (WorkQueue::shared()->count())
            processWork();
         else
            dump();
    }

    virtual void didFailProvisionalLoad(WebFrame* frame, WebError*) 
    {
        if (!done && gLayoutTestController->dumpFrameLoadCallbacks())
            printf("%s - didFailProvisionalLoadWithError\n", descriptionSuitableForTestResult(frame).c_str());
    }

    virtual void didFailLoad(WebFrame* frame, WebError*) 
    {
        if (!done && gLayoutTestController->dumpFrameLoadCallbacks())
            printf("%s - didFailLoadWithError\n", descriptionSuitableForTestResult(frame).c_str());
    }

    virtual void windowObjectClearNotification(WebFrame*, void* context, void* windowObject)
    {
        JSValueRef exception = 0;
        assert(gLayoutTestController);
#if ENABLE(DAE)
        assert(gApplicationTestController);
#endif

        gLayoutTestController->makeWindowObject((JSGlobalContextRef)context, (JSObjectRef)windowObject, &exception);
        assert(!exception);

        m_gcController->makeWindowObject((JSGlobalContextRef)context, (JSObjectRef)windowObject, &exception);
        assert(!exception);

        gAccessibilityController->makeWindowObject((JSGlobalContextRef)context, (JSObjectRef)windowObject, &exception);
        assert(!exception);

        gEventSenderController->makeWindowObject((JSGlobalContextRef)context, (JSObjectRef)windowObject, &exception);
        assert(!exception);

#if ENABLE(DAE)
        gApplicationTestController->makeWindowObject((JSGlobalContextRef)context, (JSObjectRef)windowObject, &exception);
        assert(!exception);
#endif
    }

    virtual void didFinishDocumentLoadForFrame(WebFrame* frame)
    {
        if (!done && gLayoutTestController->dumpFrameLoadCallbacks())
            printf("%s - didFinishDocumentLoadForFrame\n", descriptionSuitableForTestResult(frame).c_str());
        
        if (!done) {
            unsigned pendingFrameUnloadEvents = frame->pendingFrameUnloadEventCount();
            if (pendingFrameUnloadEvents)
                printf("%s - has %u onunload handler(s)\n", descriptionSuitableForTestResult(frame).c_str(), pendingFrameUnloadEvents);
        }
    }

    virtual void didHandleOnloadEventsForFrame(WebFrame* frame)
    {
        if (!done && gLayoutTestController->dumpFrameLoadCallbacks())
            printf("%s - didHandleOnloadEventsForFrame\n", descriptionSuitableForTestResult(frame).c_str());
    }

    virtual void titleChange(WebFrame* frame, const char *title)
    {
        if (!done && gLayoutTestController->dumpFrameLoadCallbacks())
            printf("%s - didReceiveTitle: %s\n", descriptionSuitableForTestResult(frame).c_str(), title);

        if (gLayoutTestController->dumpTitleChanges() && !done)
            printf("TITLE CHANGED: %s\n", title ? title : "");
    }
    virtual void didChangeLocationWithinPageForFrame(WebFrame*) {}

    virtual void willCloseFrame(WebFrame*) {}

    virtual void dispatchDidFirstLayout(WebFrame*) {}

    virtual void dispatchDidFirstVisuallyNonEmptyLayout(WebFrame*) {}

    virtual void dispatchNotEnoughtMemory(WebFrame*) {}

    virtual void didDisplayInsecureContent(WebFrame*)
    {
        if (!done && gLayoutTestController->dumpFrameLoadCallbacks())
            printf("didDisplayInsecureContent\n");
    }

    virtual void didRunInsecureContent(WebFrame *, WebSecurityOrigin *)
    {
        if (!done && gLayoutTestController->dumpFrameLoadCallbacks())
            printf("didRunInsecureContent\n");
    }

private:
    FrameLoadDelegate()
    : m_gcController(new GCController())
    { }

    GCController* m_gcController;
};

class JSDelegate : public JSActionDelegate {

public:
    static TransferSharedPtr<JSDelegate> createInstance()
    {
        return new JSDelegate();
    }
    ~JSDelegate() { }

    virtual void consoleMessage(WebFrame *frame, int line, const char *message)
    {
        string newMessage;
        if (message) {
            newMessage = message;
            size_t fileProtocol = newMessage.find("file://");
            if (fileProtocol != string::npos) {
                size_t pos = newMessage.find_last_of("/");
                newMessage = newMessage.substr(0, fileProtocol) + newMessage.substr(pos + 1);
            }
    }
        fprintf(stdout, "CONSOLE MESSAGE: line %d: %s\n", line, newMessage.c_str());
    }

    virtual bool jsAlert(WebFrame*, const char *message)
    {
        fprintf(stdout, "ALERT: %s\n", message);
        return true;
    }

    virtual bool jsConfirm(WebFrame *, const char *message)
    {
        fprintf(stdout, "CONFIRM: %s\n", message);
        return true;
    }

    virtual bool jsPrompt(WebFrame *, const char *message, const char *defaultValue, char **value)
    {
        fprintf(stdout, "PROMPT: %s, default text: %s\n", message, defaultValue);
        *value = strdup(defaultValue);
        return true;
    }

    virtual void exceededDatabaseQuota(WebFrame *frame, WebSecurityOrigin *origin, const char* databaseIdentifier)
    {
        const char* protocol = origin->protocol();
        const char* host = origin->host();
        unsigned short port = origin->port();

        if (!done && gLayoutTestController->dumpDatabaseCallbacks())
            printf("UI DELEGATE DATABASE CALLBACK: exceededDatabaseQuotaForSecurityOrigin:{%s, %s, %i} database:%s\n", protocol, host, port, databaseIdentifier);

        static const unsigned long long defaultQuota = 5 * 1024 * 1024;
        origin->setQuota(defaultQuota);
    }

private:
    JSDelegate() { }
};

void dumpFrameScrollPosition(WebFrame* frame)
{
    if (!frame)
        return;

    BalPoint scrollPosition = frame->scrollOffset();
    int x = getX(scrollPosition);
    int y = getY(scrollPosition);

    if (abs(x) > 0 || abs(y) > 0) {
        WebFrame* parent = frame->parentFrame();
        if (parent) {
            const char* name = frame->name();
            printf("frame '%s' ", name ? name : "");
        }
        printf("scrolled to %.f,%.f\n", (double)x, (double)y);
    }

    if (::gLayoutTestController->dumpChildFrameScrollPositions()) {
        std::vector<WebFrame*>* children = frame->children();
        for(size_t i = 0; i < children->size(); ++i)
            dumpFrameScrollPosition(children->at(i));
    }
}

const char* dumpFramesAsText(WebFrame* frame)
{
    if (!frame)
        return "";

    // Add header for all but the main frame.
    bool isMainFrame = frame->webView()->mainFrame() == frame;
    string innerText = frame->toString();
    string result = "";

    if (!isMainFrame) {
        result += "\n--------\nFrame: '";
        result += frame->name();
        result += "'\n--------\n";
    }

    result += innerText;
    result += "\n";
    
    if (gLayoutTestController->dumpChildFramesAsText()) {
        vector<WebFrame *>* children = frame->children();
        for (size_t i = 0; i < children->size(); ++i)
            result += dumpFramesAsText(children->at(i));
    }

#if ENABLE(DAE_APPLICATION)
    if (gApplicationTestController->showChildApplications()) {
        // Dump child applications      
        SharedPtr<WebApplication> webApp = webAppMgr().application(frame->webView());

        for(size_t i = 0; webApp && i < webApp->numChildren(); ++i) {
            SharedPtr<WebApplication> child = webApp->childAt(i);
            result += "\n--------\nChild";
            result += "\n--------\n";
            if (child && child->webView())
                result += dumpFramesAsText(child->webView()->mainFrame());
            else
                result += "ERROR: Child application web view is NULL!";
        }
    }
#endif

    return strdup(result.c_str());
}

static void invalidateAnyPreviousWaitToDumpWatchdog()
{
    if (waitToDumpWatchdog) {
        removeTimer();
        waitToDumpWatchdog = 0;
    }
}

static bool compareHistoryItems(WebHistoryItem* item1, WebHistoryItem* item2)
{
    const char* target1 = item1->target();
    const char* target2 = item2->target();
    return strcasecmp(target1, target2) < 0;
}

static void dumpHistoryItem(WebHistoryItem* item, int indent, bool current)
{
    assert(item);

    int start = 0;
    if (current) {
        printf("curr->");
        start = 6;
    }
    for (int i = start; i < indent; i++)
        putchar(' ');

    const char* url = item->URLString();
    if (!url)
        return;

    if (strncmp("file://", url, 7) == 0) {
        const char* pos = strstr(url, "/LayoutTests/");
        if (pos == NULL)
            return;

        string result = "(file test):";
        result += pos + strlen("/LayoutTests/");
        printf("%s", result.c_str());
    } else
        printf("%s", url);

    const char* target = item->target();
    if (target && strlen(target) > 0)
        printf(" (in frame \"%s\")", target);
    bool isTargetItem = item->isTargetItem();
    if (isTargetItem)
        printf("  **nav target**");
    putchar('\n');

    std::vector<WebHistoryItem*> kids = item->children();
    // must sort to eliminate arbitrary result ordering which defeats reproducible testing
    sort(kids.begin(), kids.end(), compareHistoryItems);

    for (unsigned i = 0; i < kids.size(); ++i) {
        WebHistoryItem* item = kids[i];
        dumpHistoryItem(item, indent + 4, false);
    }
}



static void dumpBackForwardList()
{
    printf("\n============== Back Forward List ==============\n");

    WebBackForwardList* bfList = getWebView()->backForwardList();
    if (!bfList)
        return;
    // Print out all items in the list after prevTestBFItem, which was from the previous test
    // Gather items from the end of the list, the print them out from oldest to newest

    int forwardListCount = bfList->forwardListCount();

    vector<WebHistoryItem*> itemsToPrint;
    for (int i = forwardListCount; i > 0; --i) {
        WebHistoryItem* item = bfList->itemAtIndex(i);
        if (!item)
            return;
        // something is wrong if the item from the last test is in the forward part of the b/f list
        assert(item != prevTestBFItem);
        itemsToPrint.push_back(item);
    }

    WebHistoryItem* currentItem = bfList->currentItem();
    if (!currentItem)
        return;

    assert(currentItem != prevTestBFItem);
    itemsToPrint.push_back(currentItem);
    int currentItemIndex = itemsToPrint.size() - 1;

    int backListCount = bfList->backListCount();
    for (int i = -1; i >= -backListCount; --i) {
        WebHistoryItem* item = bfList->itemAtIndex(i);
        if (!item)
            return;
        if (item == prevTestBFItem)
            break;
        itemsToPrint.push_back(item);
    }

    for (int i = itemsToPrint.size() - 1; i >= 0; --i) {
        WebHistoryItem* historyItemToPrint = itemsToPrint[i];
        dumpHistoryItem(historyItemToPrint, 8, i == currentItemIndex);
    }

    printf("===============================================\n");
}

void dump()
{
    if (!gWebView)
        return;

    invalidateAnyPreviousWaitToDumpWatchdog();
    if (dumpTree) {

        WebFrame *frame = getWebView()->mainFrame();
        string renderDump;
        bool dumpAsText = gLayoutTestController->dumpAsText();

        // FIXME: Also dump text resuls as text.
        gLayoutTestController->setDumpAsText(dumpAsText);
        if (gLayoutTestController->dumpAsText()) {
            renderDump = dumpFramesAsText(frame);
        } else {
            renderDump = frame->renderTreeDump();
        }

        if (renderDump.empty()) {
            const char* errorMessage;
            if (gLayoutTestController->dumpAsText())
                errorMessage = "[documentElement innerText]";
            else if (gLayoutTestController->dumpDOMAsWebArchive())
                errorMessage = "[[mainFrame DOMDocument] webArchive]";
            else if (gLayoutTestController->dumpSourceAsWebArchive())
                errorMessage = "[[mainFrame dataSource] webArchive]";
            else
                errorMessage = "[mainFrame renderTreeAsExternalRepresentation]";
            printf("ERROR: nil result from %s", errorMessage);
        } else {
            printf("%s", renderDump.c_str());
            if (!gLayoutTestController->dumpAsText() && !gLayoutTestController->dumpDOMAsWebArchive() && !gLayoutTestController->dumpSourceAsWebArchive())
                dumpFrameScrollPosition(frame);
        
        }

        if (gLayoutTestController->dumpBackForwardList()) {
            dumpBackForwardList();
        }

        invalidateAnyPreviousWaitToDumpWatchdog();

        if (printSeparators) {
            puts("#EOF"); // terminate the content block
            fputs("#EOF\n", stderr);
            fflush(stdout);
            fflush(stderr);
        }
    }

    if (dumpPixels) {
        if (!gLayoutTestController->dumpAsText() && !gLayoutTestController->dumpDOMAsWebArchive() && !gLayoutTestController->dumpSourceAsWebArchive()) {
            // FIXME: Add support for dumping pixels
        }
    }

    // FIXME: call displayWebView here when we support --paint

    puts("#EOF"); // terminate the (possibly empty) pixels block

    fflush(stdout);
    fflush(stderr);

    stopEventLoop();
    done = true;
}

static void crashHandler(int sig)
{
    fprintf(stderr, "%s\n", strsignal(sig));
    exit(128 + sig);
}

#if defined(USE_FREETYPE)
void initializeFonts()
{
    static int numFonts = -1;

    // Some test cases may add or remove application fonts (via @font-face).
    // Make sure to re-initialize the font set if necessary.
    FcFontSet* appFontSet = FcConfigGetFonts(0, FcSetApplication);
    if (appFontSet && numFonts >= 0 && appFontSet->nfont == numFonts)
        return;

    const char* fontDir = getenv("WEBKIT_TESTFONTS");
    // TODO : test if the directory exists
    if (!fontDir) {
        fprintf(stderr,
                "\n\n"
                "----------------------------------------------------------------------\n"
                "WEBKIT_TESTFONTS environment variable is not set correctly.\n"
                "This variable has to point to the directory containing the fonts\n"
                "you can clone from git://gitorious.org/qtwebkit/testfonts.git\n"
                "----------------------------------------------------------------------\n"
               );
        exit(1);
    }

    FcConfig *config = FcConfigCreate();
    string configFile = fontDir;
    configFile += "/fonts.conf";
    if (!FcConfigParseAndLoad (config, (FcChar8*) configFile.c_str(), true)) {
        fprintf(stderr, "Couldn't load font configuration file");
        exit(1);
    }

    if (!FcConfigAppFontAddDir (config, (FcChar8*) fontDir)) {
        fprintf(stderr, "Couldn't add font dir!");
        exit(1);
    }
    FcConfigSetCurrent(config);

    appFontSet = FcConfigGetFonts(config, FcSetApplication);
    numFonts = appFontSet->nfont;
}
#endif

void setPreferences()
{
    WebPreferences* preferences = getWebView()->preferences();

#if 0
    preferences->setStandardFontFamily("Times");
    preferences->setFixedFontFamily("Courier");
    preferences->setSerifFontFamily("Times");
    preferences->setSansSerifFontFamily("Helvetica");
    preferences->setCursiveFontFamily("Apple Chancery");
    preferences->setFantasyFontFamily("Papyrus");
#else
    preferences->setStandardFontFamily("Times New Roman");
    preferences->setFixedFontFamily("Courier New");
    preferences->setSerifFontFamily("Times New Roman");
    preferences->setSansSerifFontFamily("Arial");
    preferences->setCursiveFontFamily("Comic Sans MS");
    preferences->setFantasyFontFamily("Times New Roman");
#endif

    preferences->setAutosaves(false);
    preferences->setDefaultFontSize(16);
    preferences->setDefaultFixedFontSize(13);
    preferences->setMinimumFontSize(1);
    preferences->setJavaEnabled(false);
    preferences->setPlugInsEnabled(true);
    preferences->setDOMPasteAllowed(true);
    preferences->setEditableLinkBehavior(WebKitEditableLinkOnlyLiveWithShiftKey);
    preferences->setFontSmoothing(FontSmoothingTypeStandard);
    preferences->setUsesPageCache(false);
    preferences->setPrivateBrowsingEnabled(false);
    preferences->setJavaScriptCanOpenWindowsAutomatically(true);
    preferences->setJavaScriptEnabled(true);
    preferences->setTabsToLinks(false);
    preferences->setShouldPrintBackgrounds(true);
    preferences->setLoadsImagesAutomatically(true);

    if (persistentUserStyleSheetLocation) {
        preferences->setUserStyleSheetLocation(persistentUserStyleSheetLocation);
        preferences->setUserStyleSheetEnabled(true);
    } else
        preferences->setUserStyleSheetEnabled(false);

    preferences->setAllowUniversalAccessFromFileURLs(true);
    preferences->setAuthorAndUserStylesEnabled(true);
    preferences->setDeveloperExtrasEnabled(false);
    preferences->setExperimentalNotificationsEnabled(true);
    preferences->setShouldPaintNativeControls(false); // FIXME - need to make DRT pass with Windows native controls <http://bugs.webkit.org/show_bug.cgi?id=25592>
    preferences->setXSSAuditorEnabled(false);
    preferences->setOfflineWebApplicationCacheEnabled(true);
    
    //setAlwaysAcceptCookies(false);

#if ENABLE(NETSCAPE_PLUGIN_API)
    preferences->addExtraPluginDirectory(TEST_PLUGIN_DIR);
#endif
    
    WebInspector *inspector = getWebView()->inspector();
    if (inspector) 
        inspector->setJavaScriptProfilingEnabled(false);

    setlocale(LC_ALL, "");
}

static bool shouldLogFrameLoadDelegates(const char* pathOrURL)
{
    return strstr(pathOrURL, "/loading/") || strstr(pathOrURL, "\\loading\\");
}

static bool shouldLogHistoryDelegates(const char* pathOrURL)
{
    return strstr(pathOrURL, "/globalhistory/") || strstr(pathOrURL, "\\globalhistory\\");
}

static bool shouldOpenWebInspector(const char* pathOrURL)
{
    return strstr(pathOrURL, "/inspector/") || strstr(pathOrURL, "\\inspector\\");
}

static bool inspectorURLAvailable()
{
    string inspectorURL = getenv("INSPECTOR_URL");
    if (!inspectorURL.empty())
        return true;
    return false;
}

void runTest(const string& testPathOrURL)
{
    string pathOrURL(testPathOrURL);
    string expectedPixelHash;

    size_t separatorPos = pathOrURL.find("'");
    if (separatorPos != string::npos) {
        pathOrURL = string(testPathOrURL, 0, separatorPos);
        expectedPixelHash = string(testPathOrURL, separatorPos + 1);
    }

    char* url = autocorrectURL(pathOrURL.c_str());
    const string testURL(url);
    done = false;

    disallowedURLs.clear();

    policyDelegate = PolicyDelegate::createInstance();
    policyDelegate->setPermissive(false);
    policyDelegate->setControllerToNotifyDone(0);

    sharedEditingDelegate = EditingDelegate::createInstance();
    sharedHistoryDelegate = HistoryDelegate::createInstance();

    resourceLoadDelegate = ResourceLoadDelegate::createInstance();

    gLayoutTestController = new LayoutTestController(testURL, expectedPixelHash);
#if ENABLE(DAE)
    gApplicationTestController = new ApplicationTestController();
#endif
    gAccessibilityController = new AccessibilityController();
    gEventSenderController = new EventSenderController();

    topLoadingFrame = 0;
    
    WorkQueue::shared()->clear();
    WorkQueue::shared()->setFrozen(false);

    bool isSVGW3CTest = (gLayoutTestController->testPathOrURL().find("svg/W3C-SVG-1.1") != string::npos);
    BalRectangle rect = clientRect(isSVGW3CTest);

    SharedPtr<FrameLoadDelegate> frameLoadDelegate = FrameLoadDelegate::createInstance();
    SharedPtr<JSDelegate> jsActionDelegate = JSDelegate::createInstance();

    gLayoutTestController->setIconDatabaseEnabled(false);
    
    if (shouldOpenWebInspector(pathOrURL.c_str())) {
        if (!inspectorURLAvailable()) {
            printf("You must set environment variable INSPECTOR_URL correctly\n");
            exit(1);
        }
        gLayoutTestController->showWebInspector();
    }

    if (shouldLogFrameLoadDelegates(pathOrURL.c_str()))
        gLayoutTestController->setDumpFrameLoadCallbacks(true);

#if ENABLE(DAE)
    BalWidget *view = createWindow(0, rect);
    //SharedPtr<WebApplication> application = WebApplicationManager::instance().createApplication(view, rect, testPathOrURL.c_str(), frameLoadDelegate, jsActionDelegate);
    SharedPtr<WebApplication> application = WebApplicationManager::instance().createApplication(view, rect, 0, frameLoadDelegate, jsActionDelegate);
    gWebView = application->webView();
#else
#if !PLATFORM(QT)
    gWebView = WebView::createInstance();
#endif
    WebView* webview = gWebView.get();
    BalWidget *view = createWindow(&webview, rect);
    if (webview != gWebView.get()) 
        gWebView = webview; 
    
    getWebView()->initWithFrame(rect, "", "");
    getWebView()->setWebFrameLoadDelegate(frameLoadDelegate);
    getWebView()->setJSActionDelegate(jsActionDelegate);
#endif
    getWebView()->setGroupName("org.webkit.DumpRenderTree");
    getWebView()->setPolicyDelegate(0);
    getWebView()->setWebEditingDelegate(sharedEditingDelegate);
    getWebView()->setWebResourceLoadDelegate(resourceLoadDelegate);
    setPreferences();

#if defined(USE_FREETYPE)
    initializeFonts();
#endif

    WebBackForwardList* bfList = getWebView()->backForwardList();
    if (bfList)
        prevTestBFItem = bfList->currentItem();


    if (shouldLogHistoryDelegates(pathOrURL.c_str())) {
        gLayoutTestController->setDumpHistoryDelegateCallbacks(true);
        getWebView()->setHistoryDelegate(sharedHistoryDelegate);
    } else
        getWebView()->setHistoryDelegate(0);

//#if !ENABLE(DAE)
    getWebView()->mainFrame()->loadURL(url);
//#endif
    free(url);
    url = NULL;

    startEventLoop(view);

    if (shouldOpenWebInspector(pathOrURL.c_str()))
        gLayoutTestController->closeWebInspector();

    getWebView()->mainFrame()->stopLoading();

    const char* groupName = getWebView()->groupName();
    if (groupName)
        getWebView()->removeAllUserContentFromGroup(groupName);
    // A blank load seems to be necessary to reset state after certain tests.
    //getWebView()->mainFrame()->loadURL("about:blank");
    
    //WorkQueue::shared()->clear();

    delete gLayoutTestController;
    delete gAccessibilityController;
    delete gEventSenderController;

#if ENABLE(DAE)
    delete gApplicationTestController;
#endif
    
    frameLoadDelegate = 0;
    jsActionDelegate = 0;
    policyDelegate = 0;
    sharedEditingDelegate = 0;
    resourceLoadDelegate = 0;
    sharedHistoryDelegate = 0;
}

void init(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    init(argc, argv);

    signal(SIGILL, crashHandler);    /* 4:   illegal instruction (not reset when caught) */
    signal(SIGTRAP, crashHandler);   /* 5:   trace trap (not reset when caught) */
    signal(SIGFPE, crashHandler);    /* 8:   floating point exception */
    signal(SIGBUS, crashHandler);    /* 10:  bus error */
    signal(SIGSEGV, crashHandler);   /* 11:  segmentation violation */
    signal(SIGSYS, crashHandler);    /* 12:  bad argument to system call */
    signal(SIGPIPE, crashHandler);   /* 13:  write on a pipe with no reader */
    signal(SIGXCPU, crashHandler);   /* 24:  exceeded CPU time limit */
    signal(SIGXFSZ, crashHandler);   /* 25:  exceeded file size limit */

    struct option options[] = {
        {"notree", no_argument, &dumpTree, false},
        {"pixel-tests", no_argument, &dumpPixels, true},
        {"tree", no_argument, &dumpTree, true},
        {NULL, 0, NULL, 0}
    };

    int option;
    while ((option = getopt_long(argc, (char* const*)argv, "", options, NULL)) != -1)
        switch (option) {
            case '?':   // unknown or ambiguous option
            case ':':   // missing argument
                exit(1);
                break;
        }

    if (argc == optind+1 && strcmp(argv[optind], "-") == 0) {
        char filenameBuffer[2048];
        printSeparators = true;
        while (fgets(filenameBuffer, sizeof(filenameBuffer), stdin)) {
            char* newLineCharacter = strchr(filenameBuffer, '\n');
            if (newLineCharacter)
                *newLineCharacter = '\0';

            if (strlen(filenameBuffer) == 0)
                continue;

            runTest(filenameBuffer);
        }
    } else {
        printSeparators = (optind < argc-1 || (dumpPixels && dumpTree));
        for (int i = optind; i != argc; ++i) {
            runTest(argv[i]);
        }
    }

    return 0;
}

