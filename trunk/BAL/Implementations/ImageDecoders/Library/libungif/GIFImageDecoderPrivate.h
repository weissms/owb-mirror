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

#ifndef _GIFIMAGEREADER_H_
#define _GIFIMAGEREADER_H_

#include "GIFImageDecoder.h"
#include "ImageObserver.h"
#include "BTLogHelper.h"
#include "gif_lib.h"

using BAL::RGBA32Buffer;
using BAL::RGBA32Array;

namespace BC {

class GIFImageDecoderPrivate
{
public:
    GIFImageDecoderPrivate(BCGIFImageDecoder* decoder)
        : m_gifFile(NULL)
        , m_decoder(decoder)
        , m_readOffset(0)
        , m_transparency(-1)
        , m_duration(0)
        , m_frameCount(0)
        , m_keepFrame(false)
    {}
    
    ~GIFImageDecoderPrivate();
    
    bool decode(const Vector<char>* data,
                BCGIFImageDecoder::GIFQuery query = BCGIFImageDecoder::GIFFullQuery,
                unsigned int haltFrame = -1);

    unsigned frameCount() const { return m_frameCount; }
    int repetitionCount() const { return 0; }
    void setReadOffset(unsigned o) { m_readOffset = o; }
    bool isTransparent() const { return (m_transparency >= 0); }
    void getColorMap(unsigned char*& map, unsigned& size) const;
    unsigned frameXOffset() const { return m_gifFile->Image.Left; }
    unsigned frameYOffset() const { return m_gifFile->Image.Top; }
    unsigned frameWidth() const { return m_gifFile->Image.Width; }
    unsigned frameHeight() const { return m_gifFile->Image.Height; }
    int transparentPixel() const { return m_transparency; }
    unsigned duration() const { return m_duration < 50 ? 50 : m_duration; }

private:
    GifFileType*        m_gifFile;
    BCGIFImageDecoder*  m_decoder;
    unsigned            m_readOffset;
    int16_t             m_transparency;
    uint16_t            m_duration;
    uint8_t             m_frameCount;
    bool                m_keepFrame;
};

}

#endif
