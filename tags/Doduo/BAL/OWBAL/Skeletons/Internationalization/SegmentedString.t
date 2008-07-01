/*
 * Copyright (C) 2008 Pleyo.  All rights reserved.
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
#ifndef SegmentedString_h
#define SegmentedString_h
/**
 *  @file  SegmentedString.t
 *  SegmentedString description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include "PlatformString.h"
#include <wtf/Deque.h>

namespace OWBAL {

class SegmentedString;

class SegmentedSubstring : public OWBALBase {
public:
    /**
     *  SegmentedSubstring description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    SegmentedSubstring();
    /**
     *  SegmentedSubstring description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    SegmentedSubstring(const String& str);

    /**
     *  SegmentedSubstring description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    SegmentedSubstring(const UChar* str, int length);

    /**
     *  clear description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void clear() ;
    
    /**
     *  excludeLineNumbers description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool excludeLineNumbers() const ;
    /**
     *  doNotExcludeLineNumbers description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool doNotExcludeLineNumbers() const ;

    /**
     *  setExcludeLineNumbers description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void setExcludeLineNumbers() ;

    /**
     *  appendTo description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void appendTo(String& str) const;

public:
    int m_length;
    const UChar* m_current;

private:
    String m_string;
    bool m_doNotExcludeLineNumbers;
};

class SegmentedString : public OWBALBase {
public:
    /**
     *  SegmentedString description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    SegmentedString();
    /**
     *  SegmentedString description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    SegmentedString(const UChar* str, int length);
    /**
     *  SegmentedString description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    SegmentedString(const String& str);
    /**
     *  SegmentedString description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    SegmentedString(const SegmentedString&);

    /**
     *  operator= description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    const SegmentedString& operator=(const SegmentedString&);

    /**
     *  clear description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void clear();

    /**
     *  append description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void append(const SegmentedString&);
    /**
     *  prepend description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void prepend(const SegmentedString&);
    
    /**
     *  excludeLineNumbers description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool excludeLineNumbers() const ;
    /**
     *  setExcludeLineNumbers description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void setExcludeLineNumbers();

    /**
     *  push description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void push(UChar c);
    
    /**
     *  isEmpty description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool isEmpty() const ;
    /**
     *  length description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    unsigned length() const;

    /**
     *  advance description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void advance();
    
    /**
     *  advancePastNewline description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void advancePastNewline(int& lineNumber);
    
    /**
     *  advancePastNonNewline description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void advancePastNonNewline();
    
    /**
     *  advance description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void advance(int& lineNumber);
    
    /**
     *  escaped description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool escaped() const ;
    
    /**
     *  toString description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    String toString() const;

    /**
     *  operator* description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    const UChar& operator*() const ;
    /**
     *  operator-> description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    const UChar* operator->() const ;
    
private:
    /**
     *  append description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void append(const SegmentedSubstring&);
    /**
     *  prepend description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void prepend(const SegmentedSubstring&);

    /**
     *  advanceSlowCase description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void advanceSlowCase();
    /**
     *  advanceSlowCase description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void advanceSlowCase(int& lineNumber);
    /**
     *  advanceSubstring description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void advanceSubstring();
    /**
     *  current description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    const UChar* current() const ;

    UChar m_pushedChar1;
    UChar m_pushedChar2;
    SegmentedSubstring m_currentString;
    const UChar* m_currentChar;
    Deque<SegmentedSubstring> m_substrings;
    bool m_composite;
};

}

#endif




