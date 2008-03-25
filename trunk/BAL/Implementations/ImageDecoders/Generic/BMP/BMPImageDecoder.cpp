/*
 * Copyright (C) 2008 Pleyo.  All rights reserved.
 * Copyright (C) 2008 Guofan Wang.
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
 * DISCLAIMED. IN NO EVENT SHALL ORIGYN OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "BMPImageDecoder.h"
using BAL::RGBA32Buffer;
using BAL::RGBA32Array;

namespace BAL {
    BIImageDecoder* createBCBMPImageDecoder() { return new BC::BCBMPImageDecoder(); }
}

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;

namespace BC {

#define BMP_FORMAT_1 1
#define BMP_FORMAT_4 4
#define BMP_FORMAT_8 8
#define BMP_FORMAT_16 16
#define BMP_FORMAT_24 24
#define BMP_FORMAT_32 32

#define RGB_TYPE_QUAD    0
#define RGB_TYPE_TRIPLE  1
// BMP info struct

typedef struct tagRGBTRIPLE {
    BYTE rgbtBlue;
    BYTE rgbtGreen;
    BYTE rgbtRed;
} RGBTRIPLE;

typedef struct tagBITMAPCOREHEADER {
    DWORD bcSize;
    WORD  bcWidth;
    WORD  bcHeight;
    WORD  bcPlanes;
    WORD  bcBitCount;
} BITMAPCOREHEADER;

typedef struct _BITMAPCOREINFO {
    BITMAPCOREHEADER  bmciHeader;
    RGBTRIPLE         bmciColors[1];
} BITMAPCOREINFO;

typedef struct tagRGBQUAD {
    BYTE    rgbBlue;
    BYTE    rgbGreen;
    BYTE    rgbRed;
    BYTE    rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFOHEADER {
    DWORD  biSize;
    LONG   biWidth;
    LONG   biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    LONG   biXPelsPerMeter;
    LONG   biYPelsPerMeter;
    DWORD  biClrUsed;
    DWORD  biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagBITMAPFILEHEADER {
    WORD    bfDummy; // for memory alignment
    WORD    bfType;
    DWORD   bfSize;
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFO {
    BITMAPINFOHEADER bmiHeader;
    BYTE *bmiColors;
    RGBQUAD *palette;
    BYTE rgbType;  // 0 for RGBQUAD, 1 for RGBTRIPLE
} BITMAPINFO, *PBITMAPINFO;

class BMPImageReader {

    public:
        BMPImageReader(BCBMPImageDecoder* decoder = 0)
                : m_decoder(decoder)
        {
            m_readOffset = 0;
            memset(&m_info, 0, sizeof(BITMAPINFO));
            m_pFileInfo = NULL;
        }

        ~BMPImageReader() { close(); }

        void close() {}

        bool decode(const Vector<char>* data,
                    BCBMPImageDecoder::BMPQuery query = BCBMPImageDecoder::BMPFullQuery,
                    unsigned int haltFrame = -1)
        {
            const char *pdata = data->data();
            // At 1st time, read BMP info, show it at 2nd time.

            if (m_pFileInfo == NULL) {
                pdata -= 2;  // for 'bfDummy'
                m_pFileInfo = (BITMAPFILEHEADER *)pdata;

                m_info.bmiColors = (BYTE *)(pdata + 2 + m_pFileInfo->bfOffBits);
                pdata += sizeof(BITMAPFILEHEADER);
                unsigned short headerSize = pdata[0];

                switch (headerSize) {
                case sizeof(BITMAPCOREHEADER):
                                // copy BITMAPCOREHEADER to BITMAPINFOHEADER
                {
                    BITMAPCOREHEADER *pCoreHeader = NULL;
                    pCoreHeader = (BITMAPCOREHEADER *)pdata;
                    m_info.bmiHeader.biSize = headerSize;
                    m_info.bmiHeader.biWidth = pCoreHeader->bcWidth;
                    m_info.bmiHeader.biHeight = pCoreHeader->bcHeight;
                    m_info.bmiHeader.biPlanes = pCoreHeader->bcPlanes;
                    m_info.bmiHeader.biBitCount = pCoreHeader->bcBitCount;
                    m_info.rgbType = RGB_TYPE_TRIPLE;
                }

                break;
                case sizeof(BITMAPINFOHEADER):
                                memcpy(&m_info.bmiHeader, pdata, headerSize);
                    m_info.rgbType = RGB_TYPE_QUAD;
                    break;

                default:
                    return false;
                }

                m_decoder->setSize(m_info.bmiHeader.biWidth, m_info.bmiHeader.biHeight);

                if (m_info.bmiHeader.biBitCount < BMP_FORMAT_16) {
                    // if 'biBitCount >= BMP_FORMAT_16', BMP doesn't have palette
                    pdata += headerSize;
                    m_info.palette = (RGBQUAD *)pdata;
                }
            } else {
                if (!m_info.bmiHeader.biCompression)
                    m_decoder->outputScanlines(haltFrame);
                else
                    return false; // FIXME: need to support compressed BMP file
            }

            return true;
        }

        int repetitionCount() const { return WebCore::cAnimationNone; }

        BITMAPINFO *info() { return &m_info; }

    private:
        BITMAPFILEHEADER *m_pFileInfo;
        BITMAPINFO m_info;
        BCBMPImageDecoder* m_decoder;
        unsigned m_readOffset;
};

BCBMPImageDecoder::BCBMPImageDecoder()
        : m_reader(0)
{
}

BCBMPImageDecoder::~BCBMPImageDecoder()
{
    delete m_reader;
}

// Take the data and store it.
void BCBMPImageDecoder::setData(const Vector<char>* data, bool allDataReceived)
{
    if (mImageDecoderCommonImplementation.failed())
        return;

    // Cache our new data.
    mImageDecoderCommonImplementation.setData(data, allDataReceived);

    // Create the BMP reader.
    if (!m_reader && !mImageDecoderCommonImplementation.failed())
        m_reader = new BMPImageReader(this);
}

// Whether or not the size information has been decoded yet.
bool BCBMPImageDecoder::isSizeAvailable() const
{
    // If we have pending data to decode, send it to the BMP reader now.
    if (!mImageDecoderCommonImplementation.isSizeAvailable() && m_reader) {
        if (mImageDecoderCommonImplementation.failed())
            return false;

        // The decoder will go ahead and aggressively consume everything up until the first
        // size is encountered.
        decode(BMPSizeQuery, 0);
    }

    return mImageDecoderCommonImplementation.isSizeAvailable();
}

// The total number of frames for the image.  Will scan the image data for the answer
// (without necessarily decoding all of the individual frames).
int BCBMPImageDecoder::frameCount()
{
    return 1;
}

// The number of repetitions to perform for an animation loop.
int BCBMPImageDecoder::repetitionCount() const
{
    // We don't have to do any decoding to determine this, since the loop count was determined after
    // the initial query for size.
    if (m_reader)
        return m_reader->repetitionCount();

    return WebCore::cAnimationNone;
}

RGBA32Buffer* BCBMPImageDecoder::frameBufferAtIndex(size_t index)
{
    if (index)
        return 0;

    if (mImageDecoderCommonImplementation.getFrameBufferCache().isEmpty())
        mImageDecoderCommonImplementation.getFrameBufferCache().resize(1);

    RGBA32Buffer& frame = mImageDecoderCommonImplementation.getFrameBufferCache()[index];

    if (frame.status() != RGBA32Buffer::FrameComplete && m_reader)
        // Decode this frame.
        decode(BMPFullQuery, index + 1);

    return &frame;
}

// Feed data to the BMP reader.
void BCBMPImageDecoder::decode(BMPQuery query, unsigned haltAtFrame) const
{
    if (mImageDecoderCommonImplementation.failed())
        return;

    ImageDecoderCommonImplementation* pImpl = const_cast<ImageDecoderCommonImplementation*>
            (&mImageDecoderCommonImplementation);

    pImpl->setFailed(!m_reader->decode(mImageDecoderCommonImplementation.data(), query, haltAtFrame));

    if (mImageDecoderCommonImplementation.failed()) {
        delete m_reader;
        m_reader = 0;
    }
}

void BCBMPImageDecoder::bmpComplete()
{
    delete m_reader;
    m_reader = 0;
}

void BCBMPImageDecoder::setSize(int width, int height)
{
    if (!mImageDecoderCommonImplementation.isSizeAvailable()) {
        mImageDecoderCommonImplementation.setSizeAvailable(true);
        mImageDecoderCommonImplementation.setSize(IntSize(width, height));
    }
}

bool BCBMPImageDecoder::outputScanlines(unsigned int haltFrame)
{
    if (mImageDecoderCommonImplementation.getFrameBufferCache().isEmpty())
        return false;

    // Resize to the width and height of the image.
    RGBA32Buffer& buffer = mImageDecoderCommonImplementation.getFrameBufferCache()[0];

    if (buffer.status() == RGBA32Buffer::FrameEmpty) {
        // Let's resize our buffer now to the correct width/height.
        RGBA32Array& bytes = buffer.bytes();
        bytes.resize(
            mImageDecoderCommonImplementation.size().width() * mImageDecoderCommonImplementation.size().height());

        // Update our status to be partially complete.
        buffer.setStatus(RGBA32Buffer::FramePartial);

        // For JPEGs, the frame always fills the entire image.
        buffer.setRect(IntRect(0, 0,
                               mImageDecoderCommonImplementation.size().width(),
                               mImageDecoderCommonImplementation.size().height()));
    }

    unsigned* dst = buffer.bytes().data();

    BITMAPINFO *pinfo = m_reader->info();

    BYTE *pcolor = pinfo->bmiColors; // bmp color table
    unsigned short color_size = pinfo->bmiHeader.biBitCount > 8 ? (pinfo->bmiHeader.biBitCount >> 3) : 1;
    unsigned int linebytes = (pinfo->bmiHeader.biWidth * pinfo->bmiHeader.biBitCount + 31) / 32 * 4;
    unsigned short nExtraBytes = linebytes - ((pinfo->bmiHeader.biWidth * pinfo->bmiHeader.biBitCount) >> 3);
    unsigned char rgbSize = 0;

    switch (pinfo->rgbType) {

    case RGB_TYPE_QUAD:
        rgbSize = sizeof(RGBQUAD);
        break;

    case RGB_TYPE_TRIPLE:
        rgbSize = sizeof(RGBTRIPLE);
        break;

    default:
        rgbSize = sizeof(RGBTRIPLE);
        break;
    }

    // draw from the last line, so move to the end
    dst = dst + pinfo->bmiHeader.biHeight * pinfo->bmiHeader.biWidth;

    switch (pinfo->bmiHeader.biBitCount) {

    case BMP_FORMAT_1:

        for (unsigned short i = 0; i < pinfo->bmiHeader.biHeight; i++) {
            dst -= pinfo->bmiHeader.biWidth;

            for (unsigned short j = 0; j < pinfo->bmiHeader.biWidth; j++) {
                unsigned short k = j % 8;
                // each bit represents a pixel, so we use mask to get bit's value.
                // one byte has 8 bits, from the highest bit to the lowest bit, the masks are:
                // 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
                BYTE mask = 1 << (8 - k - 1);
                BYTE index = (*pcolor & mask) > 0 ? 1 : 0;
                RGBQUAD *p = (RGBQUAD *)((BYTE *)pinfo->palette + rgbSize * index);
                unsigned b = p->rgbBlue;
                unsigned g = p->rgbGreen;
                unsigned r = p->rgbRed;
                RGBA32Buffer::setRGBA(*(dst + j), r, g, b, 0xFF);
                // move to next byte

                if (k == 7)
                    pcolor ++;
            }

            if (nExtraBytes)
                pcolor += nExtraBytes;
        }

        break;

    case BMP_FORMAT_4:

        for (unsigned short i = 0; i < pinfo->bmiHeader.biHeight; i++) {
            dst -= pinfo->bmiHeader.biWidth;

            for (unsigned short j = 0; j < pinfo->bmiHeader.biWidth; j++) {
                unsigned short k = j % 2;
                BYTE index = (k == 0) ? (*pcolor >> 4) : (*pcolor & 0x0f);
                RGBQUAD *p = (RGBQUAD *)((BYTE *)pinfo->palette + rgbSize * index);
                unsigned b = p->rgbBlue;
                unsigned g = p->rgbGreen;
                unsigned r = p->rgbRed;
                RGBA32Buffer::setRGBA(*(dst + j), r, g, b, 0xFF);

                if (k == 1)
                    pcolor ++;
            }

            if (nExtraBytes)
                pcolor += nExtraBytes;
        }

        break;

    case BMP_FORMAT_8:

        for (unsigned short i = 0; i < pinfo->bmiHeader.biHeight; i++) {
            dst -= pinfo->bmiHeader.biWidth;

            for (unsigned short j = 0; j < pinfo->bmiHeader.biWidth; j++) {
                BYTE index = *pcolor;
                RGBQUAD *p = (RGBQUAD *)((BYTE *)pinfo->palette + rgbSize * index);
                unsigned b = p->rgbBlue;
                unsigned g = p->rgbGreen;
                unsigned r = p->rgbRed;
                pcolor += color_size;
                RGBA32Buffer::setRGBA(*(dst + j), r, g, b, 0xFF);
            }

            if (nExtraBytes)
                pcolor += nExtraBytes;
        }

        break;

    case BMP_FORMAT_16:

        for (unsigned short i = 0; i < pinfo->bmiHeader.biHeight; i++) {
            dst -= pinfo->bmiHeader.biWidth;

            for (unsigned short j = 0; j < pinfo->bmiHeader.biWidth; j++) {
                unsigned b = (*pcolor & 0x1f) << 3;
                unsigned g = ((*(pcolor + 1) & 0x03) << 6) | ((*pcolor & 0xe0) >> 2);
                unsigned r = (*(pcolor + 1) & 0x7c) << 1;
                pcolor += color_size;
                RGBA32Buffer::setRGBA(*(dst + j), r, g, b, 0xFF);
            }

            if (nExtraBytes)
                pcolor += nExtraBytes;
        }

        break;

    case BMP_FORMAT_24:

    case BMP_FORMAT_32:

        for (unsigned short i = 0; i < pinfo->bmiHeader.biHeight; i++) {
            dst -= pinfo->bmiHeader.biWidth;

            for (unsigned short j = 0; j < pinfo->bmiHeader.biWidth; j++) {
                unsigned b = *pcolor;
                unsigned g = *(pcolor + 1);
                unsigned r = *(pcolor + 2);
                pcolor += color_size;
                RGBA32Buffer::setRGBA(*(dst + j), r, g, b, 0xFF);
            }

            if (nExtraBytes)
                pcolor += nExtraBytes;
        }

        break;
    }

    buffer.setStatus(RGBA32Buffer::FrameComplete);

    return true;
}

}
