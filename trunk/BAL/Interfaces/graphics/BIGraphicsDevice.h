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
     * @param[in] width: width of the buffer to allocate
     * @param[in] height: height of the buffer to allocate
     * @param[in] depth: depth of the buffer to allocate
     */ 
    virtual void initialize(uint16_t width, uint16_t height, uint8_t depth) = 0;

    /**
     * deallocate all surfaces allocated through intialize() method.
     */
    virtual void finalize() = 0;

    /**
     * Create an empty buffer called NativeImage which can be blitted into primary buffer.
     * @param[in] IntSize: size of the buffer to allocate
     */
    virtual BINativeImage* createNativeImage(const WebCore::IntSize size) = 0;

    /**
     * Create a buffer filled with data from an RGBA32Array.
     * This method is typically used to allocate buffer for decoded images or fonts.
     * @param[in] RGBA32Array&: the data to fill the buffer with
     * @param[in] IntSize: size of the buffer to allocate (must be size of the RGBA32Array)
     */
    virtual BINativeImage* createNativeImage(RGBA32Array&, const WebCore::IntSize size) = 0;

    /**
     * Copy a NativeImage (buffer) into a widget buffer.
     * @param[in] BTWidget
     * @param[in] BINativeImage: the buffer to copy
     * @param[in] IntRect: where to copy buffer
     * @param[in] IntPoint
     * @param[in] alphaChannel: global alpha to apply to the copied buffer
     */
    virtual void copy(const BTWidget&, const BINativeImage&, WebCore::IntRect zone, WebCore::IntPoint, const uint8_t alphaChannel = 255) = 0;

    /**
     * draw an arc into widget buffer.
     * @param[in] BTWidget: use widget buffer to draw
     * @param[in] IntRect: rectangle which contains arc
     * @param[in] startAngle
     * @param[in] angleSpan: endAngle = startAngle + angleSpan
     * @param[in] Color: color of the arc to draw
     */
    virtual void drawArc(const BTWidget&, const WebCore::IntRect, uint16_t startAngle, uint16_t angleSpan, const WebCore::Color) = 0;

    /**
     * draw an ellipse into widget buffer.
     * @param[in] BTWidget: use widget buffer to draw
     * @param[in] IntRect: rectangle which contains ellipse. Ellipse specific params can be computed from this rectangle.
     * @param[in] Color: color of the ellipse to draw
     */
    virtual void drawEllipse(const BTWidget&, WebCore::IntRect, const WebCore::Color) = 0;

    /**
     * draw an line into widget buffer.
     * @param[in] BTWidget: use widget buffer to draw
     * @param[in] IntPoint: starting point
     * @param[in] IntPoint: end point
     * @param[in] Color: color of the line to draw
     */
    virtual void drawLine(const BTWidget&, WebCore::IntPoint, WebCore::IntPoint, const WebCore::Color) = 0;

    /**
     * draw an empty rectangle into widget buffer.
     * @param[in] BTWidget: use widget buffer to draw
     * @param[in] IntRect: rectangle to draw
     * @param[in] Color: color of the line to draw
     */
    virtual void drawRect(const BTWidget&, WebCore::IntRect, const WebCore::Color) = 0;

    /**
     * Draw a filled convex polygon into widget buffer.
     * I mean from a triangle to whatever.
     * Of course to draw square or rectangle, use a method like drawRect().
     * @param[in] BTWidget: use widget buffer to draw
     * @param[in] size_t: number of points of the polygon
     * @param[in] IntPoint*: list of numPoint size which contains each point coordinate
     * @param[in] Color: color of the polygon to draw
     */
    virtual void fillConvexPolygon(const BTWidget&, size_t numPoints, const WebCore::IntPoint*, const WebCore::Color) = 0;

    /**
     * Draw a filled rectangle into widget buffer.
     * @param[in] BTWidget: use widget buffer to draw
     * @param[in] IntRect: rectangle to draw
     * @param[in] Color: color of the polygon to draw
     */
    virtual void fillRect(const BTWidget&, WebCore::IntRect, const WebCore::Color) = 0;

    /**
     * Set a clipping zone for the widget buffer.
     * @param[in] BTWidget: use widget buffer
     * @param[in] IntRect: clipping zone to set
     */
    virtual void setClip(const BTWidget&, WebCore::IntRect) = 0;

    /**
     * Get the clipping zone for the widget buffer.
     * @param[in] BTWidget: use widget buffer
     * @return IntRect: the clipping zone for the widget
     */
    virtual WebCore::IntRect clip(const BTWidget&) = 0;

    /**
     * Stretch a buffer and blit it into the widget buffer.
     * If you do not need to stretch the buffer before blitting, use copy() method.
     * @param[in] BTWidget: use widget buffer
     * @param[in] BINativeImage&: the buffer to stretch and blit
     * @param[in] IntRect: size and coordinate of the buffer to blit
     * @param[in] IntRect: size and coordinate of the destination blit
     * @param[in] alphaChannel: global alpha to apply to the copied buffer
     */
    virtual void stretchBlit(const BTWidget&, const BINativeImage&, WebCore::IntRect src, WebCore::IntRect dst, const uint8_t alphaChannel = 255) = 0;

    /**
     * Get the size of the Graphics device
     * @return IntSize: size
     */
    virtual WebCore::IntSize size() const = 0;

    /**
     * Update zone in the widget buffer
     * @param[in] BTWidget: use widget buffer
     * @param[in] IntRect: zone to update
     */
    virtual void update(const BTWidget&, const WebCore::IntRect) = 0;

    /**
     * Clear a zone in the widget buffer
     * FIXME should redraw with background color.
     * @param[in] BTWidget: use widget buffer
     * @param[in] IntRect: the rectangular zone to clear
     */
    virtual void clear(const BTWidget&, const WebCore::IntRect) = 0;

    /**
     * Destructor for the Graphics device
     */
    virtual ~BIGraphicsDevice() {}
};

}

#endif // BIGRAPHICSDEVICE_H


