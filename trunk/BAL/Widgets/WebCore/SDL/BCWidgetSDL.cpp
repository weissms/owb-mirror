/*
 * Copyright (C) 2008 Pleyo.  All rights reserved.
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


#include "config.h"
#include "Widget.h"

#include "Cursor.h"
#include "FrameView.h"
#include "GraphicsContext.h"
#include "IntRect.h"
#include "Logging.h"
#include "RenderObject.h"
#if PLATFORM(AMIGAOS4)
#include "HostWindow.h"
#include <proto/intuition.h>
#else
#include "SDL.h"
#endif

namespace WebCore {

#if PLATFORM(AMIGAOS4)
typedef void BalCursor;
#else
typedef SDL_Cursor BalCursor;
#endif

//TODO : redo the WidgetPrivate on SDL
class WidgetPrivate {
public:
	BalCursor* cursor;
};

Widget::Widget(PlatformWidget widget)
{
    init(widget);
}

Widget::~Widget()
{
    ASSERT(!parent());
}

void Widget::setFocus()
{
    NotImplemented();
}

void Widget::setCursor(const Cursor& cursor)
{
#if PLATFORM(AMIGAOS4)
    if (!isFrameView())
        return;

    static const Cursor *waitCursorPtr = &waitCursor();
    HostWindow *hostWindow = static_cast<FrameView*>(this)->hostWindow();
    BalWidget *widget = hostWindow ? hostWindow->platformWindow() : 0;
    Window *window = widget ? widget->window : 0;

    if (window)
        if (waitCursorPtr == &cursor && !cursor.impl())
            IIntuition->SetWindowPointer(window, WA_BusyPointer, TRUE, TAG_DONE);
        else if (widget->curentCursor != cursor.impl()) {
            IIntuition->SetWindowPointer(window, WA_Pointer, cursor.impl(), TAG_DONE);
            widget->curentCursor = cursor.impl();
        }
#endif
}

void Widget::show()
{
    NotImplemented();
}

void Widget::hide()
{
    NotImplemented();
}

void Widget::paint(GraphicsContext* context, const IntRect &r)
{
    if (!platformWidget())
        return;

#if !PLATFORM(AMIGAOS4)
    if (!context->balExposeEvent())
        return;
    
    BalWidget* widget = platformWidget();
    SDL_Event ev;
    ev.type = SDL_VIDEOEXPOSE;
    SDL_PushEvent(&ev);
#endif
}

void Widget::setIsSelected(bool)
{
    NotImplemented();
}

void Widget::invalidateRect(const IntRect& rect)
{
/*    if (!parent()) {
        if (isFrameView())
            static_cast<FrameView*>(this)->addToDirtyRegion(rect);
        return;
    }

    // Get the root widget.
    ScrollView* outermostView = parent();
    while (outermostView && outermostView->parent())
        outermostView = outermostView->parent();
    if (!outermostView)
        return;

    IntRect windowRect = convertToContainingWindow(rect);
    outermostView->addToDirtyRegion(windowRect);*/
}

IntRect Widget::frameRect() const
{
    return m_frame;
}

void Widget::setFrameRect(const IntRect& rect)
{
	m_frame = rect;
}

}
