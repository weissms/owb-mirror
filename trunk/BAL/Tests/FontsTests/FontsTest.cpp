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

#include "config.h"
#include "BALConfiguration.h"
#include "TestHelpers/BALBitmapWriter.h"
#include "TestHelpers/BALFileReader.h"
#include "BTLogHelper.h"
#include "BTFont.h"
#include "FontFamily.h"
#include "BIFontCache.h"
#include "AtomicString.h"
#include "BIGraphicsContext.h"
#include "BIRGBA32Surface.h"
#include "StringImpl.h"
#include "../Implementations/Graphics/Cairo/RGBA32GraphicsContext.h"
#include "TestHelpers/BALFileComparison.h"

using WebCore::FontFamily;
using WebCore::FontDescription;

using namespace BAL;
using namespace BALTest;

/**
 * Low part of ASCII : from 32 to 127.
 */
static const char asciiFirstHalf[] = {
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
    0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43,
    0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b,
    0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73,
    0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0 };

/**
 * High part of ASCII : from 128 to 255.
 */
static const char asciiSecondHalf[] = {
    // warning non printable chars form 7f to 9f put cairo out of memory !!
    /*0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a,
    0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,
    0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,*/ 0xa0, 0xa1, 0xa2,
    0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae,
    0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba,
    0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6,
    0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2,
    0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde,
    0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
    0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6,
    0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff, 0 };

class FontsTest {
public:

    static void setUp()
    {
        // we're going to use AtomicString with setFamily so init
        WebCore::AtomicString::init();
    }

    static void tearDown()
    {
        // do nothing
    }

    static void testAnsiLowerHalf()
    {
        setUp();
        
        // we should do it once :
        WebCore::StringImpl message(asciiFirstHalf);

        // construct bitmap
        TextRun textRun(&message);
        TextStyle textStyle;
        FloatRect rect;
        BIGraphicsContext* context;

        FontDescription fontDescription;
        // this is normally computed by CSS and fixes the minimum size
        fontDescription.setComputedSize(28);
        Font font(fontDescription,0,0);
        // update m_fontList
        // needed or else Assertion `m_fontList' will failed.
        font.update();

        context = createContextForFont(font, textRun, textStyle, rect);
        writeBitmap("text_ansilower", context, font, textRun, textStyle, rect);
        deleteRGBA32GraphicsContext(context);
        
        tearDown();
    }

    static void testAnsiUpperHalf()
    {
        setUp();
        
        // construct message
        WebCore::StringImpl message(asciiSecondHalf);

        // construct bitmap
        TextRun textRun(&message);
        TextStyle textStyle;
        FloatRect rect;
        BIGraphicsContext* context;

        Font font = createFont();
        context = createContextForFont(font, textRun, textStyle, rect);
        writeBitmap("text_ansiupper", context, font, textRun, textStyle, rect);
        deleteRGBA32GraphicsContext(context);
        
        tearDown();
    }

    static void testNullText()
    {
        setUp();
        
        WebCore::StringImpl message("");
        TextRun textRun(&message);
        TextStyle textStyle;
        FloatRect rect;
        BIGraphicsContext* context;
        Font font = createFont();
        context = createContextForFont(font, textRun, textStyle, rect);
        //writeBitmap("/tmp/nulltext", context, font, textRun, textStyle, rect);
        deleteRGBA32GraphicsContext(context);
        
        tearDown();
    }

    static void testBold()
    {
        setUp();
        
        // construct message
        WebCore::StringImpl ansiMessage("This is a bold text");

        // construct bitmap
        TextRun textRun(&ansiMessage);
        TextStyle textStyle;
        FloatRect rect;
        BIGraphicsContext* context;

        // create bold font
        FontDescription fontDescription;
        fontDescription.setWeight(WebCore::cBoldWeight);
        fontDescription.setComputedSize(28);
        Font font(fontDescription,0,0);
        font.update();

        context = createContextForFont(font, textRun, textStyle, rect);
        writeBitmap("text_bold", context, font, textRun, textStyle, rect);
        deleteRGBA32GraphicsContext(context);
        
        tearDown();
    }

    static void testRtl()
    {
        setUp();
        
        // construct message
        WebCore::StringImpl ansiMessage("This is a right to left text");

        // construct bitmap
        TextRun textRun(&ansiMessage);
        TextStyle textStyle;
        textStyle.setRTL(true);
        FloatRect rect;
        BIGraphicsContext* context;

        // create bold font
        FontDescription fontDescription;
        fontDescription.setWeight(WebCore::cBoldWeight);
        fontDescription.setComputedSize(28);
        Font font(fontDescription,0,0);
        font.update();

        context = createContextForFont(font, textRun, textStyle, rect);
        writeBitmap("text_rtl", context, font, textRun, textStyle, rect);
        deleteRGBA32GraphicsContext(context);
        
        tearDown();
    }

    static void testItalic()
    {
        setUp();
        
        // construct message
        WebCore::StringImpl ansiMessage("This text is in Italic");

        // construct bitmap
        TextRun textRun(&ansiMessage);
        TextStyle textStyle;
        FloatRect rect;
        BIGraphicsContext* context;

        // create italic font
        FontDescription fontDescription;
        fontDescription.setComputedSize(28);
        fontDescription.setItalic(true);
        Font font(fontDescription,0,0);
        font.update();

        context = createContextForFont(font, textRun, textStyle, rect);
        writeBitmap("text_italic", context, font, textRun, textStyle, rect);
        deleteRGBA32GraphicsContext(context);
        
        tearDown();
    }

    static void testBoldItalic()
    {
        setUp();
        
        // construct message
        WebCore::StringImpl ansiMessage("This text is in Bold and Italic");

        // construct bitmap
        TextRun textRun(&ansiMessage);
        TextStyle textStyle;
        FloatRect rect;
        BIGraphicsContext* context;

        // create bold font
        FontDescription fontDescription;
        fontDescription.setWeight(WebCore::cBoldWeight);
        fontDescription.setComputedSize(28);
        fontDescription.setItalic(true);
        Font font(fontDescription,0,0);
        font.update();

        context = createContextForFont(font, textRun, textStyle, rect);
        writeBitmap("text_bolditalic", context, font, textRun, textStyle, rect);
        deleteRGBA32GraphicsContext(context);
        
        tearDown();
    }

    static void testSmallCaps()
    {
        setUp();
        
        // construct message
        WebCore::StringImpl ansiMessage("This text is in small caps");

        // construct bitmap
        TextRun textRun(&ansiMessage);
        TextStyle textStyle;
        FloatRect rect;
        BIGraphicsContext* context;

        // create bold font
        FontDescription fontDescription;
        fontDescription.setComputedSize(28);
        fontDescription.setSmallCaps(true);
        Font font(fontDescription,0,0);
        font.update();

        context = createContextForFont(font, textRun, textStyle, rect);
        writeBitmap("text_smallcaps", context, font, textRun, textStyle, rect);
        deleteRGBA32GraphicsContext(context);
        
        tearDown();
    }

    static void testSize()
    {
        setUp();
        
        testSizeWith(3);
        testSizeWith(6);
        testSizeWith(8);
        testSizeWith(10);
        testSizeWith(13);
        testSizeWith(17);
        testSizeWith(19);
        testSizeWith(100);
        
        tearDown();
    }

    static void testSizeWith(int size) {
        static char templateMessage[] = "Font test for size xxx";
        static char filename[] = "text_sizexxx";
        sprintf(templateMessage + strlen(templateMessage) - 3, "%03d", size);
        sprintf(filename + strlen(filename) - 3, "%03d", size);
        WebCore::StringImpl message(templateMessage);

        // construct bitmap
        TextRun textRun(&message);
        TextStyle textStyle;
        FloatRect rect;
        BIGraphicsContext* context;

        FontDescription fontDescription;
        fontDescription.setComputedSize(size);
        fontDescription.setSpecifiedSize(size);
        Font font(fontDescription,0,0);
        font.update();

        context = createContextForFont(font, textRun, textStyle, rect);
        writeBitmap(filename, context, font, textRun, textStyle, rect);
        deleteRGBA32GraphicsContext(context);
    }

    static void testArial() {
        setUp();
        
        // construct message
        WebCore::StringImpl ansiMessage("This text is in Arial font face");

        // construct bitmap
        TextRun textRun(&ansiMessage);
        TextStyle textStyle;
        FloatRect rect;
        BIGraphicsContext* context;


        // create font with custom family
        FontDescription fontDescription;
        fontDescription.setComputedSize(28);
        FontFamily family;
        family.setFamily("Arial");
        fontDescription.setFamily(family); // should be done before Font()
        Font font(fontDescription,0,0);

        font.update();
        context = createContextForFont(font, textRun, textStyle, rect);

        writeBitmap("text_Arial", context, font, textRun, textStyle, rect);
        deleteRGBA32GraphicsContext(context);
        
        tearDown();
    }

    static void testTimes() {
        setUp();
        
        // construct message
        WebCore::StringImpl ansiMessage("This text is in Times New Roman font face");

        // construct bitmap
        TextRun textRun(&ansiMessage);
        TextStyle textStyle;
        FloatRect rect;
        BIGraphicsContext* context;


        // create font with custom family
        FontDescription fontDescription;
        fontDescription.setComputedSize(28);
        FontFamily family;
        family.setFamily("Times New Roman");
        fontDescription.setFamily(family); // should be done before Font()
        Font font(fontDescription,0,0);

        font.update();
        context = createContextForFont(font, textRun, textStyle, rect);

        writeBitmap("text_Times", context, font, textRun, textStyle, rect);
        deleteRGBA32GraphicsContext(context);
        
        tearDown();
    }

    static void testCourier() {
        setUp();
        
            // construct message
        WebCore::StringImpl ansiMessage("This text is in Courier New font face");

        // construct bitmap
        TextRun textRun(&ansiMessage);
        TextStyle textStyle;
        FloatRect rect;
        BIGraphicsContext* context;


        // create font with custom family
        FontDescription fontDescription;
        fontDescription.setComputedSize(28);
        FontFamily family;
        family.setFamily("Courier New");
        fontDescription.setFamily(family); // should be done before Font()
        Font font(fontDescription,0,0);

        font.update();
        context = createContextForFont(font, textRun, textStyle, rect);

        writeBitmap("text_Courier", context, font, textRun, textStyle, rect);
        deleteRGBA32GraphicsContext(context);
        
        tearDown();
    }

    static void testUnicodeLatinA()
    {
        setUp();
        
        // construct message
        UChar message[0xff];
        for (int i=0; i<=0x7f; i++)
            message[i] = 0x0100+i;

        // construct bitmap
        TextRun textRun(message, 0x80);
        TextStyle textStyle;
        FloatRect rect;
        BIGraphicsContext* context;

        Font font = createFullGlyphsFont();
        context = createContextForFont(font, textRun, textStyle, rect);
        writeBitmap("text_latinA", context, font, textRun, textStyle, rect);
        deleteRGBA32GraphicsContext(context);
        
        tearDown();
    }

    static void testUnicodeLatinB() // not supported
    {
        setUp();
        
        // � is 0x20ac
        // construct message
        UChar message[0x7f];
        for (int i=0; i<=0x7f; i++)
            message[i] = 0x180+i;

        // put spaces instead of glyphs that produce "cairo_show_glyphs : out of memory"
        message[120]=0x20;
        message[121]=0x20;
        message[122]=0x20;
        message[123]=0x20;

        // construct bitmap
        TextRun textRun(message, 0x80);
        TextStyle textStyle;
        FloatRect rect;
        BIGraphicsContext* context;

        Font font = createFullGlyphsFont();
        context = createContextForFont(font, textRun, textStyle, rect);
        writeBitmap("text_latinB", context, font, textRun, textStyle, rect);
        deleteRGBA32GraphicsContext(context);
        
        tearDown();
    }

    static void testUnicodeCharsets()
    {
        setUp();
        
//        testUnicodeRange(0x2c60, 0x2c7f, "text_latinC", 1);
//        testUnicodeRange(0x2c80, 0x2cff, "text_latinCoptic", 1);
        testUnicodeRange(0x1e00, 0x1e7f, "text_latinExtAdd", 0);
//        testUnicodeRange(0xfb00, 0xfb4f, "text_latinLigature", 1);
//        testUnicodeRange(0xff00, 0xffef, "text_latinFullWidth", 1);
//        testUnicodeRange(0x20a0, 0x20b5, "text_latinCurrency", 0);
        testUnicodeRange(0x20a0, 0x20a8, "text_latinCurrency", 0);
        testUnicodeRange(0x20ac, 0x20ad, "text_latinEuro", 0);
//        testUnicodeRange(0x0370, 0x03ff, "text_Greek", 0);
        testUnicodeRange(0x0370+31, 0x0370+40, "text_Greek1", 0);
        testUnicodeRange(0x0370+51, 0x0370+95, "text_Greek2", 0);
        
        tearDown();
    }

    static void testUnicodeRange(UChar from, UChar to, const char* file, int shouldFail)
    {
        setUp();
        
        // construct message
        UChar message[0xff];
        for (int i=0; i<=to-from; i++)
            message[i] = from+i;

        // construct bitmap
        TextRun textRun(message, to-from);
        TextStyle textStyle;
        FloatRect rect;
        BIGraphicsContext* context;

        Font font = createFullGlyphsFont();
        context = createContextForFont(font, textRun, textStyle, rect);
        log(file);
        TestManager::AssertTrue(file, shouldFail == writeBitmap(file, context,
                                        font, textRun, textStyle, rect));
        deleteRGBA32GraphicsContext(context);
        
        tearDown();
    }


    static Font createFont()
    {
        FontDescription fontDescription;
        // this is normally computed by CSS and fixes the minimum size
        fontDescription.setComputedSize(28);
        Font font(fontDescription,0,0);
        // update m_fontList
        // needed or else Assertion `m_fontList' will failed.
        font.update();

        return font;
    }
    /**
     * Free Serif contains 3513 glyphs !
     */
    static Font createFullGlyphsFont()
    {
        // create font with custom family
        FontDescription fontDescription;
        fontDescription.setComputedSize(28);
        FontFamily family;
        family.setFamily("Free Serif");
        fontDescription.setFamily(family); // should be done before Font()
        Font font(fontDescription,0,0);
            // update m_fontList
            // needed or else Assertion `m_fontList' will failed.
        font.update();

        return font;
    }
    static BIGraphicsContext* createContextForFont(
            Font& font, TextRun& textRun, TextStyle& textStyle, FloatRect& rect) {
        // get rectangle dimension to hold text
        rect = font.selectionRectForText(textRun, TextStyle(),
                                         IntPoint(), font.height());
        log(make_message("selectRectForText : %f,%f,%f,%f",
            rect.x(), rect.y(), rect.width(), rect.height()));

        // now create a GraphicContext with rect to hold our text
        BIGraphicsContext* context =
                createRGBA32GraphicsContext((unsigned)rect.width(), font.height());
        context->setAlpha(0.0);
        context->setPen(Color::white);
        return context;
    }

    static int writeBitmap(const char* filename, BIGraphicsContext* context, Font& font,
                TextRun& textRun, TextStyle& textStyle, FloatRect& rect) {

        // can't do a font.drawText, color info is in context !!
        //font.drawText(context, textRun, textStyle, FloatPoint(0,font.ascent()));
        context->setFont(font);
        context->drawText(textRun, IntPoint(0, font.ascent()), textStyle);
        // get surface and write it to a file
        RGBA32Array* surface =
                ((BIRGBA32Surface*)(RGBA32GraphicsContext*)(context))->getRGBA32Array();
        if (0 == surface)
        {
            log("Can't create RGBA32Array");
            return 1;
        }

        std::string testFile("/tmp/");
        testFile += filename;
        testFile += ".bmp";
        if (0 == WriteRGBA32Bitmap(testFile.c_str(), *surface,
                    (int)rect.width(), (int)rect.height())) {
            delete surface;
            // Compare data to reference
            BALFileComparison aComparison;
                        
            std::string aPath = TestManager::GetInstance().getPath() + "FontsTests/Refs/";
            
            std::string referenceFile(aPath);
            referenceFile += filename;
            referenceFile += ".bmp";
            bool areEqual = aComparison.AreEqual(testFile, referenceFile);
            log(make_message("%s == %s ? %d", testFile.c_str(), referenceFile.c_str(), areEqual));
            TestManager::AssertTrue( "Decoded result matches reference", areEqual );
            return 0;
        }
        else {
            delete surface;
            log("WriteRGBA32Bitmap failed");
            return 1;
        }
    }



private:

};

static TestNode gtestAnsiLowerHalf =
 { "testAnsiLowerHalf", "testAnsiLowerHalf", TestNode::AUTO, FontsTest::testAnsiLowerHalf, NULL };
static TestNode gtestAnsiUpperHalf =
 { "testAnsiUpperHalf", "testAnsiUpperHalf", TestNode::AUTO, FontsTest::testAnsiUpperHalf, NULL };
static TestNode gtestBold =
 { "testBold", "testBold", TestNode::AUTO, FontsTest::testBold, NULL };
static TestNode gtestRtl =
 { "testRtl", "testRtl", TestNode::AUTO, FontsTest::testRtl, NULL };
static TestNode gtestItalic =
 { "testItalic", "testItalic", TestNode::AUTO, FontsTest::testItalic, NULL };
static TestNode gtestBoldItalic =
 { "testBoldItalic", "testBoldItalic", TestNode::AUTO, FontsTest::testBoldItalic, NULL };
static TestNode gtestSmallCaps =
 { "testSmallCaps", "testSmallCaps", TestNode::AUTO, FontsTest::testSmallCaps, NULL };
static TestNode gtestSize =
 { "testSize", "testSize", TestNode::AUTO, FontsTest::testSize, NULL };
static TestNode gtestArial =
 { "testArial", "testArial", TestNode::AUTO, FontsTest::testArial, NULL };
static TestNode gtestTimes =
 { "testTimes", "testTimes", TestNode::AUTO, FontsTest::testTimes, NULL };
static TestNode gtestCourier =
 { "testCourier", "testCourier", TestNode::AUTO, FontsTest::testCourier, NULL };
static TestNode gtestUnicodeLatinA =
 { "testUnicodeLatinA", "testUnicodeLatinA", TestNode::AUTO, FontsTest::testUnicodeLatinA, NULL };
static TestNode gtestUnicodeLatinB =
 { "testUnicodeLatinB", "testUnicodeLatinB", TestNode::AUTO, FontsTest::testUnicodeLatinB, NULL };
static TestNode gtestUnicodeCharsets =
 { "testUnicodeCharsets", "testUnicodeCharsets", TestNode::AUTO, FontsTest::testUnicodeCharsets, NULL };

TestNode* gFontTestNodeList[] = {
  &gtestAnsiLowerHalf,
  &gtestAnsiUpperHalf,
  &gtestBold,
  &gtestRtl,
  &gtestItalic,
  &gtestBoldItalic,
  &gtestSmallCaps,
  &gtestSize,
  &gtestArial,
  &gtestTimes,
  &gtestCourier,
  &gtestUnicodeLatinA,
  &gtestUnicodeLatinB,
  &gtestUnicodeCharsets,
	NULL
};

TestNode gTestSuiteFont = {
    "TestFonts",
    "test fonts",
    TestNode::TEST_SUITE,
    NULL, /* no function, it's a test suite */
		gFontTestNodeList 
};

