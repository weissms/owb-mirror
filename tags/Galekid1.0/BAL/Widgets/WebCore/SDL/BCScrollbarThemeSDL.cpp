/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "ScrollbarThemeSDL.h"

#include "BALBase.h"
#include "ChromeClient.h"
#include "Frame.h"
#include "FrameView.h"
#include "GraphicsContext.h"
#include "Logging.h"
#include "Page.h"
#include "PlatformMouseEvent.h"
#include "GraphicsContext.h"
#include "Scrollbar.h"
#include "ScrollbarClient.h"
#include "Settings.h"

namespace WKAL {

ScrollbarTheme* ScrollbarTheme::nativeTheme()
{
    static ScrollbarThemeBal theme;
    return &theme;
}

ScrollbarThemeBal::~ScrollbarThemeBal()
{
}


int ScrollbarThemeBal::scrollbarThickness(ScrollbarControlSize controlSize)
{
    BalNotImplemented();
    return 11;
}

bool ScrollbarThemeBal::hasThumb(Scrollbar* scrollbar)
{
    return thumbLength(scrollbar) > 0;
}

IntRect ScrollbarThemeBal::backButtonRect(Scrollbar* scrollbar, ScrollbarPart part, bool)
{
    if (part == BackButtonEndPart)
        return IntRect();

    // Our desired rect is essentially 17x17.

    // Our actual rect will shrink to half the available space when
    // we have < 34 pixels left.  This allows the scrollbar
    // to scale down and function even at tiny sizes.
    int thickness = scrollbarThickness();
    if (scrollbar->orientation() == HorizontalScrollbar)
        return IntRect(scrollbar->x(), scrollbar->y(),
                       scrollbar->width() < 2 * thickness ? scrollbar->width() / 2 : thickness, thickness);
    return IntRect(scrollbar->x(), scrollbar->y(),
                   thickness, scrollbar->height() < 2 * thickness ? scrollbar->height() / 2 : thickness);
}

IntRect ScrollbarThemeBal::forwardButtonRect(Scrollbar* scrollbar, ScrollbarPart part, bool)
{
    if (part == ForwardButtonStartPart)
        return IntRect();
    
    // Our desired rect is essentially 17x17.

    // Our actual rect will shrink to half the available space when
    // we have < 34 pixels left.  This allows the scrollbar
    // to scale down and function even at tiny sizes.
    int thickness = scrollbarThickness();
    if (scrollbar->orientation() == HorizontalScrollbar) {
        int w = scrollbar->width() < 2 * thickness ? scrollbar->width() / 2 : thickness;
        return IntRect(scrollbar->x() + scrollbar->width() - w, scrollbar->y(), w, thickness);
    }

    int h = scrollbar->height() < 2 * thickness ? scrollbar->height() / 2 : thickness;
    return IntRect(scrollbar->x(), scrollbar->y() + scrollbar->height() - h, thickness, h);
}

IntRect ScrollbarThemeBal::trackRect(Scrollbar* scrollbar, bool)
{
    int thickness = scrollbarThickness();
    if (scrollbar->orientation() == HorizontalScrollbar) {
        if (scrollbar->width() < 2 * thickness)
            return IntRect();
        return IntRect(scrollbar->x() + thickness, scrollbar->y(), scrollbar->width() - 2 * thickness, thickness);
    }
    if (scrollbar->height() < 2 * thickness)
        return IntRect();
    return IntRect(scrollbar->x(), scrollbar->y() + thickness, thickness, scrollbar->height() - 2 * thickness);
}

void ScrollbarThemeBal::paintButton(GraphicsContext* context, Scrollbar* scrollbar, const IntRect& rect, ScrollbarPart part)
{
    context->save();
    context->drawRect(rect);
    context->fillRect(rect, Color::gray);

    bool start = (part == BackButtonStartPart);
    if (start) {
        if (scrollbar->orientation() == HorizontalScrollbar) {
            context->drawLine(IntPoint(rect.right(), rect.y()), IntPoint(rect.x(), (rect.bottom() + rect.y())/2));
            context->drawLine(IntPoint(rect.right(), rect.bottom()), IntPoint(rect.x(), (rect.bottom() + rect.y())/2));
        } else {
            context->drawLine(IntPoint(rect.x(), rect.bottom()), IntPoint((rect.x() + rect.right())/2, rect.y()));
            context->drawLine(IntPoint(rect.right(), rect.bottom()), IntPoint((rect.x() + rect.right())/2, rect.y()));
        }
    } else {
        if (scrollbar->orientation() == HorizontalScrollbar) {
            context->drawLine(IntPoint(rect.x(), rect.y()), IntPoint(rect.right(), (rect.bottom() + rect.y())/2));
            context->drawLine(IntPoint(rect.x(), rect.bottom()), IntPoint(rect.right(), (rect.bottom() + rect.y())/2));
        } else {
            context->drawLine(IntPoint(rect.x(), rect.y()), IntPoint((rect.x() + rect.right())/2, rect.bottom()));
            context->drawLine(IntPoint(rect.right(), rect.y()), IntPoint((rect.x() + rect.right())/2, rect.bottom()));
        }
    }
    context->restore();
}

void ScrollbarThemeBal::paintThumb(GraphicsContext* context, Scrollbar* scrollbar, const IntRect& rect)
{
    context->save();
    context->drawRect(rect);
    context->fillRect(rect, Color::gray);
    context->restore();
}


void ScrollbarThemeBal::paintTrackBackground(GraphicsContext* context, Scrollbar* scrollbar, const IntRect& rect)
{
    // Just assume a forward track part.  We only paint the track as a single piece when there is no thumb.
    if (!hasThumb(scrollbar))
        paintTrackPiece(context, scrollbar, rect, ForwardTrackPart);
}

bool ScrollbarThemeBal::invalidateOnMouseEnterExit()
{
    return false;
}

void ScrollbarThemeBal::themeChanged()
{
}

bool ScrollbarThemeBal::shouldCenterOnThumb(Scrollbar*, const PlatformMouseEvent& evt)
{
    return evt.shiftKey() && evt.button() == LeftButton;
}

void ScrollbarThemeBal::paintTrackPiece(GraphicsContext* context, Scrollbar* scrollbar, const IntRect& rect, ScrollbarPart partType)
{
    context->save();
    context->drawRect(rect);
    context->fillRect(rect, Color::darkGray);
    context->restore();
}


}
