/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2007 Pleyo.  All rights reserved.
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

#ifndef PNG_DECODER_H_
#define PNG_DECODER_H_

#include "BIImageDecoder.h"
#include "ImageDecoderCommonImplementation.h"
#include "ImageAnimationObserver.h"

namespace BC {

class PNGImageReader;

// This class decodes the PNG image format.
class BCPNGImageDecoder : public BAL::BIImageDecoder
{
public:
    BCPNGImageDecoder();
    ~BCPNGImageDecoder();

    // Take the data and store it.
    virtual void setData(const Vector<char>* data, bool allDataReceived);

    // Whether or not the size information has been decoded yet.
    virtual bool isSizeAvailable() const;

    virtual BAL::RGBA32Buffer* frameBufferAtIndex(size_t index);

    virtual bool supportsAlpha() const { return true; }

    virtual IntSize size() const { return mImageDecoderCommonImplementation.size(); }

		virtual int frameCount() { return 1; }

		virtual int repetitionCount() const { return WebCore::cAnimationNone; }

public:
    void decode(bool sizeOnly = false) const;

    PNGImageReader* reader() { return m_reader; }

    // Callbacks from libpng
    void decodingFailed() { mImageDecoderCommonImplementation.setFailed( true ); }
    void headerAvailable();
    void rowAvailable(unsigned char* rowBuffer, unsigned rowIndex, int interlacePass);
    void pngComplete();

private:
    mutable PNGImageReader* m_reader;

    ImageDecoderCommonImplementation mImageDecoderCommonImplementation;
};

}

#endif
