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
#include "BCInternationalizationGeneric.h"
#include "DeprecatedString.h"

IMPLEMENT_GET_DELETE(BIInternationalization, BCInternationalizationGeneric);
using WebCore::DeprecatedString;

namespace BAL {

DeprecatedString *BCInternationalizationGeneric::toAce(const DeprecatedString *s, const unsigned l)
{
    //printf("toAce = %s\n", s->ascii() );
    return new DeprecatedString(s->ascii());
}

const UChar* BCInternationalizationGeneric::normalize(const UChar* characters, unsigned norm, size_t *length)
{
    BALNotImplemented();
    return characters;
}

bool BCInternationalizationGeneric::isSpace(unsigned short c)
{
    if (c == 0x20)
        return true;
    return false;
}

UChar32 BCInternationalizationGeneric::toLower(unsigned short c)
{
    if ((c >= 0x41 && c <= 0x5A) || (c >= 0xC0 && c <= 0xDE))
        return c+0x20;
    else if (c == 0xB5)
        return 0x3BC;
    return c;
}

UChar32 BCInternationalizationGeneric::toUpper(unsigned short c)
{
    if ((c >= 0x61 && c <= 0x7A) || (c >= 0xE0 && c <= 0xFE))
        return c-0x20;
    else if (c == 0x3BC)
        return 0xB5;
    return c;
}

UChar32 BCInternationalizationGeneric::foldCase(UChar32 c)
{
    return toLower(c);
}

int BCInternationalizationGeneric::foldCase(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error)
{
    *error = false;
    if (resultLength < srcLength) {
        *error = true;
        return srcLength;
    }
    for (int i = 0; i < srcLength; ++i)
        result[i] = foldCase(src[i]);
    return srcLength;
}

int BCInternationalizationGeneric::toLower(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error)
{
    *error = false;
    if (resultLength < srcLength) {
        *error = true;
        return srcLength;
    }
    for (int i = 0; i < srcLength; ++i)
        result[i] = toLower(src[i]);
    return srcLength;
}

int BCInternationalizationGeneric::toUpper(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error)
{
    *error = false;
    if (resultLength < srcLength) {
        *error = true;
        return srcLength;
    }
    for (int i = 0; i < srcLength; ++i)
        result[i] = toUpper(src[i]);
    return srcLength;
}

UChar32 BCInternationalizationGeneric::toTitleCase(UChar32 c)
{
    //printf( "toTitleCase = Ox%x\n", c);
    return c;
}

bool BCInternationalizationGeneric::isFormatChar(UChar32 c)
{
    //printf( "isFormatChar = Ox%x\n", c);
    return (c & 0xffff0000) == 0 && category(c) == WTF::Unicode::Other_Format;
}

bool BCInternationalizationGeneric::isSeparatorSpace(UChar32 c)
{
    //printf( "isSeparatorSpace = Ox%x\n", c);
    return (c & 0xffff0000) == 0 && category(c) == WTF::Unicode::Separator_Space;
}

bool BCInternationalizationGeneric::isPrintableChar(UChar32 c)
{
    //printf( "isPrintableChar = Ox%x\n", c);
    BALNotImplemented();
    return false;
}

bool BCInternationalizationGeneric::isDigit(UChar32 c)
{
    return (c & 0xffff0000) == 0 && category(c) == WTF::Unicode::Number_DecimalDigit;
}

bool BCInternationalizationGeneric::isPunct(UChar32 c)
{
    return (c & 0xffff0000) == 0 && (category(c) == WTF::Unicode::Punctuation_Dash
                                    || category(c) == WTF::Unicode::Punctuation_Open
                                    || category(c) == WTF::Unicode::Punctuation_Close
                                    || category(c) == WTF::Unicode::Punctuation_Connector
                                    || category(c) == WTF::Unicode::Punctuation_Other 
                                    || category(c) == WTF::Unicode::Punctuation_InitialQuote
                                    || category(c) == WTF::Unicode::Punctuation_FinalQuote);
}

UChar32 BCInternationalizationGeneric::mirroredChar(UChar32 c)
{
    //printf( "mirroredChar = Ox%x\n", c);
    return c;
}

CharCategory BCInternationalizationGeneric::category(UChar32 c)
{
    //printf( "category = Ox%x\n", c);
    if (c == 0x0D)
        return WTF::Unicode::Separator_Line;
    else if (c < 0x20)
        return WTF::Unicode::NoCategory;
    else if (c == 0x20)
        return WTF::Unicode::Separator_Space;
    else if ((c >= 0x21 && c<=0x27) 
            || c == 0x2E 
            || (c >= 0x3A && c<=0x3B) 
            || c == 0x3F 
            || c == 0x40 
            || (c >= 0x5B && c<=0x60) 
            || (c >= 0x7B && c<=0x7E))
        return WTF::Unicode::Punctuation_Other;
    else if (c == 0x28)
        return WTF::Unicode::Punctuation_Open;
    else if (c == 0x29)
        return WTF::Unicode::Punctuation_Close;
    else if ((c >= 0x2A && c<=0x2F) || (c >= 0x3C && c<=0x3E))
        return WTF::Unicode::Symbol_Math;
    else if (c >= 0x30 && c<=0x39)
        return WTF::Unicode::Number_DecimalDigit;
    else if (c >= 0x41 && c<=0x5A)
        return WTF::Unicode::Letter_Uppercase;
    else if (c >= 0x61 && c<=0x7A)
        return WTF::Unicode::Letter_Lowercase;
    else
        return WTF::Unicode::NoCategory;
}

Direction BCInternationalizationGeneric::direction(UChar32 c)
{
    //printf( "direction = Ox%x\n", c);
    return WTF::Unicode::LeftToRight;
}

bool BCInternationalizationGeneric::isLower(UChar32 c)
{
    return (c & 0xffff0000) == 0 && category(c) == WTF::Unicode::Letter_Lowercase;
}

bool BCInternationalizationGeneric::isUpper(UChar32 c)
{
    return (c & 0xffff0000) == 0 && category(c) == WTF::Unicode::Letter_Uppercase;
}

int BCInternationalizationGeneric::digitValue(UChar32 c)
{
    return c-0x30;
}

uint8_t BCInternationalizationGeneric::combiningClass(UChar32 c)
{
    //printf( "combiningClass = Ox%x\n", c);
    return c;
}

DecompositionType BCInternationalizationGeneric::decompositionType(UChar32 c)
{
    //printf( "decompositionType = Ox%x\n", c);
    return WTF::Unicode::DecompositionNone;
}

int BCInternationalizationGeneric::umemcasecmp(const UChar* a, const UChar* b, int len)
{
    for (int i = 0; i < len; ++i) {
        UChar32 c1 = toLower(a[i]);
        UChar32 c2 = toLower(b[i]);
        if (c1 != c2)
          return c1 < c2;
    }
    return 0;
}

}//namespace BAL

