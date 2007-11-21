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

#include "GIFImageDecoder.h"
#include "GIFImageDecoderPrivate.h"
#include "ImageObserver.h"
#include "BTLogHelper.h"
#include "gif_lib.h"

using BAL::RGBA32Buffer;
using BAL::RGBA32Array;

namespace BAL {
    BIImageDecoder* createBCGIFImageDecoder()
    {
        return new BC::BCGIFImageDecoder();
    }
}

namespace BC {

BCGIFImageDecoder::BCGIFImageDecoder()
    : m_frameCountValid(true)
    , m_reader(0)
{
    DBGML(MODULE_IMAGEDECODERS, LEVEL_WARNING, "Create GIF Decoder\n");
}

BCGIFImageDecoder::~BCGIFImageDecoder()
{
    delete m_reader;
    m_reader = 0;
}

// Take the data and store it.
void BCGIFImageDecoder::setData(const Vector<char>* data, bool allDataReceived)
{
    if (mImageDecoderCommonImplementation.failed())
        return;

    // Cache our new data.
    mImageDecoderCommonImplementation.setData(data, allDataReceived);

    // Our frame count is now unknown.
    m_frameCountValid = false;

    // Create the GIF reader.
    if (!m_reader && !mImageDecoderCommonImplementation.failed()) {
        m_reader = new GIFImageDecoderPrivate(this);
    }
}

// Whether or not the size information has been decoded yet.
bool BCGIFImageDecoder::isSizeAvailable() const
{
    // If we have pending data to decode, send it to the GIF reader now.
    if (!mImageDecoderCommonImplementation.isSizeAvailable() && m_reader) {
        if (mImageDecoderCommonImplementation.failed())
            return false;

        // The decoder will go ahead and aggressively consume everything up until the first
        // size is encountered.
        decode(GIFSizeQuery, 0);
    }

    return mImageDecoderCommonImplementation.isSizeAvailable();
}

// The total number of frames for the image.  Will scan the image data for the answer
// (without necessarily decoding all of the individual frames).
int BCGIFImageDecoder::frameCount()
{
    if (m_reader && !m_frameCountValid) {
        m_reader->decode(mImageDecoderCommonImplementation.data(), GIFFrameCountQuery);
        m_frameCountValid = true;
        mImageDecoderCommonImplementation.getFrameBufferCache().resize(m_reader->frameCount());
    }
    return mImageDecoderCommonImplementation.getFrameBufferCache().size();
}

// The number of repetitions to perform for an animation loop.
int BCGIFImageDecoder::repetitionCount() const
{
    // We don't have to do any decoding to determine this, since the loop count was determined after
    // the initial query for size.
    if (m_reader)
        return m_reader->repetitionCount();
    return WebCore::cAnimationNone;
}

RGBA32Buffer* BCGIFImageDecoder::frameBufferAtIndex(size_t index)
{
    if (mImageDecoderCommonImplementation.getFrameBufferCache().isEmpty())
        mImageDecoderCommonImplementation.getFrameBufferCache().resize(1);
    
    if (index >= mImageDecoderCommonImplementation.getFrameBufferCache().size())
        return 0;

    RGBA32Buffer& frame = mImageDecoderCommonImplementation.getFrameBufferCache()[index];
    if (frame.status() != RGBA32Buffer::FrameComplete && m_reader)
        // Decode this frame.
        decode(GIFFullQuery, index+1);
    return &frame;
}

// Feed data to the GIF reader.
void BCGIFImageDecoder::decode(GIFQuery query, unsigned haltAtFrame) const
{
    if (mImageDecoderCommonImplementation.failed())
        return;

    ImageDecoderCommonImplementation* pImpl = const_cast<ImageDecoderCommonImplementation*>
			( &mImageDecoderCommonImplementation );

    if (pImpl->getFrameBufferCache().isEmpty())
        pImpl->getFrameBufferCache().resize(1);
        
    pImpl->setFailed( !m_reader->decode(mImageDecoderCommonImplementation.data(), query, haltAtFrame) );

    if (mImageDecoderCommonImplementation.failed()) {
        delete m_reader;
        m_reader = 0;
    }
}

// Callbacks from the GIF reader.
void BCGIFImageDecoder::sizeNowAvailable(unsigned width, unsigned height)
{
    mImageDecoderCommonImplementation.setSize(IntSize(width, height));
    mImageDecoderCommonImplementation.setSizeAvailable(true);
}

void BCGIFImageDecoder::decodingHalted(unsigned bytesLeft)
{
    m_reader->setReadOffset(mImageDecoderCommonImplementation.data()->size() - bytesLeft);
}

void BCGIFImageDecoder::initFrameBuffer(RGBA32Buffer& buffer,
                                      RGBA32Buffer* previousBuffer,
                                      bool compositeWithPreviousFrame)
{
    // Initialize the frame rect in our buffer.
    IntRect frameRect(m_reader->frameXOffset(), m_reader->frameYOffset(),
                      m_reader->frameWidth(), m_reader->frameHeight());
    buffer.setRect(frameRect);

    bool isSubRect = (frameRect.x() > 0 || frameRect.y() > 0 ||
                      frameRect.width() < mImageDecoderCommonImplementation.size().width() ||
                      frameRect.height() < mImageDecoderCommonImplementation.size().height());

    // Let's resize our buffer now to the correct width/height and then
    // initialize portions of it if needed.
    RGBA32Array& bytes = buffer.bytes();

    // If the disposal method of the previous frame said to stick around, then we need
    // to copy that frame into our frame.  We also dont want to have any impact on
    // anything outside our frame's rect, so if we don't overlay the entire image,
    // then also composite with the previous frame.
    if (previousBuffer && (compositeWithPreviousFrame || isSubRect)) {
        bytes = previousBuffer->bytes();
        buffer.ensureHeight(mImageDecoderCommonImplementation.size().height());
        buffer.setHasAlpha(previousBuffer->hasAlpha());
    } else // Resize to the width and height of the image.
        bytes.resize(mImageDecoderCommonImplementation.size().width() * mImageDecoderCommonImplementation.size().height());

    if (isSubRect) {
        // We need to go ahead and initialize the first frame to make sure
        // that areas outside the subrect start off transparent.
        if (!previousBuffer) {
            bytes.fill(0);
            buffer.setHasAlpha(true);
        } else if (!compositeWithPreviousFrame) {
            // Now this is an interesting case.  In the case where we fill
            // the entire image, we effectively do a full clear of the image (and thus
            // don't have to initialize anything in our buffer).
            //
            // However in the case where we only fill a piece of the image, two problems occur:
            // (1) We need to wipe out the area occupied by the previous frame, which
            // could also have been a subrect.
            // (2) Anything outside the previous frame's rect *and* outside our current
            // frame's rect should be left alone.
            // We have handled (2) by just initializing our buffer from the previous frame.
            // Our subrect will correctly overwrite the previous frame's contents as we
            // decode rows.  However that still leaves the problem of having to wipe out
            // the area occupied by the previous frame that does not overlap with
            // the new frame.
            if (previousBuffer->rect() != frameRect) {
                // We have to clear out the entire previous subframe.
                bool sawAlpha = buffer.hasAlpha();
                IntRect prevRect = previousBuffer->rect();
                unsigned end = prevRect.y() + prevRect.height();
                unsigned* src;
                for (unsigned i = prevRect.y(); i < end; i++) {
                    unsigned* curr = buffer.bytes().data() + (i * mImageDecoderCommonImplementation.size().width() + prevRect.x());
                    unsigned* end = curr + prevRect.width();
                    while (curr != end) {
                        if (!sawAlpha) {
                            sawAlpha = true;
                            buffer.setHasAlpha(true);
                        }
                        RGBA32Buffer::setRGBA(*curr++, 0, 0, 0, 0);
                    }
                }
            }
        }
    }

    // Update our status to be partially complete.
    buffer.setStatus(RGBA32Buffer::FramePartial);
}

void BCGIFImageDecoder::haveDecodedRow(unsigned frameIndex,
                                     unsigned char* rowBuffer,   // Pointer to single scanline temporary buffer
                                     unsigned char* rowEnd,
                                     unsigned rowNumber,  // The row index
                                     unsigned repeatCount) // How many times to repeat the row
{
    
    if (frameIndex >= mImageDecoderCommonImplementation.getFrameBufferCache().size()) 
        mImageDecoderCommonImplementation.getFrameBufferCache().resize(frameIndex + 1);
    
    // Resize to the width and height of the image.
    RGBA32Buffer& buffer = mImageDecoderCommonImplementation.getFrameBufferCache()[frameIndex];
    RGBA32Buffer* previousBuffer = (frameIndex > 0) ? &mImageDecoderCommonImplementation.getFrameBufferCache()[frameIndex-1] : 0;
    bool compositeWithPreviousFrame = previousBuffer && previousBuffer->includeInNextFrame();

    if (buffer.status() == RGBA32Buffer::FrameEmpty)
        initFrameBuffer(buffer, previousBuffer, compositeWithPreviousFrame);

    if (rowBuffer == 0)
        return;

    unsigned colorMapSize = 0;
    unsigned char* colorMap = NULL;
    m_reader->getColorMap(colorMap, colorMapSize);
    if (!colorMap)
        return;

    // The buffers that we draw are the entire image's width and height, so a final output frame is
    // width * height RGBA32 values in size.
    //
    // A single GIF frame, however, can be smaller than the entire image, i.e., it can represent some sub-rectangle
    // within the overall image.  The rows we are decoding are within this
    // sub-rectangle.  This means that if the GIF frame's sub-rectangle is (x,y,w,h) then row 0 is really row
    // y, and each row goes from x to x+w.
    unsigned dstPos = (m_reader->frameYOffset() + rowNumber) * mImageDecoderCommonImplementation.size().width() + m_reader->frameXOffset();
    unsigned* dst = buffer.bytes().data() + dstPos;
    unsigned* currDst = dst;
    unsigned char* currentRowByte = rowBuffer;

    bool hasAlpha = m_reader->isTransparent();
    bool sawAlpha = false;
    while (currentRowByte != rowEnd) {
        if ((!hasAlpha || *currentRowByte != m_reader->transparentPixel()) && *currentRowByte < colorMapSize) {
            unsigned colorIndex = *currentRowByte * 3;
            unsigned red = colorMap[colorIndex];
            unsigned green = colorMap[colorIndex + 1];
            unsigned blue = colorMap[colorIndex + 2];
            RGBA32Buffer::setRGBA(*currDst, red, green, blue, 255);
        } else {
            if (!sawAlpha) {
                sawAlpha = true;
                buffer.setHasAlpha(true);
            }

            if (!compositeWithPreviousFrame)
                RGBA32Buffer::setRGBA(*currDst, 0, 0, 0, 0);
        }
        currDst++;
        currentRowByte++;
    }

    if (repeatCount > 1) {
        // Copy the row |repeatCount|-1 times.
        unsigned size = (currDst - dst) * sizeof(unsigned);
        unsigned width = mImageDecoderCommonImplementation.size().width();
        unsigned* end = buffer.bytes().data() + width * mImageDecoderCommonImplementation.size().height();
        currDst = dst + width;
        for (unsigned i = 1; i < repeatCount; i++) {
            if (currDst + size > end) // Protect against a buffer overrun from a bogus repeatCount.
                break;
            memcpy(currDst, dst, size);
            currDst += width;
        }
    }

    // Our partial height is rowNumber + 1, e.g., row 2 is the 3rd row, so that's a height of 3.
    // Adding in repeatCount - 1 to rowNumber + 1 works out to just be rowNumber + repeatCount.
    buffer.ensureHeight(rowNumber + repeatCount);
}

void BCGIFImageDecoder::frameComplete(unsigned frameIndex, unsigned frameDuration, bool includeInNextFrame)
{
    RGBA32Buffer& buffer = mImageDecoderCommonImplementation.getFrameBufferCache()[frameIndex];
    buffer.setStatus(RGBA32Buffer::FrameComplete);
    buffer.setDuration(frameDuration);
    buffer.setIncludeInNextFrame(includeInNextFrame);
    buffer.ensureHeight(mImageDecoderCommonImplementation.size().height());
}

void BCGIFImageDecoder::gifComplete()
{
}

}
