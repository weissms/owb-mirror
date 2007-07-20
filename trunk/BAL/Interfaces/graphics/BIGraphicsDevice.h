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
 * @file  BIGraphicsDevice.h
 *
 * Header file for BIGraphicsDevice.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef BIGRAPHICSDEVICE_H
#define BIGRAPHICSDEVICE_H

#include <stdint.h>
#include "RGBA32Array.h"

namespace WebCore {

class Image;
class IntPoint;
class IntRect;
class Color;

}

/**
 * The BIGraphicsDevice implementation
 *
 * @see Font, FontData
 */
namespace BAL {

class BINativeImage;
class BISurface;
class BTWidget;

class BIGraphicsDevice {
public:
    /**
     * Initialize an allocate the buffer.
     * Use the implementation header if the buffer is own by another object and simply passed in argument.
     */ 
    virtual void initialize(uint16_t width, uint16_t height, uint8_t depth) = 0;
    virtual void finalize() = 0;
    virtual BINativeImage* createNativeImage(const WebCore::IntSize size) = 0;
    virtual BINativeImage* createNativeImage(RGBA32Array&, const WebCore::IntSize size) = 0;
        
    virtual void copy(const BTWidget&, const BINativeImage&, WebCore::IntRect zone, WebCore::IntPoint, const uint8_t alphaChannel = 255) = 0;
    virtual void drawArc(const BTWidget&, const WebCore::IntRect, uint16_t startAngle, uint16_t angleSpan, const WebCore::Color) = 0;
    virtual void drawEllipse(const BTWidget&, WebCore::IntRect, const WebCore::Color) = 0;
    virtual void drawLine(const BTWidget&, WebCore::IntPoint, WebCore::IntPoint, const WebCore::Color) = 0;
    virtual void drawRect(const BTWidget&, WebCore::IntRect, const WebCore::Color) = 0;
    virtual void fillConvexPolygon(const BTWidget&, size_t numPoints, const WebCore::IntPoint*, const WebCore::Color) = 0;
    virtual void fillRect(const BTWidget&, WebCore::IntRect, const WebCore::Color) = 0;
    virtual void setClip(const BTWidget&, WebCore::IntRect) = 0;
    virtual WebCore::IntRect clip(const BTWidget&) = 0;
    virtual void stretchBlit(const BTWidget&, const BINativeImage&, WebCore::IntRect src, WebCore::IntRect dst, const uint8_t alphaChannel = 255) = 0;
    virtual WebCore::IntSize size() const = 0;
    virtual void update(const BTWidget&, const WebCore::IntRect) = 0;
    virtual void clear(const BTWidget&, const WebCore::IntRect) = 0;
    
    virtual ~BIGraphicsDevice() {}
};

}

#endif // BIGRAPHICSDEVICE_H


