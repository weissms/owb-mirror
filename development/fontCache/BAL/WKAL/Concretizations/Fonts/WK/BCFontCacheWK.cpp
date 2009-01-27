/*
 * Copyright (C) 2006, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Nicholas Shanks <webkit@nickshanks.com>
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
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "FontCache.h"

#include "Font.h"
#include "FontFallbackList.h"
#include "FontPlatformData.h"
#include "FontSelector.h"
#include "StringHash.h"
#include <wtf/HashMap.h>
#include <wtf/ListHashSet.h>

using namespace WTF;
namespace WKAL {

struct FontDataCacheKey {
    FontDataCacheKey(const AtomicString& family = AtomicString(), unsigned size = 0, unsigned weight = 0, bool italic = false,
                             bool isPrinterFont = false, FontRenderingMode renderingMode = NormalRenderingMode)
        : m_family(family)
        , m_size(size)
        , m_weight(weight)
        , m_italic(italic)
        , m_printerFont(isPrinterFont)
        , m_renderingMode(renderingMode)
    {
    }

    FontDataCacheKey(HashTableDeletedValueType) : m_size(hashTableDeletedSize()) { }
    bool isHashTableDeletedValue() const { return m_size == hashTableDeletedSize(); }

    bool operator==(const FontDataCacheKey& other) const
    {
        return equalIgnoringCase(m_family, other.m_family) && m_size == other.m_size && 
               m_weight == other.m_weight && m_italic == other.m_italic && m_printerFont == other.m_printerFont &&
               m_renderingMode == other.m_renderingMode;
    }

    AtomicString m_family;
    unsigned m_size;
    unsigned m_weight;
    bool m_italic;
    bool m_printerFont;
    FontRenderingMode m_renderingMode;

private:
    static unsigned hashTableDeletedSize() { return 0xFFFFFFFFU; }
};

inline unsigned computeHash(const FontDataCacheKey& fontKey)
{
    unsigned hashCodes[4] = {
        CaseFoldingHash::hash(fontKey.m_family),
        fontKey.m_size,
        fontKey.m_weight,
        static_cast<unsigned>(fontKey.m_italic) << 2 | static_cast<unsigned>(fontKey.m_printerFont) << 1 | static_cast<unsigned>(fontKey.m_renderingMode)
    };
    return StringImpl::computeHash(reinterpret_cast<UChar*>(hashCodes), sizeof(hashCodes) / sizeof(UChar));
}

struct FontDataCacheKeyHash {
    static unsigned hash(const FontDataCacheKey& font)
    {
        return computeHash(font);
    }
         
    static bool equal(const FontDataCacheKey& a, const FontDataCacheKey& b)
    {
        return a == b;
    }

    static const bool safeToCompareToEmptyOrDeleted = true;
};

struct FontDataCacheKeyTraits : WTF::GenericHashTraits<FontDataCacheKey> {
    static const bool emptyValueIsZero = true;
    static const FontDataCacheKey& emptyValue()
    {
        static FontDataCacheKey key(nullAtom);
        return key;
    }
    static void constructDeletedValue(FontDataCacheKey& slot)
    {
        new (&slot) FontDataCacheKey(HashTableDeletedValue);
    }
    static bool isDeletedValue(const FontDataCacheKey& value)
    {
        return value.isHashTableDeletedValue();
    }
};

// This is to have a fast mapping from a FontPlatformData to a SimpleFontData.
// We use raw pointers here as this map should not hold the data, it is only
// a convenience. Other place hold a reference to the SimpleFontData and the
// SimpleFontData hold a strong reference to the FontPlatformData.
typedef HashMap<FontPlatformData*, SimpleFontData*> FontPlatformDataMap;
static FontPlatformDataMap* gFontPlatformDataMap = 0;

typedef HashMap<FontDataCacheKey, RefPtr<SimpleFontData>, FontDataCacheKeyHash, FontDataCacheKeyTraits> FontDataCache;
static FontDataCache* gFontDataCache = 0;

static FontDataCache* gInactiveFontData = 0;

static HashSet<FontSelector*>* gClients;

const int cMaxInactiveFontData = 120;  // Pretty Low Threshold
const int cTargetInactiveFontData = 100;
static unsigned gGeneration = 0;

FontCache* fontCache()
{
    static FontCache cache;
    return &cache;
}

FontCache::FontCache()
{
    platformInit();
}

FontCache::~FontCache()
{
    invalidate();
    if (gFontDataCache)
        delete gFontDataCache;
    // FIXME :clean ???
    if (gFontPlatformDataMap)
        delete gFontPlatformDataMap;
    if (gInactiveFontData)
        delete gInactiveFontData;
    platformDestroy();
}


static const AtomicString& alternateFamilyName(const AtomicString& familyName)
{
    // Alias Courier <-> Courier New
    static AtomicString courier("Courier"), courierNew("Courier New");
    if (equalIgnoringCase(familyName, courier))
        return courierNew;
    if (equalIgnoringCase(familyName, courierNew))
        return courier;

    // Alias Times and Times New Roman.
    static AtomicString times("Times"), timesNewRoman("Times New Roman");
    if (equalIgnoringCase(familyName, times))
        return timesNewRoman;
    if (equalIgnoringCase(familyName, timesNewRoman))
        return times;
    
    // Alias Arial and Helvetica
    static AtomicString arial("Arial"), helvetica("Helvetica");
    if (equalIgnoringCase(familyName, arial))
        return helvetica;
    if (equalIgnoringCase(familyName, helvetica))
        return arial;

    return emptyAtom;
}

PassRefPtr<SimpleFontData> FontCache::getLastResortFallbackFont(const FontDescription& fontDescription)
{
    // FIXME: Would be even better to somehow get the user's default font here.
    // The Times fallback will almost always work, but in the highly unusual case where
    // the user doesn't have it, we fall back on Lucida Grande because that's
    // guaranteed to be there, according to Nathan Taylor. This is good enough
    // to avoid a crash at least.
    static AtomicString fontFallbacks[2] = { "Times", "Lucida Grande" };
    RefPtr<SimpleFontData> fallback = 0;
    for (int i = 0; i < 2 && !fallback; ++i)
        fallback = getCachedFontData(fontDescription, fontFallbacks[i]);

    return fallback.release();
}

PassRefPtr<SimpleFontData> FontCache::getCachedFontData(const FontDescription& fontDescription, 
                                                       const AtomicString& familyName,
                                                       bool checkingAlternateName)
{
    if (!gFontDataCache) {
        gFontDataCache = new FontDataCache;
        gInactiveFontData = new FontDataCache;
        gFontPlatformDataMap = new FontPlatformDataMap;
    }

    FontDataCacheKey key(familyName, fontDescription.computedPixelSize(), fontDescription.weight(), fontDescription.italic(),
                                 fontDescription.usePrinterFont(), fontDescription.renderingMode());

    // Look for the result in the inactive cache.
    RefPtr<SimpleFontData> result = gInactiveFontData->take(key);
    if (result) {
        ASSERT(!gFontDataCache->contains(key));
        gFontDataCache->set(key, result);
        gInactiveFontData->remove(key);
        return result;
    }

    // It was not fond in the inactive cache so look in the active cache.
    bool foundResult;
    FontDataCache::iterator it = gFontDataCache->find(key);
    if (it == gFontDataCache->end()) {
        RefPtr<FontPlatformData> newPlatformData = createFontPlatformData(fontDescription, familyName);
        result = getCachedFontData(newPlatformData.get());
        gFontDataCache->set(key, result);
        foundResult = newPlatformData;
    } else {
        result = it->second;
        foundResult = true;
    }

    if (!foundResult && !checkingAlternateName) {
        // We were unable to find a font.  We have a small set of fonts that we alias to other names, 
        // e.g., Arial/Helvetica, Courier/Courier New, etc.  Try looking up the font under the aliased name.
        const AtomicString& alternateName = alternateFamilyName(familyName);
        if (!alternateName.isEmpty())
            result = getCachedFontData(fontDescription, alternateName, true);
        if (result)
            gFontDataCache->set(key, result); // Cache the result under the old name.
    }

    return result.release();
}

PassRefPtr<SimpleFontData> FontCache::getCachedFontData(FontPlatformData* platformData)
{
    if (!platformData)
        return 0;

    if (!gFontDataCache) {
        gFontDataCache = new FontDataCache;
        gInactiveFontData = new FontDataCache;
        gFontPlatformDataMap = new FontPlatformDataMap;
    }

    FontPlatformDataMap::iterator result = gFontPlatformDataMap->find(platformData);
    if (result == gFontPlatformDataMap->end()) {
        RefPtr<SimpleFontData> newValue = adoptRef(new SimpleFontData(platformData));
        gFontPlatformDataMap->set(platformData, newValue.get());
        return newValue.release();
    }

    return result.get()->second;
}

void FontCache::releasePlatformData(FontPlatformData* platformData)
{
    if (gFontPlatformDataMap->contains(platformData))
        gFontPlatformDataMap->remove(platformData);
}

void FontCache::releaseFontData(const SimpleFontData* fontData)
{
    ASSERT(gFontDataCache);
    ASSERT(!fontData->isCustomFont());
    ASSERT(gFontPlatformDataMap->contains(const_cast<FontPlatformData*>(fontData->platformData())));

    if(fontData->hasOneRef()) {
        // Find the key associated with the font data and insert it into the inactive font cache.
        // FIXME: This may hurt performance.
        // FIXME: We cannot guarantee that we will find the data in the cache.
        FontDataCache::iterator it = gFontDataCache->begin();
        FontDataCache::iterator end = gFontDataCache->end();
        for (; it != end; ++it) {
            if (it->second.get() == fontData) {
                gInactiveFontData->add(it->first, it->second);
                gFontDataCache->remove(it);
                break;
            }
        }
    }

    if (gInactiveFontData->size() > cMaxInactiveFontData)
        purgeInactiveFontData(gInactiveFontData->size() - cTargetInactiveFontData);
}

void FontCache::purgeInactiveFontData(int count)
{
    if (!gInactiveFontData)
        return;

    static bool isPurging;  // Guard against reentry when e.g. a deleted FontData releases its small caps FontData.
    if (isPurging)
        return;

    isPurging = true;

    if (count > gInactiveFontData->size()) {
        // Remove everything
        gInactiveFontData->clear();
    } else {
        for (int i = 0; i < count; ++i)
            gInactiveFontData->remove(gInactiveFontData->begin());
    }

    isPurging = false;
}

size_t FontCache::fontDataCount()
{
    if (gFontDataCache)
        return gFontDataCache->size();
    return 0;
}

size_t FontCache::inactiveFontDataCount()
{
    if (gInactiveFontData)
        return gInactiveFontData->size();
    return 0;
}

PassRefPtr<FontData> FontCache::getFontData(const Font& font, int& familyIndex, FontSelector* fontSelector)
{
    int startIndex = familyIndex;
    const FontFamily* startFamily = &font.fontDescription().family();
    for (int i = 0; startFamily && i < startIndex; i++)
        startFamily = startFamily->next();
    const FontFamily* currFamily = startFamily;
    while (currFamily) {
        familyIndex++;
        if (currFamily->family().length()) {
            if (fontSelector) {
                FontData* data = fontSelector->getFontData(font.fontDescription(), currFamily->family());
                if (data)
                    return data;
            }
            RefPtr<SimpleFontData> curFontData = getCachedFontData(font.fontDescription(), currFamily->family());
            if (curFontData)
                return curFontData.release();
        }
        currFamily = currFamily->next();
    }

    if (!currFamily)
        familyIndex = cAllFamiliesScanned;

    // We didn't find a font. Try to find a similar font using our own specific knowledge about our platform.
    // For example on OS X, we know to map any families containing the words Arabic, Pashto, or Urdu to the
    // Geeza Pro font.
    RefPtr<FontPlatformData> result = getSimilarFontPlatformData(font);

    if (!result && startIndex == 0) {
        // If it's the primary font that we couldn't find, we try the following. In all other cases, we will
        // just use per-character system fallback.

        if (fontSelector) {
            // Try the user's preferred standard font.
            if (FontData* data = fontSelector->getFontData(font.fontDescription(), "-webkit-standard"))
                return data;
        }

        // Still no result.  Hand back our last resort fallback font.
        return getLastResortFallbackFont(font.fontDescription());
    }

    // Now that we have a result, we need to go from FontPlatformData -> FontData.
    return getCachedFontData(result.get());
}


void FontCache::addClient(FontSelector* client)
{
    if (!gClients)
        gClients = new HashSet<FontSelector*>;

    ASSERT(!gClients->contains(client));
    gClients->add(client);
}

void FontCache::removeClient(FontSelector* client)
{
    ASSERT(gClients);
    ASSERT(gClients->contains(client));

    gClients->remove(client);
}

unsigned FontCache::generation()
{
    return gGeneration;
}

void FontCache::invalidate()
{
    if (!gClients) {
        ASSERT(!gFontDataCache);
        return;
    }

    if (gFontDataCache) {
        gFontDataCache->clear();
        delete gFontDataCache;
        gFontDataCache = new FontDataCache;
    }

    gGeneration++;

    Vector<RefPtr<FontSelector> > clients;
    size_t numClients = gClients->size();
    clients.reserveCapacity(numClients);
    HashSet<FontSelector*>::iterator end = gClients->end();
    for (HashSet<FontSelector*>::iterator it = gClients->begin(); it != end; ++it)
        clients.append(*it);

    ASSERT(numClients == clients.size());
    for (size_t i = 0; i < numClients; ++i)
        clients[i]->fontCacheInvalidated();

    purgeInactiveFontData();
}

} // namespace WebCore
