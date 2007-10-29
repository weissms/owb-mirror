/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2007 Apple Computer, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GIF_DECODER_H_
#define GIF_DECODER_H_

#include "BIImageDecoder.h"
#include "ImageDecoderCommonImplementation.h"

namespace BC {

class GIFImageDecoderPrivate;

// This class decodes the GIF image format.
class BCGIFImageDecoder : public BAL::BIImageDecoder
{
public:
    BCGIFImageDecoder();
    ~BCGIFImageDecoder();

    // Take the data and store it.
    virtual void setData(const Vector<char>* data, bool allDataReceived);

    // Whether or not the size information has been decoded yet.
    virtual bool isSizeAvailable() const;

    // The total number of frames for the image.  Will scan the image data for the answer
    // (without necessarily decoding all of the individual frames).
    virtual int frameCount();

    // The number of repetitions to perform for an animation loop.
    virtual int repetitionCount() const;

    virtual BAL::RGBA32Buffer* frameBufferAtIndex(size_t index);

    virtual unsigned frameDurationAtIndex(size_t /*index*/) { return 0; }

    virtual bool supportsAlpha() const { return true; }

    virtual IntSize size() const { return mImageDecoderCommonImplementation.size(); }

public:
    enum GIFQuery { GIFFullQuery, GIFSizeQuery, GIFFrameCountQuery };

    void decode(GIFQuery query, unsigned haltAtFrame) const;

    // Callbacks from the GIF reader.
    void sizeNowAvailable(unsigned width, unsigned height);
    void decodingHalted(unsigned bytesLeft);
    void haveDecodedRow(unsigned frameIndex, unsigned char* rowBuffer, unsigned char* rowEnd, unsigned rowNumber,
                        unsigned repeatCount);
    void frameComplete(unsigned frameIndex, unsigned frameDuration, bool includeInNextFrame);
    void gifComplete();

private:
    // Called to initialize a new frame buffer (potentially compositing it
    // with the previous frame and/or clearing bits in our image based off
    // the previous frame as well).
    void initFrameBuffer(BAL::RGBA32Buffer& buffer,
                         BAL::RGBA32Buffer* previousBuffer,
                         bool compositeWithPreviousFrame);

    bool m_frameCountValid;
    mutable GIFImageDecoderPrivate* m_reader;

    ImageDecoderCommonImplementation mImageDecoderCommonImplementation;
};

}

#endif
