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
#include "BTLogHelper.h"
#include "BIGraphicsContext.h"
#include "BIGraphicsDevice.h"
#include "BTScrollView.h"
#include "TestHelpers/BALBitmapWriter.h"
#include "TestHelpers/BALFileReader.h"
#include "StringImpl.h"
#include "TestHelpers/BALFileComparison.h"
#include "RGBA32Array.h"

using namespace BAL;
using namespace BALTest;

class GraphicsDevice {
public:

static void testDrawSquare()
{
    // construct bitmap
    IntRect rect(1,1,8,8);
    IntRect widgetRect(0,0,10,10);
    RGBA32Array* array = new RGBA32Array();
    BTScrollView widget;
    
    array->resize(widgetRect.height() * widgetRect.width());
    init(widget, array, widgetRect.size());
    
    getBIGraphicsDevice()->clear(widget, widgetRect);
    getBIGraphicsDevice()->drawRect(widget, rect, Color::black);
    
    writeBitmap("draw_square", array, widgetRect);
}

static void testDrawRect()
{
    IntRect rect(1,1,8,3);
    IntRect widgetRect(0,0,10,10);
    RGBA32Array* array = new RGBA32Array();
    BTScrollView widget;
    
    array->resize(widgetRect.height() * widgetRect.width());
    init(widget, array, widgetRect.size());
    
    getBIGraphicsDevice()->clear(widget, widgetRect);
    getBIGraphicsDevice()->drawRect(widget, rect, Color::black);
    
    writeBitmap("draw_rect", array, widgetRect);
}

static void testFillSquare()
{
    // construct bitmap
    IntRect rect(1,1,8,8);
    IntRect widgetRect(0,0,10,10);
    RGBA32Array* array = new RGBA32Array();
    BTScrollView widget;
    
    array->resize(widgetRect.height() * widgetRect.width());
    init(widget, array, widgetRect.size());
    
    getBIGraphicsDevice()->clear(widget, widgetRect);
    getBIGraphicsDevice()->fillRect(widget, rect, Color::black);
    
    writeBitmap("fill_square", array, widgetRect);
    
}

static void testFillAlphaSquare()
{
    // construct bitmap
    IntRect widgetRect(0,0,100,100);
    BTScrollView widget;

    getBIGraphicsDevice()->initialize(100, 100, 32);
    widget.setBackingStore(getBIGraphicsDevice()->createNativeImage(widgetRect.size()));
    
    //we fill the widget with an opaque blue square
    getBIGraphicsDevice()->clear(widget, widgetRect);
    getBIGraphicsDevice()->fillRect(widget, widgetRect, 0xff0000ff);
    
    //blit it to the m_screen
    getBIGraphicsDevice()->update(widget, widgetRect);
    
    //we fill the widget with an alpha white square
    getBIGraphicsDevice()->clear(widget, widgetRect);
    getBIGraphicsDevice()->fillRect(widget, widgetRect, 0xaaffffff);
    
    //blit it to the m_screen (it must hide the previous m_screen even if it has alpha)
    getBIGraphicsDevice()->update(widget, widgetRect);
    
    TestManager::AskForChecking("The square should be all white, if you see some blue color press 0 to signal an error, else press 1\n Is the square all white ?");
}

static void testFillRect()
{
    IntRect rect(1,1,8,3);
    IntRect widgetRect(0,0,10,10);
    RGBA32Array* array = new RGBA32Array();
    BTScrollView widget;
    
    array->resize(widgetRect.height() * widgetRect.width());
    init(widget, array, widgetRect.size());
    
    getBIGraphicsDevice()->clear(widget, widgetRect);
    getBIGraphicsDevice()->fillRect(widget, rect, Color::black);
    
    writeBitmap("fill_rect", array, widgetRect);
    
}

static void testDrawOnePixelLargeRect()
{
    // construct bitmap
    IntRect rect(1,1,1,8);
    IntRect widgetRect(0,0,10,10);
    RGBA32Array* array = new RGBA32Array();
    BTScrollView widget;
    
    array->resize(widgetRect.height() * widgetRect.width());
    init(widget, array, widgetRect.size());
    
    getBIGraphicsDevice()->clear(widget, widgetRect);
    getBIGraphicsDevice()->drawRect(widget, rect, Color::black);

    writeBitmap("draw_1PixLargeRect", array, widgetRect);
    
}

static void testDrawOnePixelLongRect()
{
    // construct bitmap
    IntRect rect(1,1,8,1);
    IntRect widgetRect(0,0,10,10);
    RGBA32Array* array = new RGBA32Array();
    BTScrollView widget;
    
    array->resize(widgetRect.height() * widgetRect.width());
    init(widget, array, widgetRect.size());
    
    getBIGraphicsDevice()->clear(widget, widgetRect);
    getBIGraphicsDevice()->drawRect(widget, rect, Color::black);

    writeBitmap("draw_1PixLongRect", array, widgetRect);
    
}

static void testDrawNullRect()
{
    // construct bitmap
    IntRect rect(1,1,0,0);
    IntRect widgetRect(0,0,10,10);
    RGBA32Array* array = new RGBA32Array();
    BTScrollView widget;
    
    array->resize(widgetRect.height() * widgetRect.width());
    init(widget, array, widgetRect.size());
    
    getBIGraphicsDevice()->clear(widget, widgetRect);
    getBIGraphicsDevice()->drawRect(widget, rect, Color::black);
    
    writeBitmap("draw_NullRect", array, widgetRect);
    
}

static void testDrawNullRect2()
{
    // construct bitmap
    IntRect rect(1,1,8,0);
    IntRect widgetRect(0,0,10,10);
    RGBA32Array* array = new RGBA32Array();
    BTScrollView widget;
    
    array->resize(widgetRect.height() * widgetRect.width());
    init(widget, array, widgetRect.size());
    
    getBIGraphicsDevice()->clear(widget, widgetRect);
    getBIGraphicsDevice()->drawRect(widget, rect, Color::black);
    
    writeBitmap("draw_NullRect2", array, widgetRect);
    
}

static void testDrawNullRect3()
{
    // construct bitmap
    IntRect rect(1,1,0,8);
    IntRect widgetRect(0,0,10,10);
    RGBA32Array* array = new RGBA32Array();
    BTScrollView widget;
    
    array->resize(widgetRect.height() * widgetRect.width());
    init(widget, array, widgetRect.size());
    
    getBIGraphicsDevice()->clear(widget, widgetRect);
    getBIGraphicsDevice()->drawRect(widget, rect, Color::black);
    
    writeBitmap("draw_NullRect3", array, widgetRect);
    
}
static void init(BTScrollView& widget, RGBA32Array* array, IntSize size)
{
    BIGraphicsDevice* device = getBIGraphicsDevice();
    //there is no need to initialize the graphic device
    
    BINativeImage* image = device->createNativeImage(*array, size);
    widget.setBackingStore(image);
}

static int writeBitmap(const char* filename, RGBA32Array* surface, IntRect& rect)
{
    std::string testFile("/tmp/");
    testFile += filename;
    testFile += ".bmp";
    if (0 == WriteRGBA32Bitmap(testFile.c_str(), *surface,
                               (int)(rect.width()), (int)(rect.height()))) {
        // Compare data to reference
        BALFileComparison aComparison;
        
        std::string aPath = TestManager::GetInstance().getPath() + "GraphicsTests/Refs/";
        
        std::string referenceFile(aPath);
        referenceFile += filename;
        referenceFile += ".bmp";
        bool areEqual = aComparison.AreEqual(testFile, referenceFile);
        log(make_message("%s == %s ? %d", testFile.c_str(), referenceFile.c_str(), areEqual));
        TestManager::AssertTrue( "Decoded result matches reference", areEqual );
        return 0;
    }
    else {
        log("WriteRGBA32Bitmap failed");
        return 1;
    }
    return 0;
}



private:

};

static TestNode gtestDrawSquare =
{ "testDrawSquare", "testDrawSquare", TestNode::AUTO, GraphicsDevice::testDrawSquare, NULL };
static TestNode gtestDrawRect =
{ "testDrawRect", "testDrawRect", TestNode::AUTO, GraphicsDevice::testDrawRect, NULL };
static TestNode gtestFillSquare =
{ "testFillSquare", "testFillSquare", TestNode::AUTO, GraphicsDevice::testFillSquare, NULL };
static TestNode gtestFillAlphaSquare =
{ "testFillAlphaSquare", "testFillAlphaSquare", TestNode::AUTO, GraphicsDevice::testFillAlphaSquare, NULL };
static TestNode gtestFillRect =
{ "testFillRect", "testFillRect", TestNode::AUTO, GraphicsDevice::testFillRect, NULL };
static TestNode gtestDrawOnePixelLargeRect =
{ "testDrawOnePixelLargeRect", "testDrawOnePixelLargeRect", TestNode::AUTO, GraphicsDevice::testDrawOnePixelLargeRect, NULL };
static TestNode gtestDrawOnePixelLongRect =
{ "testDrawOnePixelLargeRect", "testDrawOnePixelLongRect", TestNode::AUTO, GraphicsDevice::testDrawOnePixelLongRect, NULL };
static TestNode gtestDrawNullRect =
{ "testDrawNullRect", "testDrawNullRect", TestNode::AUTO, GraphicsDevice::testDrawNullRect, NULL };
static TestNode gtestDrawNullRect2 =
{ "testDrawNullRect2", "testDrawNullRect2", TestNode::AUTO, GraphicsDevice::testDrawNullRect2, NULL };
static TestNode gtestDrawNullRect3 =
{ "testDrawNullRect3", "testDrawNullRect3", TestNode::AUTO, GraphicsDevice::testDrawNullRect3, NULL };


TestNode* gGraphicsTestNodeList[] = {
    &gtestDrawSquare,
    &gtestDrawRect,
    &gtestFillSquare,
    &gtestFillAlphaSquare,
    &gtestFillRect,
    &gtestDrawOnePixelLargeRect,
    &gtestDrawOnePixelLongRect,
    &gtestDrawNullRect,
    &gtestDrawNullRect2,
    &gtestDrawNullRect3,
	NULL
};

TestNode gTestSuiteGraphics = {
    "GraphicsTests",
    "Tests Graphics",
    TestNode::TEST_SUITE,
    NULL, /* no function, it's a test suite */
    gGraphicsTestNodeList 
};
