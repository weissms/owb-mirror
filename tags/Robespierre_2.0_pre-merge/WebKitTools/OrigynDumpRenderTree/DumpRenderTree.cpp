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
#include "BIGraphicsDevice.h"
#include "BIResourceHandleManager.h"
#include "BTDeviceChannel.h"
#include "BTTextLogFormatter.h"
#include "CString.h"
#include "Document.h"
#include "Frame.h"
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

using namespace WebCore;
using namespace BAL;

class DumpClient : public BIObserver, public FrameLoaderClientBal {
public:
    DumpClient();
    ~DumpClient();
    virtual void dispatchDidFinishLoad();
    virtual void dispatchDidFailProvisionalLoad(const ResourceError& error);
    /**
     * JSLayoutTestController notifies us.
     * @param(in) "layoutTestController"
     * @param(in) notifyDone, waitUntilDone
     */
    void observe(const String&, const String&);
private:
    void dump();
    bool m_waitUntilDone;
};

DumpClient::DumpClient()
    : m_waitUntilDone(false)
{
    BAL::getBIObserverService()->registerObserver("layoutTestController", this);
}
DumpClient::~DumpClient()
{
    BAL::getBIObserverService()->removeObserver("layoutTestController", this);
}

void DumpClient::observe(const String& topic, const String& data)
{
    if (data == "waitUntilDone")
        m_waitUntilDone = true;
    else if (data == "notifyDone")
        dump();
}
void DumpClient::dispatchDidFailProvisionalLoad(const ResourceError& error)
{
    printf("Error for '%s': %s\n", error.failingURL().deprecatedString().ascii(),
        error.localizedDescription().deprecatedString().ascii());

    getBIEventLoop()->quit();
}

void DumpClient::dispatchDidFinishLoad()
{
    if (m_waitUntilDone)
        return;
    else 
        dump();

}
void DumpClient::dump()
{
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
#ifdef __OWB_JS__
    dumpAsText = KJS::JSLayoutTestController::GetDumpAsText();
#endif //__OWB_JS__

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
        // EventLoop is a singleton, but destroy it as some events may be left in and thus interfere with next test
        m_eventLoop = 0;
        BAL::deleteBIEventLoop();
        // m_loaderClient will destroy itself
    }
    BAL::BIEventLoop*       m_eventLoop;
    FrameLoaderClientBal*   m_loaderClient;

    void handleEvent();
protected:
    void serializeToStdOut(WebCore::Frame* /*frame*/);
};


void DumpRenderTree::serializeToStdOut(WebCore::Frame* frame)
{
    DeprecatedString markup = createMarkup( frame->document() );
    fprintf( stderr, "Source:\n\n%s", markup.ascii());
}

void DumpRenderTree::handleEvent() {
    BIEvent* aEvent = NULL;
    double startTime = WebCore::currentTime();
    // wait for QUIT event or 5s and exit
    bool isEventValid = true;
    while (isEventValid)
    {
        isEventValid = m_eventLoop->WaitEvent(aEvent);
        if (isEventValid) {
            BIWindowEvent* aWindowEvent = aEvent->queryIsWindowEvent();
            if( aWindowEvent && aWindowEvent->type() == BAL::BIWindowEvent::QUIT )
            {
                break; // stop loop
            }

            // In other cases, event is handled by frame
            //tBIWindowManager()->handleEvent(aEvent);
            delete aEvent;
        }
        else {
        
        }
        if (WebCore::currentTime() - startTime > 5.0) {
            printf("Timeout !\n");
            BAL::getBIEventLoop()->quit();
        }
        aEvent = NULL;
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

    BIWindow *window = getBIWindowManager()->openWindow(0, 0, 800, 600);
    window->setFrameLoaderClient(dumpRenderTree.m_loaderClient);

    window->setURL(filename);

    dumpRenderTree.handleEvent();

    getBIWindowManager()->closeWindow(window);
    deleteBIWindowManager();
    deleteBIResourceHandleManager();
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
    } else if (argc == 2 && strcmp(argv[1], "-")) {
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
    fflush(stdout);
    return 0;
}

