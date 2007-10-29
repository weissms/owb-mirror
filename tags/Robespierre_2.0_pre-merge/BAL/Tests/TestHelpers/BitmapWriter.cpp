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
  * @file BALBitmapWriter.cpp
  *
  * Bitmap Writer
	*/
#include <string>
#include "BALBitmapWriter.h"

namespace BALTest
{

typedef struct
{
   unsigned short int type;                 /* Magic identifier            */
   unsigned int size;                       /* File size in bytes          */
   unsigned short int reserved1, reserved2;
   unsigned int offset;                     /* Offset to image data, bytes */
} HEADER;

typedef struct
{
   unsigned int size;               /* Header size in bytes      */
   int width,height;                /* Width and height of image */
   unsigned short int planes;       /* Number of colour planes   */
   unsigned short int bits;         /* Bits per pixel            */
   unsigned int compression;        /* Compression type          */
   unsigned int imagesize;          /* Image size in bytes       */
   int xresolution,yresolution;     /* Pixels per meter          */
   unsigned int ncolours;           /* Number of colours         */
   unsigned int importantcolours;   /* Important colours         */
} INFOHEADER;

int WriteRGBA32Bitmap( const std::string& aFileName, const BAL::RGBA32Array& aArray, int aWidth, int aHeight )
{
    if ((aArray.size() == 0) && (aWidth == 0 || aHeight == 0)) {
        printf("Array has a width or height but is empty !\n");
        return 1;
    }
	HEADER aHeader;
  INFOHEADER aInfoHeader;
	int aSizeOfHEADER = sizeof(aHeader);

	const char* p = aFileName.c_str();
  FILE* aOutputFile = fopen( aFileName.c_str(), "wb" );
	if( aOutputFile == NULL )
	{
	  printf( "Cannot open %s for writing\n", aFileName.c_str() );
		return 1;
	}

	memset( &aInfoHeader, 0, sizeof(INFOHEADER) );
  aInfoHeader.size = sizeof(aInfoHeader);               /* Header size in bytes      */
  aInfoHeader.width = aWidth;
	aInfoHeader.height = aHeight;
	aInfoHeader.planes = 1;
  aInfoHeader.bits = 32;
  aInfoHeader.compression = 0;
  aInfoHeader.imagesize = aWidth * aHeight * 4;          /* Image size in bytes       */
	aInfoHeader.xresolution = 0x0b12;
	aInfoHeader.yresolution = 0x0b12;

	memset( &aHeader, 0, sizeof(HEADER) );
	aHeader.type = 'MB';          /* Magic identifier            */
	aHeader.offset = aSizeOfHEADER + sizeof(aInfoHeader) - 2;
	aHeader.size = aInfoHeader.imagesize + aSizeOfHEADER + sizeof(aInfoHeader) - 2;  /* File size in bytes          */

  /* fwrite( &aHeader, 1, sizeof(HEADER), aOutputFile ); this does not work, for alignment reason !!! */
	fwrite( &aHeader.type, 1, sizeof(unsigned short int), aOutputFile );
  fwrite( &aHeader.size, 1, sizeof(int), aOutputFile );
  fwrite( &aHeader.reserved1, 1, sizeof(unsigned short int), aOutputFile );
  fwrite( &aHeader.reserved2, 1, sizeof(unsigned short int), aOutputFile );
  fwrite( &aHeader.offset, 1, sizeof(unsigned int), aOutputFile );

	fwrite( &aInfoHeader, 1, sizeof(INFOHEADER), aOutputFile );

	for( int aLine=aHeight-1; aLine >= 0; aLine-- )
	{
		for( int iCol=0; iCol < aWidth; ++iCol )
		{
            fwrite( &aArray[aLine*aWidth + iCol], 1, sizeof(unsigned), aOutputFile );
		}
	}


	fclose( aOutputFile );

	return 0;
}

}
