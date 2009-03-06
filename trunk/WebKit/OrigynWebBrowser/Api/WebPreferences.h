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

#ifndef WebPreferences_H
#define WebPreferences_H


/**
 *  @file  WebPreferences.h
 *  WebPreferences 
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"
#include "PlatformString.h"
#include "WebKitTypes.h"

typedef enum {
    WebKitEditableLinkDefaultBehavior,
    WebKitEditableLinkAlwaysLive,
    WebKitEditableLinkOnlyLiveWithShiftKey,
    WebKitEditableLinkLiveWhenNotFocused,
    WebKitEditableLinkNeverLive
} WebKitEditableLinkBehavior;


typedef enum WebKitCookieStorageAcceptPolicy {
    WebKitCookieStorageAcceptPolicyAlways = 0,
    WebKitCookieStorageAcceptPolicyNever,
    WebKitCookieStorageAcceptPolicyOnlyFromMainDocumentDomain
} WebKitCookieStorageAcceptPolicy;

typedef enum FontSmoothingType {
    FontSmoothingTypeStandard=0,
    FontSmoothingTypeLight,
    FontSmoothingTypeMedium,
    FontSmoothingTypeStrong,
    FontSmoothingTypeWindows
} FontSmoothingType;


class WebPreferences {
public:

    /**
     * Create an new instance of WebPreferences
     */
    static WebPreferences* createInstance();

    /**
     * Create an new instance of WebPreferences
     * that is filled with default values.
     */
    static WebPreferences* createInitializedInstance();

    /**
     * WebPreferences destructor
     */
    virtual ~WebPreferences();
protected:

    /**
     *  WebPreferences constructor
     */
    WebPreferences();

public:
    // IWebPreferences

    /**
     * get standard preferences 
     */
    virtual WebPreferences* standardPreferences();

    /**
     * initialise with an identifier 
     */
    virtual WebPreferences* initWithIdentifier(WebCore::String anIdentifier);

    /**
     * get identifier
     */
    virtual WebCore::String identifier();

    /**
     * standardFontFamily 
     */
    virtual WebCore::String standardFontFamily();

    /**
     *  setStandardFontFamily 
     */
    virtual void setStandardFontFamily(WebCore::String family);

    /**
     *  fixedFontFamily 
     */
    virtual WebCore::String fixedFontFamily();

    /**
     *  setFixedFontFamily 
     */
    virtual void setFixedFontFamily(WebCore::String family);

    /**
     *  serifFontFamily 
     */
    virtual WebCore::String serifFontFamily();

    /**
     *  setSerifFontFamily 
     */
    virtual void setSerifFontFamily(WebCore::String family);

    /**
     *  sansSerifFontFamily
     */
    virtual WebCore::String sansSerifFontFamily();

    /**
     *  setSansSerifFontFamily 
     */
    virtual void setSansSerifFontFamily(WebCore::String family);

    /**
     *  cursiveFontFamily 
     */
    virtual WebCore::String cursiveFontFamily();

    /**
     *  setCursiveFontFamily 
     */
    virtual void setCursiveFontFamily(WebCore::String family);

    /**
     *  fantasyFontFamily 
     */
    virtual WebCore::String fantasyFontFamily();

    /**
     *  setFantasyFontFamily 
     */
    virtual void setFantasyFontFamily(WebCore::String family);

    /**
     *  defaultFontSize 
     */
    virtual int defaultFontSize();

    /**
     *  setDefaultFontSize 
     */
    virtual void setDefaultFontSize(int fontSize);

    /**
     *  defaultFixedFontSize 
     */
    virtual int defaultFixedFontSize();

    /**
     *  setDefaultFixedFontSize 
     */
    virtual void setDefaultFixedFontSize(int fontSize);

    /**
     *  minimumFontSize
     */
    virtual int minimumFontSize();

    /**
     *  setMinimumFontSize 
     */
    virtual void setMinimumFontSize(int fontSize);

    /**
     *  minimumLogicalFontSize 
     */
    virtual int minimumLogicalFontSize();

    /**
     *  setMinimumLogicalFontSize 
     */
    virtual void setMinimumLogicalFontSize(int fontSize);

    /**
     *  defaultTextEncodingName 
     */
    virtual WebCore::String defaultTextEncodingName();

    /**
     *  setDefaultTextEncodingName 
     */
    virtual void setDefaultTextEncodingName(WebCore::String name);

    /**
     *  userStyleSheetEnabled 
     */
    virtual bool userStyleSheetEnabled();

    /**
     *  setUserStyleSheetEnabled 
     */
    virtual void setUserStyleSheetEnabled(bool enabled);

    /**
     *  userStyleSheetLocation 
     */
    virtual WebCore::String userStyleSheetLocation();

    /**
     *  setUserStyleSheetLocation 
     */
    virtual void setUserStyleSheetLocation(WebCore::String location);

    /**
     *  isJavaEnabled 
     */
    virtual bool isJavaEnabled();

    /**
     *  setJavaEnabled 
     */
    virtual void setJavaEnabled(bool enabled);

    /**
     *  isJavaScriptEnabled 
     */
    virtual bool isJavaScriptEnabled();

    /**
     *  setJavaScriptEnabled 
     */
    virtual void setJavaScriptEnabled(bool enabled);

    /**
     *  javaScriptCanOpenWindowsAutomatically 
     */
    virtual bool javaScriptCanOpenWindowsAutomatically();

    /**
     *  setJavaScriptCanOpenWindowsAutomatically 
     */
    virtual void setJavaScriptCanOpenWindowsAutomatically(bool enabled);

    /**
     *  arePlugInsEnabled 
     */
    virtual bool arePlugInsEnabled();

    /**
     *  setPlugInsEnabled 
     */
    virtual void setPlugInsEnabled(bool enabled);

    /**
     *  allowsAnimatedImages 
     */
    virtual bool allowsAnimatedImages();

    /**
     *  setAllowsAnimatedImages 
     */
    virtual void setAllowsAnimatedImages(bool enabled);

    /**
     *  allowAnimatedImageLooping 
     */
    virtual bool allowAnimatedImageLooping();

    /**
     *  setAllowAnimatedImageLooping 
     */
    virtual void setAllowAnimatedImageLooping(bool enabled);

    /**
     *  setLoadsImagesAutomatically 
     */
    virtual void setLoadsImagesAutomatically(bool enabled);

    /**
     *  loadsImagesAutomatically 
     */
    virtual bool loadsImagesAutomatically();

    /**
     *  setAutosaves
     */
    virtual void setAutosaves(bool enabled);

    /**
     *  autosaves 
     */
    virtual bool autosaves();

    /**
     *  setShouldPrintBackgrounds 
     */
    virtual void setShouldPrintBackgrounds(bool enabled);

    /**
     *  shouldPrintBackgrounds 
     */
    virtual bool shouldPrintBackgrounds();

    /**
     *  setPrivateBrowsingEnabled 
     */
    virtual void setPrivateBrowsingEnabled(bool enabled);

    /**
     *  privateBrowsingEnabled 
     */
    virtual bool privateBrowsingEnabled();

    /**
     *  setTabsToLinks 
     */
    virtual void setTabsToLinks(bool enabled);

    /**
     *  tabsToLinks 
     */
    virtual bool tabsToLinks();

    /**
     *  textAreasAreResizable 
     */
    virtual bool textAreasAreResizable();

    /**
     *  setTextAreasAreResizable 
     */
    virtual void setTextAreasAreResizable(bool enabled);

    /**
     *  usesPageCache
     */
    virtual bool usesPageCache();

    /**
     *  setUsesPageCache 
     */
    virtual void setUsesPageCache(bool usesPageCache);

    /**
     *  iconDatabaseLocation 
     */
    virtual WebCore::String iconDatabaseLocation();

    /**
     *  setIconDatabaseLocation 
     */
    virtual void setIconDatabaseLocation(WebCore::String location);

    /**
     *  iconDatabaseEnabled 
     */
    virtual bool iconDatabaseEnabled();

    /**
     *  setIconDatabaseEnabled 
     */
    virtual void setIconDatabaseEnabled(bool enabled);

    /**
     *  fontSmoothing
     */
    virtual FontSmoothingType fontSmoothing();

    /**
     *  setFontSmoothing 
     */
    virtual void setFontSmoothing(FontSmoothingType smoothingType);

    /**
     *  editableLinkBehavior 
     */
    virtual WebKitEditableLinkBehavior editableLinkBehavior();

    /**
     *  setEditableLinkBehavior 
     */
    virtual void setEditableLinkBehavior(WebKitEditableLinkBehavior behavior);

    /**
     *  cookieStorageAcceptPolicy 
     */
    virtual WebKitCookieStorageAcceptPolicy cookieStorageAcceptPolicy();

    /**
     *  setCookieStorageAcceptPolicy 
     */
    virtual void setCookieStorageAcceptPolicy(WebKitCookieStorageAcceptPolicy acceptPolicy);

    /**
     *  continuousSpellCheckingEnabled 
     */
    virtual bool continuousSpellCheckingEnabled();

    /**
     *  setContinuousSpellCheckingEnabled 
     */
    virtual void setContinuousSpellCheckingEnabled(bool enabled);

    /**
     *  grammarCheckingEnabled 
     */
    virtual bool grammarCheckingEnabled();

    /**
     *  setGrammarCheckingEnabled 
     */
    virtual void setGrammarCheckingEnabled(bool enabled);

    /**
     *  allowContinuousSpellChecking 
     */
    virtual bool allowContinuousSpellChecking();

    /**
     *  setAllowContinuousSpellChecking 
     */
    virtual void setAllowContinuousSpellChecking(bool enabled);

    /**
     *  isDOMPasteAllowed 
     */
    virtual bool isDOMPasteAllowed();

    /**
     *  setDOMPasteAllowed
     */
    virtual void setDOMPasteAllowed(bool enabled);

    /**
     *  cacheModel 
     */
    virtual WebCacheModel cacheModel();

    /**
     *  setCacheModel 
     */
    virtual void setCacheModel(WebCacheModel cacheModel);

    /**
     *  setShouldPaintCustomScrollbars
     */
    virtual void setShouldPaintCustomScrollbars(bool);

    /**
     * shouldPaintCustomScrollbars
     */
    virtual bool shouldPaintCustomScrollbars();

    // IWebPreferencesPrivate

    /**
     *  setDeveloperExtrasEnabled 
     */
    virtual void setDeveloperExtrasEnabled(bool);

    /**
     *  developerExtrasEnabled 
     */
    virtual bool developerExtrasEnabled();

    /**
     *  setAutomaticallyDetectsCacheModel 
     */
    virtual void setAutomaticallyDetectsCacheModel(bool automaticallyDetectsCacheModel);

    /**
     *  automaticallyDetectsCacheModel 
     */
    virtual bool automaticallyDetectsCacheModel();

    /**
     *  setAuthorAndUserStylesEnabled
     */
    virtual void setAuthorAndUserStylesEnabled(bool enabled);

    /**
     *  authorAndUserStylesEnabled 
     */
    virtual bool authorAndUserStylesEnabled();

    /**
     * Enable zoom for text only.
     */
    virtual void setZoomsTextOnly(bool zoomsTextOnly);

    /**
     * get zoom for text only status.
     */
    virtual bool zoomsTextOnly();

    /**
     * get allow universal access from File Urls status
     */
    virtual bool allowUniversalAccessFromFileURLs();

    /**
     * allow universal access from File Urls
     */
    virtual void setAllowUniversalAccessFromFileURLs(bool);

    // WebPreferences

    // This method accesses a different preference key than developerExtrasEnabled.
    // See <rdar://5343767> for the justification.

    /**
     *  developerExtrasDisabledByOverride 
     */
    bool developerExtrasDisabledByOverride();


    /**
     * get the topic to notify a change on webPreference
     */
    static WebCore::String webPreferencesChangedNotification();

    /**
     * get the topic to notify a remove on webPreference
     * @param[in]: 
     * @param[out]: 
     * @code
     * @endcode
     */
    static WebCore::String webPreferencesRemovedNotification();


    /**
     * set an instance of WebPreference
     */
    static void setInstance(WebPreferences* instance, WebCore::String identifier);

    /**
     * remove a instance of WebPreference for an identifier 
     */
    static void removeReferenceForIdentifier(WebCore::String identifier);

    /**
     * get shared standard preferences 
     */
    static WebPreferences* sharedStandardPreferences();

    // From WebHistory.h

    /**
     *  historyItemLimit 
     * The maximum number of items that will be stored by the WebHistory.
     */
    int historyItemLimit();

    /**
     * @discussion setHistoryAgeInDaysLimit: sets the maximum number of days to be read from
        stored history.
        @param limit The maximum number of days to be read from stored history.
     */
    void setHistoryItemLimit(int limit);

    /**
     *  historyAgeInDaysLimit 
     * The maximum number of items that will be stored by the WebHistory.
     */
    int historyAgeInDaysLimit();

    /**
     * @discussion setHistoryAgeInDaysLimit: sets the maximum number of days to be read from
        stored history.
        @param limit The maximum number of days to be read from stored history.
     */
    void setHistoryAgeInDaysLimit(int limit);


    /**
     * will add to webView 
     */
    void willAddToWebView();

    /**
     * did remove from webView 
     */
    void didRemoveFromWebView();


    /**
     * post preferences changes notification 
     */
    void postPreferencesChangesNotification();

protected:

    /**
     *  setValueForKey
     */
    void setValueForKey(WebCore::String key, WebCore::String value);

    /**
     *  valueForKey 
     */
    WebCore::String valueForKey(WebCore::String key);

    /**
     *  stringValueForKey 
     */
    WebCore::String stringValueForKey(WebCore::String key);

    /**
     *  integerValueForKey 
     */
    int integerValueForKey(WebCore::String key);

    /**
     *  boolValueForKey 
     */
    bool boolValueForKey(WebCore::String key);

    /**
     *  floatValueForKey
     */
    float floatValueForKey(WebCore::String key);

    /**
     *  longlongValueForKey 
     */
    unsigned int longlongValueForKey(WebCore::String key);

    /**
     *  setStringValue
     */
    void setStringValue(WebCore::String key, WebCore::String value);

    /**
     *  setIntegerValue 
     */
    void setIntegerValue(WebCore::String key, int value);

    /**
     *  setBoolValue 
     */
    void setBoolValue(WebCore::String key, bool value);

    /**
     *  setLongLongValue 
     */
    void setLongLongValue(WebCore::String key, unsigned int value);

    /**
     *  getInstanceForIdentifier 
     */
    static WebPreferences* getInstanceForIdentifier(WebCore::String identifier);

    /**
     *  initializeDefaultSettings 
     */
    static void initializeDefaultSettings();

    /**
     *  save 
     */
    void save();

    /**
     *  load 
     */
    void load();

protected:
    WebCore::String m_identifier;
    bool m_autoSaves;
    bool m_automaticallyDetectsCacheModel;
    unsigned m_numWebViews;
};

#endif
