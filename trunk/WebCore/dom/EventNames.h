/*
 * Copyright (C) 2005, 2007 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Jon Shier (jshier@iastate.edu)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef EventNames_h
#define EventNames_h

#include "AtomicString.h"
#include "ThreadGlobalData.h"

namespace WebCore {

#if ENABLE(CEHTML_VIDEO) || ENABLE(DAE_TUNER)
#define CEHTML_VIDEO_EVENT_NAMES(macro) \
    macro(playStateChange)
#else
#define CEHTML_VIDEO_EVENT_NAMES(macro)
#endif


#if ENABLE(HBBTV_0_8)
#define DAE_VIDEOBROADCAST_0_8_EVENT_NAMES(macro) \
    macro(programmesChanged)
#else
#define DAE_VIDEOBROADCAST_0_8_EVENT_NAMES(macro)
#endif

#if ENABLE(DAE)
#define DAE_APPLICATION_EVENT_NAMES(macro)  \
    macro(ApplicationActivated) \
    macro(ApplicationDeactivated) \
    macro(ApplicationDestroyRequest) \
    macro(ApplicationHidden) \
    macro(ApplicationShown) \
    macro(ApplicationPrimaryReceiver) \
    macro(ApplicationNotPrimaryReceiver) \
    macro(ApplicationTopmost) \
    macro(ApplicationNotTopmost)

#define DAE_SYSTEM_EVENT_NAMES(macro)  \
    macro(ApplicationLoaded) \
    macro(ApplicationUnloaded) \
    macro(KeyDown) \
    macro(KeyPress) \
    macro(KeyUp) \
    macro(LowMemory)

#define DAE_VIDEOBROADCAST_EVENT_NAMES(macro)  \
    macro(channelChangeError) \
    macro(channelChangeSucceeded) \
    macro(fullScreenChange) \
    macro(metadataSearch) \
    macro(parentalRatingChange) \
    macro(parentalRatingError) \
    macro(drmRightsError) \
    macro(channelScan)
#else
#define DAE_APPLICATION_EVENT_NAMES(macro)
#define DAE_SYSTEM_EVENT_NAMES(macro)
#define DAE_VIDEOBROADCAST_EVENT_NAMES(macro)
#endif

#if ENABLE(DAE_PVR)
#define DAE_PVR_EVENT_NAMES(macro) \
    macro(recordingChange)
#else
#define DAE_PVR_EVENT_NAMES(macro)
#endif

#define DOM_EVENT_NAMES_FOR_EACH(macro) \
    \
    macro(abort) \
    macro(beforecopy) \
    macro(beforecut) \
    macro(beforeload) \
    macro(beforepaste) \
    macro(beforeunload) \
    macro(blur) \
    macro(cached) \
    macro(change) \
    macro(checking) \
    macro(click) \
    macro(close) \
    macro(compositionend) \
    macro(compositionstart) \
    macro(compositionupdate) \
    macro(connect) \
    macro(contextmenu) \
    macro(copy) \
    macro(cut) \
    macro(dblclick) \
    macro(display) \
    macro(downloading) \
    macro(drag) \
    macro(dragend) \
    macro(dragenter) \
    macro(dragleave) \
    macro(dragover) \
    macro(dragstart) \
    macro(drop) \
    macro(error) \
    macro(focus) \
    macro(hashchange) \
    macro(input) \
    macro(invalid) \
    macro(keydown) \
    macro(keypress) \
    macro(keyup) \
    macro(load) \
    macro(loadstart) \
    macro(message) \
    macro(mousedown) \
    macro(mousemove) \
    macro(mouseout) \
    macro(mouseover) \
    macro(mouseup) \
    macro(mousewheel) \
    macro(noupdate) \
    macro(obsolete) \
    macro(offline) \
    macro(online) \
    macro(open) \
    macro(overflowchanged) \
    macro(pagehide) \
    macro(pageshow) \
    macro(paste) \
    macro(readystatechange) \
    macro(reset) \
    macro(resize) \
    macro(scroll) \
    macro(search) \
    macro(select) \
    macro(selectstart) \
    macro(storage) \
    macro(submit) \
    macro(textInput) \
    macro(unload) \
    macro(updateready) \
    macro(zoom) \
    \
    macro(DOMActivate) \
    macro(DOMAttrModified) \
    macro(DOMCharacterDataModified) \
    macro(DOMFocusIn) \
    macro(DOMFocusOut) \
    macro(DOMNodeInserted) \
    macro(DOMNodeInsertedIntoDocument) \
    macro(DOMNodeRemoved) \
    macro(DOMNodeRemovedFromDocument) \
    macro(DOMSubtreeModified) \
    macro(DOMContentLoaded) \
    \
    macro(webkitBeforeTextInserted) \
    macro(webkitEditableContentChanged) \
    \
    macro(canplay) \
    macro(canplaythrough) \
    macro(durationchange) \
    macro(emptied) \
    macro(ended) \
    macro(loadeddata) \
    macro(loadedmetadata) \
    macro(pause) \
    macro(play) \
    macro(playing) \
    macro(ratechange) \
    macro(seeked) \
    macro(seeking) \
    macro(timeupdate) \
    macro(volumechange) \
    macro(waiting) \
    \
    macro(webkitbeginfullscreen) \
    macro(webkitendfullscreen) \
    \
    macro(progress) \
    macro(stalled) \
    macro(suspend) \
    \
    macro(webkitAnimationEnd) \
    macro(webkitAnimationStart) \
    macro(webkitAnimationIteration) \
    \
    macro(webkitTransitionEnd) \
    \
    macro(orientationchange) \
    \
    CEHTML_VIDEO_EVENT_NAMES(macro) \
    \
    DAE_APPLICATION_EVENT_NAMES(macro) \
    DAE_SYSTEM_EVENT_NAMES(macro) \
    DAE_VIDEOBROADCAST_EVENT_NAMES(macro) \
    DAE_VIDEOBROADCAST_0_8_EVENT_NAMES(macro) \
    DAE_PVR_EVENT_NAMES(macro)
// end of DOM_EVENT_NAMES_FOR_EACH

    class EventNames : public Noncopyable {
        int dummy; // Needed to make initialization macro work.
        // Private to prevent accidental call to EventNames() instead of eventNames()
        EventNames();
        friend class ThreadGlobalData;

    public:
        #define DOM_EVENT_NAMES_DECLARE(name) AtomicString name##Event;
        DOM_EVENT_NAMES_FOR_EACH(DOM_EVENT_NAMES_DECLARE)
        #undef DOM_EVENT_NAMES_DECLARE
    };

    inline EventNames& eventNames()
    {
        return threadGlobalData().eventNames();
    }

}

#endif
