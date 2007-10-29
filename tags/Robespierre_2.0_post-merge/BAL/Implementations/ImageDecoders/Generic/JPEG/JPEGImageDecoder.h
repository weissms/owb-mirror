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
 #ifndef BAL_JPEG_DECODER_H_
#define BAL_JPEG_DECODER_H_

#include "BIImageDecoder.h"
#include "ImageDecoderCommonImplementation.h"
#include "ImageObserver.h"

namespace BC {

class JPEGImageReader;

/**
  * JPEG Image Decoder
  *
  * This class decodes the JPEG image format.
  */
class BCJPEGImageDecoder : public BAL::BIImageDecoder
{
public:
    BCJPEGImageDecoder();
    virtual ~BCJPEGImageDecoder();

    // Take the data and store it.
    virtual void setData(const Vector<char>* data, bool allDataReceived);

    // Whether or not the size information has been decoded yet.
    virtual bool isSizeAvailable() const;

    virtual BAL::RGBA32Buffer* frameBufferAtIndex(size_t index);

    virtual bool supportsAlpha() const { return false; }

    virtual IntSize size() const { return mImageDecoderCommonImplementation.size(); }

    virtual int frameCount() { return 1; }

    virtual int repetitionCount() const { return WebCore::cAnimationNone; } // FIXME RME move cAnimationNone

public:
    // Following implementation needs to be public, to be accessed by reader
    void decode(bool sizeOnly = false) const;

    JPEGImageReader* reader() { return m_reader; }

    void setSize(int width, int height) {
        if (!mImageDecoderCommonImplementation.isSizeAvailable()) {
            mImageDecoderCommonImplementation.setSizeAvailable(true);
            mImageDecoderCommonImplementation.setSize( IntSize(width, height) );
        }
    }

    bool outputScanlines();
    void jpegComplete();

private:
    mutable JPEGImageReader* m_reader;

    ImageDecoderCommonImplementation mImageDecoderCommonImplementation;
};

}

#endif
