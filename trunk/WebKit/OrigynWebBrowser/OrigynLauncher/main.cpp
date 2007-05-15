/**
 * @file  main.cpp
 *
 * @brief bal launcher
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date: 2006/05/11 13:44:56 $
 *
 * This header file is private. Only IDL interface is public.
 *
 */
#include "config.h"

#include "BALConfiguration.h"
#include "BIEventLoop.h"
#include "BIKeyboardEvent.h"
#include "BIWindowEvent.h"
#include "BIWebView.h"
#include "BTLogHelper.h"
#include "BTDeviceChannel.h"
#include "BTTextLogFormatter.h"
#include "CookieJar.h"
#include "KURL.h"
#include "wtf/RefPtr.h"

using namespace BAL;

static void handle_event( BIEventLoop& aEventLoop ) {
    BIEvent* aEvent;
    bool isQuitCalled = false;
    bool isEventValid = false;
    while (!isQuitCalled)
    {
        isEventValid = aEventLoop.WaitEvent(aEvent);
        if (isEventValid) {
            BIKeyboardEvent* aKeyboardEvent = aEvent->queryIsKeyboardEvent();
            if(aKeyboardEvent)
            {
                if(aKeyboardEvent->virtualKeyCode() == BAL::BIKeyboardEvent::VK_ESCAPE ) {
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

int
main(int argc, char *argv[])
{
#ifdef BAL_LOG // logger is not defined in NDEBUG
    // Setup a nice formatter for log
    RefPtr<BALFacilities::BTDeviceChannel> channel = new BALFacilities::BTDeviceChannel();
    RefPtr<BALFacilities::BTTextLogFormatter> textLogFormatter
            = new BALFacilities::BTTextLogFormatter();
    channel->setFormatter(textLogFormatter.get());
    BALFacilities::logger.addChannel(channel.get());
#endif

    BIEventLoop* aEventLoop = getBIEventLoop();
    if( aEventLoop == NULL ) {
      printf("No event loop\n");
      return 1;
    }

    BIWebView *view = getBIWebView();

    if(::WebCore::cookiesEnabled())
        getBICookieJar();

    if( argc >= 2 ) {
        view->setURL(argv[1]);
    } else {
        view->setURL("http://www.google.com");
    }
    handle_event( *aEventLoop );
    delete view;
    delete aEventLoop;

    return 0;
}
