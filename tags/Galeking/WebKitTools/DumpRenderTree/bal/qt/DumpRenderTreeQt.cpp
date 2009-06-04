#include <WebKit.h>
#include <DumpRenderTree.h>

// Choose some default values.
const unsigned int maxViewWidth = 800;
const unsigned int maxViewHeight = 600;
unsigned int waitToDumpWatchdog = 0;

BalRectangle clientRect(bool isSVGW3CTest)
{
    return QRect(0, 0, 0, 0);
}

void init(int argc, char *argv[])
{
}

BalWidget* createWindow(WebView *webView, BalRectangle rect)
{
    return 0;
}

void startEventLoop(BalWidget *view)
{
    done = false;
}

void stopEventLoop()
{
    done = true;
}

static bool waitToDumpWatchdogFired(void*)
{
    const char* message = "FAIL: Timed out waiting for notifyDone to be called\n";
    fprintf(stderr, "%s", message);
    fprintf(stdout, "%s", message);
    waitToDumpWatchdog = 0;
    dump();
    return false;
}


void addTimetoDump(int timeoutSeconds)
{
}

void removeTimer()
{
}
