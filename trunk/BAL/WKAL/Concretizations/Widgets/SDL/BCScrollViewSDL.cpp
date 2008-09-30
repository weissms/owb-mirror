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
#include "ScrollView.h"

#include "FloatRect.h"
#include "FocusController.h"
#include "Frame.h"
#include "FrameView.h"
#include "GraphicsContext.h"
#include "IntRect.h"
#include "PlatformMouseEvent.h"
#include "PlatformWheelEvent.h"
#include "Page.h"
#include "RenderLayer.h"
#include "ScrollbarSDL.h"
#include "ScrollbarTheme.h"
#include "SDL.h"
#if PLATFORM(AMIGAOS4)
#define GRAPHICS_MONITOR_H
#include <exec/nodes.h>
#undef __amigaos4__
#include <graphics/gfxnodes.h>
#include <graphics/gels.h>
#define __amigaos4__
#include <proto/graphics.h>
#include <graphics/blitattr.h>
#include <proto/intuition.h>
#include <intuition/gadgetclass.h>
#include <proto/layers.h>
#endif

using std::max;
using std::min;

namespace WKAL {

#if PLATFORM(AMIGAOS4)
void updateView(BalWidget *widget, IntRect rect)
{
    if (!widget || !widget->window)
        return;

    int x = rect.x();
    int y = rect.y();
    int width = rect.width();
    int height = rect.height();

    if (width <= 0 || height <= 0)
        return;

    IGraphics->BltBitMapTags(BLITA_Source,         cairo_image_surface_get_data(widget->surface),
                             BLITA_SrcType,        BLITT_ARGB32,
                             BLITA_SrcBytesPerRow, cairo_image_surface_get_stride(widget->surface),
                             BLITA_SrcX,           x,
                             BLITA_SrcY,           y,
                             BLITA_Width,          width,
                             BLITA_Height,         height,
                             BLITA_Dest,           widget->window->RPort,
                             BLITA_DestType,       BLITT_RASTPORT,
                             BLITA_DestX,          x + widget->offsetx,
                             BLITA_DestY,          y + widget->offsety,
                             TAG_DONE);
}
#else
void updateView(SDL_Surface *surf, SDL_Rect sdlRect)
{
    if (!surf)
        return;
    //printf("updateView x=%d y=%d w=%d h=%d\n", sdlRect.x, sdlRect.y, sdlRect.w, sdlRect.h);
    /* use SDL_Flip only if double buffering is available */
    sdlRect.x = max(sdlRect.x, (Sint16)0);
    sdlRect.y = max(sdlRect.y, (Sint16)0);
    if (surf->flags & SDL_DOUBLEBUF)
        SDL_Flip(surf);
    else
        SDL_UpdateRect(surf, sdlRect.x, sdlRect.y, sdlRect.w, sdlRect.h);
}
#endif

class ScrollViewScrollbar : public ScrollbarSDL {
public:
    ScrollViewScrollbar(ScrollbarClient*, ScrollbarOrientation, ScrollbarControlSize);

protected:
    void frameRectsChanged() const;
};

class ScrollView::ScrollViewPrivate : public ScrollbarClient
{
public:
    ScrollViewPrivate(ScrollView* _view)
        : view(_view)
        , inUpdateScrollbars(false)
        , horizontalAdjustment(0)
        , verticalAdjustment(0)
    {}

    ~ScrollViewPrivate()
    {
        setHasHorizontalScrollbar(false);
        setHasVerticalScrollbar(false);
    }

    void scrollBackingStore(const IntSize& scrollDelta);

    void setHasHorizontalScrollbar(bool hasBar);
    void setHasVerticalScrollbar(bool hasBar);

    virtual void valueChanged(Scrollbar*);
    virtual IntRect windowClipRect() const;
    virtual bool isActive() const;

    //static void adjustmentChanged(BalAdjustment*, void *);

    ScrollView* view;
    bool hasStaticBackground;
    IntSize viewPortSize;
    bool inUpdateScrollbars;
    HashSet<Widget*> children;
    BalAdjustment* horizontalAdjustment;
    BalAdjustment* verticalAdjustment;
};

ScrollViewScrollbar::ScrollViewScrollbar(ScrollbarClient* client, ScrollbarOrientation orientation, ScrollbarControlSize size)
    : ScrollbarSDL(client, orientation, size)
{
}

void ScrollViewScrollbar::frameRectsChanged() const
{
    if (!parent())
        return;

    ASSERT(parent()->isFrameView());

    FrameView* frameView = static_cast<FrameView*>(parent());
    IntPoint loc = frameView->convertToContainingWindow(frameRect().location());

    // Don't allow the allocation size to be negative
    IntSize sz = frameRect().size();
    sz.clampNegativeToZero();

//     GtkAllocation allocation = { loc.x(), loc.y(), sz.width(), sz.height() };
//     gtk_widget_size_allocate(gtkWidget(), &allocation);
}

void ScrollView::ScrollViewPrivate::setHasHorizontalScrollbar(bool hasBar)
{
#if PLATFORM(AMIGAOS4)
    if (!view->parent())
        hasBar = false;
#endif
    if (hasBar && !view->m_horizontalScrollbar && !horizontalAdjustment) {
        view->m_horizontalScrollbar = Scrollbar::createNativeScrollbar(this, HorizontalScrollbar, RegularScrollbar);
        view->addChild(view->m_horizontalScrollbar.get());
    } else if (!hasBar && view->m_horizontalScrollbar) {
        view->removeChild(view->m_horizontalScrollbar.get());
        view->m_horizontalScrollbar = 0;
    }
}

void ScrollView::ScrollViewPrivate::setHasVerticalScrollbar(bool hasBar)
{
#if PLATFORM(AMIGAOS4)
    if (!view->parent())
        hasBar = false;
#endif
    if (hasBar && !view->m_verticalScrollbar && !verticalAdjustment) {
        view->m_verticalScrollbar = Scrollbar::createNativeScrollbar(this, VerticalScrollbar, RegularScrollbar);
        view->addChild(view->m_verticalScrollbar.get());
    } else if (!hasBar && view->m_verticalScrollbar) {
        view->removeChild(view->m_verticalScrollbar.get());
        view->m_verticalScrollbar = 0;
    }
}

void ScrollView::ScrollViewPrivate::scrollBackingStore(const IntSize& scrollDelta)
{
    // Since scrolling is double buffered, we will be blitting the scroll view's intersection
    // with the clip rect every time to keep it smooth.
    IntRect clipRect = view->windowClipRect();
    IntRect scrollViewRect = view->convertToContainingWindow(IntRect(0, 0, view->visibleWidth(), view->visibleHeight()));

    IntRect updateRect = clipRect;
    updateRect.intersect(scrollViewRect);

    //FIXME update here?

#if PLATFORM(AMIGAOS4)
    if (view->canBlitOnScroll()) { // The main frame can just blit the WebView window
        // FIXME: Find a way to blit subframes without blitting overlapping content

        int dx, dy;
        dx = scrollDelta.width();
        dy = scrollDelta.height();

        view->scrollBackingStore(-dx, -dy, scrollViewRect, clipRect);

        int svWidth = scrollViewRect.width();
        int svHeight = scrollViewRect.height();
        int dirtyX = 0, dirtyY = 0, dirtyW = 0, dirtyH = 0;

        if (dy == 0 && dx < 0 && -dx < svWidth) {
            dirtyW = -dx;
            dirtyH = svHeight;
        }
        else if (dy == 0 && dx > 0 && dx < svWidth) {
            dirtyX = svWidth - dx;
            dirtyW = dx;
            dirtyH = svHeight;
        }
        else if (dx == 0 && dy < 0 && -dy < svHeight) {
            dirtyW = svWidth;
            dirtyH = -dy;
        }
        else if (dx == 0 && dy > 0 && dy < svHeight) {
            dirtyY = svHeight - dy;
            dirtyW = svWidth;
            dirtyH = dy;
        }

        BalWidget *containingWindow = view->containingWindow();
        if (containingWindow && dirtyW) {
            RastPort *RPort = containingWindow->window->RPort;
            Layer *Layer = RPort->Layer;
            struct Hook *oldhook = ILayers->InstallLayerHook(Layer, LAYERS_NOBACKFILL);
            int x = containingWindow->offsetx + scrollViewRect.x();
            int y = containingWindow->offsety + scrollViewRect.y();
            IGraphics->ScrollRasterBF(RPort, dx, dy, x, y, x + svWidth - 1, y + svHeight - 1);
            ILayers->InstallLayerHook(Layer, oldhook);

            view->frameRectsChanged();
            view->addToDirtyRegion(IntRect(scrollViewRect.x() + dirtyX, scrollViewRect.y() + dirtyY, dirtyW, dirtyH));
            if (dx && hBar)
                view->addToDirtyRegion(IntRect(scrollViewRect.x() + hBar->x(), scrollViewRect.y() + hBar->y(), hBar->width(), hBar->height()));
            if (dy && vBar)
                view->addToDirtyRegion(IntRect(scrollViewRect.x() + vBar->x(), scrollViewRect.y() + vBar->y(), vBar->width(), vBar->height()));
            containingWindow->expose = true;
        }
        else {
            view->frameRectsChanged();
            view->update();
        }
    }
    else  {
        // We need to go ahead and repaint the entire backing store.  Do it now before moving the
        // plugins.
        view->addToDirtyRegion(updateRect);
        view->updateBackingStore();

        view->frameRectsChanged();

        // Now update the window (which should do nothing but a blit of the backing store's updateRect and so should
        // be very fast).
        view->update();
    }
#else
    if (view->canBlitOnScroll()) // The main frame can just blit the WebView window
       // FIXME: Find a way to blit subframes without blitting overlapping content
       view->scrollBackingStore(-scrollDelta.width(), -scrollDelta.height(), scrollViewRect, clipRect);
    else  {
       // We need to go ahead and repaint the entire backing store.  Do it now before moving the
       // plugins.
       view->addToDirtyRegion(updateRect);
       view->updateBackingStore();
    }

    view->frameRectsChanged();

    // Now update the window (which should do nothing but a blit of the backing store's updateRect and so should
    // be very fast).
    view->update();
#endif
}

/*void ScrollView::ScrollViewPrivate::adjustmentChanged(BalAdjustment* adjustment, void* _that)
{
    ScrollViewPrivate* that = reinterpret_cast<ScrollViewPrivate*>(_that);

    // Figure out if we really moved.
    IntSize newOffset = m_view->m_scrollOffset;
//     if (adjustment == that->horizontalAdjustment)
//         newOffset.setWidth(static_cast<int>(gtk_adjustment_get_value(adjustment)));
//     else if (adjustment == that->verticalAdjustment)
//         newOffset.setHeight(static_cast<int>(gtk_adjustment_get_value(adjustment)));

    IntSize scrollDelta = newOffset - m_view->m_scrollOffset;
    if (scrollDelta == IntSize())
        return;
    m_view->m_scrollOffset = newOffset;

    if (that->scrollbarsSuppressed)
        return;

    that->scrollBackingStore(scrollDelta);
    static_cast<FrameView*>(that->view)->frame()->sendScrollEvent();
}*/

void ScrollView::ScrollViewPrivate::valueChanged(Scrollbar* bar)
{
    // Figure out if we really moved.
    IntSize newOffset = view->m_scrollOffset;
    if (bar) {
        if (bar == view->m_horizontalScrollbar)
            newOffset.setWidth(bar->value());
        else if (bar == view->m_verticalScrollbar)
            newOffset.setHeight(bar->value());
    }
    IntSize scrollDelta = newOffset - view->m_scrollOffset;
    if (scrollDelta == IntSize())
        return;
    view->m_scrollOffset = newOffset;

    if (view->scrollbarsSuppressed())
        return;

    scrollBackingStore(scrollDelta);
    static_cast<FrameView*>(view)->frame()->sendScrollEvent();
}

IntRect ScrollView::ScrollViewPrivate::windowClipRect() const
{
    return static_cast<const FrameView*>(view)->windowClipRect(false);
}

bool ScrollView::ScrollViewPrivate::isActive() const
{
    Page* page = static_cast<const FrameView*>(view)->frame()->page();
    return page && page->focusController()->isActive();
}

ScrollView::ScrollView()
    : m_data(new ScrollViewPrivate(this))
{
    init();
}

ScrollView::~ScrollView()
{
    delete m_data;
}

/*
 * The following is assumed:
 *   (hadj && vadj) || (!hadj && !vadj)
 */
void ScrollView::setBalAdjustments(BalAdjustment* hadj, BalAdjustment* vadj)
{
    ASSERT(!hadj == !vadj);

    if (m_data->horizontalAdjustment) {
/*        g_signal_handlers_disconnect_by_func(G_OBJECT(m_data->horizontalAdjustment), (gpointer)ScrollViewPrivate::adjustmentChanged, m_data);
        g_signal_handlers_disconnect_by_func(G_OBJECT(m_data->verticalAdjustment), (gpointer)ScrollViewPrivate::adjustmentChanged, m_data);
        g_object_unref(m_data->horizontalAdjustment);
        g_object_unref(m_data->verticalAdjustment);*/
        ;
    }

    m_data->horizontalAdjustment = hadj;
    m_data->verticalAdjustment = vadj;

    if (m_data->horizontalAdjustment) {
//         g_signal_connect(m_data->horizontalAdjustment, "value-changed", G_CALLBACK(ScrollViewPrivate::adjustmentChanged), m_data);
//         g_signal_connect(m_data->verticalAdjustment, "value-changed", G_CALLBACK(ScrollViewPrivate::adjustmentChanged), m_data);

        /*
         * disable the scrollbars (if we have any) as the GtkAdjustment over
         */
        m_data->setHasVerticalScrollbar(false);
        m_data->setHasHorizontalScrollbar(false);

//         g_object_ref(m_data->horizontalAdjustment);
//         g_object_ref(m_data->verticalAdjustment);
    }

    updateScrollbars(m_scrollOffset);
}

void ScrollView::updateContents(const IntRect& updateRect, bool now)
{
    //printf("this=%p updateRect x=%d y=%d w=%d h=%d\n", this, updateRect.x(), updateRect.y(), updateRect.width(), updateRect.height());
    if (updateRect.isEmpty())
        return;

    IntPoint windowPoint = contentsToWindow(updateRect.location());
    IntRect containingWindowRect = updateRect;
    containingWindowRect.setLocation(windowPoint);

    //printf("updateContents containingWindowRect x=%d y=%d w=%d h=%d\n", containingWindowRect.x(), containingWindowRect.y(), containingWindowRect.width(), containingWindowRect.height());
    // Cache the dirty spot.
    addToDirtyRegion(containingWindowRect);

    if (/*now && */containingWindow()) {
#if PLATFORM(AMIGAOS4)
        containingWindow()->expose = true;
#else
        //updateView(containingWindow(), containingWindowRect);
        SDL_Event ev;
        ev.type = SDL_VIDEOEXPOSE;
        SDL_PushEvent(&ev);
#endif
    }
}

void ScrollView::update()
{
    ASSERT(containingWindow());

    IntRect documentDirtyRect = frameRect();
    //documentDirtyRect.move(scrollOffset().width(), scrollOffset().height());
    //printf("update documentDirtyRect x=%d y=%d w=%d h=%d\n", documentDirtyRect.x(), documentDirtyRect.y(), documentDirtyRect.width(), documentDirtyRect.height());
    addToDirtyRegion(documentDirtyRect);
#if PLATFORM(AMIGAOS4)
    if (containingWindow())
        containingWindow()->expose = true;
#else
    //updateView(containingWindow(), frameRect());
    SDL_Event ev;
    ev.type = SDL_VIDEOEXPOSE;
    SDL_PushEvent(&ev);
#endif
}

void ScrollView::setScrollPosition(const IntPoint& scrollPoint)
{
    IntPoint newScrollPosition = scrollPoint.shrunkTo(maximumScrollPosition());
    newScrollPosition.clampNegativeToZero();
    if (newScrollPosition == scrollPosition())
        return;
    updateScrollbars(IntSize(newScrollPosition.x(), newScrollPosition.y()));
}

void ScrollView::setFrameRect(const IntRect& newGeometry)
{
    ASSERT(isFrameView());
    IntRect oldGeometry = frameRect();
    Widget::setFrameRect(newGeometry);

    if (newGeometry == oldGeometry)
        return;
    if (newGeometry.width() != oldGeometry.width() || newGeometry.height() != oldGeometry.height()) {
        updateScrollbars(m_scrollOffset);
        static_cast<FrameView*>(this)->setNeedsLayout();
    }

    frameRectsChanged();
}

void ScrollView::platformAddChild(Widget* child)
{
}

void ScrollView::platformRemoveChild(Widget* child)
{
}

void ScrollView::updateScrollbars(const IntSize& desiredOffset)
{
    // Don't allow re-entrancy into this function.
    if (m_data->inUpdateScrollbars)
        return;

    // FIXME: This code is here so we don't have to fork FrameView.h/.cpp.
    // In the end, FrameView should just merge with ScrollView.
    Frame *frame = static_cast<const FrameView*>(this)->frame();
    if (frame && frame->prohibitsScrolling())
        return;

    m_data->inUpdateScrollbars = true;

    bool hasVerticalScrollbar = m_verticalScrollbar;
    bool hasHorizontalScrollbar = m_horizontalScrollbar;
    bool oldHasVertical = hasVerticalScrollbar;
    bool oldHasHorizontal = hasHorizontalScrollbar;
    ScrollbarMode hScroll = m_horizontalScrollbarMode;
    ScrollbarMode vScroll = m_verticalScrollbarMode;

    const int scrollbarThickness = ScrollbarTheme::nativeTheme()->scrollbarThickness();

#if PLATFORM(AMIGAOS4)
    if (parent())
#endif
    for (int pass = 0; pass < 2; pass++) {
        bool scrollsVertically;
        bool scrollsHorizontally;

        if (!m_scrollbarsSuppressed && (hScroll == ScrollbarAuto || vScroll == ScrollbarAuto)) {
            // Do a layout if pending before checking if scrollbars are needed.
            if (hasVerticalScrollbar != oldHasVertical || hasHorizontalScrollbar != oldHasHorizontal)
                static_cast<FrameView*>(this)->layout();

            scrollsVertically = (vScroll == ScrollbarAlwaysOn) || (vScroll == ScrollbarAuto && contentsHeight() > height());
            if (scrollsVertically)
                scrollsHorizontally = (hScroll == ScrollbarAlwaysOn) || (hScroll == ScrollbarAuto && contentsWidth() + scrollbarThickness > width());
            else {
                scrollsHorizontally = (hScroll == ScrollbarAlwaysOn) || (hScroll == ScrollbarAuto && contentsWidth() > width());
                if (scrollsHorizontally)
                    scrollsVertically = (vScroll == ScrollbarAlwaysOn) || (vScroll == ScrollbarAuto && contentsHeight() + scrollbarThickness > height());
            }
        }
        else {
            scrollsHorizontally = (hScroll == ScrollbarAuto) ? hasHorizontalScrollbar : (hScroll == ScrollbarAlwaysOn);
            scrollsVertically = (vScroll == ScrollbarAuto) ? hasVerticalScrollbar : (vScroll == ScrollbarAlwaysOn);
        }

        if (hasVerticalScrollbar != scrollsVertically) {
            m_data->setHasVerticalScrollbar(scrollsVertically);
            hasVerticalScrollbar = scrollsVertically;
        }

        if (hasHorizontalScrollbar != scrollsHorizontally) {
            m_data->setHasHorizontalScrollbar(scrollsHorizontally);
            hasHorizontalScrollbar = scrollsHorizontally;
        }
    }

    // Set up the range (and page step/line step).
    IntSize maxScrollPosition(contentsWidth() - visibleWidth(), contentsHeight() - visibleHeight());
    IntSize scroll = desiredOffset.shrunkTo(maxScrollPosition);
    scroll.clampNegativeToZero();

    if (m_data->horizontalAdjustment) {
//        m_data->horizontalAdjustment->page_size = visibleWidth();
//        m_data->horizontalAdjustment->step_increment = visibleWidth() / 10.0;
//        m_data->horizontalAdjustment->page_increment = visibleWidth() * 0.9;
//        m_data->horizontalAdjustment->lower = 0;
//        m_data->horizontalAdjustment->upper = contentsWidth();
//        gtk_adjustment_changed(m_data->horizontalAdjustment);

        if (m_scrollOffset.width() != scroll.width()) {
//            m_data->horizontalAdjustment->value = scroll.width();
            //gtk_adjustment_value_changed(m_data->horizontalAdjustment);
        }
    } else if (m_horizontalScrollbar) {
        int clientWidth = visibleWidth();
        m_horizontalScrollbar->setEnabled(contentsWidth() > clientWidth);
        int pageStep = (clientWidth - cAmountToKeepWhenPaging);
        if (pageStep < 0) pageStep = clientWidth;
        IntRect oldRect(m_horizontalScrollbar->frameRect());
        IntRect hBarRect = IntRect(0,
                                   height() - m_horizontalScrollbar->height(),
                                   width() - (m_verticalScrollbar ? m_verticalScrollbar->width() : 0),
                                   m_horizontalScrollbar->height());
        m_horizontalScrollbar->setFrameRect(hBarRect);
        if (!m_scrollbarsSuppressed && oldRect != m_horizontalScrollbar->frameRect())
            m_horizontalScrollbar->invalidate();

        if (m_scrollbarsSuppressed)
            m_horizontalScrollbar->setSuppressInvalidation(true);
        m_horizontalScrollbar->setSteps(cScrollbarPixelsPerLineStep, pageStep);
        m_horizontalScrollbar->setProportion(clientWidth, contentsWidth());
        m_horizontalScrollbar->setValue(scroll.width());
        if (m_scrollbarsSuppressed)
            m_horizontalScrollbar->setSuppressInvalidation(false);
    }

    if (m_data->verticalAdjustment) {
 //       m_data->verticalAdjustment->page_size = visibleHeight();
 //       m_data->verticalAdjustment->step_increment = visibleHeight() / 10.0;
//        m_data->verticalAdjustment->page_increment = visibleHeight() * 0.9;
//        m_data->verticalAdjustment->lower = 0;
//        m_data->verticalAdjustment->upper = contentsHeight();
        //gtk_adjustment_changed(m_data->verticalAdjustment);

        if (m_scrollOffset.height() != scroll.height()) {
            //m_data->verticalAdjustment->value = scroll.height();
//             addToDirtyRegion(frameGeometry());
//             updateView(containingWindow(), frameGeometry());
//             SDL_Event ev;
//             ev.type = SDL_VIDEOEXPOSE;
//             SDL_PushEvent(&ev);
            //gtk_adjustment_value_changed(m_data->verticalAdjustment);
        }
    } else if (m_verticalScrollbar) {
        int clientHeight = visibleHeight();
        m_verticalScrollbar->setEnabled(contentsHeight() > clientHeight);
        int pageStep = (clientHeight - cAmountToKeepWhenPaging);
        if (pageStep < 0) pageStep = clientHeight;
        IntRect oldRect(m_verticalScrollbar->frameRect());
        IntRect vBarRect = IntRect(width() - m_verticalScrollbar->width(),
                                   0,
                                   m_verticalScrollbar->width(),
                                   height() - (m_horizontalScrollbar ? m_horizontalScrollbar->height() : 0));
        m_verticalScrollbar->setFrameRect(vBarRect);
        if (!m_scrollbarsSuppressed && oldRect != m_verticalScrollbar->frameRect())
            m_verticalScrollbar->invalidate();

        if (m_scrollbarsSuppressed)
            m_verticalScrollbar->setSuppressInvalidation(true);
        m_verticalScrollbar->setSteps(cScrollbarPixelsPerLineStep, pageStep);
        m_verticalScrollbar->setProportion(clientHeight, contentsHeight());
        m_verticalScrollbar->setValue(scroll.height());
        if (m_scrollbarsSuppressed)
            m_verticalScrollbar->setSuppressInvalidation(false);
    }

    if (oldHasVertical != (m_verticalScrollbar != 0) || oldHasHorizontal != (m_horizontalScrollbar != 0))
        frameRectsChanged();

    // See if our offset has changed in a situation where we might not have scrollbars.
    // This can happen when editing a body with overflow:hidden and scrolling to reveal selection.
    // It can also happen when maximizing a window that has scrollbars (but the new maximized result
    // does not).
    IntSize scrollDelta = scroll - m_scrollOffset;
    if (scrollDelta != IntSize()) {
       m_scrollOffset = scroll;
       m_data->scrollBackingStore(scrollDelta);
    }

    m_data->inUpdateScrollbars = false;
}

void ScrollView::paint(GraphicsContext* context, const IntRect& rect)
{
#if PLATFORM(AMIGAOS4)
    // FIXME: This code is here so we don't have to fork FrameView.h/.cpp.
    // In the end, FrameView should just merge with ScrollView.
    ASSERT(isFrameView());

    if (context->paintingDisabled())
        return;

    IntRect documentDirtyRect = rect;
    documentDirtyRect.intersect(frameRect());

    context->save();

    context->translate(x(), y());
    documentDirtyRect.move(-x(), -y());

    context->translate(-scrollX(), -scrollY());
    documentDirtyRect.move(scrollX(), scrollY());

    context->clip(enclosingIntRect(visibleContentRect()));
    static_cast<const FrameView*>(this)->frame()->paint(context, documentDirtyRect);
    context->restore();

    // Now paint the scrollbars.
    if (!m_data->scrollbarsSuppressed && (m_data->hBar || m_data->vBar)) {
        context->save();
        IntRect scrollViewDirtyRect = rect;
        scrollViewDirtyRect.intersect(frameRect());
        context->translate(x(), y());
        scrollViewDirtyRect.move(-x(), -y());
        if (m_data->hBar)
            m_data->hBar->paint(context, scrollViewDirtyRect);
        if (m_data->vBar)
            m_data->vBar->paint(context, scrollViewDirtyRect);

        /*
         * FIXME: TODO: Check if that works with RTL
         */
        // Fill the scroll corner with white.
        IntRect hCorner;
        if (m_data->hBar && width() - m_data->hBar->width() > 0) {
            hCorner = IntRect(m_data->hBar->width(),
                              height() - m_data->hBar->height(),
                              width() - m_data->hBar->width(),
                              m_data->hBar->height());
            if (hCorner.intersects(scrollViewDirtyRect))
                context->fillRect(hCorner, Color::white);
        }

        if (m_data->vBar && height() - m_data->vBar->height() > 0) {
            IntRect vCorner(width() - m_data->vBar->width(),
                            m_data->vBar->height(),
                            m_data->vBar->width(),
                            height() - m_data->vBar->height());
            if (vCorner != hCorner && vCorner.intersects(scrollViewDirtyRect))
                context->fillRect(vCorner, Color::white);
        }

        context->restore();
    }

    if (!parent()) {
        IntRect r(rect);
        r.intersect(frameRect());
        if(!r.isEmpty())
            updateView(containingWindow(), r);
    }
#else
    //printf("paint rect %d %d %d %d\n", rect.x(), rect.y(), rect.width(), rect.height());
    ASSERT(isFrameView());

    if (context->paintingDisabled())
        return;

    IntRect documentDirtyRect = rect;
    // FIXME: was not here (and there is another one just down).
    documentDirtyRect.intersect(frameRect());

    context->save();

    context->translate(x(), y());
    documentDirtyRect.move(-x(), -y());

    context->translate(-scrollX(), -scrollY());
    documentDirtyRect.move(scrollX(), scrollY());

    //printf("this = %p documentDirtyRect x=%d y=%d w=%d h=%d\n", this, documentDirtyRect.x(), documentDirtyRect.y(), documentDirtyRect.width(), documentDirtyRect.height());
    //printf("scrollOffset %d %d\n", scrollOffset().width(), scrollOffset().height());
    documentDirtyRect.intersect(enclosingIntRect(visibleContentRect()));
    //printf("this = %p frameGeometry() x=%d y=%d w=%d h=%d\n", this, frameGeometry().x(), frameGeometry().y(), frameGeometry().width(), frameGeometry().height());
    //context->clip(frameGeometry());

    if (documentDirtyRect.isEmpty())
        return ;
    //printf("this = %p documentDirtyRect x=%d y=%d w=%d h=%d\n", this, documentDirtyRect.x(), documentDirtyRect.y(), documentDirtyRect.width(), documentDirtyRect.height());

    static_cast<const FrameView*>(this)->frame()->paint(context, documentDirtyRect);

    context->restore();

    // Now paint the scrollbars.
    if (!m_scrollbarsSuppressed && (m_horizontalScrollbar || m_verticalScrollbar)) {
        context->save();
        IntRect scrollViewDirtyRect = rect;
        scrollViewDirtyRect.intersect(frameRect());
        context->translate(x(), y());
        scrollViewDirtyRect.move(-x(), -y());
        if (m_horizontalScrollbar)
            m_horizontalScrollbar->paint(context, scrollViewDirtyRect);
        if (m_verticalScrollbar)
            m_verticalScrollbar->paint(context, scrollViewDirtyRect);

        // Fill the scroll corner with white.
        IntRect hCorner;
        if (m_horizontalScrollbar && width() - m_horizontalScrollbar->width() > 0) {
            hCorner = IntRect(m_horizontalScrollbar->width(),
                              height() - m_horizontalScrollbar->height(),
                              width() - m_horizontalScrollbar->width(),
                              m_horizontalScrollbar->height());
            if (hCorner.intersects(scrollViewDirtyRect))
                context->fillRect(hCorner, Color::white);
        }

        if (m_verticalScrollbar && height() - m_verticalScrollbar->height() > 0) {
            IntRect vCorner(width() - m_verticalScrollbar->width(),
                            m_verticalScrollbar->height(),
                            m_verticalScrollbar->width(),
                            height() - m_verticalScrollbar->height());
            if (vCorner != hCorner && vCorner.intersects(scrollViewDirtyRect))
                context->fillRect(vCorner, Color::white);
        }

        context->restore();
    }
    IntRect r(rect);
    r.intersect(frameRect());
    //printf("this = %p updateView rect = %d %d %d %d => r %d %d %d %d\n", this, rect.x(), rect.y(), rect.width(), rect.height(), r.x(), r.y(), r.width(), r.height());
    if(!r.isEmpty())
        updateView(containingWindow(), r);
    
#endif
}

bool ScrollView::scroll(ScrollDirection direction, ScrollGranularity granularity)
{
    if (direction == ScrollUp || direction == ScrollDown) {
        if (m_verticalScrollbar)
            return m_verticalScrollbar->scroll(direction, granularity);
    } else {
        if (m_horizontalScrollbar)
            return m_horizontalScrollbar->scroll(direction, granularity);
    }
    return false;
}

bool ScrollView::isOffscreen() const
{
    return false;
}

void ScrollView::addToDirtyRegion(const IntRect& containingWindowRect)
{
    ASSERT(isFrameView());
    const FrameView* frameView = static_cast<const FrameView*>(this);
    Page* page = frameView->frame() ? frameView->frame()->page() : 0;
    if (!page)
        return;
    page->chrome()->addToDirtyRegion(containingWindowRect);
}

void ScrollView::scrollBackingStore(int dx, int dy, const IntRect& scrollViewRect, const IntRect& clipRect)
{
    ASSERT(isFrameView());
    const FrameView* frameView = static_cast<const FrameView*>(this);
    Page* page = frameView->frame() ? frameView->frame()->page() : 0;
    if (!page)
        return;
    page->chrome()->scrollBackingStore(dx, dy, scrollViewRect, clipRect);
}

void ScrollView::updateBackingStore()
{
    ASSERT(isFrameView());
    const FrameView* frameView = static_cast<const FrameView*>(this);
    Page* page = frameView->frame() ? frameView->frame()->page() : 0;
    if (!page)
        return;
    page->chrome()->updateBackingStore();
}

}
