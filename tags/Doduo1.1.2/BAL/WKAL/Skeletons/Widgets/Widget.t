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
#include "IntPoint.h"
#include "IntRect.h"
#include "IntSize.h"


namespace WKAL {

    class Cursor;
    class Event;
    class Font;
    class GraphicsContext;
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
        virtual ~Widget();


    /**
     *  platformWidget
     */
    PlatformWidget platformWidget() const ;


    /**
     *  setPlatformWidget
     */
    void setPlatformWidget(PlatformWidget widget);


    /**
     *  x description
     */
    int x() const ;


    /**
     * get y value
     * @param[out] : y value
     * @code
     * int y = w->y();
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
    IntPoint pos() const ;


    /**
     * setFrameRect set the Frame rectangle
     * @param[in] : new Frame rectangle
     * @code
     * @endcode
     */
    virtual void setFrameRect(const IntRect&);


    /**
     * frameRect get the Frame rectangle
     * @param[out] : the Frame rectangle
     * @code
     * @endcode
     */
    virtual IntRect frameRect() const;


    /**
     * boundsRect get the bound rectangle.
     * @param[out] : the bound rectangle
     * @code
     * @endcode
     */
    IntRect boundsRect() const 


    /**
     *  boundsRect description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    IntRect boundsRect() const 


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
    void move(const IntPoint& p) ;

    /**
     * paint
     */
    virtual void paint(GraphicsContext*, const IntRect&);

    /**
     *  invalidate description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void invalidate() ;

    /**
     * invalidateRect
     */
     virtual void invalidateRect(const IntRect&) = 0;

    /**
     * set focus
     * @code
     * w->setFocus();
     * @endcode
     */
        virtual void setFocus();

    /**
     * setCursor
     */
     void setCursor(const Cursor&);

    /**
     * get cursor
     * @param[out] : cursor
     * @code
     * Cursor c = w->cursor();
     * @endcode
     */
        Cursor cursor();

    /**
     * show
     */
     virtual void show();

    /**
     * hide
     */
     virtual void hide();

    /**
     * isSelfVisible
     */
     bool isSelfVisible() const ;

    /**
     * isParentVisible
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
    void setSelfVisible(bool v) ;

    /**
     * setIsSelected
     */
     void setIsSelected(bool);

    /**
     *  isFrameView description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual bool isFrameView() const ;

    /**
     * isPluginView
     */
     virtual bool isPluginView() const ;

    /**
     * remove from parent
     * @code
     * w->removeFromParent();
     * @endcode
     */
        void removeFromParent();

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
     * handle event
     * @param[in] : event
     * @code
     * w->handleEvent(e);
     * @endcode
     */
        virtual void handleEvent(Event*) ;

    /**
     *  setContainingWindow description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void setContainingWindow(PlatformWindow window) 

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
     * convert from containing window
     * @param[in] : rect
     * @param[out] : rect
     * @code
     * IntRect r = w->convertFromContainingWindow(r1);
     * @endcode
     */
        IntRect convertFromContainingWindow(const IntRect&) const;


    /**
     * frameRectsChanged description
     * @code
     * @endcode
     */
    virtual void frameRectsChanged() const ;

private:
   
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

private:
    ScrollView* m_parent;
    PlatformWidget m_widget;
    bool m_selfVisible;
    bool m_parentVisible;

    IntRect m_frame; // Not used when a native widget exists.
    PlatformWindow m_containingWindow; // Not used when a native widget exists.
    WidgetPrivate* data;
};
} // namespace WebCore

#endif