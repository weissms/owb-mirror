/*
 * Copyright (C) 2007 Pleyo.  All rights reserved.
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

/**
 * @file  BCGraphicsDeviceSDL.h
 *
 * Header file for BCGraphicsDeviceSDL.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef BCGRAPHICSDEVICESDL_H
#define BCGRAPHICSDEVICESDL_H

#include "BIGraphicsDevice.h"
#include "BTWidget.h"
#include <SDL/SDL.h>
#if PLATFORM(MAC)
// SDL on Mac has some tricks with main()
#include "SDL/SDL_main.h"
#endif

namespace BAL {

class BCNativeImage;

/**
 * The BIGraphicsDevice implementation
 *
 * @see Font, FontData
 */
class BCGraphicsDeviceSDL : public BIGraphicsDevice {
public:
    BCGraphicsDeviceSDL();
    /**
     *
     */
    virtual void initialize(uint16_t width, uint16_t height, uint8_t depth);
    /**
     *
     */
    virtual void finalize();
    virtual BINativeImage* createNativeImage(const WebCore::IntSize size);    
    virtual BINativeImage* createNativeImage(RGBA32Array&, const WebCore::IntSize size);    
    
    /**
     *
     */
    virtual void copy(const BTWidget&, const BINativeImage&, WebCore::IntRect, WebCore::IntPoint, const uint8_t alphaChannel);
    /**
     *
     */
    virtual void drawArc(const BTWidget&, const IntRect rect, uint16_t startAngle, uint16_t angleSpan, const WebCore::Color);
    /**
     *
     */
    virtual void drawEllipse(const BTWidget&, WebCore::IntRect, const WebCore::Color);
    /**
     *
     */
    virtual void drawLine(const BTWidget&, WebCore::IntPoint, WebCore::IntPoint, const WebCore::Color);
    /**
     *
     */
    virtual void drawRect(const BTWidget&, WebCore::IntRect, const WebCore::Color);
    /**
     *
     */
    virtual void fillConvexPolygon(const BTWidget&, size_t numPoints, const WebCore::IntPoint*, const WebCore::Color);
    /**
     *
     */
    virtual void fillRect(const BTWidget&, WebCore::IntRect, const WebCore::Color);
    /**
     *
     */
    virtual void setClip(const BTWidget&, WebCore::IntRect);
    /**
     *
     */
    virtual IntRect clip(const BTWidget&);
    /**
     *
     */
    virtual void stretchBlit(const BTWidget&, const BINativeImage&, WebCore::IntRect src, WebCore::IntRect dst, const uint8_t alphaChannel);
    virtual WebCore::IntSize size() const;
    virtual void update(const BTWidget&, const WebCore::IntRect);
    virtual void clear(const BTWidget&, const WebCore::IntRect);
private:
    inline void drawArc(const BTWidget&, const WebCore::Color, int zone, int xc, int yc, float& x0, float& y0, float x1, float y1, bool doSwap = true);
    SDL_Surface* applyTransparency(const BINativeImage&, const uint8_t alphaChannel);

    SDL_Surface* 	m_screen;
#ifndef NDEBUG
    bool                m_layoutTests;
#endif
};

}

#endif // BCGRAPHICSDEVICESDL_H


