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
 * @file BIImageDecoder.h
 *
 * Interface of Image Decoder.
 */

#ifndef BIIMAGE_DECODER_H_
#define BIIMAGE_DECODER_H_

#include "config.h"
#include "IntRect.h"
#include <wtf/Vector.h>
#include "DeprecatedArray.h"
#include "RGBA32Array.h"
#include "BTRGBA32Buffer.h"

using WebCore::DeprecatedByteArray;
using WebCore::IntSize;
using WebCore::IntRect;

namespace BAL {

/**
 * The ImageDecoder class represents the interface class for specific image format decoders
 * (e.g., GIF, JPG, PNG, ICO) to derive from.  All decoders decode into RGBA32 format
 * and the base class manages the RGBA32 frame cache.
 */
class BIImageDecoder
{
public:
    /**
    * BIImageDecoder destructor
    */
    virtual ~BIImageDecoder() { };

    /**
     * All specific decoder plugins must do something with the data they are given.
     * FIXME : Vector should be passed by ref
     */
    virtual void setData(const Vector<char>* data, bool allDataReceived) = 0;

    /**
     *  Whether or not the size information has been decoded yet.
     */
    virtual bool isSizeAvailable() const = 0;

    /**
     * Requests the size.
     */
    virtual WebCore::IntSize size() const = 0;

    /** The total number of frames for the image.  Classes that support multiple frames
     * will scan the image data for the answer if they need to (without necessarily
     * decoding all of the individual frames).
     */
    virtual int frameCount() = 0;

    /**
     * The number of repetitions to perform for an animation loop.
     */
    virtual int repetitionCount() const = 0;

    /** Called to obtain the RGBA32Buffer full of decoded data for rendering.  The
     * decoder plugin will decode as much of the frame as it can before handing
     * back the buffer.
     */
    virtual RGBA32Buffer* frameBufferAtIndex(size_t index) = 0;

    /**
     * Whether or not the underlying image format even supports alpha transparency.
     */
    virtual bool supportsAlpha() const = 0 ;
};

}

#endif
