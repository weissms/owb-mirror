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

// code from libungif4g/gba/giftest.c
#define RGB8(r,g,b) ( (((b)>>3)<<10) | (((g)>>3)<<5) | ((r)>>3) )
#define GAMMA(x)    (x)
const short InterlacedOffset[] = { 0, 4, 2, 1 }; /* The way Interlaced image should. */
const short InterlacedJumps[] = { 8, 8, 4, 2 };    /* be read - offsets and jumps... */

namespace BC {

int readFunc(GifFileType* GifFile, GifByteType* buf, int count)
{
    char* ptr = reinterpret_cast<char*>(GifFile->UserData);
    memcpy(buf, ptr, count);
    GifFile->UserData = ptr + count;
    return count;
}

GIFImageDecoderPrivate::~GIFImageDecoderPrivate()
{
    if (m_gifFile && DGifCloseFile(m_gifFile) == GIF_ERROR) {
        DBGML(MODULE_IMAGEDECODERS, LEVEL_WARNING, "Error cleaning gif struct\n");
    }
}
bool GIFImageDecoderPrivate::decode(const Vector<char>* data,
            BCGIFImageDecoder::GIFQuery query,
            unsigned int haltFrame)
{
    if (m_gifFile == 0 && m_frameCount != 0) // gif is complete
        return false;

    char* userData = const_cast<char*>(data->data());
    unsigned char* rowBuffer; 
    int i, j, Row, Col, Width, Height, extCode, Count;
    GifRecordType RecordType;
    GifByteType *extension;

    if ((m_gifFile = DGifOpen(static_cast<void*>(userData), readFunc)) == NULL) {
        DBGML(MODULE_IMAGEDECODERS, LEVEL_WARNING, "Can't open gif stream\n");
        return false;   
    }

    m_decoder->sizeNowAvailable(m_gifFile->SWidth, m_gifFile->SHeight);

    m_frameCount = 0;
    rowBuffer = new unsigned char[m_gifFile->SWidth];
    
    /* Scan the content of the GIF file and load the image(s) in: */
    do {
        if (DGifGetRecordType(m_gifFile, &RecordType) == GIF_ERROR) {
            // PrintGifError(); may give more info
            DBGML(MODULE_IMAGEDECODERS, LEVEL_WARNING, "Record type error\n");
            return false;
        }

        switch (RecordType) {
        case IMAGE_DESC_RECORD_TYPE:
            if (DGifGetImageDesc(m_gifFile) == GIF_ERROR) {
                DBGML(MODULE_IMAGEDECODERS, LEVEL_WARNING, "Image desc record type error\n");
                return false;
            }
            Row = m_gifFile->Image.Top; /* Image Position relative to Screen. */
            Col = m_gifFile->Image.Left;
            Width = m_gifFile->Image.Width;
            Height = m_gifFile->Image.Height;
            if (m_gifFile->Image.Interlace) {
                /* Need to perform 4 passes on the images: */
                for (Count = i = 0; i < 4; i++)
                    for (j = Row + InterlacedOffset[i]; j < Row + Height;
                    j += InterlacedJumps[i]) {
                        if (DGifGetLine(m_gifFile, rowBuffer, Width) == GIF_ERROR) {
                            DBGML(MODULE_IMAGEDECODERS, LEVEL_WARNING, "Line error\n");
                            return false;
                        }
                        else
                            m_decoder->haveDecodedRow(m_frameCount, rowBuffer, rowBuffer+Width, j, 0);
                    }
            }
            else {
                for (i = 0; i < Height; i++) {
                    if (DGifGetLine(m_gifFile, rowBuffer, Width) == GIF_ERROR) {
                        DBGML(MODULE_IMAGEDECODERS, LEVEL_WARNING, "Line error\n");
                        return false;
                    }
                    else
                    {
                        m_decoder->haveDecodedRow(m_frameCount, rowBuffer, rowBuffer+Width, i/*row*/, 0);
                    }
                }
            }
            
            m_decoder->frameComplete(m_frameCount, duration(), m_keepFrame/*bool includeInNextFrame*/);
            m_frameCount++;
            
            break;
        case EXTENSION_RECORD_TYPE:
            /* Skip any extension blocks in file: */
            if (DGifGetExtension(m_gifFile, &extCode, &extension) == GIF_ERROR) {
                DBGML(MODULE_IMAGEDECODERS, LEVEL_WARNING, "Extension block error\n");
                return false;
            }
            while (extension != NULL) {
                switch (extCode) {
                    
                    case GRAPHICS_EXT_FUNC_CODE:
                        if (extension[0] >= 4) { // extension length large enough ?
                            if ((extension[1] & 1) && (m_transparency < 0)) {
                               m_transparency = (int)extension[4];
                            }
                            m_duration = extension[2] * 10;
                        }
                        else {
                            m_transparency = -1;
                            m_duration = 0;   
                        }
                        m_keepFrame = ((extension[1] & 0x7) != 1); // keep frame in buffer ?
                        
                        break;
                    case COMMENT_EXT_FUNC_CODE:
                    case PLAINTEXT_EXT_FUNC_CODE:
                    case APPLICATION_EXT_FUNC_CODE:
                        break;
                    default:
                        break;
                }
                extension = NULL;
                    
                if (DGifGetExtensionNext(m_gifFile, &extension) == GIF_ERROR) {
                    PrintGifError();
                    return false;
                }
            }
            break;
        case TERMINATE_RECORD_TYPE:
            break;
        default:                    /* Should be traps by DGifGetRecordType. */
            break;
        }
    }
    while (RecordType != TERMINATE_RECORD_TYPE);

    delete rowBuffer;
    m_decoder->gifComplete();

    if (m_gifFile && DGifCloseFile(m_gifFile) == GIF_ERROR) {
        DBGML(MODULE_IMAGEDECODERS, LEVEL_WARNING, "Error cleaning gif struct\n");
    }
    m_gifFile = 0;
    return true; // decoding completed correctly
}

void GIFImageDecoderPrivate::getColorMap(unsigned char*& map, unsigned& size) const
{
    if (m_gifFile->Image.ColorMap) {
        map = reinterpret_cast<unsigned char*>(m_gifFile->Image.ColorMap->Colors);
        size = m_gifFile->Image.ColorMap->ColorCount;
        return;
    }
    if (m_gifFile->SColorMap) {
        map = reinterpret_cast<unsigned char*>(m_gifFile->SColorMap->Colors);
        size = m_gifFile->SColorMap->ColorCount;
        return;            
    }
    DBGML(MODULE_IMAGEDECODERS, LEVEL_WARNING, "No colormap\n");
}

}
