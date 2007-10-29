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
#include "BTFont.h"
#include "BTFontData.h"
#include "BIFontPlatformDataPrivate.h"
#include "BTFontPlatformData.h"
#include "PlatformString.h"

#include "../TestManager/TestManager.h"

using WebCore::FontFamily;
using WebCore::String;
using namespace BAL;

class FontTestStress {
    public:

        static void TestFont_FontPlatformDataPrivateSimple()
        {
            BIFontPlatformDataPrivate *aFontPlatformDataPrivate = createBIFontPlatformDataPrivate();
            TestManager::AssertTrue("FontPlatformDataPrivate exists", aFontPlatformDataPrivate != NULL);
            deleteBIFontPlatformDataPrivate(aFontPlatformDataPrivate);
            aFontPlatformDataPrivate = NULL;
        }

        static void TestFont_FontPlatformDataPrivateMultiple1()
        {
            BIFontPlatformDataPrivate *aFontPlatformDataPrivate;
            for (uint8_t i = 0; i <= 254; i++) {
                aFontPlatformDataPrivate = createBIFontPlatformDataPrivate();
                TestManager::AssertTrue("FontPlatformDataPrivate exists", aFontPlatformDataPrivate != NULL);
                deleteBIFontPlatformDataPrivate(aFontPlatformDataPrivate);
                aFontPlatformDataPrivate = NULL;
            }
        }

        static void TestFont_FontPlatformDataPrivateMultiple2()
        {
            BIFontPlatformDataPrivate *aFontPlatformDataPrivate[254];
            for (uint8_t i = 0; i <= 254; i++) {
                aFontPlatformDataPrivate[i] = createBIFontPlatformDataPrivate();
                TestManager::AssertTrue("FontPlatformDataPrivate exists", aFontPlatformDataPrivate != NULL);
            }
            for (uint8_t i = 0; i <= 254; i++) {
                deleteBIFontPlatformDataPrivate(aFontPlatformDataPrivate[i]);
                aFontPlatformDataPrivate[i] = NULL;
            }
        }

        static void TestFont_FontPlatformDataSimple()
        {
            BTFontPlatformData aFontPlatformData;
            FontDescription description;

            BTFontPlatformData *aNewFontPlatformData = new BTFontPlatformData(description, description.family().family());
            TestManager::AssertTrue("BTFontPlatformData exists", aNewFontPlatformData != NULL);
            aFontPlatformData.init();
            TestManager::AssertTrue("BIFontPlatformDataPrivate exists", aFontPlatformData.impl() != NULL);
            //FIXME: result depends on implementation
            //TestManager::AssertTrue("BTFontPlatformData are different", (aFontPlatformData == *aNewFontPlatformData) == false);
            delete aNewFontPlatformData;
            aNewFontPlatformData = NULL;
        }

        static void TestFont_FontPlatformDataMultiple()
        {
            BTFontPlatformData *aFontPlatformData[254];
            FontDescription description;

            for (uint8_t i = 0; i <= 254; i++) {
                aFontPlatformData[i] = new BTFontPlatformData(description, description.family().family());
                TestManager::AssertTrue("FontPlatformData exists", aFontPlatformData[i] != NULL);
            }
            TestManager::AssertTrue("BIFontPlatformDataPrivate exists", aFontPlatformData[238]->impl() != NULL);
            TestManager::AssertTrue("BTFontPlatformData are equal", (*aFontPlatformData[3] == *aFontPlatformData[117]) == true);
            for (uint8_t i = 0; i <= 254; i++) {
                delete aFontPlatformData[i];
                aFontPlatformData[i] = NULL;
            }
        }

        static void TestFont_FontData()
        {
            //FIXME: always fail with freetype if not run first !!!
            WebCore::AtomicString::init();
            FontFamily family;
            family.setFamily("Vera");
            FontDescription fontDescription;
            fontDescription.setComputedSize(11);
            fontDescription.setFamily(family);
            BTFontPlatformData *aFontPlatformData = new BTFontPlatformData(fontDescription, family.family());
            String string("foo bar");

            BTFontData *aFontData = new BTFontData(*aFontPlatformData);
            TestManager::AssertTrue("string containsCharacters", aFontData->containsCharacters(string.charactersWithNullTermination(), string.length()));
            delete aFontData;
            aFontData = NULL;
            delete aFontPlatformData;
            aFontPlatformData = NULL;
        }

};

static TestNode gTestFont_FontPlatformDataPrivateSimple = { "TestFont_FontPlatformDataPrivateSimple", "TestFont_FontPlatformDataPrivateSimple", TestNode::AUTO, FontTestStress::TestFont_FontPlatformDataPrivateSimple, NULL };
static TestNode gTestFont_FontPlatformDataPrivateMultiple1 = { "TestFont_FontPlatformDataPrivateMultiple1", "TestFont_FontPlatformDataPrivateMultiple1", TestNode::AUTO, FontTestStress::TestFont_FontPlatformDataPrivateMultiple1, NULL };
static TestNode gTestFont_FontPlatformDataPrivateMultiple2 = { "TestFont_FontPlatformDataPrivateMultiple2", "TestFont_FontPlatformDataPrivateMultiple2", TestNode::AUTO, FontTestStress::TestFont_FontPlatformDataPrivateMultiple2, NULL };
static TestNode gTestFont_FontPlatformDataSimple = { "TestFont_FontPlatformDataSimple", "TestFont_FontPlatformDataSimple", TestNode::AUTO, FontTestStress::TestFont_FontPlatformDataSimple, NULL };
static TestNode gTestFont_FontPlatformDataMultiple = { "TestFont_FontPlatformDataMultiple", "TestFont_FontPlatformDataMultiple", TestNode::AUTO, FontTestStress::TestFont_FontPlatformDataMultiple, NULL };
static TestNode gTestFont_FontData = { "TestFont_FontData", "TestFont_FontData", TestNode::AUTO, FontTestStress::TestFont_FontData, NULL };

TestNode* gFontTestStressNodeList[] = {
    &gTestFont_FontData,
    &gTestFont_FontPlatformDataPrivateSimple,
    &gTestFont_FontPlatformDataPrivateMultiple1,
    &gTestFont_FontPlatformDataPrivateMultiple2,
    &gTestFont_FontPlatformDataSimple,
    &gTestFont_FontPlatformDataMultiple,
    NULL
};

TestNode gTestSuiteFontStress = {
    "TestFontStress",
    "test FontStress",
    TestNode::TEST_SUITE,
    NULL, /* no function, it's a test suite */
    gFontTestStressNodeList 
};
