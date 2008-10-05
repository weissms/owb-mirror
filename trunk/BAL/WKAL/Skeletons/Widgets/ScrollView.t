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
     *  valueChanged description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual void valueChanged(Scrollbar*);


    /**
     *  hostWindow description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual HostWindow* hostWindow() const = 0;


    /**
     *  windowClipRect description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual IntRect windowClipRect(bool clipToContents = true) const = 0;

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
     *  setCanHaveScrollbars description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual void setCanHaveScrollbars(bool flag);


    /**
     *  canHaveScrollbars description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool canHaveScrollbars() const ;


    /**
     *  setProhibitsScrolling description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void setProhibitsScrolling(bool b) ;


    /**
     *  prohibitsScrolling description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool prohibitsScrolling() const 


    /**
     * allowsScrolling get whether the scrolling is enabled.
     * @param[out] : bool (whether the scrolling is authorized).
     * @code
     * @endcode
     */
    bool allowsScrolling() const 

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
     *  scroll description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool scroll(ScrollDirection, ScrollGranularity);


    /**
     *  scrollContents description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void scrollContents(const IntSize& scrollDelta);

    /**
     * setScrollbarsSuppressed set whether the scrollbar are suppressed.
     * @param[in] : bool (whether the scrollbar should be suppressed), bool (whether there should be a repaint (default: false))
     * @code
     * @endcode
     */
    void setScrollbarsSuppressed(bool suppressed, bool repaintOnUnsuppress = false);

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
     *  contentsToWindow description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    IntPoint contentsToWindow(const IntPoint&) const;


    /**
     *  windowToContents description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    IntRect windowToContents(const IntRect&) const;


    /**
     *  contentsToWindow description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    IntRect contentsToWindow(const IntRect&) const


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
     * windowResizerRect description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual IntRect windowResizerRect() const ;


    /**
     * adjustScrollbarsAvoidingResizerCount description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void adjustScrollbarsAvoidingResizerCount(int overlapDelta);


    /**
     * frameRectsChanged description
     * @code
     * @endcode
     */
    virtual void frameRectsChanged() const

    /**
     * inWindow
     */
    bool inWindow() const 

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

#if HAVE(ACCESSIBILITY)


    /**
     * get contents to screen 
     * @param[in] : rect
     * @param[out] : result rect
     * @code
     * IntRect r = s->contentsToScreen(r1);
     * @endcode
     */
        IntRect contentsToScreen(const IntRect&) const
#endif

   protected:
       /**
        * updateContents
	*/
        void updateContents(const IntRect&, bool now = false);

    /**
     *  setFrameRect description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual void setFrameRect(const IntRect&);


    /**
     *  scrollbarUnderMouse description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    Scrollbar* scrollbarUnderMouse(const PlatformMouseEvent& mouseEvent);


    /**
     *  wheelEvent description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void wheelEvent(PlatformWheelEvent&);


    /**
     *  convertChildToSelf description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    IntPoint convertChildToSelf(const Widget* child, const IntPoint& point) const;


    public:
       /**
        * update
	*/
        void update();

public:
    HashSet<Widget*> m_children;
    bool m_canBlitOnScroll;
    IntSize m_scrollOffset; // FIXME: Would rather store this as a position, but we will wait to make this change until more code is shared.
    IntSize m_contentsSize


    /**
     * platformAddChild description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void platformAddChild(Widget*);



    /**
     *  show description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual void show();


    /**
     *  hide description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual void hide();


    /**
     *  setParentVisible description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual void setParentVisible(bool);


    /**
     *  addPanScrollIcon description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void addPanScrollIcon(const IntPoint&);


    /**
     *  removePanScrollIcon description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void removePanScrollIcon();


    /**
     *  repaintContentRectangle description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual void repaintContentRectangle(const IntRect&, bool now = false);


    /**
     *  paintContents description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual void paintContents(GraphicsContext*, const IntRect& damageRect) = 0;


    /**
     *  contentsResized description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual void contentsResized() = 0;


    /**
     *  visibleContentsResized description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual void visibleContentsResized() = 0



    /**
     * platformSetScrollbarModes description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void platformSetScrollbarModes();



    /**
     *  destroy description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void destroy()

    /**
     * platformSetCanBlitOnScroll description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void platformSetCanBlitOnScroll();



    /**
     *  setHasHorizontalScrollbar description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void setHasHorizontalScrollbar(bool);


    /**
     *  setHasVerticalScrollbar description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void setHasVerticalScrollbar(bool);


    /**
     *  platformInit description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void platformInit();


    /**
     *  platformDestroy description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void platformDestroy()


    /**
     * platformContentsSize description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    IntSize platformContentsSize() const;



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

    publi


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
        void scrollBackingStore(int dx, int dy, const IntRect& scrollViewRect, const IntRect& clipRect)

    private:
     /**
      * updateScrollbars
      */
        void updateScrollbars(const IntSize& desiredOffset);


    public

    /**
     * adjustOverlappingScrollbarCount.
     * @param[in] : a integer representing the overlap delta.
     * @code
     * @endcode
     */
        void adjustOverlappingScrollbarCount(int overlapDelta) ;

    publi

    };
    /**
     *  platformSetContentsSize description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void platformSetContentsSize();
    /**
     *  platformContentsToScreen description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    IntRect platformContentsToScreen(const IntRect&) const;
    /**
     *  platformScreenToContents description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    IntPoint platformScreenToContents(const IntPoint&) const;
    /**
     *  platformSetScrollPosition description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void platformSetScrollPosition(const IntPoint&);
    /**
     *  platformScroll description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool platformScroll(ScrollDirection, ScrollGranularity);
    /**
     *  platformSetScrollbarsSuppressed description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void platformSetScrollbarsSuppressed(bool repaintOnUnsuppress);
    /**
     *  platformRepaintContentRectangle description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void platformRepaintContentRectangle(const IntRect&, bool now);
    /**
     *  platformIsOffscreen description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool platformIsOffscreen() const;
    /**
     *  platformHandleHorizontalAdjustment description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool platformHandleHorizontalAdjustment(const IntSize&);
    /**
     *  platformHandleVerticalAdjustment description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool platformHandleVerticalAdjustment(const IntSize&);
    /**
     *  platformHasHorizontalAdjustment description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool platformHasHorizontalAdjustment() const;
    /**
     *  platformHasVerticalAdjustment description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool platformHasVerticalAdjustment() const;
    /**
     *  rootPreventsBlitting description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool rootPreventsBlitting() const ;
    /**
     *  setBalAdjustments description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void setBalAdjustments(BalAdjustment* hadj, BalAdjustment* vadj);
    /**
     *  setScrollOffset description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void setScrollOffset(const IntSize& offset) ;
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    

    /**
     *  ScrollView::contentsToWindow description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
inline IntRect ScrollView::contentsToWindow(const IntRect& rect) c