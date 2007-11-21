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
#include "BIGraphicsContext.h"
#include "BIGraphicsDevice.h"
#include "RGBA32Array.h"
#include "../TestManager/TestManager.h"

using namespace BAL;

class GraphicsDeviceStress {
    public:

static void testGetBIGraphicsDevice1()
{
    BIGraphicsDevice *device = getBIGraphicsDevice();
    TestManager::AssertTrue("Graphics device exists", device != NULL);
    deleteBIGraphicsDevice();
    device = NULL;
}

static void testGetBIGraphicsDevice2()
{
    for (uint8_t i = 0; i < 255; i++) {
        BIGraphicsDevice *device = getBIGraphicsDevice();
        TestManager::AssertTrue("Graphics device exists", device != NULL);
        deleteBIGraphicsDevice();
        device = NULL;
    }
}

static void testInitialize1()
{
    getBIGraphicsDevice()->initialize(0, 0, 0);
    getBIGraphicsDevice()->finalize();
    deleteBIGraphicsDevice();
}

static void testInitialize2()
{
    BIGraphicsDevice *device = getBIGraphicsDevice();
    for (uint8_t i = 0; i < 255; i++) {
        device->initialize(0, 0, 0);
        device->finalize();
    }
    device = NULL;
    deleteBIGraphicsDevice();
}

static void testInitialize3()
{
    BIGraphicsDevice *device = getBIGraphicsDevice();
    for (uint8_t i = 0; i < 255; i++)
        device->initialize(0, 0, 0);
    for (uint8_t i = 0; i < 255; i++)
        device->finalize();
    device = NULL;
    deleteBIGraphicsDevice();
}

static void testCreateNativeImage1()
{
    BIGraphicsDevice *device = getBIGraphicsDevice();
    WebCore::IntSize size(10, 10);
    BINativeImage *img = device->createNativeImage(size);
    TestManager::AssertTrue("Native image created", img != NULL);
    delete img;
    img = NULL;
    device = NULL;
    deleteBIGraphicsDevice();
}

static void testCreateNativeImage2()
{
    BIGraphicsDevice *device = getBIGraphicsDevice();
    WebCore::IntSize size(10, 10);
    BINativeImage *img;
    for (uint8_t i = 0; i < 255; i++) {
        img = device->createNativeImage(size);
        TestManager::AssertTrue("Native image created", img != NULL);
        delete img;
        img = NULL;
    }
    device = NULL;
    deleteBIGraphicsDevice();
}

static void testCreateNativeImage3()
{
    BIGraphicsDevice *device = getBIGraphicsDevice();
    WebCore::IntSize size(10, 10);
    BINativeImage *img[255];
    for (uint8_t i = 0; i < 255; i++) {
        img[i] = device->createNativeImage(size);
        TestManager::AssertTrue("Native image created", img[i] != NULL);
    }
    for (uint8_t i = 0; i < 255; i++) {
        delete img[i];
        img[i] = NULL;
    }
    device = NULL;
    deleteBIGraphicsDevice();
}

static void testCreateNativeImageWithBuffer1()
{
    BIGraphicsDevice *device = getBIGraphicsDevice();
    WebCore::IntSize size(10, 10);
    RGBA32Array *buffer = new RGBA32Array(size.width() * size.height());
    memset(buffer, 0, sizeof(buffer));

    BINativeImage *img = device->createNativeImage(*buffer, size);
    TestManager::AssertTrue("Native image created", img != NULL);
    delete img;
    img = NULL;
    device = NULL;
    deleteBIGraphicsDevice();
}

static void testCreateNativeImageWithBuffer2()
{
    BIGraphicsDevice *device = getBIGraphicsDevice();
    WebCore::IntSize size(10, 10);

    for (uint8_t i = 0; i < 255; i++) {
        RGBA32Array *buffer = new RGBA32Array(size.width() * size.height());
        memset(buffer, i, sizeof(buffer));
        BINativeImage *img = device->createNativeImage(*buffer, size);
        TestManager::AssertTrue("Native image created", img != NULL);
        delete img;
        img = NULL;
    }
    device = NULL;
    deleteBIGraphicsDevice();
}

static void testCreateNativeImageWithBuffer3()
{
    BIGraphicsDevice *device = getBIGraphicsDevice();
    BINativeImage *img[255];
    WebCore::IntSize size(10, 10);
    RGBA32Array *buffer = new RGBA32Array(size.width() * size.height());
    memset(buffer, 0, sizeof(buffer));

    for (uint8_t i = 0; i < 255; i++) {
        img[i] = device->createNativeImage(*buffer, size);
        TestManager::AssertTrue("Native image created", img[i] != NULL);
    }
    for (uint8_t i = 0; i < 255; i++) {
        delete img[i];
        img[i] = NULL;
    }
    device = NULL;
    deleteBIGraphicsDevice();
}

private:

};

static TestNode gtestGetBIGraphicsDevice1 =
{ "testGetBIGraphicsDevice1", "testGetBIGraphicsDevice1", TestNode::AUTO, GraphicsDeviceStress::testGetBIGraphicsDevice1, NULL };
static TestNode gtestGetBIGraphicsDevice2 =
{ "testGetBIGraphicsDevice2", "testGetBIGraphicsDevice2", TestNode::AUTO, GraphicsDeviceStress::testGetBIGraphicsDevice2, NULL };
static TestNode gtestInitialize1 =
{ "testInitialize1", "testInitialize1", TestNode::AUTO, GraphicsDeviceStress::testInitialize1, NULL };
static TestNode gtestInitialize2 =
{ "testInitialize2", "testInitialize2", TestNode::AUTO, GraphicsDeviceStress::testInitialize2, NULL };
static TestNode gtestInitialize3 =
{ "testInitialize3", "testInitialize3", TestNode::AUTO, GraphicsDeviceStress::testInitialize3, NULL };
static TestNode gtestCreateNativeImage1 =
{ "testFillCreateNativeImage1", "testCreateNativeImage1", TestNode::AUTO, GraphicsDeviceStress::testCreateNativeImage1, NULL };
static TestNode gtestCreateNativeImage2 =
{ "testFillCreateNativeImage2", "testCreateNativeImage2", TestNode::AUTO, GraphicsDeviceStress::testCreateNativeImage2, NULL };
static TestNode gtestCreateNativeImage3 =
{ "testFillCreateNativeImage3", "testCreateNativeImage3", TestNode::AUTO, GraphicsDeviceStress::testCreateNativeImage3, NULL };
static TestNode gtestCreateNativeImageWithBuffer1 =
{ "testCreateNativeImageWithBuffer1", "testCreateNativeImageWithBuffer1", TestNode::AUTO, GraphicsDeviceStress::testCreateNativeImageWithBuffer1, NULL };
static TestNode gtestCreateNativeImageWithBuffer2 =
{ "testCreateNativeImageWithBuffer2", "testCreateNativeImageWithBuffer2", TestNode::AUTO, GraphicsDeviceStress::testCreateNativeImageWithBuffer2, NULL };

TestNode* gGraphicsTestStressNodeList[] = {
    &gtestGetBIGraphicsDevice1,
    &gtestGetBIGraphicsDevice2,
    &gtestInitialize1,
    &gtestInitialize2,
    &gtestInitialize3,
    &gtestCreateNativeImage1,
    &gtestCreateNativeImage2,
    &gtestCreateNativeImage3,
    &gtestCreateNativeImageWithBuffer1,
    &gtestCreateNativeImageWithBuffer2,
    NULL
};

TestNode gTestSuiteGraphicsStress = {
    "GraphicsTestsStress",
    "Tests Graphics Stress",
    TestNode::TEST_SUITE,
    NULL, /* no function, it's a test suite */
    gGraphicsTestStressNodeList 
};
