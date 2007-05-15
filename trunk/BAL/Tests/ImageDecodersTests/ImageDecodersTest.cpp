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
#include <iostream>
#include <string>
#include <stdexcept>
#include <sys/stat.h>

#include "wtf/Vector.h"

#include "BALConfiguration.h"
#include "TestHelpers/BALFileReader.h"
#include "TestHelpers/BALBitmapWriter.h"
#include "TestHelpers/BALFileHelper.h"
#include "TestHelpers/BALFileComparison.h"

#include "BIImageDecoder.h"

using namespace BALTest;

class ImageDecodersTests {
public: 

  static void testJPEGDecoder()
  {
        std::string mireJpg = TestManager::GetInstance().getPath() + "ImageDecodersTests/Refs/mire.jpeg";
        std::string mireBmp = TestManager::GetInstance().getPath() + "ImageDecodersTests/Refs/mirejpeg.bmp";
	BAL::BIImageDecoder* aDecoder = BAL::createBCJPEGImageDecoder();
	testDecoder( aDecoder, false /* don't support alpha */,
	 	     mireJpg.c_str(),
		     mireBmp.c_str(), 1 );
		BAL::deleteBIImageDecoder(aDecoder);
  }

  static void testPNGDecoder()
  {
        std::string affichePng = TestManager::GetInstance().getPath() + "ImageDecodersTests/Refs/affiche.png";
        std::string afficheBmp = TestManager::GetInstance().getPath() + "ImageDecodersTests/Refs/affichepng.bmp";
        BAL::BIImageDecoder* aDecoder = BAL::createBCPNGImageDecoder();
	testDecoder( aDecoder, true /* supports alpha */,
			affichePng.c_str(),
			afficheBmp.c_str(), 1 );
		BAL::deleteBIImageDecoder(aDecoder);
  }

  static void testPNGDecoderAlpha()
  {
        std::string butterflyPng = TestManager::GetInstance().getPath() + "ImageDecodersTests/Refs/butterfly.png";
        std::string butterflyBmp = TestManager::GetInstance().getPath() + "ImageDecodersTests/Refs/butterflypng.bmp";
	BAL::BIImageDecoder* aDecoder = BAL::createBCPNGImageDecoder();
	  testDecoder( aDecoder, true /* supports alpha */,
			butterflyPng.c_str(),
			butterflyBmp.c_str(), 1 );
		BAL::deleteBIImageDecoder(aDecoder);
  }
	
  static void testGIFDecoderSimple()
  {
        std::string arteGif = TestManager::GetInstance().getPath() + "ImageDecodersTests/Refs/arte.gif";
        std::string arteBmp = TestManager::GetInstance().getPath() + "ImageDecodersTests/Refs/artegif.bmp";
	BAL::BIImageDecoder* aDecoder = BAL::createBCGIFImageDecoder();
	testDecoder( aDecoder, true /* supports alpha */,
			arteGif.c_str(),
			arteBmp.c_str(), 1 );
		BAL::deleteBIImageDecoder(aDecoder);
  }
		
  static void testGIFDecoderAnimated()
  {
        std::string bartGif = TestManager::GetInstance().getPath() + "ImageDecodersTests/Refs/Bart_and_Homer.gif";
        std::string bartBmp = TestManager::GetInstance().getPath() + "ImageDecodersTests/Refs/Bart_and_Homer%d.bmp";
		BAL::BIImageDecoder* aDecoder = BAL::createBCGIFImageDecoder();
	  testDecoder( aDecoder, true /* supports alpha */,
			bartGif.c_str(),
			bartBmp.c_str(), 6 );
		BAL::deleteBIImageDecoder(aDecoder);
  }

  static void testGIFDecoderAlpha()
  {
        std::string butterflyGif = TestManager::GetInstance().getPath() + "ImageDecodersTests/Refs/butterfly.gif";
        std::string butterflyBmp = TestManager::GetInstance().getPath() + "ImageDecodersTests/Refs/butterflygif.bmp";
		BAL::BIImageDecoder* aDecoder = BAL::createBCGIFImageDecoder();
	  testDecoder( aDecoder, true /* supports alpha */,
			butterflyGif.c_str(),
			butterflyBmp.c_str(), 1 );
		BAL::deleteBIImageDecoder(aDecoder);
  }
protected:
	/**
	* testDecoder
	*
	* @param[in] aDecoder : the image decoder to test
	* @param[in] bSupportsAlpha : whether decoder supports alpha
	* @param[in] aInputFile : the input file to read and decoder
	* @param[in] aRefFile : the reference file
	* @param[in] aExpectedFrameCount : the expected frame count
	* @note If aExpectedFrameCount is more than 1, then reference file \
	* contains a %d, to be replaced by index of frame starting at 1.
	*/
  static void testDecoder( BAL::BIImageDecoder* aDecoder,
		bool bSupportsAlpha,
		const std::string& aInputFile,
		const std::string& aRefFile,
		int aExpectedFrameCount )
  {
		TestManager::AssertTrue( "decoder here", aDecoder != NULL );
		if( aDecoder == NULL ) {
			return;
		}

		// Validate init values		
		TestManager::AssertTrue( "Size not available", !aDecoder->isSizeAvailable() );
		TestManager::AssertTrue( "Width is zero", aDecoder->size().width() == 0 );
		TestManager::AssertTrue( "Height is zero", aDecoder->size().height() == 0 );
		//TestManager::AssertTrue( "Frame count is zero", aDecoder->frameCount() == 0 ); FIXME returns 1 many times
		TestManager::AssertTrue( "Decoder supports alpha", aDecoder->supportsAlpha() == bSupportsAlpha );

		// Open a file
		BALFileReader aReader( aInputFile );
		TestManager::AssertTrue( "Input file is opened", aReader.IsOpened() );
		
		// Read it
		Vector<char> aArray;
		aReader.Read( aArray );
		
		// Set data into decoder
		aDecoder->setData( &aArray, true );
		TestManager::AssertTrue( "Frame count as expected", aDecoder->frameCount() == aExpectedFrameCount );
		
		for( int aFrame=0; aFrame < aDecoder->frameCount(); aFrame++ )
		{
			// Get resultant RGB buffer, frame per frame
			BAL::RGBA32Buffer* aRGBBuffer = aDecoder->frameBufferAtIndex(aFrame);
			TestManager::AssertTrue( "Buffer address is not NULL", aRGBBuffer != NULL );
			if( aRGBBuffer == NULL ) {
				return;
			}
		
			TestManager::AssertTrue( "Size is available", aDecoder->isSizeAvailable() );
			TestManager::AssertTrue( "Height is not null", aDecoder->size().height() );
			TestManager::AssertTrue( "Width is not null", aDecoder->size().width() );

			//printf( "Supports alpha %d\n", aDecoder->supportsAlpha() );
			//printf( "has alpha %d\n", aRGBBuffer->hasAlpha() );
				
			const BAL::RGBA32Array& aByteArray = aRGBBuffer->bytes();

      TestManager::AssertTrue( "Buffer Height is equal to decoded size",
        aDecoder->size().height() == aRGBBuffer->height());
			TestManager::AssertTrue( "Buffer Width is equal to decoded width",
        aDecoder->size().width() == aRGBBuffer->width());
		
			// Create temporary file
			std::string aTempFile;
			GetTemporaryUnexistingFileName( "/tmp/bitmap", ".bmp", aTempFile );
		
			// Write into it
			WriteRGBA32Bitmap( aTempFile, aByteArray, aRGBBuffer->rect().width(), aRGBBuffer->rect().height() );
		
			// Build reference file name : for gif case, there can be more than one frame,
			// so replace the %d in the name with the frame index
			char aReferenceFileName[1024];
			sprintf( aReferenceFileName, aRefFile.c_str(), aFrame+1 );
			
			// Compare data to reference
			BALFileComparison aComparison;
			bool bAreEqual = aComparison.AreEqual( aTempFile, aReferenceFileName );
			TestManager::AssertTrue( "Decoded result matches reference", bAreEqual );
		
			// Delete temporary file if equal
			// TODO write a BAL file object for deletion ?
			unlink( aTempFile.c_str() );
		}
	}
};

static TestNode gtestJPEGDecoder =
 { "testJPEGDecoder", "testJPEGDecoder", TestNode::AUTO, ImageDecodersTests::testJPEGDecoder, NULL };
static TestNode gtestPNGDecoder =
 { "testPNGDecoder", "testPNGDecoder", TestNode::AUTO, ImageDecodersTests::testPNGDecoder, NULL };
static TestNode gtestGIFDecoderSimple =
 { "testGIFDecoderSimple", "testGIFDecoderSimple", TestNode::AUTO, ImageDecodersTests::testGIFDecoderSimple, NULL };
static TestNode gtestGIFDecoderAnimated =
 { "testGIFDecoderAnimated", "testGIFDecoderAnimated", TestNode::AUTO, ImageDecodersTests::testGIFDecoderAnimated, NULL };
// FIXME RME Why in comment
// static TestNode testGIFDecoderAlpha =
// { "testGIFDecoderAlpha", "testGIFDecoderAlpha", TestNode::AUTO, ImageDecodersTests::testGIFDecoderAlpha, NULL };

TestNode* gImageDecoderTestNodeList[] = {
  &gtestJPEGDecoder,
  &gtestPNGDecoder,
  &gtestGIFDecoderSimple,
  &gtestGIFDecoderAnimated,
	NULL
};

TestNode gTestSuiteImageDecoder = {
    "TestImageDecoder",
    "test image decoders",
    TestNode::TEST_SUITE,
    NULL, /* no function, it's a test suite */
		gImageDecoderTestNodeList 
};
