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
#ifndef StringImpl_h
#define StringImpl_h
/**
 *  @file  StringImpl.t
 *  StringImpl description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include <kjs/identifier.h>
#include <limits.h>
#include <wtf/ASCIICType.h>
#include <wtf/Forward.h>
#include <wtf/RefCounted.h>
#include <wtf/Vector.h>
#include <wtf/unicode/Unicode.h>



namespace OWBAL {

class AtomicString;
class StringBuffer;

struct CStringTranslator;
struct HashAndCharactersTranslator;
struct Length;
struct StringHash;
struct UCharBufferTranslator;

class StringImpl : public OWBALBase, public RefCounted<StringImpl> {
    friend class AtomicString;
    friend struct CStringTranslator;
    friend struct HashAndCharactersTranslator;
    friend struct UCharBufferTranslator;
private:
    /**
     *  StringImpl description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    StringImpl();
    /**
     *  StringImpl description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    StringImpl(const UChar*, unsigned length);
    /**
     *  StringImpl description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    StringImpl(const char*, unsigned length);

    struct AdoptBuffer { };
    /**
     *  StringImpl description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    StringImpl(UChar*, unsigned length, AdoptBuffer);

    struct WithTerminatingNullCharacter { };
    /**
     *  StringImpl description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    StringImpl(const StringImpl&, WithTerminatingNullCharacter);

    // For AtomicString.
    /**
     *  StringImpl description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    StringImpl(const UChar*, unsigned length, unsigned hash);
    /**
     *  StringImpl description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    StringImpl(const char*, unsigned length, unsigned hash);

public:
    /**
     *  ~StringImpl description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    ~StringImpl();

    /**
     *  create description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    static PassRefPtr<StringImpl> create(const UChar*, unsigned length);
    /**
     *  create description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    static PassRefPtr<StringImpl> create(const char*, unsigned length);
    /**
     *  create description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    static PassRefPtr<StringImpl> create(const char*);

    /**
     *  createWithTerminatingNullCharacter description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    static PassRefPtr<StringImpl> createWithTerminatingNullCharacter(const StringImpl&);

    /**
     *  createStrippingNullCharacters description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    static PassRefPtr<StringImpl> createStrippingNullCharacters(const UChar*, unsigned length);
    /**
     *  adopt description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    static PassRefPtr<StringImpl> adopt(StringBuffer&);
    /**
     *  adopt description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    static PassRefPtr<StringImpl> adopt(Vector<UChar>&);

    /**
     *  characters description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    const UChar* characters() ;
    /**
     *  length description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    unsigned length() ;

    /**
     *  hasTerminatingNullCharacter description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool hasTerminatingNullCharacter() ;

    /**
     *  hash description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    unsigned hash() ;
    /**
     *  computeHash description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    static unsigned computeHash(const UChar*, unsigned len);
    /**
     *  computeHash description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    static unsigned computeHash(const char*);
    
    // Makes a deep copy. Helpful only if you need to use a String on another thread.
    // Since StringImpl objects are immutable, there's no other reason to make a copy.
    /**
     *  copy description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PassRefPtr<StringImpl> copy();

    /**
     *  substring description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PassRefPtr<StringImpl> substring(unsigned pos, unsigned len = UINT_MAX);

    /**
     *  operator[] description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    UChar operator[](unsigned i) ;
    /**
     *  characterStartingAt description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    UChar32 characterStartingAt(unsigned);

    /**
     *  toLength description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    Length toLength();

    /**
     *  containsOnlyWhitespace description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool containsOnlyWhitespace();

    /**
     *  toIntStrict description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    int toIntStrict(bool* ok = 0, int base = 10);
    /**
     *  toUIntStrict description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    unsigned toUIntStrict(bool* ok = 0, int base = 10);
    /**
     *  toInt64Strict description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    int64_t toInt64Strict(bool* ok = 0, int base = 10);
    /**
     *  toUInt64Strict description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    uint64_t toUInt64Strict(bool* ok = 0, int base = 10);

    /**
     *  toInt description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    int toInt(bool* ok = 0); // ignores trailing garbage
    /**
     *  toUInt description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    unsigned toUInt(bool* ok = 0); // ignores trailing garbage
    /**
     *  toInt64 description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    int64_t toInt64(bool* ok = 0); // ignores trailing garbage
    /**
     *  toUInt64 description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    uint64_t toUInt64(bool* ok = 0); // ignores trailing garbage

    /**
     *  toDouble description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    double toDouble(bool* ok = 0);
    /**
     *  toFloat description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    float toFloat(bool* ok = 0);

    /**
     *  toCoordsArray description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    Length* toCoordsArray(int& len);
    /**
     *  toLengthArray description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    Length* toLengthArray(int& len);
    /**
     *  isLower description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool isLower();
    /**
     *  lower description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PassRefPtr<StringImpl> lower();
    /**
     *  upper description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PassRefPtr<StringImpl> upper();
    /**
     *  secure description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PassRefPtr<StringImpl> secure(UChar aChar);
    /**
     *  capitalize description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PassRefPtr<StringImpl> capitalize(UChar previousCharacter);
    /**
     *  foldCase description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PassRefPtr<StringImpl> foldCase();

    /**
     *  stripWhiteSpace description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PassRefPtr<StringImpl> stripWhiteSpace();
    /**
     *  simplifyWhiteSpace description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PassRefPtr<StringImpl> simplifyWhiteSpace();

    /**
     *  find description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    int find(const char*, int index = 0, bool caseSensitive = true);
    /**
     *  find description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    int find(UChar, int index = 0);
    /**
     *  find description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    int find(StringImpl*, int index, bool caseSensitive = true);

    /**
     *  reverseFind description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    int reverseFind(UChar, int index);
    /**
     *  reverseFind description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    int reverseFind(StringImpl*, int index, bool caseSensitive = true);
    
    /**
     *  startsWith description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool startsWith(StringImpl* m_data, bool caseSensitive = true) ;
    /**
     *  endsWith description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    bool endsWith(StringImpl*, bool caseSensitive = true);

    /**
     *  replace description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PassRefPtr<StringImpl> replace(UChar, UChar);
    /**
     *  replace description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PassRefPtr<StringImpl> replace(UChar, StringImpl*);
    /**
     *  replace description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PassRefPtr<StringImpl> replace(StringImpl*, StringImpl*);
    /**
     *  replace description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PassRefPtr<StringImpl> replace(unsigned index, unsigned len, StringImpl*);

    /**
     *  empty description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    static StringImpl* empty();

    /**
     *  ascii description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    Vector<char> ascii();

    /**
     *  defaultWritingDirection description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    WTF::Unicode::Direction defaultWritingDirection();


private:
    unsigned m_length;
    const UChar* m_data;
    mutable unsigned m_hash;
    bool m_inTable;
    bool m_hasTerminatingNullCharacter;
};

    /**
     *  equal description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
bool equal(StringImpl*, StringImpl*);
    /**
     *  equal description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
bool equal(StringImpl*, const char*);
    /**
     *  equal description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
inline bool equal(const char* a, StringImpl* b) ;

    /**
     *  equalIgnoringCase description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
bool equalIgnoringCase(StringImpl*, StringImpl*);
    /**
     *  equalIgnoringCase description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
bool equalIgnoringCase(StringImpl*, const char*);
    /**
     *  equalIgnoringCase description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
inline bool equalIgnoringCase(const char* a, StringImpl* b) ;

// Golden ratio - arbitrary start value to avoid mapping all 0's to all 0's
// or anything like that.
const unsigned phi = 0x9e3779b9U;

// Paul Hsieh's SuperFastHash
// http://www.azillionmonkeys.com/qed/hash.html
    /**
     *  StringImpl::computeHash description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
inline unsigned StringImpl::computeHash(const UChar* data, unsigned length);

// Paul Hsieh's SuperFastHash
// http://www.azillionmonkeys.com/qed/hash.html
    /**
     *  StringImpl::computeHash description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
inline unsigned StringImpl::computeHash(const char* data);

    /**
     *  isSpaceOrNewline description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
static inline bool isSpaceOrNewline(UChar c);

}

namespace WTF {

    // WebCore::StringHash is the default hash for StringImpl* and RefPtr<StringImpl>
    template<typename T> struct DefaultHash;
    template<> struct DefaultHash<WebCore::StringImpl*> {
        typedef WebCore::StringHash Hash;
    };
    template<> struct DefaultHash<RefPtr<WebCore::StringImpl> > {
        typedef WebCore::StringHash Hash;
    };

}

#endif




