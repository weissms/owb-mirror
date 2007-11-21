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
  * @file BTRGBA32Buffer.h
  *
  * Type RGBA32Buffer
  */

#ifndef BTRGBA32BUFFER_H
#define BTRGBA32BUFFER_H

#include "config.h"
#include "IntRect.h"
#include <wtf/Vector.h>
#include "DeprecatedArray.h"
#include "RGBA32Array.h"

namespace BAL {

typedef WebCore::DeprecatedArray<char> DeprecatedByteArray;

/**
 * The RGBA32Buffer object represents the decoded image data in RGBA32 format.  This buffer is what all
 * decoders write a single frame into.  Frames are then instantiated for drawing by being handed this buffer.
 */
class RGBA32Buffer
{
public:
    enum FrameStatus { FrameEmpty, FramePartial, FrameComplete };

    RGBA32Buffer() : m_height(0), m_status(FrameEmpty), m_duration(0),
                     m_includeInNextFrame(false), m_hasAlpha(false)
    {}

    /**
     * get RGBA32Array
     */
    const RGBA32Array& bytes() const { return m_bytes; }
    RGBA32Array& bytes() { return m_bytes; }
    /**
     * return rect
     */
    const WebCore::IntRect& rect() const { return m_rect; }
    /**
     * return height
     */
    unsigned height() const { return m_height; }
    /**
     * return width
     */
    unsigned width() const { return m_rect.width(); }
    /**
     * return status
     */
    FrameStatus status() const { return m_status; }
    /**
     * return duration
     */
    unsigned duration() const { return m_duration; }
    /**
     * test if the buffer is include in next frame
     */
    bool includeInNextFrame() const { return m_includeInNextFrame; }
    /**
     * test if the buffer has alpha
     */
    bool hasAlpha() const { return m_hasAlpha; }

    /**
     * set buffet rect
     */
    void setRect(const WebCore::IntRect& r) { m_rect = r; }
    /**
     * set height
     */
    void ensureHeight(unsigned rowIndex) { if (rowIndex > m_height) m_height = rowIndex; }
    /**
     * set status
     */
    void setStatus(FrameStatus s) { m_status = s; }
    /**
     * set duration
     */
    void setDuration(unsigned duration) { m_duration = duration; }
    /**
     * set if the buffer is in next frame
     */
    void setIncludeInNextFrame(bool n) { m_includeInNextFrame = n; }
    /**
     * set if the buffer has alpha
     */
    void setHasAlpha(bool alpha) { m_hasAlpha = alpha; }

    /**
     * set RGBA
     */
    static void setRGBA(unsigned& pos, unsigned r, unsigned g, unsigned b, unsigned a)
    {
        // We store this data pre-multiplied.
        if (a == 0)
            pos = 0;
        else {
            if (a < 255) {
                float alphaPercent = a / 255.0f;
                r = (unsigned int)((float)r * alphaPercent);
                g = (unsigned int)((float)g * alphaPercent);
                b = (unsigned int)((float)b * alphaPercent);
            }
            pos = (a << 24 | r << 16 | g << 8 | b);
        }
    }

private:
    RGBA32Array m_bytes;
    WebCore::IntRect m_rect;    // The rect of the original specified frame within the overall buffer.
                       // This will always just be the entire buffer except for GIF frames
                       // whose original rect was smaller than the overall image size.
    unsigned m_height; // The height (the number of rows we've fully decoded).
    FrameStatus m_status; // Whether or not this frame is completely finished decoding.
    unsigned m_duration; // The animation delay.
    bool m_includeInNextFrame; // Whether or not the next buffer should be initially populated with our data.
    bool m_hasAlpha; // Whether or not any of the pixels in the buffer have transparency.
};

}

#endif
