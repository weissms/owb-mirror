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
#include "BCInternationalizationICU.h"
#include "DeprecatedString.h"

IMPLEMENT_GET_DELETE(BIInternationalization, BCInternationalizationICU);
using WebCore::DeprecatedString;

namespace BAL {

DeprecatedString *BCInternationalizationICU::toAce(const DeprecatedString *s, const unsigned l)
{
    UChar buffer[l];
    UErrorCode error = U_ZERO_ERROR;
    int32_t numCharactersConverted = uidna_IDNToASCII
        (reinterpret_cast<const UChar *>(s->unicode()), s->length(), buffer, l, UIDNA_ALLOW_UNASSIGNED, 0, &error);
    if (error != U_ZERO_ERROR) {
        return new DeprecatedString(s->ascii());
    }
    return new DeprecatedString(reinterpret_cast<WebCore::DeprecatedChar *>(buffer), numCharactersConverted);
}

const UChar* BCInternationalizationICU::normalize(const UChar* characters, unsigned norm, size_t *length)
{
    // FIXME: What's the right place to do normalization?
    // It's a little strange to do it inside the encode function.
    // Perhaps normalization should be an explicit step done before calling encode.

    const UChar* source = characters;
    size_t sourceLength = *length;

    Vector<UChar> normalizedCharacters;

    UErrorCode err = U_ZERO_ERROR;
    if (unorm_quickCheck(source, sourceLength, (UNormalizationMode)norm, &err) != UNORM_YES) {
        // First try using the length of the original string, since normalization to NFC rarely increases length.
        normalizedCharacters.resize(sourceLength);
        int32_t normalizedLength = unorm_normalize(source, *length, (UNormalizationMode)norm, 0, normalizedCharacters.data(), *length, &err);
        if (err == U_BUFFER_OVERFLOW_ERROR) {
            err = U_ZERO_ERROR;
            normalizedCharacters.resize(normalizedLength);
            normalizedLength = unorm_normalize(source, *length, (UNormalizationMode)norm, 0, normalizedCharacters.data(), normalizedLength, &err);
        }
        ASSERT(U_SUCCESS(err));

        source = normalizedCharacters.data();
        *length = normalizedLength;
    }
    return source;
}

bool BCInternationalizationICU::isSpace(unsigned short c)
{
    return c <= 0x7F ? isspace(c) : (u_charDirection(c) == U_WHITE_SPACE_NEUTRAL);
}

UChar32 BCInternationalizationICU::toLower(unsigned short c)
{
    return c <= 0x7F ? tolower(c) : u_tolower(c);
}

UChar32 BCInternationalizationICU::toUpper(unsigned short c)
{
    return c <= 0x7F ? toupper(c) : u_toupper(c);
}

UChar32 BCInternationalizationICU::foldCase(UChar32 c)
{
    return u_foldCase(c, U_FOLD_CASE_DEFAULT);
}

int BCInternationalizationICU::foldCase(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error)
{
    UErrorCode status = U_ZERO_ERROR;
    int realLength = u_strFoldCase(result, resultLength, src, srcLength, U_FOLD_CASE_DEFAULT, &status);
    *error = !U_SUCCESS(status);
    return realLength;
}

int BCInternationalizationICU::toLower(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error)
{
    UErrorCode status = U_ZERO_ERROR;
    int realLength = u_strToLower(result, resultLength, src, srcLength, "", &status);
    *error = !!U_FAILURE(status);
    return realLength;
}

int BCInternationalizationICU::toUpper(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error)
{
    UErrorCode status = U_ZERO_ERROR;
    int realLength = u_strToUpper(result, resultLength, src, srcLength, "", &status);
    *error = !!U_FAILURE(status);
    return realLength;
}

UChar32 BCInternationalizationICU::toTitleCase(UChar32 c)
{
    return u_totitle(c);
}

bool BCInternationalizationICU::isFormatChar(UChar32 c)
{
    return u_charType(c) == U_FORMAT_CHAR;
}

bool BCInternationalizationICU::isSeparatorSpace(UChar32 c)
{
    return u_charType(c) == U_SPACE_SEPARATOR;
}

bool BCInternationalizationICU::isPrintableChar(UChar32 c)
{
    return !!u_isprint(c);
}

bool BCInternationalizationICU::isDigit(UChar32 c)
{
    return !!u_isdigit(c);
}

bool BCInternationalizationICU::isPunct(UChar32 c)
{
    return !!u_ispunct(c);
}

UChar32 BCInternationalizationICU::mirroredChar(UChar32 c)
{
    return u_charMirror(c);
}

CharCategory BCInternationalizationICU::category(UChar32 c)
{
    return static_cast<CharCategory> (U_GET_GC_MASK(c));
}

Direction BCInternationalizationICU::direction(UChar32 c)
{
    return static_cast<Direction> (u_charDirection(c));
}

bool BCInternationalizationICU::isLower(UChar32 c)
{
    return !!u_islower(c);
}

bool BCInternationalizationICU::isUpper(UChar32 c)
{
    return !!u_isUUppercase(c);
}

int BCInternationalizationICU::digitValue(UChar32 c)
{
    return u_charDigitValue(c);
}

uint8_t BCInternationalizationICU::combiningClass(UChar32 c)
{
    return u_getCombiningClass(c);
}

DecompositionType BCInternationalizationICU::decompositionType(UChar32 c)
{
    return static_cast<DecompositionType> (u_getIntPropertyValue(c, UCHAR_DECOMPOSITION_TYPE));
}

int BCInternationalizationICU::umemcasecmp(const UChar* a, const UChar* b, int len)
{
    return u_memcasecmp(a, b, len, U_FOLD_CASE_DEFAULT);
}

}//namespace BAL

