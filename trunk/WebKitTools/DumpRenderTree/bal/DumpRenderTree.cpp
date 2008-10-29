/*
 * Copyright (C) 2007 Eric Seidel <eric@webkit.org>
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


/*
 *          Copyright 2006 Origyn
 */
#include "config.h"

#include "CString.h"
#include "Document.h"
#include "Frame.h"
#include "WebFrameLoaderClient.h"
#include "HTMLElement.h"
#include "KURL.h"
#include "markup.h"
#include "Page.h"
#include "RenderTreeAsText.h"
#include "RenderView.h"
#include <iostream> // NOTE log define interferes with iostream
#include <signal.h>
#include "SystemTime.h"
#include "ObserverData.h"
#include "ObserverServiceData.h"

#include "runtime.h"
#include "runtime_root.h"
#include "ExecState.h"
#include "JSGlobalObject.h"
//#include "kjs_window.h"
#include "JavaScriptCore/APICast.h"
#include "LayoutTestController.h"
#include "GCController.h"
#include "EventSender.h"
#include "JSObject.h"
#include "WorkQueue.h"
#include "API/JSStringRef.h"

#include "BCGraphicsContext.h"
#include <Api/WebFrame.h>
#include <Api/WebView.h>
#include "Page.h"
#include "Frame.h"
#include "FrameView.h"
#include "ScriptController.h"


using namespace WebCore;
using namespace OWBAL;
using namespace WKAL;

void startEventLoop(BalWidget *view);
void stopEventLoop();
BalWidget* createWindow(WebView *webView);
static WebView *webView = 0;

/**
 * The BIWebView implementation for DumpRenderTree
 *
 * @see Font, FontData
 */
class DumpRenderTree : public ObserverData {
public:
    DumpRenderTree();
    ~DumpRenderTree();

    void handleEvent();
    void initJSObjects(WebCore::Frame* frame);
    /**
     * JSLayoutTestController notifies us.
     * @param(in) "layoutTestController"
     * @param(in) notifyDone, waitUntilDone
     */
    void observe(const String&, const String&, void*);
protected:
    void dump();

    bool                    m_waitUntilDone;
    Frame*                  m_mainFrame;
    LayoutTestController*   m_layoutTestController;
    GCController*           m_gcController;
};

DumpRenderTree::DumpRenderTree()
    : m_waitUntilDone(false)
    , m_mainFrame(NULL)
    , m_layoutTestController(NULL)
{
    m_layoutTestController = new LayoutTestController( false, false);
    m_gcController = new GCController();
    OWBAL::ObserverServiceData::createObserverService()->registerObserver("layoutTestController", this);
}
DumpRenderTree::~DumpRenderTree()
{
    OWBAL::ObserverServiceData::createObserverService()->removeObserver("layoutTestController", this);
}

void DumpRenderTree::observe(const String& topic, const String& data, void*)
{
    // for now implement callbacks with an observer
    if ((data == "loadDone") && !m_waitUntilDone)
        dump();
    else if (data == "waitUntilDone")
        //m_waitUntilDone = true;
        dump();
    else if (data == "notifyDone")
        dump();
}


void DumpRenderTree::dump()
{
    Document* my_doc;
    my_doc = m_mainFrame->contentRenderer()->document();
    bool dumpAsText = false;

    dumpAsText = m_layoutTestController->dumpAsText();

    if (dumpAsText)
    {
        printf("%s\n", my_doc->body()->innerText().utf8().data());
    }
    else
    {
        String txt = externalRepresentation(m_mainFrame->contentRenderer());
        CString utf8Str = txt.utf8();
        const char *utf8 = utf8Str.data();
        if (utf8)
            printf("%s", utf8);
        else
            printf("FrameBal::dumpRenderTree() no data\n");
    }
    stopEventLoop();
}
void DumpRenderTree::initJSObjects(WebCore::Frame* frame)
{
    m_mainFrame = frame;

    JSValueRef exception = 0;

    JSContextRef context = toRef(frame->script()->globalObject()->globalExec());
    JSObjectRef windowObject = toRef(frame->script()->globalObject());
    ASSERT(windowObject);
    m_layoutTestController->makeWindowObject(context, windowObject, &exception);
    m_gcController->makeWindowObject(context, windowObject, &exception);

    JSStringRef eventSenderStr = JSStringCreateWithUTF8CString("eventSender");
    JSValueRef eventSender = makeEventSender(context);
    JSObjectSetProperty(context, windowObject, eventSenderStr, eventSender, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete, 0);
    JSStringRelease(eventSenderStr);
}

void DumpRenderTree::handleEvent() {
    //printf("DumpRenderTree::handleEvent\n");
}


static void crashHandler(int sig)
{
    fprintf(stderr, "%s\n", strsignal(sig));
    exit(128 + sig);
}

void runTest(const char* filename)
{
    DumpRenderTree dumpRenderTree;

    webView = WebView::createInstance();

    IntRect clientRect(0, 0, 800, 600);
   
    webView->initWithFrame(clientRect,"", "");

    BalWidget *view = createWindow(webView);

    dumpRenderTree.initJSObjects(core(webView->mainFrame()));

    WorkQueue::shared()->clear();
    WorkQueue::shared()->setFrozen(false);

    webView->mainFrame()->loadURL(filename);

    dumpRenderTree.handleEvent();
    
    startEventLoop(view);
    
    WorkQueue::shared()->clear();

    delete webView;
}

void init(int argc, char *argv[]);

int
main(int argc, char *argv[])
{
    init(argc, argv);
#ifdef BAL_LOG // logger is not defined in NDEBUG
    // disable logging
    //BALFacilities::logger.setIsActive(false);
#endif

    signal(SIGILL, crashHandler);    /* 4:   illegal instruction (not reset when caught) */
    signal(SIGTRAP, crashHandler);   /* 5:   trace trap (not reset when caught) */
    signal(SIGFPE, crashHandler);    /* 8:   floating point exception */
    signal(SIGBUS, crashHandler);    /* 10:  bus error */
    signal(SIGSEGV, crashHandler);   /* 11:  segmentation violation */
    signal(SIGSYS, crashHandler);    /* 12:  bad argument to system call */
    signal(SIGPIPE, crashHandler);   /* 13:  write on a pipe with no reader */
    signal(SIGXCPU, crashHandler);   /* 24:  exceeded CPU time limit */
    signal(SIGXFSZ, crashHandler);   /* 25:  exceeded file size limit */

    // mandatory but do not overwrite
    // the following disables some outputs
    setenv("LAYOUT_TEST", "1", 0);
    // this disables the GraphicsDevice and doesn't display anything
    setenv("DISABLE_DISPLAY", "1", 0);

    if( argc > 2 ) {
        printf("Usage: %s /absolute/file/path/or/url\n", argv[0] );
    } else if (argc == 2 && strcmp(argv[1], "-")) {
        runTest(argv[1]);
        printf("#EOF\n");
    } else {
        char filenameBuffer[2048];
        while (fgets(filenameBuffer, sizeof(filenameBuffer), stdin)) {
            char* newLineCharacter = strchr(filenameBuffer, '\n');
            if (newLineCharacter)
                *newLineCharacter = '\0';

            if (strlen(filenameBuffer) == 0)
                continue;

            runTest(filenameBuffer);

            fprintf(stdout, "#EOF\n");
            fflush(stdout);

            fprintf(stderr, "#EOF\n");
            fflush(stderr);
        }
    }

    // ENV var set will be removed
    return 0;
}

