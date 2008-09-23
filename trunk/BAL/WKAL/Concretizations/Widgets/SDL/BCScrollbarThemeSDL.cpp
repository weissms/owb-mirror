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
#include "GraphicsContext.h"
#include "Scrollbar.h"

namespace WKAL {

ScrollbarTheme* ScrollbarTheme::nativeTheme()
{
    static ScrollbarThemeBal theme;
    return &theme;
}

ScrollbarThemeBal::~ScrollbarThemeBal()
{
}

bool ScrollbarThemeBal::paint(Scrollbar* scrollbar, GraphicsContext* graphicsContext, const IntRect& damageRect)
{
    // Create the ScrollbarControlPartMask based on the damageRect
    ScrollbarControlPartMask scrollMask = NoPart;

    IntRect backButtonPaintRect;
    IntRect forwardButtonPaintRect;
    if (hasButtons(scrollbar)) {
        backButtonPaintRect = backButtonRect(scrollbar, true);
        if (damageRect.intersects(backButtonPaintRect))
            scrollMask |= BackButtonStartPart;
        forwardButtonPaintRect = forwardButtonRect(scrollbar, true);
        if (damageRect.intersects(forwardButtonPaintRect))
            scrollMask |= ForwardButtonStartPart;
    }

    IntRect startTrackRect;
    IntRect thumbRect;
    IntRect endTrackRect;
    IntRect trackPaintRect = trackRect(scrollbar, true);
    bool thumbPresent = hasThumb(scrollbar);
    if (thumbPresent) {
        IntRect track = trackRect(scrollbar);
        splitTrack(scrollbar, track, startTrackRect, thumbRect, endTrackRect);
        if (damageRect.intersects(thumbRect)) {
            scrollMask |= ThumbPart;
            if (trackIsSinglePiece()) {
                // The track is a single strip that paints under the thumb.
                // Add both components of the track to the mask.
                scrollMask |= BackTrackPart;
                scrollMask |= ForwardTrackPart;
            }
        }
        if (damageRect.intersects(startTrackRect))
            scrollMask |= BackTrackPart;
        if (damageRect.intersects(endTrackRect))
            scrollMask |= ForwardTrackPart;
    } else if (damageRect.intersects(trackPaintRect)) {
        scrollMask |= BackTrackPart;
        scrollMask |= ForwardTrackPart;
    }

    // Paint the track.
    if ((scrollMask & ForwardTrackPart) || (scrollMask & BackTrackPart)) {
        if (!thumbPresent || trackIsSinglePiece())
            paintTrack(graphicsContext, scrollbar, trackPaintRect, ForwardTrackPart | BackTrackPart);
        else {
            if (scrollMask & BackTrackPart)
                paintTrack(graphicsContext, scrollbar, startTrackRect, BackTrackPart);
            if (scrollMask & ForwardTrackPart)
                paintTrack(graphicsContext, scrollbar, endTrackRect, ForwardTrackPart);
        }
    }

    // Paint the back and forward buttons.
    if (scrollMask & BackButtonStartPart)
        paintButton(graphicsContext, scrollbar, backButtonPaintRect, BackButtonStartPart);
    if (scrollMask & ForwardButtonStartPart)
        paintButton(graphicsContext, scrollbar, forwardButtonPaintRect, ForwardButtonStartPart);
    
    // Paint the thumb.
    if (scrollMask & ThumbPart)
        paintThumb(graphicsContext, scrollbar, thumbRect);

    return true;
}

int ScrollbarThemeBal::scrollbarThickness(ScrollbarControlSize controlSize)
{
    BalNotImplemented();
    return 11;
    /*
    static int size;
    if (!size) {
        MozGtkScrollbarMetrics metrics;
        moz_gtk_get_scrollbar_metrics(&metrics);
        size = metrics.slider_width;
    }
    return size;
    */
}

bool ScrollbarThemeBal::hasThumb(Scrollbar* scrollbar)
{
    return thumbLength(scrollbar) > 0;
}

IntRect ScrollbarThemeBal::backButtonRect(Scrollbar* scrollbar, bool)
{
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

IntRect ScrollbarThemeBal::forwardButtonRect(Scrollbar* scrollbar, bool)
{
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

void ScrollbarThemeBal::splitTrack(Scrollbar* scrollbar, const IntRect& trackRect, IntRect& beforeThumbRect, IntRect& thumbRect, IntRect& afterThumbRect)
{
    // This function won't even get called unless we're big enough to have some combination of these three rects where at least
    // one of them is non-empty.
    int thickness = scrollbarThickness();
    int thumbPos = thumbPosition(scrollbar);
    if (scrollbar->orientation() == HorizontalScrollbar) {
        thumbRect = IntRect(trackRect.x() + thumbPos, trackRect.y() + (trackRect.height() - thickness) / 2, thumbLength(scrollbar), thickness);
        beforeThumbRect = IntRect(trackRect.x(), trackRect.y(), thumbPos, trackRect.height());
        afterThumbRect = IntRect(thumbRect.x() + thumbRect.width(), trackRect.y(), trackRect.right() - thumbRect.right(), trackRect.height());
    } else {
        thumbRect = IntRect(trackRect.x() + (trackRect.width() - thickness) / 2, trackRect.y() + thumbPos, thickness, thumbLength(scrollbar));
        beforeThumbRect = IntRect(trackRect.x(), trackRect.y(), trackRect.width(), thumbPos);
        afterThumbRect = IntRect(trackRect.x(), thumbRect.y() + thumbRect.height(), trackRect.width(), trackRect.bottom() - thumbRect.bottom());
    }
}

void ScrollbarThemeBal::paintTrack(GraphicsContext* context, Scrollbar* scrollbar, const IntRect& rect, ScrollbarControlPartMask mask)
{
    context->save();
    context->drawRect(rect);
    context->fillRect(rect, Color::darkGray);
    context->restore();
}

void ScrollbarThemeBal::paintButton(GraphicsContext* context, Scrollbar* scrollbar, const IntRect& rect, ScrollbarControlPartMask mask)
{
    context->save();
    context->drawRect(rect);
    context->fillRect(rect, Color::gray);

    bool start = mask & BackButtonStartPart;
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

int ScrollbarThemeBal::thumbPosition(Scrollbar* scrollbar)
{
    if (scrollbar->enabled())
        return static_cast<int> (scrollbar->currentPos() * (trackLength(scrollbar) - thumbLength(scrollbar)) / scrollbar->maximum());
    return 0;
}

int ScrollbarThemeBal::trackLength(Scrollbar* scrollbar)
{
    return (scrollbar->orientation() == HorizontalScrollbar) ? trackRect(scrollbar).width() : trackRect(scrollbar).height();
}

int ScrollbarThemeBal::thumbLength(Scrollbar* scrollbar)
{
    if (!scrollbar->enabled())
        return 0;
    float proportion = (float)(scrollbar->visibleSize()) / scrollbar->totalSize();
    int trackLen = trackLength(scrollbar);
    int length = static_cast<int> (proportion * trackLen);
    int minLength = (scrollbar->orientation() == HorizontalScrollbar) ? scrollbarThickness() : scrollbarThickness();
    length = (length > minLength) ? length : minLength;
    if (length > trackLen)
        length = 0; // Once the thumb is below the track length, it just goes away (to make more room for the track).
    return length;
}

}
