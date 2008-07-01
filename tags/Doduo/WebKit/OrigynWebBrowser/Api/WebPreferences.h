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
 *  WebPreferences description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"
#include "PlatformString.h"
#include "PlatformString.h"

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

typedef enum WebCacheModel {
    WebCacheModelDocumentViewer = 0,
    WebCacheModelDocumentBrowser = 1,
    WebCacheModelPrimaryWebBrowser = 2
} WebCacheModel;

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
     *  createInstance description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    static WebPreferences* createInstance();

    /**
     *  ~WebPreferences description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual ~WebPreferences();
protected:

    /**
     *  WebPreferences description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    WebPreferences();

public:
    // IWebPreferences

    /**
     *  standardPreferences description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebPreferences* standardPreferences();

    /**
     *  initWithIdentifier description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebPreferences* initWithIdentifier(WebCore::String anIdentifier);

    /**
     *  identifier description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String identifier();

    /**
     *  standardFontFamily description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String standardFontFamily();

    /**
     *  setStandardFontFamily description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setStandardFontFamily(WebCore::String family);

    /**
     *  fixedFontFamily description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String fixedFontFamily();

    /**
     *  setFixedFontFamily description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setFixedFontFamily(WebCore::String family);

    /**
     *  serifFontFamily description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String serifFontFamily();

    /**
     *  setSerifFontFamily description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setSerifFontFamily(WebCore::String family);

    /**
     *  sansSerifFontFamily description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String sansSerifFontFamily();

    /**
     *  setSansSerifFontFamily description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setSansSerifFontFamily(WebCore::String family);

    /**
     *  cursiveFontFamily description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String cursiveFontFamily();

    /**
     *  setCursiveFontFamily description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setCursiveFontFamily(WebCore::String family);

    /**
     *  fantasyFontFamily description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String fantasyFontFamily();

    /**
     *  setFantasyFontFamily description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setFantasyFontFamily(WebCore::String family);

    /**
     *  defaultFontSize description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual int defaultFontSize();

    /**
     *  setDefaultFontSize description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setDefaultFontSize(int fontSize);

    /**
     *  defaultFixedFontSize description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual int defaultFixedFontSize();

    /**
     *  setDefaultFixedFontSize description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setDefaultFixedFontSize(int fontSize);

    /**
     *  minimumFontSize description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual int minimumFontSize();

    /**
     *  setMinimumFontSize description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setMinimumFontSize(int fontSize);

    /**
     *  minimumLogicalFontSize description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual int minimumLogicalFontSize();

    /**
     *  setMinimumLogicalFontSize description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setMinimumLogicalFontSize(int fontSize);

    /**
     *  defaultTextEncodingName description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String defaultTextEncodingName();

    /**
     *  setDefaultTextEncodingName description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setDefaultTextEncodingName(WebCore::String name);

    /**
     *  userStyleSheetEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool userStyleSheetEnabled();

    /**
     *  setUserStyleSheetEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setUserStyleSheetEnabled(bool enabled);

    /**
     *  userStyleSheetLocation description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String userStyleSheetLocation();

    /**
     *  setUserStyleSheetLocation description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setUserStyleSheetLocation(WebCore::String location);

    /**
     *  isJavaEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool isJavaEnabled();

    /**
     *  setJavaEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setJavaEnabled(bool enabled);

    /**
     *  isJavaScriptEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool isJavaScriptEnabled();

    /**
     *  setJavaScriptEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setJavaScriptEnabled(bool enabled);

    /**
     *  javaScriptCanOpenWindowsAutomatically description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool javaScriptCanOpenWindowsAutomatically();

    /**
     *  setJavaScriptCanOpenWindowsAutomatically description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setJavaScriptCanOpenWindowsAutomatically(bool enabled);

    /**
     *  arePlugInsEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool arePlugInsEnabled();

    /**
     *  setPlugInsEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setPlugInsEnabled(bool enabled);

    /**
     *  allowsAnimatedImages description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool allowsAnimatedImages();

    /**
     *  setAllowsAnimatedImages description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setAllowsAnimatedImages(bool enabled);

    /**
     *  allowAnimatedImageLooping description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool allowAnimatedImageLooping();

    /**
     *  setAllowAnimatedImageLooping description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setAllowAnimatedImageLooping(bool enabled);

    /**
     *  setLoadsImagesAutomatically description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setLoadsImagesAutomatically(bool enabled);

    /**
     *  loadsImagesAutomatically description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool loadsImagesAutomatically();

    /**
     *  setAutosaves description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setAutosaves(bool enabled);

    /**
     *  autosaves description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool autosaves();

    /**
     *  setShouldPrintBackgrounds description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setShouldPrintBackgrounds(bool enabled);

    /**
     *  shouldPrintBackgrounds description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool shouldPrintBackgrounds();

    /**
     *  setPrivateBrowsingEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setPrivateBrowsingEnabled(bool enabled);

    /**
     *  privateBrowsingEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool privateBrowsingEnabled();

    /**
     *  setTabsToLinks description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setTabsToLinks(bool enabled);

    /**
     *  tabsToLinks description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool tabsToLinks();

    /**
     *  textAreasAreResizable description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool textAreasAreResizable();

    /**
     *  setTextAreasAreResizable description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setTextAreasAreResizable(bool enabled);

    /**
     *  usesPageCache description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool usesPageCache();

    /**
     *  setUsesPageCache description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setUsesPageCache(bool usesPageCache);

    /**
     *  iconDatabaseLocation description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String iconDatabaseLocation();

    /**
     *  setIconDatabaseLocation description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setIconDatabaseLocation(WebCore::String location);

    /**
     *  iconDatabaseEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool iconDatabaseEnabled();

    /**
     *  setIconDatabaseEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setIconDatabaseEnabled(bool enabled);

    /**
     *  fontSmoothing description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual FontSmoothingType fontSmoothing();

    /**
     *  setFontSmoothing description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setFontSmoothing(FontSmoothingType smoothingType);

    /**
     *  editableLinkBehavior description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebKitEditableLinkBehavior editableLinkBehavior();

    /**
     *  setEditableLinkBehavior description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setEditableLinkBehavior(WebKitEditableLinkBehavior behavior);

    /**
     *  cookieStorageAcceptPolicy description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebKitCookieStorageAcceptPolicy cookieStorageAcceptPolicy();

    /**
     *  setCookieStorageAcceptPolicy description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setCookieStorageAcceptPolicy(WebKitCookieStorageAcceptPolicy acceptPolicy);

    /**
     *  continuousSpellCheckingEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool continuousSpellCheckingEnabled();

    /**
     *  setContinuousSpellCheckingEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setContinuousSpellCheckingEnabled(bool enabled);

    /**
     *  grammarCheckingEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool grammarCheckingEnabled();

    /**
     *  setGrammarCheckingEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setGrammarCheckingEnabled(bool enabled);

    /**
     *  allowContinuousSpellChecking description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool allowContinuousSpellChecking();

    /**
     *  setAllowContinuousSpellChecking description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setAllowContinuousSpellChecking(bool enabled);

    /**
     *  isDOMPasteAllowed description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool isDOMPasteAllowed();

    /**
     *  setDOMPasteAllowed description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setDOMPasteAllowed(bool enabled);

    /**
     *  cacheModel description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCacheModel cacheModel();

    /**
     *  setCacheModel description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setCacheModel(WebCacheModel cacheModel);

    // IWebPreferencesPrivate

    /**
     *  setDeveloperExtrasEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setDeveloperExtrasEnabled(bool);

    /**
     *  developerExtrasEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool developerExtrasEnabled();

    /**
     *  setAutomaticallyDetectsCacheModel description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setAutomaticallyDetectsCacheModel(bool automaticallyDetectsCacheModel);

    /**
     *  automaticallyDetectsCacheModel description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool automaticallyDetectsCacheModel();

    /**
     *  setAuthorAndUserStylesEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setAuthorAndUserStylesEnabled(bool enabled);

    /**
     *  authorAndUserStylesEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool authorAndUserStylesEnabled();

    // WebPreferences

    // This method accesses a different preference key than developerExtrasEnabled.
    // See <rdar://5343767> for the justification.

    /**
     *  developerExtrasDisabledByOverride description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    bool developerExtrasDisabledByOverride();


    /**
     *  webPreferencesChangedNotification description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    static WebCore::String webPreferencesChangedNotification();

    /**
     *  webPreferencesRemovedNotification description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    static WebCore::String webPreferencesRemovedNotification();


    /**
     *  setInstance description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    static void setInstance(WebPreferences* instance, WebCore::String identifier);

    /**
     *  removeReferenceForIdentifier description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    static void removeReferenceForIdentifier(WebCore::String identifier);

    /**
     *  sharedStandardPreferences description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    static WebPreferences* sharedStandardPreferences();

    // From WebHistory.h

    /**
     *  historyItemLimit description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    int historyItemLimit();

    /**
     *  setHistoryItemLimit description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void setHistoryItemLimit(int limit);

    /**
     *  historyAgeInDaysLimit description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    int historyAgeInDaysLimit();

    /**
     *  setHistoryAgeInDaysLimit description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void setHistoryAgeInDaysLimit(int limit);


    /**
     *  willAddToWebView description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void willAddToWebView();

    /**
     *  didRemoveFromWebView description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void didRemoveFromWebView();


    /**
     *  postPreferencesChangesNotification description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void postPreferencesChangesNotification();

protected:

    /**
     *  setValueForKey description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void setValueForKey(WebCore::String key, WebCore::String value);

    /**
     *  valueForKey description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    WebCore::String valueForKey(WebCore::String key);

    /**
     *  stringValueForKey description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    WebCore::String stringValueForKey(WebCore::String key);

    /**
     *  integerValueForKey description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    int integerValueForKey(WebCore::String key);

    /**
     *  boolValueForKey description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    bool boolValueForKey(WebCore::String key);

    /**
     *  floatValueForKey description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    float floatValueForKey(WebCore::String key);

    /**
     *  longlongValueForKey description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    unsigned int longlongValueForKey(WebCore::String key);

    /**
     *  setStringValue description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void setStringValue(WebCore::String key, WebCore::String value);

    /**
     *  setIntegerValue description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void setIntegerValue(WebCore::String key, int value);

    /**
     *  setBoolValue description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void setBoolValue(WebCore::String key, bool value);

    /**
     *  setLongLongValue description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void setLongLongValue(WebCore::String key, unsigned int value);

    /**
     *  getInstanceForIdentifier description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    static WebPreferences* getInstanceForIdentifier(WebCore::String identifier);

    /**
     *  initializeDefaultSettings description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    static void initializeDefaultSettings();

    /**
     *  save description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void save();

    /**
     *  load description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void load();

protected:
    WebCore::String m_identifier;
    bool m_autoSaves;
    bool m_automaticallyDetectsCacheModel;
    unsigned m_numWebViews;
};

#endif
