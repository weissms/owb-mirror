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
#ifndef BIINTERNATIONALIZATION_H_
#define BIINTERNATIONALIZATION_H_

#ifndef __BAL_I18N__
#include <unicode/uchar.h>
#include <unicode/ubrk.h>
#include <unicode/umachine.h>
#include <unicode/ucnv.h>
#include <unicode/unorm.h>
#include <unicode/ustring.h>
#include <unicode/utf8.h>
#include <unicode/uidna.h>
#include <unicode/urename.h>
#else

#include "BTInternationalization.h"

namespace BAL {
    /**
    * @brief the BIInternationalization
    *
    * The Internationalization base class
    *
    */
    class BIInternationalization {
    public:
        // this is mandatory
            ~BIInternationalization(){}
            static int u_charDirection(UChar32 c);
            static UChar32 u_tolower(UChar32 c);
            static UChar32 u_toupper(UChar32 c);
            static UChar32 u_foldCase(UChar32 c, uint32_t options);
            static int32_t u_memcasecmp(const UChar *s1, const UChar *s2, int32_t length, uint32_t options);
            static int32_t uidna_IDNToASCII(  const UChar* src, int32_t srcLength,
                            UChar* dest, int32_t destCapacity,
                            int32_t options,
                            UParseError* parseError,
                            UErrorCode* status);
            static UChar32 u_charMirror(UChar32 c);
            static UBool u_isUUppercase(UChar32 c);
            static uint8_t u_getCombiningClass(UChar32 c);
            static int32_t unorm_normalize(const UChar *source, int32_t sourceLength,
                            UNormalizationMode mode, int32_t options,
                            UChar *result, int32_t resultLength,
                            UErrorCode *status);
            static const char *ucnv_getStandardName(const char *name, const char *standard, UErrorCode *pErrorCode);
            static UBool u_isdigit(UChar32 c);
            static UBool u_islower(UChar32 c);
            static int32_t u_strToLower(UChar *dest, int32_t destCapacity,
                        const UChar *src, int32_t srcLength,
                        const char *locale,
                        UErrorCode *pErrorCode);
            static UBool U_SUCCESS(UErrorCode code);
            static UBool U_FAILURE(UErrorCode code);
            static int32_t u_strToUpper(UChar *dest, int32_t destCapacity,
                        const UChar *src, int32_t srcLength,
                        const char *locale,
                        UErrorCode *pErrorCode);
            static UChar *u_memset(UChar *dest, UChar c, int32_t count);
            static int32_t u_strFoldCase(UChar *dest, int32_t destCapacity,
                        const UChar *src, int32_t srcLength,
                        uint32_t options,
                        UErrorCode *pErrorCode);
            static int32_t ubrk_first(UBreakIterator *bi);
            static int32_t ubrk_next(UBreakIterator *bi);
            static UChar32 u_totitle(UChar32 c);
            static UBreakIterator *ubrk_open(UBreakIteratorType type,
                const char *locale,
                const UChar *text,
                int32_t textLength,
                UErrorCode *status);
            static void ubrk_setText(UBreakIterator* bi,
                        const UChar*    text,
                        int32_t         textLength,
                        UErrorCode*     status);
            static void ucnv_close(UConverter * converter);
            static UConverter* ucnv_open(const char *converterName, UErrorCode *err);
            static UNormalizationCheckResult unorm_quickCheck(const UChar *source, int32_t sourcelength,
                            UNormalizationMode mode,
                            UErrorCode *status);
            static void UCNV_FROM_U_CALLBACK_ESCAPE (
                            const void *context,
                            UConverterFromUnicodeArgs *fromUArgs,
                            const UChar* codeUnits,
                            int32_t length,
                            UChar32 codePoint,
                            UConverterCallbackReason reason,
                            UErrorCode * err);

            static void ucnv_setFromUCallBack (UConverter * converter,
                                UConverterFromUCallback newAction,
                                const void *newContext,
                                UConverterFromUCallback *oldAction,
                                const void **oldContext,
                                UErrorCode * err);
            static void ucnv_setSubstChars(UConverter *converter,
                            const char *subChars,
                            int8_t len,
                            UErrorCode *err);
            static void UCNV_FROM_U_CALLBACK_SUBSTITUTE (
                            const void *context,
                            UConverterFromUnicodeArgs *fromUArgs,
                            const UChar* codeUnits,
                            int32_t length,
                            UChar32 codePoint,
                            UConverterCallbackReason reason,
                            UErrorCode * err);

            static void ucnv_fromUnicode (UConverter * converter,
                            char **target,
                            const char *targetLimit,
                            const UChar ** source,
                            const UChar * sourceLimit,
                            int32_t* offsets,
                            UBool flush,
                            UErrorCode * err);
            static void ucnv_toUnicode(UConverter *converter,
                        UChar **target,
                        const UChar *targetLimit,
                        const char **source,
                        const char *sourceLimit,
                        int32_t *offsets,
                        UBool flush,
                        UErrorCode *err);
            static void UCNV_TO_U_CALLBACK_STOP (
                            const void *context,
                            UConverterToUnicodeArgs *toUArgs,
                            const char* codeUnits,
                            int32_t length,
                            UConverterCallbackReason reason,
                            UErrorCode * err);
            static void ucnv_setToUCallBack (UConverter * converter,
                                UConverterToUCallback newAction,
                                const void* newContext,
                                UConverterToUCallback *oldAction,
                                const void** oldContext,
                                UErrorCode * err);
            static int32_t ucnv_toUChars(UConverter *cnv,
                        UChar *dest, int32_t destCapacity,
                        const char *src, int32_t srcLength,
                        UErrorCode *pErrorCode);
            static int8_t u_charType(UChar32 c);
            static UBool u_isprint(UChar32 c);
            static UBool u_ispunct(UChar32 c);
            static int32_t ubrk_preceding(UBreakIterator *bi,
                    int32_t offset);
            static int32_t ubrk_following(UBreakIterator *bi,
                    int32_t offset);
            static int32_t u_getIntPropertyValue(UChar32 c, UProperty which);
            static int32_t u_charDigitValue(UChar32 c);
            static int32_t ubrk_current(const UBreakIterator *bi);
            /**
            * Returns the number of available converters, as per the alias file.
            *
            * @return the number of available converters
            * @see ucnv_getAvailableName
            * @stable ICU 2.0
            */
            static int32_t ucnv_countAvailable(void);

/**
 * Gets the canonical converter name of the specified converter from a list of
 * all available converters contaied in the alias file. All converters
 * in this list can be opened.
 *
 * @param n the index to a converter available on the system (in the range <TT>[0..ucnv_countAvaiable()]</TT>)
 * @return a pointer a string (library owned), or <TT>NULL</TT> if the index is out of bounds.
 * @see ucnv_countAvailable
 * @stable ICU 2.0
 */
static const char* ucnv_getAvailableName(int32_t n);

            /**
 * Gives the number of aliases for a given converter or alias name.
 * If the alias is ambiguous, then the preferred converter is used
 * and the status is set to U_AMBIGUOUS_ALIAS_WARNING.
 * This method only enumerates the listed entries in the alias file.
 * @param alias alias name
 * @param pErrorCode error status
 * @return number of names on alias list for given alias
 * @stable ICU 2.0
 */
static uint16_t ucnv_countAliases(const char *alias, UErrorCode *pErrorCode);
/**
 * Gives the name of the alias at given index of alias list.
 * This method only enumerates the listed entries in the alias file.
 * If the alias is ambiguous, then the preferred converter is used
 * and the status is set to U_AMBIGUOUS_ALIAS_WARNING.
 * @param alias alias name
 * @param n index in alias list
 * @param pErrorCode result of operation
 * @return returns the name of the alias at given index
 * @see ucnv_countAliases
 * @stable ICU 2.0
 */
static const char * ucnv_getAlias(const char *alias, uint16_t n, UErrorCode *pErrorCode);
/**
 * Gets the internal, canonical name of the converter (zero-terminated).
 * The lifetime of the returned string will be that of the converter
 * passed to this function.
 * @param converter the Unicode converter
 * @param err UErrorCode status
 * @return the internal name of the converter
 * @see ucnv_getDisplayName
 * @stable ICU 2.0
 */
static const char * ucnv_getName(const UConverter *converter, UErrorCode *err);

    };
}


inline int u_charDirection(UChar32 c)
{
    return BAL::BIInternationalization::u_charDirection( c );
}
inline UChar32 u_tolower(UChar32 c)
{
    return BAL::BIInternationalization::u_tolower( c );
}

inline UChar32 u_toupper(UChar32 c)
{
    return BAL::BIInternationalization::u_toupper( c );
}

inline UChar32 u_foldCase(UChar32 c, uint32_t options)
{
    return BAL::BIInternationalization::u_foldCase( c, options );
}

inline int32_t u_memcasecmp(const UChar *s1, const UChar *s2, int32_t length, uint32_t options)
{
    return BAL::BIInternationalization::u_memcasecmp( s1, s2, length, options );
}

inline int32_t uidna_IDNToASCII(  const UChar* src, int32_t srcLength,
                   UChar* dest, int32_t destCapacity,
                   int32_t options,
                   UParseError* parseError,
                   UErrorCode* status)
{
    return BAL::BIInternationalization::uidna_IDNToASCII( src, srcLength, dest, destCapacity, options, parseError, status );
}

inline UChar32 u_charMirror(UChar32 c)
{
    return BAL::BIInternationalization::u_charMirror( c );
}

inline UBool u_isUUppercase(UChar32 c)
{
    return BAL::BIInternationalization::u_isUUppercase( c );
}

inline uint8_t u_getCombiningClass(UChar32 c)
{
    return BAL::BIInternationalization::u_getCombiningClass( c );
}

inline int32_t unorm_normalize(const UChar *source, int32_t sourceLength,
                UNormalizationMode mode, int32_t options,
                UChar *result, int32_t resultLength,
                UErrorCode *status)
{
    return BAL::BIInternationalization::unorm_normalize( source, sourceLength, mode, options, result, resultLength, status );
}

inline const char *ucnv_getStandardName(const char *name, const char *standard, UErrorCode *pErrorCode)
{
    return BAL::BIInternationalization::ucnv_getStandardName( name, standard, pErrorCode );
}

inline UBool u_isdigit(UChar32 c)
{
    return BAL::BIInternationalization::u_isdigit( c );
}

inline UBool u_islower(UChar32 c)
{
    return BAL::BIInternationalization::u_islower( c );
}

inline int32_t u_strToLower(UChar *dest, int32_t destCapacity,
             const UChar *src, int32_t srcLength,
             const char *locale,
             UErrorCode *pErrorCode)
{
    return BAL::BIInternationalization::u_strToLower( dest, destCapacity, src, srcLength, locale, pErrorCode );
}

inline int32_t u_strToUpper(UChar *dest, int32_t destCapacity,
             const UChar *src, int32_t srcLength,
             const char *locale,
             UErrorCode *pErrorCode)
{
    return BAL::BIInternationalization::u_strToUpper( dest, destCapacity, src, srcLength, locale, pErrorCode );
}

inline UChar *u_memset(UChar *dest, UChar c, int32_t count)
{
    return BAL::BIInternationalization::u_memset( dest, c, count );
}

inline int32_t u_strFoldCase(UChar *dest, int32_t destCapacity,
              const UChar *src, int32_t srcLength,
              uint32_t options,
              UErrorCode *pErrorCode)
{
    return BAL::BIInternationalization::u_strFoldCase( dest, destCapacity, src, srcLength, options, pErrorCode );
}

inline int32_t ubrk_first(UBreakIterator *bi)
{
    return BAL::BIInternationalization::ubrk_first( bi );
}

inline int32_t ubrk_next(UBreakIterator *bi)
{
    return BAL::BIInternationalization::ubrk_next( bi );
}

inline UChar32 u_totitle(UChar32 c)
{
    return BAL::BIInternationalization::u_totitle( c );
}

inline UBreakIterator *ubrk_open(UBreakIteratorType type,
      const char *locale,
      const UChar *text,
      int32_t textLength,
      UErrorCode *status)
{
    return BAL::BIInternationalization::ubrk_open( type, locale, text, textLength, status );
}

inline void ubrk_setText(UBreakIterator* bi,
             const UChar*    text,
             int32_t         textLength,
             UErrorCode*     status)
{
    return BAL::BIInternationalization::ubrk_setText( bi, text, textLength, status );
}

inline void ucnv_close(UConverter * converter)
{
    return BAL::BIInternationalization::ucnv_close( converter );
}

inline UConverter* ucnv_open(const char *converterName, UErrorCode *err)
{
    return BAL::BIInternationalization::ucnv_open( converterName, err );
}

inline UNormalizationCheckResult unorm_quickCheck(const UChar *source, int32_t sourcelength,
                 UNormalizationMode mode,
                 UErrorCode *status)
{
    return BAL::BIInternationalization::unorm_quickCheck( source, sourcelength, mode, status );
}

inline void UCNV_FROM_U_CALLBACK_ESCAPE (
                  const void *context,
                  UConverterFromUnicodeArgs *fromUArgs,
                  const UChar* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    return BAL::BIInternationalization::UCNV_FROM_U_CALLBACK_ESCAPE( context, fromUArgs, codeUnits, length, codePoint, reason, err );
}

inline void ucnv_setFromUCallBack (UConverter * converter,
                       UConverterFromUCallback newAction,
                       const void *newContext,
                       UConverterFromUCallback *oldAction,
                       const void **oldContext,
                       UErrorCode * err)
{
    return BAL::BIInternationalization::ucnv_setFromUCallBack( converter, newAction, newContext, oldAction, oldContext, err );
}

inline void ucnv_setSubstChars(UConverter *converter,
                   const char *subChars,
                   int8_t len,
                   UErrorCode *err)
{
    return BAL::BIInternationalization::ucnv_setSubstChars( converter, subChars, len, err );
}

inline void UCNV_FROM_U_CALLBACK_SUBSTITUTE (
                  const void *context,
                  UConverterFromUnicodeArgs *fromUArgs,
                  const UChar* codeUnits,
                  int32_t length,
                  UChar32 codePoint,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    return BAL::BIInternationalization::UCNV_FROM_U_CALLBACK_SUBSTITUTE( context, fromUArgs, codeUnits, length, codePoint, reason, err);
}

inline void ucnv_fromUnicode (UConverter * converter,
                  char **target,
                  const char *targetLimit,
                  const UChar ** source,
                  const UChar * sourceLimit,
                  int32_t* offsets,
                  UBool flush,
                  UErrorCode * err)
{
    return BAL::BIInternationalization::ucnv_fromUnicode( converter, target, targetLimit, source, sourceLimit, offsets, flush, err );
}

inline void ucnv_toUnicode(UConverter *converter,
               UChar **target,
               const UChar *targetLimit,
               const char **source,
               const char *sourceLimit,
               int32_t *offsets,
               UBool flush,
               UErrorCode *err)
{
    return BAL::BIInternationalization::ucnv_toUnicode( converter, target, targetLimit, source, sourceLimit, offsets, flush, err );
}

inline void UCNV_TO_U_CALLBACK_STOP (
                  const void *context,
                  UConverterToUnicodeArgs *toUArgs,
                  const char* codeUnits,
                  int32_t length,
                  UConverterCallbackReason reason,
                  UErrorCode * err)
{
    return BAL::BIInternationalization::UCNV_TO_U_CALLBACK_STOP( context, toUArgs, codeUnits, length, reason, err );
}

inline void ucnv_setToUCallBack (UConverter * converter,
                     UConverterToUCallback newAction,
                     const void* newContext,
                     UConverterToUCallback *oldAction,
                     const void** oldContext,
                     UErrorCode * err)
{
    return BAL::BIInternationalization::ucnv_setToUCallBack( converter, newAction, newContext, oldAction, oldContext, err );
}

inline int32_t ucnv_toUChars(UConverter *cnv,
              UChar *dest, int32_t destCapacity,
              const char *src, int32_t srcLength,
              UErrorCode *pErrorCode)
{
    return BAL::BIInternationalization::ucnv_toUChars( cnv, dest, destCapacity, src, srcLength, pErrorCode );
}

inline int8_t u_charType(UChar32 c)
{
    return BAL::BIInternationalization::u_charType( c );
}

inline UBool u_isprint(UChar32 c)
{
    return BAL::BIInternationalization::u_isprint( c );
}

inline UBool u_ispunct(UChar32 c)
{
    return BAL::BIInternationalization::u_ispunct( c );
}

inline int32_t ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{
    return BAL::BIInternationalization::ubrk_preceding( bi, offset );
}

inline int32_t ubrk_following(UBreakIterator *bi,
           int32_t offset)
{
    return BAL::BIInternationalization::ubrk_following( bi, offset );
}

inline int32_t u_getIntPropertyValue(UChar32 c, UProperty which)
{
    return BAL::BIInternationalization::u_getIntPropertyValue( c, which );
}

inline int32_t u_charDigitValue(UChar32 c)
{
    return BAL::BIInternationalization::u_charDigitValue( c );
}

inline int32_t ubrk_current(const UBreakIterator *bi)
{
    return BAL::BIInternationalization::ubrk_current( bi );
}

inline int32_t ucnv_countAvailable(void)
{
    return BAL::BIInternationalization::ucnv_countAvailable();
}

inline const char* ucnv_getAvailableName(int32_t n)
{
    return BAL::BIInternationalization::ucnv_getAvailableName(n);
}

inline uint16_t ucnv_countAliases(const char *alias, UErrorCode *pErrorCode)
{
    return BAL::BIInternationalization::ucnv_countAliases(alias, pErrorCode);
}

inline const char * ucnv_getAlias(const char *alias, uint16_t n, UErrorCode *pErrorCode)
{
    return BAL::BIInternationalization::ucnv_getAlias(alias, n, pErrorCode);
}

inline const char * ucnv_getName(const UConverter *converter, UErrorCode *err)
{
    return BAL::BIInternationalization::ucnv_getName(converter, err);
}

#endif // __INTER__
#endif // BIINTERNATIONALIZATION_H_

