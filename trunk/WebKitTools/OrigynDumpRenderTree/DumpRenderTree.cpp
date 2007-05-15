/*
 *          Copyright 2006 Origyn
 */

/**
 * @file  DumpRenderTree.cpp
 *
 * @brief This file dumps the render tree, of a given url. This
 *   file has been created from GdkLauncher, with dump code
 *   from the "win32" dump tool.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 *
 */

#include "config.h"

#include "BALConfiguration.h"
#include "BIEventLoop.h"
#include "BIWebView.h"
#include "BTDeviceChannel.h"
#include "BTTextLogFormatter.h"
#include "CString.h"
#include "Document.h"
#include "Frame.h"
#include "FrameLoaderClientBal.h"
#include "HTMLElement.h"
#include "JSLayoutTestController.h"
#include "KURL.h"
#include "markup.h"
#include "RenderTreeAsText.h"
#include <iostream> // NOTE log define interferes with iostream
#include <signal.h>
#include "SystemTime.h"
#include "BTLogHelper.h"

using namespace WebCore;
using namespace BAL;

class DumpClient : public FrameLoaderClientBal {
        virtual void frameLoadCompleted();
};

void DumpClient::frameLoadCompleted() {
    Document* my_doc;
    my_doc = frame()->renderer()->document();
    bool dumpAsText = false;

    // normal way to access js object follows. But this may crash at ~Frame()
    // so using a static var in JSLayoutTestController instead
#if 0
    KJS::JSValue *controllerProperty, *dumpProperty;
    KJS::ExecState *execState =
            KJS::Window::retrieveWindow(aFrame)->interpreter()->globalExec();
    controllerProperty =
            KJS::Window::retrieveWindow(aFrame)->getValueProperty(execState,
            KJS::Window::layoutTestController);
    dumpProperty = (static_cast<KJS::JSLayoutTestController*>(controllerProperty))->
        getValueProperty(execState, KJS::JSLayoutTestController::DumpAsText_attr);

    dumpAsText = dumpProperty->getBoolean();
#endif
    dumpAsText = KJS::JSLayoutTestController::GetDumpAsText();

    if (dumpAsText)
    {
        printf("%s\n", my_doc->body()->innerText().deprecatedString().ascii());
    }
    else
    {
        String txt = externalRepresentation(frame()->renderer());
        CString utf8Str = txt.utf8();
        const char *utf8 = utf8Str.data();
        if (utf8)
            printf("%s", utf8);
        else
            printf("FrameBal::dumpRenderTree() no data\n");
//        DeprecatedString renderDump = externalRepresentation(aFrame->renderer());
//        printf( "%s", renderDump.ascii());
    }
        BAL::getBIEventLoop()->quit();

}
/**
 * The BIWebView implementation for DumpRenderTree
 *
 * @see Font, FontData
 */
class DumpRenderTree {
public:
    DumpRenderTree() {
        m_eventLoop = BAL::getBIEventLoop();
        m_loaderClient = new DumpClient;
    }
    ~DumpRenderTree() {
        // BCEventLoop is a singleton, do not destroy (may be used later by runTest)
        m_eventLoop = 0;
        // m_loaderClient will destroy itself
    }
    BAL::BIEventLoop*       m_eventLoop;
    FrameLoaderClientBal*   m_loaderClient;

    void handleEvent();
protected:
//    void dumpRenderTreeToStdOut(WebCore::Frame* aFrame);
    void serializeToStdOut(WebCore::Frame* /*frame*/);
};


void DumpRenderTree::serializeToStdOut(WebCore::Frame* frame)
{
    DeprecatedString markup = createMarkup( frame->document() );
    fprintf( stderr, "Source:\n\n%s", markup.ascii());
}

void DumpRenderTree::handleEvent() {
    BIEvent* aEvent;
    double startTime = WebCore::currentTime();
    // wait for QUIT event or 5s and exit
    while (WebCore::currentTime() - startTime < 5.0)
    {
        bool isEventValid = m_eventLoop->WaitEvent(aEvent);
        if (isEventValid) {
            BIKeyboardEvent* aKeyboardEvent = aEvent->queryIsKeyboardEvent();
            if(aKeyboardEvent)
            {
                if(aKeyboardEvent->virtualKeyCode() == BAL::BIKeyboardEvent::VK_ESCAPE )
                {
                    break; // stop loop
                }
            }

            BIWindowEvent* aWindowEvent = aEvent->queryIsWindowEvent();
            if( aWindowEvent && aWindowEvent->type() == BAL::BIWindowEvent::QUIT )
            {
                break; // stop loop
            }

            // In other cases, event is handled by frame
            getBIWebView()->handleEvent( aEvent );
            delete aEvent;
        }
    }
}

void runTest(const char* filename)
{
    DumpRenderTree dumpRenderTree;

    if( dumpRenderTree.m_eventLoop == NULL ) {
      log("No event loop\n");
      return;
    }

    // mandatory but do not overwrite
    setenv("LAYOUT_TEST", "1", 0);
    setenv("DISABLE_DISPLAY", "1", 0);
    BIWebView *view = getBIWebView();
    view->setFrameLoaderClient(dumpRenderTree.m_loaderClient);
    view->setURL(filename);
    dumpRenderTree.handleEvent();
    deleteBIWebView();

    // ENV var set will be removed
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

    if( argc > 2 ) {
        printf("Usage: %s /absolute/file/path/or/url\n", argv[0] );
    } else if (argc == 2) {
        runTest(argv[1]);
    } else {
        // to be used with run-webkit-tests
        // waits for filenames on stdin and then run test
        char filenameBuffer[2048];
        while (fgets(filenameBuffer, sizeof(filenameBuffer), stdin)) {
            char *newLineCharacter = strchr(filenameBuffer, '\n');
            if (newLineCharacter)
                *newLineCharacter = '\0';

            if (strlen(filenameBuffer) == 0)
                continue;

            log(filenameBuffer);
            runTest(filenameBuffer);
            puts("#EOF");
            fflush(stdout);
        }
    }
    fflush(stdout);
    return 0;
}

