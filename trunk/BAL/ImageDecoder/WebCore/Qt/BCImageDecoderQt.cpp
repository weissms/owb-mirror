/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
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

#include "config.h"
#include "ImageDecoder.h"

// It seems that we need more definitions than Qt to compile. Those definitons should go here (the implementation is in Graphics/WebCore/Qt/BCImageDecoderQt.*)

namespace WebCore {

RGBA32Buffer::RGBA32Buffer()
    : m_status(FrameEmpty)
    , m_duration(0)
    , m_disposalMethod(DisposeNotSpecified)
{
} 

void RGBA32Buffer::clear()
{
    m_status = FrameEmpty;
    // NOTE: Do not reset other members here; clearFrameBufferCache()
    // calls this to free the bitmap data, but other functions like
    // initFrameBuffer() and frameComplete() may still need to read
    // other metadata out of this frame later.
}

void RGBA32Buffer::zeroFill()
{
}

void RGBA32Buffer::copyBitmapData(const RGBA32Buffer& other)
{
    if (this == &other)
        return;
}

bool RGBA32Buffer::setSize(int newWidth, int newHeight)
{
    // NOTE: This has no way to check for allocation failure if the
    // requested size was too big...
    // Zero the image.
    zeroFill();

    return true;
}

bool RGBA32Buffer::hasAlpha() const
{
    return false;
}

void RGBA32Buffer::setHasAlpha(bool alpha)
{
}

void RGBA32Buffer::setStatus(FrameStatus status)
{
    m_status = status;
}

RGBA32Buffer& RGBA32Buffer::operator=(const RGBA32Buffer& other)
{
    if (this == &other)
        return *this;

    copyBitmapData(other);
    setRect(other.rect());
    setStatus(other.status());
    setDuration(other.duration());
    setDisposalMethod(other.disposalMethod());
    return *this;
}

int RGBA32Buffer::width() const {
    return 0;
}

int RGBA32Buffer::height() const {
    return 0;
}

} // namespace WebCore
