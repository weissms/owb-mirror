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
#ifndef Scrollbar_h
#define Scrollbar_h
/**
 *  @file  Scrollbar.t
 *  Scrollbar description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include <wtf/RefCounted.h>
#include "ScrollTypes.h"
#include "Timer.h"
#include "Widget.h"
#include <wtf/MathExtras.h>
#include <wtf/PassRefPtr.h>

namespace WKAL {

class GraphicsContext;
class IntRect;
class ScrollbarClient;
class ScrollbarTheme;
class PlatformMouseEvent;

// These match the numbers we use over in WebKit (WebFrameView.m).
#define LINE_STEP   40
#define PAGE_KEEP   40


class Scrollbar : public WKALBase, public Widget, public RefCounted<Scrollbar> {
protected:
    /**
     * Constructor for Scrollbar.
     * @param[in] : a pointer to a ScrollbarClient.
     * @param[in] : a ScrollbarOrientation.
     * @param[in] : a ScrollbarOrientation.
     * @param[in] : a pointer to a ScrollbarTheme.
     * @code
     * @endcode
     */
    Scrollbar(ScrollbarClient*, ScrollbarOrientation, ScrollbarControlSize, ScrollbarTheme* = 0);

public:
    /**
     * Destructor for Scrollbar.
     * @code
     * @endcode
     */
    virtual ~Scrollbar();

    // Must be implemented by platforms that can't simply use the Scrollbar base class.  Right now those two platforms
    // are Mac and GTK.
    /**
     * Create native scrollbar.
     * @param[in] : a pointer to a ScrollbarClient.
     * @param[in] : a ScrollbarOrientation.
     * @param[in] : a ScrollbarControlSize.
     * @param[out] : a PassRefPtr to the Scrollbar.
     * @code
     * @endcode
     */
    static PassRefPtr<Scrollbar> createNativeScrollbar(ScrollbarClient* client, ScrollbarOrientation orientation, ScrollbarControlSize size);
    
    /**
     * Set client for the scrollbar.
     * @param[in] : a pointer to a ScrollbarClient.
     * @code
     * @endcode
     */
    void setClient(ScrollbarClient* client) ;
    /**
     * Get the client for the scriollbar.
     * @param[out] : a pointer to the ScrollbarClient.
     * @code
     * @endcode
     */
    ScrollbarClient* client() const ;

    /**
     * Get the scrollbar orientation.
     * @param[out] : the scrollbar orientation.
     * @code
     * @endcode
     */
    ScrollbarOrientation orientation() const ;
    
    /**
     * value.
     * @param[out] : an integer.
     * @code
     * @endcode
     */
    int value() const ;
    /**
     * Get the current scrollbar position.
     * @param[out] :athe current position for the scrollbar.
     * @code
     * @endcode
     */
    float currentPos() const ;
    /**
     * pressedPos.
     * @param[out] : an integer.
     * @code
     * @endcode
     */
    int pressedPos() const ;
    /**
     * Get the scrollbar visible size.
     * @param[out] : the visible size.
     * @code
     * @endcode
     */
    int visibleSize() const ;
    /**
     * Get the scrollbar  total size.
     * @param[out] : the total size.
     * @code
     * @endcode
     */
    int totalSize() const ;
    /**
     * maximum.
     * @param[out] : an integer.
     * @code
     * @endcode
     */
    int maximum() const ;
    /**
     * Get the control size.
     * @param[out] : the control size.
     * @code
     * @endcode
     */
    ScrollbarControlSize controlSize() const ;

    /**
     * Get the line step.
     * @param[out] : the line step.
     * @code
     * @endcode
     */
    int lineStep() const ;
    /**
     * Get the page step.
     * @param[out] : the page step.
     * @code
     * @endcode
     */
    int pageStep() const ;
    /**
     * Get the pixel step.
     * @param[out] :athe pixel step.
     * @code
     * @endcode
     */
    float pixelStep() const ;
    
    /**
     * pressedPart.
     * @param[out] : a ScrollbarPart.
     * @code
     * @endcode
     */
    ScrollbarPart pressedPart() const ;
    /**
     * hoveredPart.
     * @param[out] : a ScrollbarPart.
     * @code
     * @endcode
     */
    ScrollbarPart hoveredPart() const ;

    /**
     * Set steps.
     * @param[in] : the line step.
     * @param[in] : the page step.
     * @param[in] : the pixel step.
     * @code
     * @endcode
     */
    void setSteps(int lineStep, int pageStep, int pixelsPerStep = 1);
    /**
     * setValue.
     * @param[in] : an integer.
     * @param[out] : a boolean.
     * @code
     * @endcode
     */
    bool setValue(int);
    /**
     * Set proportion.
     * @param[in] : the visible size.
     * @param[in] : the total size.
     * @code
     * @endcode
     */
    void setProportion(int visibleSize, int totalSize);
    /**
     * Set pressed position.
     * @param[in] : an integer.
     * @code
     * @endcode
     */
    void setPressedPos(int p) ;

    /**
     * scroll.
     * @param[in] : a ScrollDirection.
     * @param[in] : a ScrollGranularity.
     * @param[in] : a multiplier.
     * @param[out] : a boolean.
     * @code
     * @endcode
     */
    bool scroll(ScrollDirection, ScrollGranularity, float multiplier = 1.0f);
    
    /**
     * paint.
     * @param[in] : a pointer to the graphics context.
     * @param[in] : a rectangle.
     * @code
     * @endcode
     */
    virtual void paint(GraphicsContext*, const IntRect& damageRect);

    // These methods are used for platform scrollbars to give :hover feedback.  They will not get called
    // when the mouse went down in a scrollbar, since it is assumed the scrollbar will start
    // grabbing all events in that case anyway.

    /**
     * enabled.
     * @param[out] : a boolean.
     * @code
     * @endcode
     */
    bool enabled() const ;


    /**
     * Set enabled.
     * @param[in] : a boolean.
     * @code
     * @endcode
     */
    virtual void setEnabled(bool e);


    /**
     *  mouseMoved description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool mouseMoved(const PlatformMouseEvent&);


    /**
     *  mouseExited description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool mouseExited();


    /**
     *  mouseUp description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool mouseUp();


    /**
     *  mouseDown description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool mouseDown(const PlatformMouseEvent&);


    /**
     *  transformEvent description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PlatformMouseEvent transformEvent(const PlatformMouseEvent&)
    /**
     * Check wether it should handle mouse out event.
     * @param[in] : a mouse event.
     * @param[out] : a boolean.
     * @code
     * @endcode
     */
    virtual bool handleMouseOutEvent(const PlatformMouseEvent&);
    
    // Used by some platform scrollbars to know when they've been released from captur

    /**
     * Check wether it should handle mouse press event.
     * @param[in] : a mouse event.
     * @param[out] : a boolean.
     * @code
     * @endcode
     */
    virtual bool handleMousePressEvent(const PlatformMouseEvent&);

    /**
     * Set parent.
     * @param[in] : a pointer to the scrollView.
     * @code
     * @endcode
     */
    virtual void setParent(ScrollView*)

protected:

    /**
     * invalidate a rectangle.
     * @param[in] : the rectangle to invalidate.
     * @code
     * @endcode
     */
    virtual void invalidateRect(const IntRect&);


    /**
     * suppress invalidation.
     * @param[out] : a boolean
     * @code
     * @endcode
     */
    bool suppressInvalidation() const ;


    /**
     * set suppress invalidation.
     * @param[in] : a boolean.
     * @code
     * @endcode
     */
    void setSuppressInvalidation(bool s) ;

    /**
     * Update thumb position.
     * @code
     * @endcode
     */
    virtual void updateThumbPosition();
    /**
     * Update thumb proportion.
     * @code
     * @endcode
     */
    virtual void updateThumbProportion();
    
    /**
     * autoscrollTimerFired.
     * @param[in] : a pointer to a Scrollbar timer.
     * @code
     * @endcode
     */
    void autoscrollTimerFired(Timer<Scrollbar>*);
    /**
     * Start timer if needed.
     * @param[in] : a delay.
     * @code
     * @endcode
     */
    void startTimerIfNeeded(double delay);
    /**
     * Stop timer if needed.
     * @code
     * @endcode
     */
    void stopTimerIfNeeded();
    /**
     * autoscrollPressedPart.
     * @param[in] : a delay.
     * @code
     * @endcode
     */
    void autoscrollPressedPart(double delay);
    /**
     * pressedPartScrollDirection.
     * @param[out] : a ScrollDirection.
     * @code
     * @endcode
     */
    ScrollDirection pressedPartScrollDirection();
    /**
     * pressedPartScrollGranularity.
     * @param[out] : a ScrollGranularity.
     * @code
     * @endcode
     */
    ScrollGranularity pressedPartScrollGranularity();
    
    /**
     * Move the thumb.
     * @param[in] : a position.
     * @code
     * @endcode
     */
    void moveThumb(int pos);

    ScrollbarClient* m_client;
    ScrollbarOrientation m_orientation;
    ScrollbarControlSize m_controlSize;
    ScrollbarTheme* m_theme;
    
    int m_visibleSize;
    int m_totalSize;
    float m_currentPos;
    int m_lineStep;
    int m_pageStep;
    float m_pixelStep;

    ScrollbarPart m_hoveredPart;
    ScrollbarPart m_pressedPart;
    int m_pressedPos;
    Timer<Scrollbar> m_scrollTimer;
    bool m_overlapsResizer;
};

}