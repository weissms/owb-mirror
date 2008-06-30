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

#ifndef WebView_H
#define WebView_H


/**
 *  @file  WebView.h
 *  WebView description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"
#include "WebFrame.h"
#include "WebFrame.h"
#include "WebPreferences.h"

#include <IntRect.h>
#include <Timer.h>
#include <BCObserverData.h>
#include <wtf/OwnPtr.h>
#include "GraphicsContext.h"
#include "BALBase.h"

class WebFrame;
class WebBackForwardList;
class WebInspector;
class WebInspectorClient;
class WebViewPrivate;
class DefaultDownloadDelegate;
class DefaultPolicyDelegate;
class WebHistoryItem;
class WebScriptObject;
class WebElementPropertyBag;


    /**
     *  kit description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
WebView* kit(WebCore::Page*);

    /**
     *  core description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
WebCore::Page* core(WebView*);

namespace WebCore {
    class String;
    class IntRect;
    class Image;
    class IntPoint;
}


#define WebViewProgressStartedNotification "WebProgressStartedNotification"
#define WebViewProgressEstimateChangedNotification "WebProgressEstimateChangedNotification"
#define WebViewProgressFinishedNotification "WebProgressFinishedNotification"

class MouseEventPrivate;

class WebView 
    : public OWBAL::BCObserverData
{
public:

    /**
     *  createInstance description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    static WebView* createInstance();

    /**
     *  ~WebView description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual ~WebView();
protected:

    /**
     *  WebView description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    WebView();

public:

    /**
     *  canShowMIMEType description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool canShowMIMEType(WebCore::String mimeType);

    /**
     *  canShowMIMETypeAsHTML description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool canShowMIMETypeAsHTML(WebCore::String mimeType);

    /**
     *  setMIMETypesShownAsHTML description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setMIMETypesShownAsHTML(WebCore::String mimeTypes, int cMimeTypes);

    /**
     *  initWithFrame description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void initWithFrame(WebCore::IntRect frame, WebCore::String frameName, WebCore::String groupName);

    /**
     *  setDownloadDelegate description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setDownloadDelegate(DefaultDownloadDelegate *d);

    /**
     *  *downloadDelegate description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual DefaultDownloadDelegate *downloadDelegate();

    /**
     *  setPolicyDelegate description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setPolicyDelegate(DefaultPolicyDelegate *d);

    /**
     *  *policyDelegate description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual DefaultPolicyDelegate *policyDelegate();

    /**
     *  *mainFrame description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebFrame *mainFrame();

    /**
     *  *focusedFrame description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebFrame *focusedFrame();

    /**
     *  *backForwardList description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebBackForwardList *backForwardList();

    /**
     *  setMaintainsBackForwardList description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setMaintainsBackForwardList(bool flag);

    /**
     *  goBack description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool goBack();

    /**
     *  goForward description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool goForward();

    /**
     *  goToBackForwardItem description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool goToBackForwardItem(WebHistoryItem *item);

    /**
     *  setTextSizeMultiplier description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setTextSizeMultiplier(float multiplier);

    /**
     *  textSizeMultiplier description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual float textSizeMultiplier();

    /**
     *  setApplicationNameForUserAgent description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setApplicationNameForUserAgent(WebCore::String applicationName);

    /**
     *  applicationNameForUserAgent description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String applicationNameForUserAgent();

    /**
     *  setCustomUserAgent description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setCustomUserAgent(WebCore::String userAgentString);

    /**
     *  customUserAgent description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String customUserAgent();

    /**
     *  userAgentForURL description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String userAgentForURL(WebCore::String url);

    /**
     *  supportsTextEncoding description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool supportsTextEncoding();

    /**
     *  setCustomTextEncodingName description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setCustomTextEncodingName(WebCore::String encodingName);

    /**
     *  customTextEncodingName description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String customTextEncodingName();

    /**
     *  setMediaStyle description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setMediaStyle(WebCore::String media);

    /**
     *  mediaStyle description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String mediaStyle();

    /**
     *  stringByEvaluatingJavaScriptFromString description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String stringByEvaluatingJavaScriptFromString(WebCore::String script);

    /**
     *  *windowScriptObject description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebScriptObject *windowScriptObject();

    /**
     *  setPreferences description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setPreferences(WebPreferences *prefs);

    /**
     *  *preferences description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebPreferences *preferences();

    /**
     *  setPreferencesIdentifier description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setPreferencesIdentifier(WebCore::String anIdentifier);

    /**
     *  preferencesIdentifier description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String preferencesIdentifier();

    /**
     *  setHostWindow description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //virtual void setHostWindow(BalWidget *window);

    /**
     *  *hostWindow description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //virtual BalWidget *hostWindow();

    /**
     *  searchFor description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool searchFor(WebCore::String str, bool forward, bool caseFlag, bool wrapFlag);

    /**
     *  setGroupName description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setGroupName(WebCore::String);

    /**
     *  groupName description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String groupName();

    /**
     *  estimatedProgress description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual double estimatedProgress();

    /**
     *  isLoading description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool isLoading();

    /**
     *  elementAtPoint description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebElementPropertyBag* elementAtPoint(WebCore::IntPoint point);

    /**
     *  selectedText description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String selectedText();

    /**
     *  centerSelectionInVisibleArea description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void centerSelectionInVisibleArea();

    /**
     *  moveDragCaretToPoint description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void moveDragCaretToPoint(WebCore::IntPoint point);

    /**
     *  removeDragCaret description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void removeDragCaret();

    /**
     *  setDrawsBackground description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setDrawsBackground(bool drawsBackground);

    /**
     *  drawsBackground description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool drawsBackground();

    /**
     *  setMainFrameURL description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setMainFrameURL(WebCore::String urlString);

    /**
     *  mainFrameURL description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String mainFrameURL();

    /**
     *  mainFrameDocument description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::Document* mainFrameDocument();

    /**
     *  mainFrameTitle description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::String mainFrameTitle();

    /**
     *  registerURLSchemeAsLocal description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void registerURLSchemeAsLocal(WebCore::String scheme);


    /**
     *  setSmartInsertDeleteEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setSmartInsertDeleteEnabled(bool flag);

    /**
     *  smartInsertDeleteEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool smartInsertDeleteEnabled();

    /**
     *  setContinuousSpellCheckingEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setContinuousSpellCheckingEnabled(bool flag);

    /**
     *  isContinuousSpellCheckingEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool isContinuousSpellCheckingEnabled();

    /**
     *  spellCheckerDocumentTag description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual int spellCheckerDocumentTag();

    /**
     *  *undoManager description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //virtual WebUndoManager *undoManager();
    

    /**
     *  setEditingDelegate description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //virtual void setEditingDelegate(WebEditingDelegate *d);

    /**
     *  editingDelegate description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //virtual WebEditingDelegate * editingDelegate();


    /**
     *  hasSelectedRange description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool hasSelectedRange();

    /**
     *  cutEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool cutEnabled();

    /**
     *  copyEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool copyEnabled();

    /**
     *  pasteEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool pasteEnabled();

    /**
     *  deleteEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool deleteEnabled();

    /**
     *  editingEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool editingEnabled();

    /**
     *  isGrammarCheckingEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool isGrammarCheckingEnabled();

    /**
     *  setGrammarCheckingEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setGrammarCheckingEnabled(bool enabled);

    /**
     *  setPageSizeMultiplier description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setPageSizeMultiplier(float multiplier);

    /**
     *  pageSizeMultiplier description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual float pageSizeMultiplier();


    /**
     *  canZoomPageIn description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool canZoomPageIn();

    /**
     *  zoomPageIn description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void zoomPageIn();

    /**
     *  canZoomPageOut description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool canZoomPageOut();

    /**
     *  zoomPageOut description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void zoomPageOut();

    /**
     *  canResetPageZoom description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool canResetPageZoom();

    /**
     *  resetPageZoom description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void resetPageZoom();


    /**
     *  canMakeTextLarger description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool canMakeTextLarger();

    /**
     *  makeTextLarger description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void makeTextLarger();

    /**
     *  canMakeTextSmaller description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool canMakeTextSmaller();

    /**
     *  makeTextSmaller description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void makeTextSmaller();

    /**
     *  canMakeTextStandardSize description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool canMakeTextStandardSize();

    /**
     *  makeTextStandardSize description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void makeTextStandardSize();


    /**
     *  replaceSelectionWithText description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void replaceSelectionWithText(WebCore::String text);

    /**
     *  replaceSelectionWithMarkupString description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void replaceSelectionWithMarkupString(WebCore::String markupString);

    /**
     *  replaceSelectionWithArchive description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void replaceSelectionWithArchive(WebArchive *archive);

    /**
     *  deleteSelection description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void deleteSelection();

    /**
     *  clearSelection description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void clearSelection();


    /**
     *  copy description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void copy();

    /**
     *  cut description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void cut();

    /**
     *  paste description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void paste();

    /**
     *  copyURL description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void copyURL(WebCore::String url);

    /**
     *  copyFont description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void copyFont();

    /**
     *  pasteFont description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void pasteFont();

    /**
     *  delete_ description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void delete_();

    /**
     *  pasteAsPlainText description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void pasteAsPlainText();

    /**
     *  pasteAsRichText description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void pasteAsRichText();

    /**
     *  changeFont description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void changeFont();

    /**
     *  changeAttributes description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void changeAttributes();

    /**
     *  changeDocumentBackgroundColor description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void changeDocumentBackgroundColor();

    /**
     *  changeColor description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void changeColor();

    /**
     *  alignCenter description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void alignCenter();

    /**
     *  alignJustified description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void alignJustified();

    /**
     *  alignLeft description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void alignLeft();

    /**
     *  alignRight description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void alignRight();

    /**
     *  checkSpelling description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void checkSpelling();

    /**
     *  showGuessPanel description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void showGuessPanel();

    /**
     *  performFindPanelAction description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void performFindPanelAction();

    /**
     *  startSpeaking description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void startSpeaking();

    /**
     *  stopSpeaking description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void stopSpeaking();


    /**
     *  observe description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void observe(const WebCore::String &topic, const WebCore::String &data, void *userData);


    /**
     *  setCustomDropTarget description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //virtual void setCustomDropTarget(DropTarget* dt);

    /**
     *  removeCustomDropTarget description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //virtual void removeCustomDropTarget();

    /**
     *  setInViewSourceMode description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setInViewSourceMode(bool flag);

    /**
     *  inViewSourceMode description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool inViewSourceMode();

    /**
     *  viewWindow description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual BalWidget* viewWindow();

    /**
     *  setViewWindow description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setViewWindow(BalWidget*);

    /**
     *  scrollOffset description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::IntPoint scrollOffset();

    /**
     *  scrollBy description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void scrollBy(WebCore::IntPoint offset);

    /**
     *  visibleContentRect description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::IntRect visibleContentRect();

    /**
     *  updateFocusedAndActiveState description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void updateFocusedAndActiveState();

    /**
     *  executeCoreCommandByName description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void executeCoreCommandByName(WebCore::String name, WebCore::String value);

    /**
     *  markAllMatchesForText description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual unsigned int markAllMatchesForText(WebCore::String search, bool caseSensitive, bool highlight, unsigned int limit);

    /**
     *  unmarkAllTextMatches description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void unmarkAllTextMatches();

    /**
     *  rectsForTextMatches description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual Vector<WebCore::IntRect> rectsForTextMatches();

    /**
     *  generateSelectionImage description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::Image* generateSelectionImage(bool forceWhiteText);

    /**
     *  selectionRect description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebCore::IntRect selectionRect();
    /*virtual  DragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);*
    virtual  DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
    virtual  DragLeave();
    virtual  Drop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);*/


    /**
     *  canHandleRequest description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool canHandleRequest(WebMutableURLRequest *request);

    /**
     *  clearFocusNode description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void clearFocusNode();

    /**
     *  setInitialFocus description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setInitialFocus(bool forward);

    /**
     *  setTabKeyCyclesThroughElements description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setTabKeyCyclesThroughElements(bool cycles);

    /**
     *  tabKeyCyclesThroughElements description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool tabKeyCyclesThroughElements();

    /**
     *  setAllowSiteSpecificHacks description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setAllowSiteSpecificHacks(bool allows);

    /**
     *  addAdditionalPluginDirectory description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void addAdditionalPluginDirectory(WebCore::String directory);

    /**
     *  loadBackForwardListFromOtherView description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void loadBackForwardListFromOtherView(WebView *otherView);

    /**
     *  inspector description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual WebInspector* inspector();

    /**
     *  clearUndoRedoOperations description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void clearUndoRedoOperations();

    /**
     *  shouldClose description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual bool shouldClose();

    /**
     *  setProhibitsMainFrameScrolling description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setProhibitsMainFrameScrolling(bool);

    /**
     *  setShouldApplyMacFontAscentHack description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void setShouldApplyMacFontAscentHack(bool);

    /**
     *  windowAncestryDidChange description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void windowAncestryDidChange();

    /**
     *  paintDocumentRectToContext description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void paintDocumentRectToContext(WebCore::IntRect rect, PlatformGraphicsContext *pgc);


    /**
     *  page description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    WebCore::Page* page();
    

    /**
     *  onExpose description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void onExpose(BalEventExpose);

    /**
     *  onKeyDown description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void onKeyDown(BalEventKey);

    /**
     *  onKeyUp description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void onKeyUp(BalEventKey);

    /**
     *  onMouseMotion description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void onMouseMotion(BalEventMotion);

    /**
     *  onMouseButtonDown description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void onMouseButtonDown(BalEventButton);

    /**
     *  onMouseButtonUp description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void onMouseButtonUp(BalEventButton);

    /**
     *  onScroll description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void onScroll(BalEventScroll);

    /**
     *  onResize description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void onResize(BalResizeEvent);

    /**
     *  onQuit description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void onQuit(BalQuitEvent);

    /**
     *  onUserEvent description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void onUserEvent(BalUserEvent);


    /**
     *  paint description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void paint();

    /**
     *  ensureBackingStore description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    bool ensureBackingStore();

    /**
     *  addToDirtyRegion description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void addToDirtyRegion(const WebCore::IntRect&);

    /**
     *  dirtyRegion description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    WebCore::IntRect dirtyRegion();

    /**
     *  clearDirtyRegion description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void clearDirtyRegion();

    /**
     *  scrollBackingStore description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void scrollBackingStore(WebCore::FrameView*, int dx, int dy, const WebCore::IntRect& scrollViewRect, const WebCore::IntRect& clipRect);

    /**
     *  updateBackingStore description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void updateBackingStore(WebCore::FrameView*);

    /**
     *  deleteBackingStore description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void deleteBackingStore();

    /**
     *  frameRect description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    WebCore::IntRect frameRect();

    /**
     *  closeWindow description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void closeWindow();

    /**
     *  closeWindowSoon description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void closeWindowSoon();

    /**
     *  close description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void close();

    /**
     * didClose description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    bool didClose() const { return m_didClose; }

    /**
     *  selectionChanged description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void selectionChanged();

    /**
     *  registerDragDrop description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //HRESULT registerDragDrop();

    /**
     *  revokeDragDrop description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //HRESULT revokeDragDrop();

    /**
     * topLevelFrame description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    // Convenient to be able to violate the rules of COM here for easy movement to the frame.
    WebFrame* topLevelFrame() const { return m_mainFrame; }

    /**
     *  userAgentForKURL description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    const WebCore::String& userAgentForKURL(const WebCore::KURL& url);


    /**
     *  canHandleRequest description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    static bool canHandleRequest(const WebCore::ResourceRequest&);

    /**
     * setIsBeingDestroyed description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void setIsBeingDestroyed() { m_isBeingDestroyed = true; }

    /**
     * isBeingDestroyed description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    bool isBeingDestroyed() const { return m_isBeingDestroyed; }


    /**
     *  interpretKeyEvent description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    const char* interpretKeyEvent(const WebCore::KeyboardEvent*);

    /**
     *  handleEditingKeyboardEvent description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    bool handleEditingKeyboardEvent(WebCore::KeyboardEvent*);

    /**
     * isPainting description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    bool isPainting() const { return m_paintCount > 0; }


    /**
     *  setToolTip description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void setToolTip(const WebCore::String&);

#if ENABLE(ICON_DATABASE)

    /**
     *  registerForIconNotification description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void registerForIconNotification(bool listen);

    /**
     *  dispatchDidReceiveIconFromWebFrame description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void dispatchDidReceiveIconFromWebFrame(WebFrame*);


    /**
     *  notifyDidAddIcon description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void notifyDidAddIcon();
#endif

    /**
     *  notifyPreferencesChanged description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void notifyPreferencesChanged(WebPreferences*);


    /**
     *  setCacheModel description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    static void setCacheModel(WebCacheModel);

    /**
     *  cacheModel description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    static WebCacheModel cacheModel();

    /**
     *  didSetCacheModel description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    static bool didSetCacheModel();

    /**
     *  maxCacheModelInAnyInstance description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    static WebCacheModel maxCacheModelInAnyInstance();

    /**
     * updateActiveStateSoon description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void updateActiveStateSoon() const;

    /**
     *  deleteBackingStoreSoon description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void deleteBackingStoreSoon();

    /**
     *  cancelDeleteBackingStoreSoon description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void cancelDeleteBackingStoreSoon();

    /**
     * topLevelParent description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    BalWidget* topLevelParent() const { return m_topLevelParent; }


    /**
     *  updateActiveState description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void updateActiveState();

    /**
     * parseConfigFile description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void parseConfigFile(WebCore::String url = "");

    //FIXME EventLoop
    //bool onGetObject(WPARAM, LPARAM, LRESULT&) const;

    /**
     *  AccessibleObjectFromWindow description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //static STDMETHODIMP AccessibleObjectFromWindow(HWND, DWORD objectID, REFIID, void** ppObject);

private:

    /**
     *  setZoomMultiplier description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void setZoomMultiplier(float multiplier, bool isTextOnly);

    /**
     *  zoomMultiplier description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    float zoomMultiplier(bool isTextOnly);

    /**
     *  canZoomIn description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    bool canZoomIn(bool isTextOnly);

    /**
     *  zoomIn description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void zoomIn(bool isTextOnly);

    /**
     *  canZoomOut description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    bool canZoomOut(bool isTextOnly);

    /**
     *  zoomOut description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void zoomOut(bool isTextOnly);

    /**
     *  canResetZoom description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    bool canResetZoom(bool isTextOnly);

    /**
     *  resetZoom description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void resetZoom(bool isTextOnly);

    /**
     *  active description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    bool active();

protected:

    /**
     *  getIMMContext description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //HIMC getIMMContext();

    /**
     *  releaseIMMContext description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //void releaseIMMContext(HIMC);

    /**
     * allowSiteSpecificHacks description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    static bool allowSiteSpecificHacks() { return s_allowSiteSpecificHacks; } 

    /**
     *  continuousCheckingAllowed description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    bool continuousCheckingAllowed();

    /**
     *  initializeToolTipWindow description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void initializeToolTipWindow();

    /**
     *  closeWindowTimerFired description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void closeWindowTimerFired(WebCore::Timer<WebView>*);

    /**
     *  prepareCandidateWindow description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //void prepareCandidateWindow(WebCore::Frame*, HIMC);

    /**
     *  updateSelectionForIME description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
//     void updateSelectionForIME();

    /**
     *  onIMERequestCharPosition description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //bool onIMERequestCharPosition(WebCore::Frame*, IMECHARPOSITION*, LRESULT*);

    /**
     *  onIMERequestReconvertString description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //bool onIMERequestReconvertString(WebCore::Frame*, RECONVERTSTRING*, LRESULT*);

    /**
     * developerExtrasEnabled description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    bool developerExtrasEnabled() const;

    // AllWebViewSet functions

    /**
     *  addToAllWebViewsSet description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void addToAllWebViewsSet();

    /**
     *  removeFromAllWebViewsSet description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    void removeFromAllWebViewsSet();


    /**
     *  windowReceivedMessage description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    //virtual void windowReceivedMessage(HWND, UINT message, WPARAM, LPARAM);

    BalWidget *m_viewWindow;
    WebFrame* m_mainFrame;
    WebCore::Page* m_page;
    
    OwnPtr<WebCore::Image> m_backingStoreBitmap;
    WebCore::IntPoint m_backingStoreSize;
    WebCore::IntRect m_backingStoreDirtyRegion;

    DefaultPolicyDelegate* m_policyDelegate;
    DefaultDownloadDelegate* m_downloadDelegate;
    WebPreferences* m_preferences;
    WebInspector* m_webInspector;

    bool m_userAgentOverridden;
    bool m_useBackForwardList;
    WebCore::String m_userAgentCustom;
    WebCore::String m_userAgentStandard;
    float m_zoomMultiplier;
    bool m_zoomMultiplierIsTextOnly;
    WebCore::String m_overrideEncoding;
    WebCore::String m_applicationName;
    bool m_mouseActivated;
    // WebCore dragging logic needs to be able to inspect the drag data
    // this is updated in DragEnter/Leave/Drop
    //COMPtr<IDataObject> m_dragData;
    //COMPtr<IDropTargetHelper> m_dropTargetHelper;
    //UChar m_currentCharacterCode;
    bool m_isBeingDestroyed;
    unsigned m_paintCount;
    bool m_hasSpellCheckerDocumentTag;
    bool m_smartInsertDeleteEnabled;
    bool m_didClose;
    bool m_hasCustomDropTarget;
    unsigned m_inIMEComposition;
    BalWidget* m_toolTipHwnd;
    WebCore::String m_toolTip;
    bool m_deleteBackingStoreTimerActive;

    static bool s_allowSiteSpecificHacks;

    WebCore::Timer<WebView> m_closeWindowTimer;
    //OwnPtr<TRACKMOUSEEVENT> m_mouseOutTracker;

    BalWidget* m_topLevelParent;
    WebViewPrivate *d;
};

#endif
