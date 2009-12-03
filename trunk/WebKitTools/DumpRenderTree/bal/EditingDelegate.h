/*
 * Copyright (C) 2009 Pleyo.  All rights reserved.
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

#ifndef EditingDelegate_h
#define EditingDelegate_h

#include <WebKit.h>

class EditingDelegate : public WebEditingDelegate {
public:
    static TransferSharedPtr<EditingDelegate> createInstance();
    virtual ~EditingDelegate();

    void setAcceptsEditing(bool b) { m_acceptsEditing = b; }

    virtual bool shouldBeginEditingInDOMRange(WebView *webView, DOMRange *range);
    
    virtual bool shouldEndEditingInDOMRange(WebView *webView, DOMRange *range);
    
    virtual bool shouldInsertNode(WebView *webView, DOMNode *node, DOMRange *range, WebViewInsertAction action);
    
    virtual bool shouldInsertText(WebView *webView, const char* text, DOMRange *range, WebViewInsertAction action);
    
    virtual bool shouldDeleteDOMRange(WebView *webView, DOMRange *range);
    
    virtual bool shouldChangeSelectedDOMRange(WebView *webView, DOMRange *currentRange, DOMRange *proposedRange, WebSelectionAffinity selectionAffinity, bool stillSelecting);
    
    virtual bool shouldApplyStyle(WebView *webView, DOMCSSStyleDeclaration *style, DOMRange *range);
    
    virtual bool shouldChangeTypingStyle(WebView *webView, DOMCSSStyleDeclaration *currentStyle, DOMCSSStyleDeclaration *proposedStyle);
    
    virtual bool doPlatformCommand(WebView *webView, const char*);
    
    virtual void webViewDidBeginEditing();
    
    virtual void webViewDidChange();
    
    virtual void webViewDidEndEditing();
    
    virtual void webViewDidChangeTypingStyle();
    
    virtual void webViewDidChangeSelection();
    
    //virtual WebUndoManager* undoManagerForWebView(WebView *webView) { return E_NOTIMPL; }

    virtual void ignoreWordInSpellDocument(WebView *view, const char* word) {}
        
    virtual void learnWord(const char* word) {}
        
    virtual void checkSpellingOfString(WebView *view, const char* text, int length, int *misspellingLocation, int *misspellingLength) {}
        
    //virtual void checkGrammarOfString(WebView *view, const char* text, int length, EnumWebGrammarDetails **grammarDetails, int *badGrammarLocation, int *badGrammarLength) {}
        
    virtual void updateSpellingUIWithGrammarString(const char* string, int location, int length, const char* userDescription, char** guesses, int guessesCount) {}
        
    virtual void updateSpellingUIWithMisspelledWord(const char* word) {}
        
    virtual void showSpellingUI(bool show) {}
        
    virtual bool spellingUIIsShowing() { return false;}
        
    //virtual EnumSpellingGuesses* guessesForWord(const char* word) { return 0;}

private:
    EditingDelegate();

    bool m_acceptsEditing;
};

#endif // !defined(EditingDelegate_h)
