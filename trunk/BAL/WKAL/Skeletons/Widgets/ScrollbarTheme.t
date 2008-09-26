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
#ifndef ScrollbarTheme_h
#define ScrollbarTheme_h
/**
 *  @file  ScrollbarTheme.t
 *  ScrollbarTheme description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include "IntRect.h"
#include "ScrollTypes.h"

namespace WKAL {

class GraphicsContext;
class Scrollbar;

class ScrollbarTheme : public WKALBase {
public:
    /**
     * Destructor for ScrollbarTheme.
     * @code
     * @endcode
     */
    virtual ~ScrollbarTheme() ;

    /**
     * Paint the scrollbar.
     * @param[in] : a pointer to the scrollbar.a
     * @param[in] : a pointer to the graphics context.
     * @param[in] : a dirty region to redraw.
     * @param[out] : a boolean.
     * @code
     * @endcode
     */
    virtual bool paint(Scrollbar*, GraphicsContext* context, const IntRect& damageRect) ;


    /**
     * hitTest.
     * @param[in] : a pointer to the scrollbar.
     * @param[in] : a mouse event.
     * @param[out] : a ScrollbarPart.
     * @code
     * @endcode
     */
    virtual ScrollbarPart hitTest(Scrollbar*, const PlatformMouseEvent&) ;

    /**
     * Retrieve the scrollbar thickness.
     * @param[in] : a ScrollbarControlSize.
     * @param[out] : the thickness.
     * @code
     * @endcode
     */
    virtual int scrollbarThickness(ScrollbarControlSize = RegularScrollbar) ;


    /**
     *  buttonsPlacement description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual ScrollbarButtonsPlacement buttonsPlacement() const ;

    /**
     * check wether it supports control tints.
     * @param[out] : a boolean.
     * @code
     * @endcode
     */
    virtual bool supportsControlTints() const ;

    /**
     * themeChanged
     */
    virtual void themeChanged();

    /**
     * invalidateOnMouseEnterExit
     */
     virtual bool invalidateOnMouseEnterExit();

    /**
     * invalidateParts.
     * @param[in] : a pointer to the scrollbar.
     * @param[in] : a ScrollbarControlPartMask.
     * @code
     * @endcode
     */
    void invalidateParts(Scrollbar* scrollbar, ScrollbarControlPartMask mask);

    /**
     * invalidateParts.
     * @param[in] : a pointer to the scrollbar.
     * @param[in] : a ScrollbarPart
     * @code
     * @endcode
     */
    void invalidateParts(Scrollbar* scrollbar, ScrollbarPart);

    /**
     * shouldCenterOnThumb.
     * @param[in] : a pointer to the scrollbar.
     * @param[in] : a mouse event.
     * @param[out] : a boolean.
     * @code
     * @endcode
     */
    virtual bool shouldCenterOnThumb(Scrollbar*, const PlatformMouseEvent&);

    /**
     * get thumbPosition
     */
    virtual int thumbPosition(Scrollbar*);

    /**
     * Get the thumb length.
     * @param[in] : a pointer to the scrollbar.
     * @param[out] : the thumb length.
     * @code
     * @endcode
     */
    virtual int thumbLength(Scrollbar*);

    /**
     * trackPosition
     */
     virtual int trackPosition(Scrollbar*);

    /**
     * Get the trackLength.
     * @param[in] : a pointer to the scrollbar.
     * @param[out] : the track length.
     * @code
     * @endcode
     */
    virtual int trackLength(Scrollbar*);

    /**
     * initialAutoscrollTimerDelay
     */
     virtual double initialAutoscrollTimerDelay()

    /**
     * Get the autoscroll timer delay.
     * @param[out] : the auto scroll timer delay.
     * @code
     * @endcode
     */
    virtual double autoscrollTimerDelay() ;

    /**
     * registerScrollbar
     */
    virtual void registerScrollbar(Scrollbar*) ;

    /**
     * unregisterScrollbar
     */
    virtual void unregisterScrollbar(Scrollbar*) ;

    /**
     * nativeTheme
     */ 
    static ScrollbarTheme* nativeTheme(); // Must be implemented to return the correct theme subclass.
};

}
#endif

