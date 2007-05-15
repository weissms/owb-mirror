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
 #include "BIInternationalization.h"

using namespace BAL;

int BIInternationalization::u_charDirection(UChar32 )
{
    return 0;
}

UChar32 BIInternationalization::u_tolower(UChar32 )
{
    return 0;
}

UChar32 BIInternationalization::u_toupper(UChar32 )
{
    return 0;
}

UChar32 BIInternationalization::u_foldCase(UChar32 , uint32_t )
{
    return 0;
}

int32_t BIInternationalization::u_memcasecmp(const UChar*, const UChar*, int32_t , uint32_t )
{
    return 0;
}

int32_t BIInternationalization::uidna_IDNToASCII(const UChar* , int32_t , UChar* , int32_t ,
                   int32_t ,
                   UParseError* ,
                   UErrorCode* )
{
    return 0;
}

UChar32 BIInternationalization::u_charMirror(UChar32 )
{
    return 0;
}

UBool BIInternationalization::u_isUUppercase(UChar32 )
{
    return 0;
}

uint8_t BIInternationalization::u_getCombiningClass(UChar32 )
{
    return 0;
}

int32_t BIInternationalization::unorm_normalize(const UChar*, int32_t , UNormalizationMode , int32_t ,
                UChar*, int32_t ,
                UErrorCode*)
{
    return 0;
}

const char* BIInternationalization::ucnv_getStandardName(const char*, const char*, UErrorCode*)
{
    return 0;
}

UBool BIInternationalization::u_isdigit(UChar32 )
{
    return 0;
}

UBool BIInternationalization::u_islower(UChar32 )
{
    return 0;
}

int32_t BIInternationalization::u_strToLower(UChar*, int32_t , const UChar*, int32_t ,
             const char*,
             UErrorCode*)
{
    return 0;
}

int32_t BIInternationalization::u_strToUpper(UChar*, int32_t , const UChar*, int32_t ,
             const char*,
             UErrorCode*)
{
    return 0;
}

UChar* BIInternationalization::u_memset(UChar*, UChar , int32_t )
{
    return 0;
}

int32_t BIInternationalization::u_strFoldCase(UChar*, int32_t , const UChar*, int32_t ,
              uint32_t ,
              UErrorCode*)
{
    return 0;
}

int32_t BIInternationalization::ubrk_first(UBreakIterator*)
{
    return 0;
}

int32_t BIInternationalization::ubrk_next(UBreakIterator*)
{
    return 0;
}

UChar32 BIInternationalization::u_totitle(UChar32)
{
    return 0;
}

UBreakIterator* BIInternationalization::ubrk_open(UBreakIteratorType , const char*, const UChar*,
      int32_t ,
      UErrorCode*)
{
    return 0;
}

void BIInternationalization::ubrk_setText(UBreakIterator* , const UChar*, int32_t, UErrorCode*)
{
}

void BIInternationalization::ucnv_close(UConverter*)
{
}

UConverter* BIInternationalization::ucnv_open(const char*, UErrorCode*)
{
    return 0;
}

UNormalizationCheckResult BIInternationalization::unorm_quickCheck(const UChar*, int32_t , UNormalizationMode, UErrorCode*)
{
    return 0;
}

void BIInternationalization::UCNV_FROM_U_CALLBACK_ESCAPE (
                  const void*,
                  UConverterFromUnicodeArgs*,
                  const UChar*,
                  int32_t,
                  UChar32,
                  UConverterCallbackReason,
                  UErrorCode*)
{
}


void BIInternationalization::ucnv_setFromUCallBack(UConverter*, UConverterFromUCallback,
                       const void*,
                       UConverterFromUCallback*,
                       const void**,
                       UErrorCode* )
{
}

void BIInternationalization::ucnv_setSubstChars(UConverter*,
                   const char*,
                   int8_t,
                   UErrorCode*)
{
}

void BIInternationalization::UCNV_FROM_U_CALLBACK_SUBSTITUTE (
                  const void*,
                  UConverterFromUnicodeArgs*,
                  const UChar*,
                  int32_t,
                  UChar32,
                  UConverterCallbackReason,
                  UErrorCode*)
{
}


void BIInternationalization::ucnv_fromUnicode(UConverter*,
                  char**,
                  const char*,
                  const UChar** ,
                  const UChar* ,
                  int32_t*,
                  UBool,
                  UErrorCode*)
{
}

void BIInternationalization::ucnv_toUnicode(UConverter*,
               UChar**,
               const UChar*,
               const char**,
               const char*,
               int32_t*,
               UBool,
               UErrorCode*)
{
}

void BIInternationalization::UCNV_TO_U_CALLBACK_STOP (
                  const void*,
                  UConverterToUnicodeArgs*,
                  const char*,
                  int32_t,
                  UConverterCallbackReason,
                  UErrorCode*)
{
}

void BIInternationalization::ucnv_setToUCallBack(UConverter*,
                     UConverterToUCallback,
                     const void*,
                     UConverterToUCallback*,
                     const void**,
                     UErrorCode*)
{
}

int32_t BIInternationalization::ucnv_toUChars(UConverter*, UChar*, int32_t, const char*, int32_t, UErrorCode*)
{
    return 0;
}

int8_t BIInternationalization::u_charType(UChar32)
{
    return 0;
}

UBool BIInternationalization::u_isprint(UChar32)
{
    return 0;
}

UBool BIInternationalization::u_ispunct(UChar32 c)
{
    return 0;
}
int32_t BIInternationalization::ubrk_preceding(UBreakIterator*, int32_t)
{
    return 0;
}

int32_t BIInternationalization::ubrk_following(UBreakIterator*, int32_t)
{
    return 0;
}

int32_t BIInternationalization::u_getIntPropertyValue(UChar32, UProperty)
{
    return 0;
}

int32_t BIInternationalization::u_charDigitValue(UChar32)
{
    return 0;
}

int32_t BIInternationalization::ubrk_current(const UBreakIterator*)
{
    return 0;
}

int32_t BIInternationalization::ucnv_countAvailable(void)
{
    return 0;
}

const char* BIInternationalization::ucnv_getAvailableName(int32_t n)
{
    return 0;
}

uint16_t BIInternationalization::ucnv_countAliases(const char *alias, UErrorCode *pErrorCode)
{
    return 0;
}

const char* BIInternationalization::ucnv_getAlias(const char *alias, uint16_t n, UErrorCode* pErrorCode)
{
    return 0;
}

const char* BIInternationalization::ucnv_getName(const UConverter* converter, UErrorCode* err)
{
    return 0;
}
