/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com
 * Copyright (C) 2007 Pleyo
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ScrollView.h"
#include "BIGraphicsContext.h"

#include <algorithm>
#include "assert.h"
#include "BALConfiguration.h"
#include "BIEventLoop.h"
#include "BIWheelEvent.h"
#include "BIWindow.h"
#include "BTLogHelper.h"
#include "Cursor.h"
#include "FloatRect.h"
#include "IntRect.h"
#include "PlatformScrollBar.h"

using namespace std;
using WebCore::ScrollbarAuto;
using WebCore::ScrollbarAlwaysOff;

namespace BAL
{

class BTScrollView::BTScrollViewPrivate
{
public:
    BTScrollViewPrivate()
            : hasStaticBackground(false)
            , suppressScrollBars(false)
            , vScrollbarMode(ScrollbarAuto)
            , hScrollbarMode(ScrollbarAuto)
    {}
    IntSize scrollOffset;
    IntSize contentsSize;
    bool hasStaticBackground;
    bool suppressScrollBars;
    ScrollbarMode vScrollbarMode;
    ScrollbarMode hScrollbarMode;
    IntRect viewportArea;
};


BTScrollView::BTScrollView()
        : m_data(new BTScrollViewPrivate())
{
}

BTScrollView::~BTScrollView()
{
    delete m_data;
}

void BTScrollView::setDrawable(BIWindow *drawable)
{
    BTWidget::setDrawable(drawable);
    m_data->viewportArea = IntRect(drawable->x(), drawable->y(), drawable->width(), drawable->height());
    m_data->contentsSize = IntSize(drawable->width(), drawable->height());
}

void BTScrollView::updateContents(const IntRect& updateRect, bool now)
{
    logm(MODULE_WIDGETS, make_message("viewportArea=%dx%d contentSize=%dx%d "
            "scrollOffset=%dx%d\nupdateRect=(%d,%d) %dx%d",
            visibleWidth(), visibleHeight(), contentsWidth(), contentsHeight(),
            m_data->scrollOffset.width(), m_data->scrollOffset.height(),
            updateRect.x(), updateRect.y(), updateRect.width(), updateRect.height()));
    IntRect adjustedDirtyRect(updateRect);
    adjustedDirtyRect.move(-m_data->scrollOffset);
    // do not redraw immediatly and synchronously :
    // page is not yet completely rendered and it will be displayed incomplete
    // Instead post an expose event which will call redraw() asynchronously
    BIWindowEvent *event;
    if (updateRect.isEmpty()) {
        adjustedDirtyRect.setWidth(contentsWidth());
        adjustedDirtyRect.setHeight(contentsHeight());
    }
    event = createBIWindowEvent(BIWindowEvent::EXPOSE, now, adjustedDirtyRect);
    getBIEventLoop()->PushEvent(event);
}

int BTScrollView::visibleWidth() const
{
    return m_data->viewportArea.width();
}

int BTScrollView::visibleHeight() const
{
    return m_data->viewportArea.height();
}

/**
Region of the content currently visible in the viewport
in the content views coordinate system
**/
FloatRect BTScrollView::visibleContentRect() const
{
    FloatRect contentRect = FloatRect(m_data->viewportArea);
    contentRect.move(m_data->scrollOffset);
    return contentRect;
}

void BTScrollView::setContentsPos(int newX, int newY)
{
    int dx = newX - contentsX();
    int dy = newY - contentsY();
    scrollBy(dx, dy);
}

void BTScrollView::resizeContents(int w,int h)
{
    IntSize newSize(w,h);
    if (m_data->contentsSize != newSize) {
        m_data->contentsSize = newSize;
//        updateScrollBars();
    }
}

int BTScrollView::contentsX() const
{
    return scrollOffset().width();
}

int BTScrollView::contentsY() const
{
    return scrollOffset().height();
}

int BTScrollView::contentsWidth() const
{
    return m_data->contentsSize.width();
}

int BTScrollView::contentsHeight() const
{
    return m_data->contentsSize.height();
}


IntPoint BTScrollView::windowToContents(const IntPoint& contentsPoint) const
{
  return contentsPoint + scrollOffset();
}

IntPoint BTScrollView::contentsToWindow(const IntPoint& viewportPoint) const
{
    return IntPoint(viewportPoint) - scrollOffset();
}

IntSize BTScrollView::scrollOffset() const
{
    return m_data->scrollOffset;
}

void BTScrollView::scrollBy(int dx, int dy)
{
    BALNotImplemented();
}

ScrollbarMode BTScrollView::hScrollbarMode() const
{
    return m_data->hScrollbarMode;
}

ScrollbarMode BTScrollView::vScrollbarMode() const
{
    return m_data->vScrollbarMode;
}

void BTScrollView::suppressScrollbars(bool suppressed, bool repaintOnSuppress)
{
    m_data->suppressScrollBars = suppressed;
//    if (repaintOnSuppress)
//        updateScrollBars();
}

void BTScrollView::setHScrollbarMode(ScrollbarMode newMode)
{
    if (m_data->hScrollbarMode != newMode) {
        m_data->hScrollbarMode = newMode;
//        updateScrollBars();
    }
}

void BTScrollView::setVScrollbarMode(ScrollbarMode newMode)
{
    if (m_data->vScrollbarMode != newMode) {
        m_data->vScrollbarMode = newMode;
//        updateScrollBars();
    }
}

void BTScrollView::setScrollbarsMode(ScrollbarMode newMode)
{
    m_data->hScrollbarMode = m_data->vScrollbarMode = newMode;
//    updateScrollBars();
}

void BTScrollView::setStaticBackground(bool flag)
{
    m_data->hasStaticBackground = flag;
}

void BTScrollView::scrollPointRecursively(int dx, int dy) {
  	BALNotImplemented();
}

void BTScrollView::addChild(Widget* widget) {
    BALNotImplemented();
}

void BTScrollView::removeChild(Widget* widget) {
    BALNotImplemented();
}

bool BTScrollView::inWindow() const {
    return true;
}

void BTScrollView::wheelEvent(PlatformWheelEvent&)
{
    BALNotImplemented();
}

PlatformScrollbar* BTScrollView::scrollbarUnderMouse(const PlatformMouseEvent& mouseEvent)
{
//    BALNotImplemented();
    return 0;
}

}
