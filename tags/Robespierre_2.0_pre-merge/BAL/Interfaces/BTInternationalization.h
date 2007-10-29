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
#ifndef BTINTERNATIONALIZATION_H_
#define BTINTERNATIONALIZATION_H_

#include <stdint.h>

#define U_WHITE_SPACE_NEUTRAL  9
#define U_FOLD_CASE_DEFAULT 0
#define U_ZERO_ERROR 0
#define UIDNA_ALLOW_UNASSIGNED 0x0001
#define U_PARSE_CONTEXT_LEN 16
#define UNORM_NFC 4
#define UNORM_UNICODE_3_2 0x20

#define UBRK_CHARACTER 0
#define UBRK_WORD 1
#define UBRK_LINE 2
#define UBRK_SENTENCE 3
#define UBRK_TITLE 4
#define UBRK_DONE ((int32_t) -1)
#define U_AMBIGUOUS_ALIAS_WARNING -122
#define U_BUFFER_OVERFLOW_ERROR   15
#define UNORM_NO 0
#define UNORM_YES 1
#define UNORM_MAYBE 2
#define UCNV_ESCAPE_XML_DEC   "D"
#define U_OTHER_NEUTRAL        10
#define U_LEFT_TO_RIGHT        0
#define U_ARABIC_NUMBER         5
#define U_EUROPEAN_NUMBER 2
#define U_RIGHT_TO_LEFT    1
#define U_FORMAT_CHAR             16
#define U_SPACE_SEPARATOR         12
#define U_NON_SPACING_MARK        6
#define U_COMBINING_SPACING_MARK  8
#define U_ENCLOSING_MARK          7
#define U_DECIMAL_DIGIT_NUMBER    9
#define U_LETTER_NUMBER           10
#define U_OTHER_NUMBER            11
#define U_SPACE_SEPARATOR         12
#define U_LINE_SEPARATOR          13
#define U_PARAGRAPH_SEPARATOR     14
#define U_CONTROL_CHAR            15
#define U_FORMAT_CHAR             16
#define U_SURROGATE               18
#define U_PRIVATE_USE_CHAR        17
#define U_GENERAL_OTHER_TYPES     0
#define U_UPPERCASE_LETTER        1
#define U_LOWERCASE_LETTER        2
#define U_TITLECASE_LETTER        3
#define U_MODIFIER_LETTER         4
#define U_OTHER_LETTER            5
#define U_CONNECTOR_PUNCTUATION   22
#define U_DASH_PUNCTUATION        19
#define U_START_PUNCTUATION       20
#define U_END_PUNCTUATION         21
#define U_INITIAL_PUNCTUATION     28
#define U_FINAL_PUNCTUATION       29
#define U_OTHER_PUNCTUATION       23
#define U_MATH_SYMBOL             24
#define U_CURRENCY_SYMBOL         25
#define U_MODIFIER_SYMBOL         26
#define U_OTHER_SYMBOL            27
#define U_DIR_NON_SPACING_MARK    17
#define U_BOUNDARY_NEUTRAL        18
#define U_RIGHT_TO_LEFT_EMBEDDING  14
#define U_LEFT_TO_RIGHT_EMBEDDING  11
#define U_RIGHT_TO_LEFT_OVERRIDE   15
#define U_LEFT_TO_RIGHT_OVERRIDE   12
#define U_POP_DIRECTIONAL_FORMAT   16
#define U_EUROPEAN_NUMBER_SEPARATOR 3
#define U_EUROPEAN_NUMBER_TERMINATOR  4
#define U_COMMON_NUMBER_SEPARATOR     6
#define U_BLOCK_SEPARATOR             7
#define U_SEGMENT_SEPARATOR           8
#define U_RIGHT_TO_LEFT_ARABIC   13
#define U_RIGHT_TO_LEFT_EMBEDDING     14
#define U_RIGHT_TO_LEFT_OVERRIDE      15
#define U_LEFT_TO_RIGHT_OVERRIDE      12
#define UCHAR_DECOMPOSITION_TYPE      0x1003
#define U_DT_FONT 5
#define U_DT_COMPAT 2



#define U16_IS_SURROGATE(c) (((c)&0xfffff800)==0xd800)
#define U16_IS_SURROGATE_LEAD(c) (((c)&0x400)==0)
#define U16_IS_TRAIL(c) (((c)&0xfffffc00)==0xdc00)
#define U16_SURROGATE_OFFSET ((0xd800<<10UL)+0xdc00-0x10000)
#define U16_GET_SUPPLEMENTARY(lead, trail) \
    (((UChar32)(lead)<<10UL)+(UChar32)(trail)-U16_SURROGATE_OFFSET)
#define U16_LEAD(supplementary) (UChar)(((supplementary)>>10)+0xd7c0)
#define U16_TRAIL(supplementary) (UChar)(((supplementary)&0x3ff)|0xdc00)

#define U_MASK(x) ((uint32_t)1<<(x))
#define U_GC_LL_MASK    U_MASK(U_LOWERCASE_LETTER)
#define U_GC_LU_MASK    U_MASK(U_UPPERCASE_LETTER)
#define U_GC_LO_MASK    U_MASK(U_OTHER_LETTER)
#define U_GC_LT_MASK    U_MASK(U_TITLECASE_LETTER)
#define U_GC_NL_MASK    U_MASK(U_LETTER_NUMBER)
#define U_GET_GC_MASK(c) U_MASK(u_charType(c))
#define U_GC_MC_MASK    U_MASK(U_COMBINING_SPACING_MARK)
#define U_GC_ME_MASK    U_MASK(U_ENCLOSING_MARK)
#define U_GC_MN_MASK    U_MASK(U_NON_SPACING_MARK)
#define U_GC_LM_MASK    U_MASK(U_MODIFIER_LETTER)
#define U_GC_ND_MASK    U_MASK(U_DECIMAL_DIGIT_NUMBER)

#define U16_IS_LEAD(c) (((c)&0xfffffc00)==0xd800)
#define U16_NEXT(s, i, length, c) { \
    (c)=(s)[(i)++]; \
    if(U16_IS_LEAD(c)) { \
        uint16_t __c2; \
        if((i)<(length) && U16_IS_TRAIL(__c2=(s)[(i)])) { \
            ++(i); \
            (c)=U16_GET_SUPPLEMENTARY((c), __c2); \
        } \
    } \
}
#define U16_GET_SUPPLEMENTARY(lead, trail) \
    (((UChar32)(lead)<<10UL)+(UChar32)(trail)-U16_SURROGATE_OFFSET)

typedef int32_t UChar32;
typedef int16_t UChar;
typedef int UErrorCode;
typedef int8_t UBool;
typedef int UNormalizationMode;
typedef void UBreakIterator;
typedef int UBreakIteratorType;
typedef void UConverter;
typedef int UNormalizationCheckResult;
typedef int UConverterCallbackReason;
typedef int UCharDirection;
typedef int UProperty;
typedef int UDecompositionType;

typedef struct UParseError {
    int32_t        line;
    int32_t        offset;
    UChar          preContext[U_PARSE_CONTEXT_LEN];
    UChar          postContext[U_PARSE_CONTEXT_LEN];
}UParseError;

typedef struct {
    uint16_t size;              /**< The size of this struct. @stable ICU 2.0 */
    UBool flush;                /**< The internal state of converter will be reset and data flushed if set to TRUE. @stable ICU 2.0    */
    UConverter *converter;      /**< Pointer to the converter that is opened and to which this struct is passed as an argument. @stable ICU 2.0  */
    const UChar *source;        /**< Pointer to the source source buffer. @stable ICU 2.0    */
    const UChar *sourceLimit;   /**< Pointer to the limit (end + 1) of source buffer. @stable ICU 2.0    */
    char *target;               /**< Pointer to the target buffer. @stable ICU 2.0    */
    const char *targetLimit;    /**< Pointer to the limit (end + 1) of target buffer. @stable ICU 2.0     */
    int32_t *offsets;           /**< Pointer to the buffer that recieves the offsets. *offset = blah ; offset++;. @stable ICU 2.0  */
} UConverterFromUnicodeArgs;

typedef struct {
    uint16_t size;              /**< The size of this struct   @stable ICU 2.0 */
    UBool flush;                /**< The internal state of converter will be reset and data flushed if set to TRUE. @stable ICU 2.0   */
    UConverter *converter;      /**< Pointer to the converter that is opened and to which this struct is passed as an argument. @stable ICU 2.0 */
    const char *source;         /**< Pointer to the source source buffer. @stable ICU 2.0    */
    const char *sourceLimit;    /**< Pointer to the limit (end + 1) of source buffer. @stable ICU 2.0    */
    UChar *target;              /**< Pointer to the target buffer. @stable ICU 2.0    */
    const UChar *targetLimit;   /**< Pointer to the limit (end + 1) of target buffer. @stable ICU 2.0     */
    int32_t *offsets;           /**< Pointer to the buffer that recieves the offsets. *offset = blah ; offset++;. @stable ICU 2.0  */
} UConverterToUnicodeArgs;

typedef void (*UConverterFromUCallback) (
                    const void* context,
                    UConverterFromUnicodeArgs *args,
                    const UChar* codeUnits,
                    int32_t length,
                    UChar32 codePoint,
                    UConverterCallbackReason reason,
                    UErrorCode *pErrorCode);

typedef void (*UConverterToUCallback) (
                  const void* context,
                  UConverterToUnicodeArgs *args,
                  const char *codeUnits,
                  int32_t length,
                  UConverterCallbackReason reason,
                  UErrorCode *pErrorCode);

inline UBool U_SUCCESS(UErrorCode code) { return (UBool)(code<=U_ZERO_ERROR); }
inline UBool U_FAILURE(UErrorCode code) { return (UBool)(code>U_ZERO_ERROR); }


#endif //BTINTERNATIONALIZATION_H_
