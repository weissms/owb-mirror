#include "config.h"
#include <Api/WebFrame.h>
#include <Api/WebView.h>
#include "Page.h"
#include "Frame.h"
#include "FrameView.h"
#include "IntRect.h"

using namespace WebCore;

static WebView* webView;
volatile bool done;

void init(int argc, char *argv[])
{
}

BalWidget* createWindow(WebView *webView)
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
