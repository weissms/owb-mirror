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

/**
 * @file  BCInternationalization.h
 *
 * Header file for BCInternationalization.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef BCINTERNATIONALISATION_H
#define BCINTERNATIONALISATION_H

#include "BIInternationalization.h"
//#include "DeprecatedString.h"

namespace BAL{

class BCInternationalization : public BIInternationalization {
public:
    BCInternationalization(){};
    virtual ~BCInternationalization(){};

    virtual DeprecatedString *toAce( const DeprecatedString *s, const unsigned l );
    virtual const UChar* normalize(const UChar* characters, unsigned norm, size_t *length );
    virtual bool isSpace(unsigned short c);
    virtual UChar32 toLower(unsigned short c);
    virtual int toLower(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error);
    virtual UChar32 toUpper(unsigned short c);
    virtual int toUpper(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error);
    virtual UChar32 foldCase(UChar32 c);
    virtual int foldCase(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error);
    virtual UChar32 toTitleCase(UChar32 c);
    virtual bool isFormatChar(UChar32 c);
    virtual bool isSeparatorSpace(UChar32 c);
    virtual bool isPrintableChar(UChar32 c);
    virtual bool isDigit(UChar32 c);
    virtual bool isPunct(UChar32 c);
    virtual UChar32 mirroredChar(UChar32 c);
    virtual CharCategory category(UChar32 c);
    virtual Direction direction(UChar32 c);
    virtual bool isLower(UChar32 c);
    virtual bool isUpper(UChar32 c);
    virtual int digitValue(UChar32 c);
    virtual uint8_t combiningClass(UChar32 c);
    virtual DecompositionType decompositionType(UChar32 c);
    virtual int umemcasecmp(const UChar* a, const UChar* b, int len);
};

}
#endif // BCInternationalization
