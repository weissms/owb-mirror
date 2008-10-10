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
#ifndef ScrollbarThemeComposite_h
#define ScrollbarThemeComposite_h
/**
 *  @file  ScrollbarThemeComposite.t
 *  ScrollbarThemeComposite description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include "ScrollbarTheme.h"

namespace WKAL {

class ScrollbarThemeComposite : public WKALBase, public ScrollbarTheme {
public:
    /**
     * Paint the scrollbar.
     * @param[in] : a pointer to the scrollbar to paint.
     * @param[in] : a pointer to the graphics context.
     * @param[in] : the zone to repaint.
     * @param[out] : a boolean.
     * @code
     * @endcode
     */
    virtual bool paint(Scrollbar*, GraphicsContext* context, const IntRect& damageRect);

protected:

    /**
     * hitTest.
     * @param[in] : a pointer to the Scrollbar.
     * @param[in] : a mouse event to test.
     * @param[out] : a ScrollbarPart.
     * @code
     * @endcode
     */
    virtual ScrollbarPart hitTest(Scrollbar*, const PlatformMouseEvent&);


    /**
     * invalidatePart.
     * @param[in] : a pointer to the Scrollbar.
     * @param[in] : a ScrollbarPart.
     * @code
     * @endcode
     */
    virtual void invalidatePart(Scrollbar*, ScrollbarPart);


    /**
     * Get the thumb position.
     * @param[in] : a pointer to the Scrollbar.
     * @param[out] : the thumb position.
     * @code
     * @endcode
     */
    virtual int thumbPosition(Scrollbar*);


    /**
     * Get the thumb length.
     * @param[in] : a pointer to the Scrollbar.
     * @param[out] : the thumb length.
     * @code
     * @endcode
     */
    virtual int thumbLength(Scrollbar*);


    /**
     * Get the track position.
     * @param[in] : a pointer to the Scrollbar.
     * @param[out] : the track position.
     * @code
     * @endcode
     */
    virtual int trackPosition(Scrollbar*);


    /**
     * Get the track length.
     * @param[in] : a pointer to the Scrollbar.
     * @param[out] : the track length.
     * @code
     * @endcode
     */
    virtual int trackLength(Scrollbar*);


    /**
     *  paintScrollCorner 
     */
    virtual void paintScrollCorner(ScrollView*, GraphicsContext*, const IntRect& cornerRect);

    /**
     * Check wether the scrollbar has buttons.
     * @param[in] : a pointer to the scrollbar.
     * @param[out] : a boolean.
     * @code
     * @endcode
     */
    virtual bool hasButtons(Scrollbar*) = 0;
    /**
     * Check wether the scrollbar has thumb.
     * @param[in] : a pointer to the scrollbar.
     * @param[out] : a boolean.
     * @code
     * @endcode
     */
    virtual bool hasThumb(Scrollbar*) = 0;

    /**
     * backButtonRect.
     * @param[in] : a pointer to the scrollbar.
     * @param[in] : an optional boolean set to false by default.
     * @param[out] : a rectangle.
     * @code
     * @endcode
     */
    virtual IntRect backButtonRect(Scrollbar*, ScrollbarPart, bool painting = false) = 0;
    /**
     * forwardButtonRect.
     * @param[in] : a pointer to the scrollbar.
     * @param[in] : an optional boolean set to false by default.
     * @param[out] : a rectangle.
     * @code
     * @endcode
     */
    virtual IntRect forwardButtonRect(Scrollbar*, ScrollbarPart, bool painting = false) = 0;
    /**
     * trackRect.
     * @param[in] : a pointer to the scrollbar.
     * @param[in] : an optional boolean set to false by default.
     * @param[out] : a rectangle.
     * @code
     * @endcode
     */
    virtual IntRect trackRect(Scrollbar*, bool painting = false) = 0;

    /**
     * splitTrack.
     * @param[in] : a pointer to the scrollbar.
     * @param[in] : a rectangle.
     * @param[in] : a rectangle.
     * @param[in] : a rectangle.
     * @param[in] : a rectangle.
     * @code
     * @endcode
     */
    virtual void splitTrack(Scrollbar*, const IntRect& track, IntRect& startTrack, IntRect& thumb, IntRect& endTrack);
    
    // Assume the track is a single piece by defaul


    /**
     * Get the minimum thumb length.
     * @param[in] : a pointer to the Scrollbar.
     * @param[out] : the minimum thumb length.
     * @code
     * @endcode
     */
    virtual int minimumThumbLength(Scrollbar*);


    /**
     *  paintScrollbarBackground
     */
    virtual void paintScrollbarBackground(GraphicsContext*, Scrollbar*) ;


    /**
     *  paintTrackBackground 
     */
    virtual void paintTrackBackground(GraphicsContext*, Scrollbar*, const IntRect&) ;


    /**
     *  paintTrackPiece 
     */
    virtual void paintTrackPiece(GraphicsContext*, Scrollbar*, const IntRect&, ScrollbarPart) ;

    /**
     * Paint the button.
     * @param[in] : a pointer to the graphics context.
     * @param[in] : a pointer to a scrollbar.
     * @param[in] : a rectangle.
     * @param[in] : a ScrollbarControlPartMask.
     * @code
     * @endcode
     */
    virtual void paintButton(GraphicsContext*, Scrollbar*, const IntRect&, ScrollbarPart) ;
    /**
     * Paint the thumb.
     * @param[in] : a pointer to the graphics context.
     * @param[in] : a pointer to a scrollbar.
     * @param[in] : a rectangle.
     * @code
     * @endcode
     */
    virtual void paintThumb(GraphicsContext*, Scrollbar*, const IntRect&) ;
};

}
