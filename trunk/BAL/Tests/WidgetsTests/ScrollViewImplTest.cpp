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
#include "BTLogHelper.h"
#include "TestManager/TestManager.h"
#include "ScrollView.h"

using namespace BAL;

class ScrollViewImplTest {

public:

static void creation()
{
    BTScrollView *scrollView = new BTScrollView;
    TestManager::AssertTrue("scrollview creation", scrollView != NULL);
    delete scrollView;
    scrollView = NULL;
}

static void addChild()
{
    BTScrollView *scrollView = new BTScrollView;
    BTWidget *widget = new BTWidget();
    TestManager::AssertTrue("widget creation", widget != NULL);
    scrollView->addChild(widget);
    scrollView->removeChild(widget);
    delete widget;
    widget = NULL;
    delete scrollView;
    scrollView = NULL;
}

static void testBackingStore()
{
    BTScrollView *scrollView = new BTScrollView;
    BIGraphicsDevice *device = getBIGraphicsDevice();
    BINativeImage *img = device->createNativeImage(WebCore::IntSize());

    scrollView->setBackingStore(img);
    BINativeImage *image = scrollView->backingStore();
    TestManager::AssertTrue("image is the same", img == image);

    delete scrollView;
    scrollView = NULL;
    img = NULL;
    deleteBIGraphicsDevice();
    device = NULL;
}

static void testResize()
{
    BTScrollView *scrollView = new BTScrollView;
    BIGraphicsDevice *device = getBIGraphicsDevice();
    BINativeImage *img = device->createNativeImage(WebCore::IntSize());

    scrollView->setBackingStore(img);
    scrollView->resize(10, 10);
    BINativeImage *image = scrollView->backingStore();
    TestManager::AssertTrue("ScrollView is 10 width", scrollView->contentsWidth() == 10);
    TestManager::AssertTrue("ScrollView is 10 height", scrollView->contentsHeight() == 10);
    TestManager::AssertTrue("image is 10x10", image->size() == WebCore::IntSize(10, 10));


    delete scrollView;
    scrollView = NULL;
    img = NULL;
    deleteBIGraphicsDevice();
    device = NULL;
}

private:

};

static TestNode gtestScrollViewCreation = { "testScrollViewCreation", "testScrollViewCreation",
  TestNode::AUTO, ScrollViewImplTest::creation, NULL };
static TestNode gtestScrollViewAddWidget = { "testScrollViewAddWidget", "testScrollViewAddWidget", TestNode::AUTO, ScrollViewImplTest::addChild, NULL };
static TestNode gtestScrollViewBackingStore = { "testScrollViewBackingStore", "testScrollViewBackingStore", TestNode::AUTO, ScrollViewImplTest::testBackingStore, NULL };
static TestNode gtestScrollViewResize = { "testScrollViewResize", "testScrollViewResize", TestNode::AUTO, ScrollViewImplTest::testResize, NULL };

TestNode* gScrollViewTestNodeList[] = {
    &gtestScrollViewCreation,
    &gtestScrollViewAddWidget,
    &gtestScrollViewBackingStore,
    &gtestScrollViewResize,
    NULL
};

TestNode gTestSuiteScrollView = {
    "TestScrollView",
    "test scroll view",
    TestNode::TEST_SUITE,
    NULL, /* no function, it's a test suite */
    gScrollViewTestNodeList 
};  
