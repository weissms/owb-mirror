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
#ifndef Widget_h
#define Widget_h
/**
 *  @file  Widget.t
 *  Widget description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include <wtf/Platform.h>






namespace WKAL {

    class Cursor;
    class Event;
    class Font;
    class GraphicsContext;
    class IntPoint;
    class IntRect;
    class IntSize;
    class PlatformMouseEvent;
    class ScrollView;
    class WidgetClient;
    class WidgetPrivate;

    class Widget : public WKALBase {
    public:
    /**
     * Widget default contructor
     * @code
     * Widget w;
     * @endcode
     */
        Widget(PlatformWidget = 0);

    /**
     * Widget destructor
     * @code
     * delete w;
     * @endcode
     */
        virtual ~Widget(PlatformWidget = 0);


    /**
     *  platformWidget description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PlatformWidget platformWidget() const ;


    /**
     *  setPlatformWidget description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void setPlatformWidget(PlatformWidget widget);


    /**
     *  x description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    int x() const ;


    /**
     *  y description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    int y() const ;


    /**
     *  width description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    int width() const ;


    /**
     *  height description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    int height() const ;


    /**
     *  size description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    IntSize size() const ;


    /**
     *  pos description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    IntPoint pos() const 

    /**
     * get enabled status
     * @param[out] : status
     * @code
     * bool e = w->isEnabled();
     * @endcode
     */
        virtual bool isEnabled() const

    /**
     * get y value
     * @param[out] : y value
     * @code
     * int y = w->y();
     * @endcode
     */
        int y() const;


    /**
     *  boundsGeometry description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    IntRect boundsGeometry() const ;


    /**
     *  resize description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void resize(int w, int h) ;


    /**
     *  resize description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void resize(const IntSize& s) ;


    /**
     *  move description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void move(int x, int y) ;


    /**
     *  move description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void move(const IntPoint& p) 

    /**
     * get height value
     * @param[out] : height value
     * @code
     * int h = w->height();
     * @endcode
     */
        int height() const;


    /**
     *  invalidate description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void invalidate() 

    /**
     * resize
     * @param[in] : width size
     * @param[in] : height size
     * @code
     * w->resize(w, h);
     * @endcode
     */
        void resize(int, int)

    /**
     * get position
     * @param[out] : position
     * @code
     * IntPoint p = w->pos();
     * @endcode
     */
        IntPoint pos() const

    /**
     * move
     * @param[in] : point
     * @code
     * w->move(p);
     * @endcode
     */
        void move(const IntPoint&)

    /**
     * invalidate
     * @code
     * w->invalidate();
     * @endcode
     */
        virtual void invalidate()

    /**
     * set frame geometry
     * @param[in] : rect
     * @code
     * w->setFrameGeometry(r);
     * @endcode
     */
        virtual void setFrameGeometry(const IntRect&)

    /**
     * set focus
     * @code
     * w->setFocus();
     * @endcode
     */
        virtual void setFocus();


    /**
     *  isSelfVisible description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool isSelfVisible() const ;


    /**
     *  isParentVisible description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool isParentVisible() const ;


    /**
     *  isVisible description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool isVisible() const ;


    /**
     *  setParentVisible description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual void setParentVisible(bool visible) ;


    /**
     *  setSelfVisible description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void setSelfVisible(bool v) 

    /**
     * get cursor
     * @param[out] : cursor
     * @code
     * Cursor c = w->cursor();
     * @endcode
     */
        Cursor cursor();


    /**
     *  isFrameView description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual bool isFrameView() const 

    /**
     * hide
     * @code
     * w->hide();
     * @endcode
     */
        virtual void hide()

    /**
     * set client
     * @param[in] : widget client
     * @code
     * w->setClient(wc);
     * @endcode
     */
        void setClient(WidgetClient*);


    /**
     *  setParent description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual void setParent(ScrollView* view);


    /**
     *  parent description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    ScrollView* parent() const ;


    /**
     *  root description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    ScrollView* root() const;


    /**
     *  windowClipRect description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual IntRect windowClipRect() const 

    /**
     * test if the widget is a frameView
     * @param[out] : true if the widget is a frameView
     * @code
     * bool fv = w->isFrameView();
     * @endcode
     */
        virtual bool isFrameView() const;


    /**
     *  containingWindow description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PlatformWindow containingWindow() const;


    /**
     *  setContainingWindow description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void setContainingWindow(PlatformWindow window) 

    /**
     * remove from parent
     * @code
     * w->removeFromParent();
     * @endcode
     */
        virtual void removeFromParent()

    /**
     * handle event
     * @param[in] : event
     * @code
     * w->handleEvent(e);
     * @endcode
     */
        virtual void handleEvent(Event*) 

    /**
     * get containing window 
     * @param[out] : platform widget
     * @code
     * PlatformWidget pw = w->containingWindow();
     * @endcode
     */
        PlatformWidget containingWindow() const;


    /**
     *  geometryChanged description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual void geometryChanged() const ;


    /**
     *  init description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void init(PlatformWidget);


    /**
     *  releasePlatformWidget description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void releasePlatformWidget();


    /**
     *  retainPlatformWidget description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void retainPlatformWidget();

    /**
     * set parent
     * @param[in] : scrollView
     * @code
     * w->setParent(s);
     * @endcode
     */
        virtual void setParent(ScrollView*);

    /**
     * get parent
     * @param[out] : scroll view
     * @code
     * ScrollView *s = w->parent();
     * @endcode
     */
        ScrollView* parent() const;

    /**
     * geometry changed
     * @code
     * w->geometryChanged();
     * @endcode
     */
        virtual void geometryChanged() const;

    /**
     * convert to containing window
     * @param[in] : rect
     * @param[out] : rect
     * @code
     * IntRect r = w->convertToContainingWindow(r1);
     * @endcode
     */
        IntRect convertToContainingWindow(const IntRect&) const;

    /**
     * convert to containing window
     * @param[in] : point
     * @param[out] : point
     * @code
     * IntPoint p =  w->convertToContainingWindow(p1);
     * @endcode
     */
        IntPoint convertToContainingWindow(const IntPoint&) const;

    /**
     * convert from containing window
     * @param[in] : point
     * @param[out] : point
     * @code
     * IntPoint p = w->convertFromContainingWindow(p1);
     * @endcode
     */
        IntPoint convertFromContainingWindow(const IntPoint&) const;

    /**
     * convert child to self
     * @param[in] : widget
     * @param[in] : point
     * @param[out] : point
     * @code
     * IntPoint p = w->convertChildToSelf(wi, p1);
     * @endcode
     */
        virtual IntPoint convertChildToSelf(const Widget*, const IntPoint&) const;

    /**
     * convert self to child
     * @param[in] : widget
     * @param[in] : point
     * @param[out] : point
     * @code
     * IntPoint p = w->convertSelfToChild(wi, p1);
     * @endcode
     */
        virtual IntPoint convertSelfToChild(const Widget*, const IntPoint&) const;

    /**
     * get suppress invalidation status
     * @param[out] : status
     * @code
     * bool s = w->suppressInvalidation();
     * @endcode
     */
        bool suppressInvalidation() const;

    /**
     * set suppress invalidation status
     * @param[in] : status
     * @code
     * w->setSuppressInvalidation(s);
     * @endcode
     */
        void setSuppressInvalidation(bool);

    /**
     * get BalWidget
     * @param[out] : BalWidget
     * @code
     * BalWidget *bw = w->balWidget(PlatformWidget = 0);
     * @endcode
     */
        BalWidget* balWidget() const;
protected:
    /**
     * set BalWidget 
     * @param[in] : BalWidget
     * @code
     * w->setBalWidget(bw);
     * @endcode
     */
        void setBalWidget(BalWidget*);


    private:
        WidgetPrivate* data;
    };