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
#ifndef _IMAGE_DECODER_COMMON_IMPLEMENTATION_
#define _IMAGE_DECODER_COMMON_IMPLEMENTATION_

#include <wtf/Vector.h>
#include "DeprecatedArray.h"
#include "BTRGBA32Buffer.h"

namespace BC
{

/**
 * @file ImageDecoderCommonImplementation.h
 * The Image Decoder Common Implementation
 */
class ImageDecoderCommonImplementation
{
//friend class BIImageDecoder;
public:
    ImageDecoderCommonImplementation() : m_sizeAvailable(false), m_failed(false) { }
    WebCore::IntSize size() const { return m_size; }
    void setSize( const WebCore::IntSize& aS ) { m_size = aS; }
    bool isSizeAvailable() const { return m_sizeAvailable; }
    void setSizeAvailable(bool b) { m_sizeAvailable = b; }
    bool failed() const { return m_failed; }
    void setFailed( bool b ) { m_failed = b; }
    void setData(const Vector<char>* data, bool /*allDataReceived*/) { m_data = data; } // FIXME treat allDataReceived
    Vector<BAL::RGBA32Buffer>& getFrameBufferCache() { return m_frameBufferCache; }
    const Vector<char>* data() const { return m_data; }

protected:
    const Vector<char>* m_data; // The encoded data.
    Vector<BAL::RGBA32Buffer> m_frameBufferCache;
    bool m_sizeAvailable;
    bool m_failed;
    WebCore::IntSize m_size;
};

} // namespace

#endif
