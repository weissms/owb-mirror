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

#include "config.h"
#include "BALConfiguration.h"
#include "BIGraphicsDevice.h"
#include "BINativeImage.h"
#include "BIWidget.h"
#include "TestManager/TestManager.h"
#include "Widget.h"

using namespace BAL;

class WidgetImplTest {

public:

static void widgetCreation()
{
    BTWidget *widget = new BTWidget();
    TestManager::AssertTrue("widget exist", widget != NULL);
    delete widget;
    widget = NULL;
}

static void widgetParent()
{
    BTWidget *widgetParent = new BTWidget();
    BTWidget *widgetChild = new BTWidget();

    widgetChild->setParent(widgetParent);
    widgetChild->removeFromParent();

    delete widgetChild;
    widgetChild = NULL;
    delete widgetParent;
    widgetParent = NULL;
}

//FIXME: Re-enable this test as soon as backingStore for widget is implemented
static void widgetBackingStore()
{
    BTWidget *widget = new BTWidget();
    BIGraphicsDevice *device = getBIGraphicsDevice();
    BINativeImage *img = device->createNativeImage(WebCore::IntSize());

    widget->setBackingStore(img);
    BINativeImage *image = widget->backingStore();
    TestManager::AssertTrue("image is the same", img == image);

    delete widget;
    widget = NULL;
    img = NULL;
    deleteBIGraphicsDevice();
    device = NULL;
}

//FIXME: Re-enable this test as soon as backingStore for widget is implemented
static void widgetResize()
{
    BTWidget *widget = new BTWidget;
    BIGraphicsDevice *device = getBIGraphicsDevice();
    BINativeImage *img = device->createNativeImage(WebCore::IntSize());

    widget->setBackingStore(img);
    widget->resize(10, 10);
    BINativeImage *image = widget->backingStore();
    TestManager::AssertTrue("widget is 10x10 size", widget->size() == WebCore::IntSize(10, 10));
    TestManager::AssertTrue("image is 10x10 size", image->size() == WebCore::IntSize(10, 10));


    delete widget;
    widget = NULL;
    img = NULL;
    deleteBIGraphicsDevice();
    device = NULL;
}



private:

};

static TestNode gtestWidgetCreation = { "testWidgetCreation", "testWidgetCreation", TestNode::AUTO, WidgetImplTest::widgetCreation, NULL };
static TestNode gtestWidgetParent = { "testWidgetParent", "testWidgetParent", TestNode::AUTO, WidgetImplTest::widgetParent, NULL };
static TestNode gtestWidgetBackingStore = { "testWidgetBackingStore", "testWidgetBackingStore", TestNode::AUTO, WidgetImplTest::widgetBackingStore, NULL };
static TestNode gtestWidgetResize = { "testWidgetResize", "testWidgetResize", TestNode::AUTO, WidgetImplTest::widgetResize, NULL };

TestNode* gWidgetTestNodeList[] = {
    &gtestWidgetCreation,
    &gtestWidgetParent,
//     &gtestWidgetBackingStore,
//     &gtestWidgetResize,
    NULL
};

TestNode gTestSuiteWidget = {
    "TestWidget",
    "test vidget",
    TestNode::TEST_SUITE,
    NULL, /* no function, it's a test suite */
    gWidgetTestNodeList 
};
