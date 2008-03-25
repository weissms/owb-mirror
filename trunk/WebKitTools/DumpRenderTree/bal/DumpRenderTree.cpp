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

#include "BALConfiguration.h"
#include "BIEventLoop.h"
#include "BIGraphicsDevice.h"
#include "BIResourceHandleManager.h"
#include "BTDeviceChannel.h"
#include "BTTextLogFormatter.h"
#include "CString.h"
#include "Document.h"
#include "Frame.h"
#include "FrameBal.h"
#include "FrameLoaderClientBal.h"
#include "HTMLElement.h"
#ifdef __OWB_JS__
#include "JSLayoutTestController.h"
#endif //__OWB_JS__
#include "KURL.h"
#include "markup.h"
#include "Page.h"
#include "RenderTreeAsText.h"
#include <iostream> // NOTE log define interferes with iostream
#include <signal.h>
#include "SystemTime.h"
#include "BTLogHelper.h"
#include "BIObserver.h"
#include "BIObserverService.h"
#include "BIWindow.h"
#include "BIWindowManager.h"

#include "bindings/runtime.h"
#include "bindings/runtime_root.h"
#include "ExecState.h"
#include "kjs/JSGlobalObject.h"
#include "kjs_window.h"
#include "JavaScriptCore/APICast.h"
#include "LayoutTestController.h"
#include "object.h"
#include "WorkQueue.h"


using namespace WebCore;
using namespace BAL;


/**
 * The BIWebView implementation for DumpRenderTree
 *
 * @see Font, FontData
 */
class DumpRenderTree : public BIObserver {
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
    void observe(const String&, const String&);
protected:
    void dump();

    bool                    m_waitUntilDone;
    Frame*                  m_mainFrame;
    LayoutTestController*   m_layoutTestController;
    BAL::BIEventLoop*       m_eventLoop;
};

DumpRenderTree::DumpRenderTree()
    : m_waitUntilDone(false)
    , m_mainFrame(NULL)
    , m_layoutTestController(NULL)
{
    m_eventLoop = BAL::getBIEventLoop();
    m_layoutTestController = new LayoutTestController(0, 0);
    if (m_eventLoop == NULL) {
        DBG("No event loop\n");
        exit(1);
    }
    BAL::getBIObserverService()->registerObserver("layoutTestController", this);
}
DumpRenderTree::~DumpRenderTree()
{
    // EventLoop is a singleton, but destroy it as some events may be left in and thus interfere with next test
    m_eventLoop = 0;
    // FIXME don't delete event loop for now, it won't setup again correctly
    BAL::deleteBIEventLoop();
    BAL::getBIObserverService()->removeObserver("layoutTestController", this);
}

void DumpRenderTree::observe(const String& topic, const String& data)
{
    // for now implement callbacks with an observer
    if ((data == "loadDone") && !m_waitUntilDone)
        dump();
    else if (data == "waitUntilDone")
        m_waitUntilDone = true;
    else if (data == "notifyDone")
        dump();
}

void DumpRenderTree::dump()
{
    Document* my_doc;
    my_doc = m_mainFrame->renderer()->document();
    bool dumpAsText = false;

    dumpAsText = m_layoutTestController->dumpAsText();

    if (dumpAsText)
    {
        printf("%s\n", my_doc->body()->innerText().deprecatedString().ascii());
    }
    else
    {
        String txt = externalRepresentation(m_mainFrame->renderer());
        CString utf8Str = txt.utf8();
        const char *utf8 = utf8Str.data();
        if (utf8)
            printf("%s", utf8);
        else
            printf("FrameBal::dumpRenderTree() no data\n");
    }

    BAL::getBIEventLoop()->quit();

}
void DumpRenderTree::initJSObjects(WebCore::Frame* frame)
{
    m_mainFrame = frame;

    JSValueRef exception = 0;

    KJS::Bindings::RootObject *root = frame->bindingRootObject();
    KJS::ExecState *exec = root->interpreter()->globalExec();
    KJS::JSGlobalObject *rootObject = root->interpreter()->globalObject();
    KJS::JSObject *window = rootObject->get(exec, KJS::Identifier("window"))->getObject();

    m_layoutTestController->makeWindowObject(toRef(exec), toRef(window), &exception);
}

void DumpRenderTree::handleEvent() {
    BIEvent* event = NULL;
    double startTime = WebCore::currentTime();
    // wait for QUIT event or 5s and exit
    bool isEventValid = true;

    while (true)
    {
        if (event) {    
            delete event;
            event = NULL;
        }
        isEventValid = m_eventLoop->WaitEvent(event);
        if (isEventValid) {
            BIWindowEvent* aWindowEvent = event->queryIsWindowEvent();
            if( aWindowEvent && aWindowEvent->type() == BAL::BIWindowEvent::QUIT )
                break; // stop loop
        }

        if (WebCore::currentTime() - startTime > 5.0) {
            printf("WARNING Timeout occured !\n");
            BAL::getBIEventLoop()->quit();
        }
    }
}

void runTest(const char* filename)
{
    DumpRenderTree dumpRenderTree;

    BIWindow *window = getBIWindowManager()->openWindow(0, 0, 800, 600);

    dumpRenderTree.initJSObjects(window->mainFrame());

    WorkQueue::shared()->clear();
    WorkQueue::shared()->setFrozen(false);

    window->setURL(filename);

    dumpRenderTree.handleEvent();

    WorkQueue::shared()->clear();

    getBIWindowManager()->closeWindow(window);

    deleteBIWindowManager();
    deleteBIResourceHandleManager();
}

static void crashHandler(int sig)
{
    fprintf(stderr, "%s\n", strsignal(sig));
    exit(128 + sig);
}

int
main(int argc, char *argv[])
{
#ifdef BAL_LOG // logger is not defined in NDEBUG
    // disable logging
    BALFacilities::logger.setIsActive(false);
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
    } else {
        char filenameBuffer[2048];
        while (fgets(filenameBuffer, sizeof(filenameBuffer), stdin)) {
            char* newLineCharacter = strchr(filenameBuffer, '\n');
            if (newLineCharacter)
                *newLineCharacter = '\0';

            if (strlen(filenameBuffer) == 0)
                continue;

            runTest(filenameBuffer);

            puts("#EOF\n");
            fflush(stdout);
            if (ferror(stdin)) {
                printf("error stdin\n");
                break;
            }
            if (ferror(stdout)) {
                printf("error stdout\n");
                break;
            }
        }
    }

    // ENV var set will be removed
    return 0;
}

