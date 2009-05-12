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

#include "LayoutTestController.h"
#include "WorkQueue.h"
#include "WorkQueueItem.h"

#include <WebKit.h>
#include <string>

#include <string.h>
#include "signal.h"
#include <unistd.h>
#include <cassert>
#include <getopt.h>
#include <stdlib.h>


using namespace std;


void startEventLoop(BalWidget *view);
void stopEventLoop();
BalRectangle clientRect(bool);
BalWidget* createWindow(WebView *webView, BalRectangle rect);

volatile bool done;
static WebView *webView = 0;
WebFrame* topLoadingFrame = 0;
LayoutTestController* gLayoutTestController = 0;

static bool printSeparators;
static int dumpPixels;
static int dumpTree = 1;


WebView *getWebView()
{
    return webView;
}

bool getDone()
{
    return done;
}

void setDone(bool d)
{
    done = d;
}

// FIXME : move this in openURL
static char* autocorrectURL(const char* url)
{
    if (strncmp("http://", url, 7) != 0 && strncmp("https://", url, 8) != 0) {
        string s = "file://";
        s += url;
        return strdup(s.c_str());
    }

    return strdup(url);
}

static bool shouldLogFrameLoadDelegates(const char* pathOrURL)
{
    return strstr(pathOrURL, "loading/");
}


static bool processWork()
{
    // if we finish all the commands, we're ready to dump state
    if (WorkQueue::shared()->processWork() && !gLayoutTestController->waitToDump())
        dump();
    return false;
}

class FrameLoadDelegate : public WebFrameLoadDelegate {

public:
    FrameLoadDelegate() { }
    ~FrameLoadDelegate() { }

    virtual void didStartProvisionalLoad(WebFrame* frame)
    {
        if (!topLoadingFrame && !done)
            topLoadingFrame = frame;
    }

    virtual void didReceiveServerRedirectForProvisionalLoadForFrame(WebFrame*) { }

    virtual void willPerformClientRedirectToURL(WebFrame*, const char* url, double delaySeconds, double fireDate) { }

    virtual void didCancelClientRedirectForFrame(WebFrame*) { }

    virtual void didCommitLoad(WebFrame*) { }

    virtual void didFinishLoad(WebFrame* frame)
    {
        if (frame != topLoadingFrame)
            return;

        topLoadingFrame = 0;

        if (!webView)
            return;

        WorkQueue::shared()->setFrozen(true); // first complete load freezes the queue for the rest of this test
        if (gLayoutTestController->waitToDump())
            return;

        if (WorkQueue::shared()->count())
            processWork();
         else
            dump();
    }

    virtual void didFailLoad(WebFrame*) { }

    virtual void windowObjectClearNotification(WebFrame*, void *context, void *windowObject)
    {
        JSValueRef exception = 0;
        assert(gLayoutTestController);

        gLayoutTestController->makeWindowObject((JSGlobalContextRef)context, (JSObjectRef)windowObject, &exception);
        assert(!exception);
    }


    virtual void titleChange(WebFrame*, const char *title)
    {
        if (gLayoutTestController->dumpTitleChanges() && !done)
            printf("TITLE CHANGED: %s\n", title ? title : "");
    }
};

class JSDelegate : public JSActionDelegate {

public:
    JSDelegate() { }
    ~JSDelegate() { }

    virtual void consoleMessage(WebFrame *frame, int line, const char *message)
    {
        fprintf(stdout, "CONSOLE MESSAGE: line %d: %s\n", line, message);
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
};

void dumpFrameScrollPosition(WebFrame* frame)
{
}

void displayWebView()
{
}

const char* dumpFramesAsText(WebFrame* frame)
{
    if (!frame)
        return "";

    // Add header for all but the main frame.
    bool isMainFrame = (webView->mainFrame() == frame);
    string innerText = frame->toString();
    string result;
    if (isMainFrame) {
        result = innerText;
        result += "\n";
    } else {
        result.append("\n--------\nFrame: '");
        result.append(frame->name());
        result.append("'\n--------\n");
    }
    
    if (gLayoutTestController->dumpChildFramesAsText()) {
        vector<WebFrame *>* children = frame->children();
        for (size_t i = 0; i < children->size(); ++i)
            result += dumpFramesAsText(children->at(i));
    }

    return strdup(result.c_str());
}

static void invalidateAnyPreviousWaitToDumpWatchdog()
{
    if (waitToDumpWatchdog) {
        removeTimer();
        waitToDumpWatchdog = 0;
    }
}

void dump()
{
    if (!webView)
        return;

    invalidateAnyPreviousWaitToDumpWatchdog();
    if (dumpTree) {

        WebFrame *frame = webView->mainFrame();
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
            // FIXME: not implemented
        }

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

    gLayoutTestController = new LayoutTestController(testURL, expectedPixelHash);
    topLoadingFrame = 0;
    gLayoutTestController->setIconDatabaseEnabled(false);
    
    if (shouldLogFrameLoadDelegates(pathOrURL.c_str()))
        gLayoutTestController->setDumpFrameLoadCallbacks(true);    
    
    WorkQueue::shared()->clear();
    WorkQueue::shared()->setFrozen(false);

    bool isSVGW3CTest = (gLayoutTestController->testPathOrURL().find("svg/W3C-SVG-1.1") != string::npos);
    webView = WebView::createInstance();
    BalRectangle rect = clientRect(isSVGW3CTest);
    webView->initWithFrame(rect,"", "");
    BalWidget *view = createWindow(webView, rect);

    FrameLoadDelegate* frameLoadDelegate = new FrameLoadDelegate();
    webView->setWebFrameLoadDelegate(frameLoadDelegate);

    JSDelegate* jsActionDelegate = new JSDelegate();
    webView->setJSActionDelegate(jsActionDelegate);

    // FIXME : fix preferences
    // FIXME : add ExtraPluginDirectory
    //PluginDatabase::installedPlugins()->addExtraPluginDirectory(filenameToString(directory));

    webView->mainFrame()->loadURL(url);

    free(url);
    url = NULL;

    startEventLoop(view);
    // A blank load seems to be necessary to reset state after certain tests.
    webView->mainFrame()->loadURL("about:blank");
    
    //WorkQueue::shared()->clear();

    gLayoutTestController->deref();
    gLayoutTestController = 0;
    delete webView;
    delete frameLoadDelegate;
    delete jsActionDelegate;
    webView = 0;
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

