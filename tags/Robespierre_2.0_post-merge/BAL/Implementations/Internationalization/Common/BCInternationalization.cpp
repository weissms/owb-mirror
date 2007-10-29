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
#include "BCInternationalization.h"
#include "DeprecatedString.h"

IMPLEMENT_GET_DELETE( BIInternationalization, BCInternationalization );
using WebCore::DeprecatedString;
namespace BAL {

DeprecatedString *BCInternationalization::toAce( const DeprecatedString *s, const unsigned l )
{
    //printf("toAce = %s\n", s->ascii() );
    return new DeprecatedString(s->ascii());
}

const UChar* BCInternationalization::normalize(const UChar* characters, unsigned norm, size_t *length )
{
    // FIXME: What's the right place to do normalization?
    // It's a little strange to do it inside the encode function.
    // Perhaps normalization should be an explicit step done before calling encode.

    /*const UChar* source = characters;
    size_t sourceLength = length;

    Vector<UChar> normalizedCharacters;

    UErrorCode err = U_ZERO_ERROR;
    if (unorm_quickCheck(source, sourceLength, (UNormalizationMode)norm, &err) != UNORM_YES) {
        // First try using the length of the original string, since normalization to NFC rarely increases length.
        normalizedCharacters.resize(sourceLength);
        int32_t normalizedLength = unorm_normalize(source, length, (UNormalizationMode)norm, 0, normalizedCharacters.data(), length, &err);
        if (err == U_BUFFER_OVERFLOW_ERROR) {
            err = U_ZERO_ERROR;
            normalizedCharacters.resize(normalizedLength);
            normalizedLength = unorm_normalize(source, length, (UNormalizationMode)norm, 0, normalizedCharacters.data(), normalizedLength, &err);
        }
        ASSERT(U_SUCCESS(err));

        source = normalizedCharacters.data();
        sourceLength = normalizedLength;
    }
    return source;*/
    //const UChar* source = characters;
    return characters;
}

bool BCInternationalization::isSpace(unsigned short c)
{
    if( c == 0x20 )
        return true;
    return false;
    //return c <= 0x7F ? isspace(c) : (u_charDirection(c) == U_WHITE_SPACE_NEUTRAL);
}

UChar32 BCInternationalization::toLower(unsigned short c)
{
    if( ( c >= 0x41 && c <= 0x5A ) || ( c >= 0xC0 && c <= 0xDE ) )
        return c+0x20;
    else if ( c == 0xB5 )
        return 0x3BC;
    return c;
    //return c <= 0x7F ? tolower(c) : u_tolower(c);
}

UChar32 BCInternationalization::toUpper(unsigned short c)
{
    if( ( c >= 0x61 && c <= 0x7A ) || ( c >= 0xE0 && c <= 0xFE ) )
        return c-0x20;
    else if ( c == 0x3BC )
        return 0xB5;
    return c;
    //return c <= 0x7F ? toupper(c) : u_toupper(c);
}

UChar32 BCInternationalization::foldCase(UChar32 c)
{
    return toLower(c);//u_foldCase(c, U_FOLD_CASE_DEFAULT);
}

int BCInternationalization::foldCase(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error)
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

int BCInternationalization::toLower(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error)
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

int BCInternationalization::toUpper(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error)
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

UChar32 BCInternationalization::toTitleCase(UChar32 c)
{
    //printf( "toTitleCase = Ox%x\n", c);
    return c;//u_totitle(c);
}

bool BCInternationalization::isFormatChar(UChar32 c)
{
    //printf( "isFormatChar = Ox%x\n", c);
    return  (c & 0xffff0000) == 0 && category(c) == WTF::Unicode::Other_Format;
}

bool BCInternationalization::isSeparatorSpace(UChar32 c)
{
    //printf( "isSeparatorSpace = Ox%x\n", c);
    return  (c & 0xffff0000) == 0 && category(c) == WTF::Unicode::Separator_Space;
}

bool BCInternationalization::isPrintableChar(UChar32 c)
{
    //printf( "isPrintableChar = Ox%x\n", c);
    return false;//!!u_isprint(c);
}

bool BCInternationalization::isDigit(UChar32 c)
{
    return  (c & 0xffff0000) == 0 && category(c) == WTF::Unicode::Number_DecimalDigit;
}

bool BCInternationalization::isPunct(UChar32 c)
{
    return  (c & 0xffff0000) == 0 && ( category(c) == WTF::Unicode::Punctuation_Dash
                                    || category(c) == WTF::Unicode::Punctuation_Open
                                    || category(c) == WTF::Unicode::Punctuation_Close
                                    || category(c) == WTF::Unicode::Punctuation_Connector
                                    || category(c) == WTF::Unicode::Punctuation_Other 
                                    || category(c) == WTF::Unicode::Punctuation_InitialQuote
                                    || category(c) == WTF::Unicode::Punctuation_FinalQuote);
}

UChar32 BCInternationalization::mirroredChar(UChar32 c)
{
    //printf( "mirroredChar = Ox%x\n", c);
    return c;//u_charMirror(c);
}

CharCategory BCInternationalization::category(UChar32 c)
{
    //printf( "category = Ox%x\n", c);
    if( c == 0x0D )
        return WTF::Unicode::Separator_Line;
    else if( c < 0x20 )
        return WTF::Unicode::NoCategory;
    else if( c == 0x20 )
        return WTF::Unicode::Separator_Space;
    else if( ( c >= 0x21 && c<=0x27) 
            || c == 0x2E 
            || ( c >= 0x3A && c<=0x3B ) 
            || c == 0x3F 
            || c == 0x40 
            || ( c >= 0x5B && c<=0x60 ) 
            || ( c >= 0x7B && c<=0x7E ) )
        return WTF::Unicode::Punctuation_Other;
    else if( c == 0x28 )
        return WTF::Unicode::Punctuation_Open;
    else if( c == 0x29 )
        return WTF::Unicode::Punctuation_Close;
    else if( (c >= 0x2A && c<=0x2F ) || ( c >= 0x3C && c<=0x3E ) )
        return WTF::Unicode::Symbol_Math;
    else if( c >= 0x30 && c<=0x39 )
        return WTF::Unicode::Number_DecimalDigit;
    else if( c >= 0x41 && c<=0x5A )
        return WTF::Unicode::Letter_Uppercase;
    else if( c >= 0x61 && c<=0x7A )
        return WTF::Unicode::Letter_Lowercase;
    else
        return WTF::Unicode::NoCategory;
}

Direction BCInternationalization::direction(UChar32 c)
{
    //printf( "direction = Ox%x\n", c);
    return WTF::Unicode::LeftToRight;//static_cast<Direction>(u_charDirection(c));
}

bool BCInternationalization::isLower(UChar32 c)
{
    return  (c & 0xffff0000) == 0 && category(c) == WTF::Unicode::Letter_Lowercase;
}

bool BCInternationalization::isUpper(UChar32 c)
{
    return  (c & 0xffff0000) == 0 && category(c) == WTF::Unicode::Letter_Uppercase;
}

int BCInternationalization::digitValue(UChar32 c)
{
    return c-0x30;//u_charDigitValue(c);
}

uint8_t BCInternationalization::combiningClass(UChar32 c)
{
    //printf( "combiningClass = Ox%x\n", c);
    return c;//u_getCombiningClass(c);
}

DecompositionType BCInternationalization::decompositionType(UChar32 c)
{
    //printf( "decompositionType = Ox%x\n", c);
    return WTF::Unicode::DecompositionNone;//static_cast<DecompositionType>(u_getIntPropertyValue(c, UCHAR_DECOMPOSITION_TYPE));
}

int BCInternationalization::umemcasecmp(const UChar* a, const UChar* b, int len)
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

