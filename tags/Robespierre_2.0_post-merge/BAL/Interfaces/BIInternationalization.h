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

#include "BALConfiguration.h"
#include "BTInternationalization.h"
//#include "DeprecatedString.h"

 namespace WebCore{
     class DeprecatedString;
 }

using WebCore::DeprecatedString;
//using WebCore::DeprecatedChar;

namespace WTF {
  namespace Unicode {

    enum Direction {
      LeftToRight = U_LEFT_TO_RIGHT,
      RightToLeft = U_RIGHT_TO_LEFT,
      EuropeanNumber = U_EUROPEAN_NUMBER,
      EuropeanNumberSeparator = U_EUROPEAN_NUMBER_SEPARATOR,
      EuropeanNumberTerminator = U_EUROPEAN_NUMBER_TERMINATOR,
      ArabicNumber = U_ARABIC_NUMBER,
      CommonNumberSeparator = U_COMMON_NUMBER_SEPARATOR,
      BlockSeparator = U_BLOCK_SEPARATOR,
      SegmentSeparator = U_SEGMENT_SEPARATOR,
      WhiteSpaceNeutral = U_WHITE_SPACE_NEUTRAL,
      OtherNeutral = U_OTHER_NEUTRAL,
      LeftToRightEmbedding = U_LEFT_TO_RIGHT_EMBEDDING,
      LeftToRightOverride = U_LEFT_TO_RIGHT_OVERRIDE,
      RightToLeftArabic = U_RIGHT_TO_LEFT_ARABIC,
      RightToLeftEmbedding = U_RIGHT_TO_LEFT_EMBEDDING,
      RightToLeftOverride = U_RIGHT_TO_LEFT_OVERRIDE,
      PopDirectionalFormat = U_POP_DIRECTIONAL_FORMAT,
      NonSpacingMark = U_DIR_NON_SPACING_MARK,
      BoundaryNeutral = U_BOUNDARY_NEUTRAL
    };

    enum DecompositionType {
      DecompositionNone = U_DT_NONE,
      DecompositionCanonical = U_DT_CANONICAL,
      DecompositionCompat = U_DT_COMPAT,
      DecompositionCircle = U_DT_CIRCLE,
      DecompositionFinal = U_DT_FINAL,
      DecompositionFont = U_DT_FONT,
      DecompositionFraction = U_DT_FRACTION,
      DecompositionInitial = U_DT_INITIAL,
      DecompositionIsolated = U_DT_ISOLATED,
      DecompositionMedial = U_DT_MEDIAL,
      DecompositionNarrow = U_DT_NARROW,
      DecompositionNoBreak = U_DT_NOBREAK,
      DecompositionSmall = U_DT_SMALL,
      DecompositionSquare = U_DT_SQUARE,
      DecompositionSub = U_DT_SUB,
      DecompositionSuper = U_DT_SUPER,
      DecompositionVertical = U_DT_VERTICAL,
      DecompositionWide = U_DT_WIDE,
    };

    enum CharCategory {
      NoCategory =  0,
      Other_NotAssigned = U_MASK(U_GENERAL_OTHER_TYPES),
      Letter_Uppercase = U_MASK(U_UPPERCASE_LETTER),
      Letter_Lowercase = U_MASK(U_LOWERCASE_LETTER),
      Letter_Titlecase = U_MASK(U_TITLECASE_LETTER),
      Letter_Modifier = U_MASK(U_MODIFIER_LETTER),
      Letter_Other = U_MASK(U_OTHER_LETTER),

      Mark_NonSpacing = U_MASK(U_NON_SPACING_MARK),
      Mark_Enclosing = U_MASK(U_ENCLOSING_MARK),
      Mark_SpacingCombining = U_MASK(U_COMBINING_SPACING_MARK),

      Number_DecimalDigit = U_MASK(U_DECIMAL_DIGIT_NUMBER),
      Number_Letter = U_MASK(U_LETTER_NUMBER),
      Number_Other = U_MASK(U_OTHER_NUMBER),

      Separator_Space = U_MASK(U_SPACE_SEPARATOR),
      Separator_Line = U_MASK(U_LINE_SEPARATOR),
      Separator_Paragraph = U_MASK(U_PARAGRAPH_SEPARATOR),

      Other_Control = U_MASK(U_CONTROL_CHAR),
      Other_Format = U_MASK(U_FORMAT_CHAR),
      Other_PrivateUse = U_MASK(U_PRIVATE_USE_CHAR),
      Other_Surrogate = U_MASK(U_SURROGATE),

      Punctuation_Dash = U_MASK(U_DASH_PUNCTUATION),
      Punctuation_Open = U_MASK(U_START_PUNCTUATION),
      Punctuation_Close = U_MASK(U_END_PUNCTUATION),
      Punctuation_Connector = U_MASK(U_CONNECTOR_PUNCTUATION),
      Punctuation_Other = U_MASK(U_OTHER_PUNCTUATION),

      Symbol_Math = U_MASK(U_MATH_SYMBOL),
      Symbol_Currency = U_MASK(U_CURRENCY_SYMBOL),
      Symbol_Modifier = U_MASK(U_MODIFIER_SYMBOL),
      Symbol_Other = U_MASK(U_OTHER_SYMBOL),

      Punctuation_InitialQuote = U_MASK(U_INITIAL_PUNCTUATION),
      Punctuation_FinalQuote = U_MASK(U_FINAL_PUNCTUATION)
    };
  }
}

using WTF::Unicode::CharCategory;
using WTF::Unicode::Direction;
using WTF::Unicode::DecompositionType;

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
            virtual ~BIInternationalization(){}

            virtual DeprecatedString *toAce( const DeprecatedString *s, const unsigned l ) = 0;
            virtual const UChar* normalize(const UChar* characters, unsigned norm, size_t *length ) = 0;
            virtual bool isSpace(unsigned short c) = 0;
            virtual UChar32 toLower(unsigned short c) = 0;
            virtual int toLower(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error) = 0;
            virtual UChar32 toUpper(unsigned short c) = 0;
            virtual int toUpper(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error) = 0;
            virtual UChar32 foldCase(UChar32 c) = 0;
            virtual int foldCase(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error) = 0;
            virtual UChar32 toTitleCase(UChar32 c) = 0;
            virtual bool isFormatChar(UChar32 c) = 0;
            virtual bool isSeparatorSpace(UChar32 c) = 0;
            virtual bool isPrintableChar(UChar32 c) = 0;
            virtual bool isDigit(UChar32 c) = 0;
            virtual bool isPunct(UChar32 c) = 0;
            virtual UChar32 mirroredChar(UChar32 c) = 0;
            virtual CharCategory category(UChar32 c) = 0;
            virtual Direction direction(UChar32 c) = 0;
            virtual bool isLower(UChar32 c) = 0;
            virtual bool isUpper(UChar32 c) = 0;
            virtual int digitValue(UChar32 c) = 0;
            virtual uint8_t combiningClass(UChar32 c) = 0;
            virtual DecompositionType decompositionType(UChar32 c) = 0;
            virtual int umemcasecmp(const UChar* a, const UChar* b, int len) = 0;
    };
}

using BAL::BIInternationalization;

namespace WTF {
  namespace Unicode {

    inline DeprecatedString *toAce( const DeprecatedString *s, const unsigned l )
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->toAce(s, l);
    }
    
    inline const UChar* normalize(const UChar* characters, unsigned norm, size_t *length )
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->normalize(characters, norm, length);
    }
    
    inline bool isSpace(unsigned short c)
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->isSpace(c);
    }
    
     inline UChar32 toLower(unsigned short c)
     {
         BIInternationalization *inter = ::BAL::getBIInternationalization();
         return inter->toLower(c);
     }
    
    inline int toLower(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error)
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->toLower(result, resultLength, src, srcLength, error);
    }
    
    inline UChar32 toUpper(unsigned short c)
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->toUpper(c);
    }
    
    inline int toUpper(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error)
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->toUpper(result, resultLength, src, srcLength, error);
    }
    
    inline UChar32 foldCase(UChar32 c)
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->foldCase(c);
    }
    
    inline int foldCase(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error)
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->foldCase(result, resultLength, src, srcLength, error);
    }
    
    inline UChar32 toTitleCase(UChar32 c)
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->toTitleCase(c);
    }
    
    inline bool isFormatChar(UChar32 c)
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->isFormatChar(c);
    }
    
     inline bool isSeparatorSpace(UChar32 c)
     {
         BIInternationalization *inter = ::BAL::getBIInternationalization();
         return inter->isSeparatorSpace(c);
     }
     
     inline bool isPrintableChar(UChar32 c)
     {
         BIInternationalization *inter = ::BAL::getBIInternationalization();
         return inter->isPrintableChar(c);
     }
    
    inline bool isDigit(UChar32 c)
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->isDigit(c);
    }
    
    inline bool isPunct(UChar32 c)
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->isPunct(c);
    }
    
    inline UChar32 mirroredChar(UChar32 c)
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->mirroredChar(c);
    }
    
     inline CharCategory category(UChar32 c)
     {
         BIInternationalization *inter = ::BAL::getBIInternationalization();
         return inter->category(c);
     }
    
    inline Direction direction(UChar32 c)
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->direction(c);
    }
    
    inline bool isLower(UChar32 c)
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->isLower(c);
    }
    
    inline bool isUpper(UChar32 c)
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->isUpper(c);
    }
    
    inline int digitValue(UChar32 c)
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->digitValue(c);
    }
    
    inline uint8_t combiningClass(UChar32 c)
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->combiningClass(c);
    }
    
    inline DecompositionType decompositionType(UChar32 c)
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->decompositionType(c);
    }
    
    inline int umemcasecmp(const UChar* a, const UChar* b, int len)
    {
        BIInternationalization *inter = ::BAL::getBIInternationalization();
        return inter->umemcasecmp(a, b, len);
    }
   }
}

#endif // BIINTERNATIONALIZATION_H_
