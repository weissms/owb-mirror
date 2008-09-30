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
#ifndef ScrollView_h
#define ScrollView_h
/**
 *  @file  ScrollView.t
 *  ScrollView description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include "IntRect.h"
#include "ScrollTypes.h"
#include "Widget.h"
#include <wtf/HashSet.h>





namespace WKAL {

    class FloatRect;
    class PlatformWheelEvent;
    class Scrollbar;

    class ScrollView : public WKALBase, public Widget {
    public:
    /**
     * ScrollView default constructor
     * @code
     * ScrollView s;
     * @endcode
     */
        ScrollView();

    /**
     * ScrollView destructor
     * @code
     * delete s;
     * @endcode
     */
        ~ScrollView();


    /**
     *  children description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    const HashSet<Widget*>* children() const ;

    /**
     * addChild
     */
     void addChild(Widget*);

     /**
      * removeChild
      */
     void removeChild(Widget*);

    /**
     *  horizontalScrollbar description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    Scrollbar* horizontalScrollbar() const ;


    /**
     *  verticalScrollbar description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    Scrollbar* verticalScrollbar() const ;


    /**
     * isScrollViewScrollbar return whether the Widget is a ScrollViewScrollbar
     * @param[in] : Widget
     * @param[out] : bool
     * @code
     * @endcode
     */
    bool isScrollViewScrollbar(const Widget* child) const ;


    /**
     * setScrollbarModes set both the horizontal and vertical scrollbar modes.
     * @param[in] : new horizontal and vertical scrollbar modes.
     * @code
     * @endcode
     */
    void setScrollbarModes(ScrollbarMode horizontalMode, ScrollbarMode verticalMode);


    /**
     * setHorizontalScrollbarMode set the horizontal scrollbar mode.
     * @param[in] : new horizontal mode.
     * @code
     * @endcode
     */
    void setHorizontalScrollbarMode(ScrollbarMode mode) ;


    /**
     * setVerticalScrollbarMode set the vertical scrollbar mode.
     * @param[in] : new vertical scrollbar mode.
     * @code
     * @endcode
     */
    void setVerticalScrollbarMode(ScrollbarMode mode) ;


    /**
     * scrollbarModes get both the horizontal and vertical scrollbar modes.
     * @param[in] : 2 ScrollbarMode that will be set to the horizontal and the vertical scrollbar modes (in this order).
     * @code
     * @endcode
     */
    void scrollbarModes(ScrollbarMode& horizontalMode, ScrollbarMode& verticalMode) const;


    /**
     * horizontalScrollbarMode get the horizontal scrollbar mode.
     * @param[out] : the horizontal scrollbar mode.
     * @code
     * @endcode
     */
    ScrollbarMode horizontalScrollbarMode() const ;


    /**
     *  verticalScrollbarMode get the vertical scrollbar mode.
     * @param[out] : the vertical scrollbar mode.
     * @code
     * @endcode
     */
    ScrollbarMode verticalScrollbarMode() const ;


    /**
     * setAllowsScrolling set whether the scrolling is allowed.
     * @param[in] : bool (the new scrolling flag)
     * @code
     * @endcode
     */
    virtual void setAllowsScrolling(bool flag);


    /**
     * allowsScrolling get whether the scrolling is enabled.
     * @param[out] : bool (whether the scrolling is authorized).
     * @code
     * @endcode
     */
    bool allowsScrolling() const ;

    /**
     *  setCanBlitOnScroll description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void setCanBlitOnScroll(bool);

    /**
     *  canBlitOnScroll description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool canBlitOnScroll() const ;


    /**
     *  visibleContentRect description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    IntRect visibleContentRect(bool includeScrollbars = false) const;

    /**
     * get visible width
     * @param[out] : width
     * @code
     * int w = s->visibleWidth();
     * @endcode
     */
    int visibleWidth() const ;

    /**
     * get visible height
     * @param[out] : height
     * @code
     * int h = s->visibleHeight();
     * @endcode
     */
    int visibleHeight() const ;

    /**
     *  contentsSize description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    IntSize contentsSize() const;


    /**
     *  contentsWidth description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    int contentsWidth() const ;


    /**
     *  contentsHeight description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    int contentsHeight() const ;

    /**
     *  setContentsSize description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void setContentsSize(const IntSize&);

    /**
     *  scrollPosition description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    IntPoint scrollPosition() const ;


    /**
     *  scrollOffset description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    IntSize scrollOffset() const ;


    /**
     *  maximumScrollPosition description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    IntPoint maximumScrollPosition() const;


    /**
     *  scrollX description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    int scrollX() const ;


    /**
     *  scrollY description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    int scrollY() const ;


    /**
     *  setScrollPosition description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void setScrollPosition(const IntPoint&);


    /**
     *  scrollBy description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void scrollBy(const IntSize& s) ;


    /**
     *  scrollRectIntoViewRecursively description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void scrollRectIntoViewRecursively(const IntRect&);


    /**
     * setScrollbarsSuppressed set whether the scrollbar are suppressed.
     * @param[in] : bool (whether the scrollbar should be suppressed), bool (whether there should be a repaint (default: false))
     * @code
     * @endcode
     */
    void setScrollbarsSuppressed(bool suppressed, bool repaintOnUnsuppress = false);


    /**
     * scrollbarsSuppressed get whether the scrollbars are suppressed.
     * @param[out] : bool
     * @code
     * @endcode
     */
    bool scrollbarsSuppressed() const 

    /**
     * set horizontal scrollbar mode
     * @param[in] : scrollbar mode
     * @code
     * s->setHScrollbarMode(m);
     * @endcode
     */
        virtual void setHScrollbarMode(ScrollbarMode);

        // Set the mode for both scrollbars at once

    /**
     * This gives us a means of blocking painting on our scrollbars until the first layout has occurred.
     * @param[in] : suppressed
     * @param[in] : repaint on unsuppress
     * @code
     * s->suppressScrollbars(su, r);
     * @endcode
     */
        void suppressScrollbars(bool suppressed, bool repaintOnUnsuppress = false);

    /**
     * get horizontal scrollbar mode
     * @param[out] : scrollbar mode
     * @code
     * ScrollbarMode m = s->hScrollbarMode();
     * @endcode
     */
        ScrollbarMode hScrollbarMode() const;


    /**
     * Event coordinates are assumed to be in the coordinate space of a window that contains
     * the entire widget hierarchy. It is up to the platform to decide what the precise definition
     * of containing window is. (For example on Mac it is the containing NSWindow.)
     * @param[in] : point
     * @param[out] : result point
     * @code
     * IntPoint p = s->windowToContents(p1);
     * @endcode
     */
        IntPoint windowToContents(const IntPoint&) const;


    /**
     * isOffscreen return whether the ScrollView is offscreen
     * @param[out] : bool
     * @code
     * @endcode
     */
    bool isOffscreen() const;


    /**
     * windowResizerRect description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual IntRect windowResizerRect() const ;


    /**
     * containsScrollbarsAvoidingResizer description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool containsScrollbarsAvoidingResizer() const;


    /**
     * adjustScrollbarsAvoidingResizerCount description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void adjustScrollbarsAvoidingResizerCount(int overlapDelta);


    /**
     * setParent set the parent ScrollView
     * @param[in] : the parent ScrollView
     * @code
     * @endcode
     */
    virtual void setParent(ScrollView*);


    /**
     * frameRectsChanged description
     * @code
     * @endcode
     */
    virtual void frameRectsChanged() const;

    /**
     * Event coordinates are assumed to be in the coordinate space of a window that contains
     * the entire widget hierarchy. It is up to the platform to decide what the precise definition
     * of containing window is. (For example on Mac it is the containing NSWindow.)
     * @param[in] : rect
     * @param[out] : result rect
     * @code
     * IntRect p = s->windowToContents(p1);
     * @endcode
     */
        IntRect windowToContents(const IntRect&) const ;

    /**
     * inWindow
     */
    bool inWindow() const ;

    /**
     * scrollbarUnderMouse 
     */
     Scrollbar* scrollbarUnderMouse(const PlatformMouseEvent& mouseEvent) ;

    /**
     * scroll
     * @param[in] : direction
     * @param[in] : granularity
     * @param[out] : status
     * @code
     * bool sc = s->scroll(d, g);
     * @endcode
     */
        bool scroll(ScrollDirection, ScrollGranularity) ;


    /**
     * convert self to child. 
     * @param[in] : a pointer to a widget.
     * @param[in] : an IntPoint.
     * @param[out] : an IntPoint.
     * @code
     * @endcode
     */
        IntPoint convertSelfToChild(const Widget* child, const IntPoint& point) const;

#if HAVE(ACCESSIBILITY)


    /**
     * get contents to screen 
     * @param[in] : rect
     * @param[out] : result rect
     * @code
     * IntRect r = s->contentsToScreen(r1);
     * @endcode
     */
        IntRect contentsToScreen(const IntRect&) const;

    /**
     * get screen to contents 
     * @param[in] : point
     * @param[out] : result point
     * @code
     * IntPoint p = s->screenToContents(p1);
     * @endcode
     */
        IntPoint screenToContents(const IntPoint&) const;
#endif

   protected:
       /**
        * updateContents
	*/
        void updateContents(const IntRect&, bool now = false);
       /**
        * updateWindowRect
	*/
        void updateWindowRect(const IntRect&, bool now = false);
    public:
       /**
        * update
	*/
        void update();

public:
    HashSet<Widget*> m_children;
    bool m_canBlitOnScroll;
    IntSize m_scrollOffset; // FIXME: Would rather store this as a position, but we will wait to make this change until more code is shared.
    IntSize m_contentsSize;

    /**
     *  init description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void init();


    /**
     * platformAddChild description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void platformAddChild(Widget*);


    /**
     * platformRemoveChild description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void platformRemoveChild(Widget*);



    /**
     * platformSetScrollbarModes description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void platformSetScrollbarModes();


    /**
     * platformScrollbarModes description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void platformScrollbarModes(ScrollbarMode& horizontal, ScrollbarMode& vertical) const;

    /**
     * platformSetCanBlitOnScroll description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void platformSetCanBlitOnScroll();


    /**
     * platformVisibleContentRect description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    IntRect platformVisibleContentRect(bool includeScrollbars) const;


    /**
     * platformContentsSize description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    IntSize platformContentsSize() const;


    /**
     * platformSetContentsSize description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void platformSetContentsSize();



    private:

    /**
     * platformSetScrollbarsSuppressed description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void platformSetScrollbarsSuppressed(bool repaintOnUnsuppress);



        class ScrollViewPrivate;
        ScrollViewPrivate* m_data;

        friend class ScrollViewPrivate; // FIXME: Temporary.

    public:
    /**
     * @see Widget
     */
        virtual void paint(GraphicsContext*, const IntRect&);


    /**
     * setFrameRect set Widget.t
     * @param[in] : const intRect&
     * @code
     * @endcode
     */
    virtual void setFrameRect(const IntRect&)

    /**
     * setFrameGeometry
     */
        virtual void setFrameGeometry(const IntRect&)

    /**
     * scrollBackingStore
     */
        void scrollBackingStore(int dx, int dy, const IntRect& scrollViewRect, const IntRect& clipRect);

    /**
     * update backing store
     * @code
     * s->updateBackingStore();
     * @endcode
     */
        void updateBackingStore();

    private:
     /**
      * updateScrollbars
      */
        void updateScrollbars(const IntSize& desiredOffset);


    public:

    /**
     * resizerOverlapsContent
     */
        bool resizerOverlapsContent() const ;

    /**
     * adjustOverlappingScrollbarCount.
     * @param[in] : a integer representing the overlap delta.
     * @code
     * @endcode
     */
        void adjustOverlappingScrollbarCount(int overlapDelta) ;

    public:
    /**
     * setGtkAdjustments
     */
        void setBalAdjustments(BalAdjustment* hadj, BalAdjustment* vadj);

    };

    /**
     *  ScrollView::contentsToWindow description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
inline IntRect ScrollView::contentsToWindow(const IntRect& rect) const;
    /**
     *  ScrollView::windowToContents description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
inline IntRect ScrollView::windowToContents(const IntRect& rect) const;
    
   
#endif

} // namespace WebCore

#endif // ScrollView_h