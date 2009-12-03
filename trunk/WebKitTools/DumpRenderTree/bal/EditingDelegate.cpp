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

#include "EditingDelegate.h"

#include "DumpRenderTree.h"
#include "LayoutTestController.h"
#include <string>

using std::string;

TransferSharedPtr<EditingDelegate> EditingDelegate::createInstance()
{
    return new EditingDelegate();
}

EditingDelegate::EditingDelegate()
    : m_acceptsEditing(true)
{
}

EditingDelegate::~EditingDelegate()
{
}

static const char* dump(DOMRange* range)
{
    if (!range)
        return 0;

    int startOffset = range->startOffset();
    
    int endOffset = range->endOffset();

    DOMNode* startContainer = range->startContainer();

    DOMNode* endContainer = range->endContainer();
    if (startContainer && endContainer) {
        char buffer[1024];
        sprintf(buffer, "range from %d of %s to %d of %s", startOffset, dumpPath(startContainer).c_str(), endOffset, dumpPath(endContainer).c_str());
        string buf = buffer;
        return strdup(buf.c_str());
    }
    return 0;
}

bool EditingDelegate::shouldBeginEditingInDOMRange(WebView* webView, DOMRange* range)
{
    if (::gLayoutTestController->dumpEditingCallbacks() && !done)
        printf("EDITING DELEGATE: shouldBeginEditingInDOMRange:%s\n", dump(range));
    return m_acceptsEditing;
}

bool EditingDelegate::shouldEndEditingInDOMRange(WebView* webView, DOMRange* range)
{
    if (::gLayoutTestController->dumpEditingCallbacks() && !done)
        printf("EDITING DELEGATE: shouldEndEditingInDOMRange:%s\n", dump(range));
    return m_acceptsEditing;
}

bool EditingDelegate::shouldInsertNode(WebView* webView, DOMNode* node, DOMRange* range, WebViewInsertAction action)
{
    static const char* insertactionstring[] = {
        "WebViewInsertActionTyped",
        "WebViewInsertActionPasted",
        "WebViewInsertActionDropped",
    };

    if (::gLayoutTestController->dumpEditingCallbacks() && !done)
        printf("EDITING DELEGATE: shouldInsertNode:%s replacingDOMRange:%s givenAction:%s\n", dumpPath(node).c_str(), dump(range), insertactionstring[action]);
    return m_acceptsEditing;
}

bool EditingDelegate::shouldInsertText(WebView* webView, const char* text, DOMRange* range, WebViewInsertAction action)
{
    static const char* insertactionstring[] = {
        "WebViewInsertActionTyped",
        "WebViewInsertActionPasted",
        "WebViewInsertActionDropped",
    };

    if (::gLayoutTestController->dumpEditingCallbacks() && !done)
        printf("EDITING DELEGATE: shouldInsertText:%s replacingDOMRange:%s givenAction:%s\n", text ? text : "", dump(range), insertactionstring[action]);
    return m_acceptsEditing;
}

bool EditingDelegate::shouldDeleteDOMRange(WebView* webView, DOMRange* range)
{
    if (::gLayoutTestController->dumpEditingCallbacks() && !done)
        printf("EDITING DELEGATE: shouldDeleteDOMRange:%s\n", dump(range));
    return m_acceptsEditing;
}

bool EditingDelegate::shouldChangeSelectedDOMRange(WebView* webView, DOMRange* currentRange, DOMRange* proposedRange, WebSelectionAffinity selectionAffinity, bool stillSelecting)
{
    static const char* affinitystring[] = {
        "NSSelectionAffinityUpstream",
        "NSSelectionAffinityDownstream"
    };
    static const char* boolstring[] = {
        "FALSE",
        "TRUE"
    };

    if (::gLayoutTestController->dumpEditingCallbacks() && !done)
        printf("EDITING DELEGATE: shouldChangeSelectedDOMRange:%s toDOMRange:%s affinity:%s stillSelecting:%s\n", dump(currentRange), dump(proposedRange), affinitystring[selectionAffinity], boolstring[stillSelecting]);
    return m_acceptsEditing;
}

bool EditingDelegate::shouldApplyStyle(WebView* webView, DOMCSSStyleDeclaration* style, DOMRange* range)
{
    if (::gLayoutTestController->dumpEditingCallbacks() && !done)
        printf("EDITING DELEGATE: shouldApplyStyle:%s toElementsInDOMRange:%s\n", "'style description'", dump(range));
    return m_acceptsEditing;
}

bool EditingDelegate::shouldChangeTypingStyle(WebView* webView, DOMCSSStyleDeclaration* currentStyle, DOMCSSStyleDeclaration* proposedStyle)
{
    if (::gLayoutTestController->dumpEditingCallbacks() && !done)
        printf("EDITING DELEGATE: shouldChangeTypingStyle:%s toStyle:%s\n", "'currentStyle description'", "'proposedStyle description'");
    return m_acceptsEditing;
}

bool EditingDelegate::doPlatformCommand(WebView *webView, const char* command)
{
    if (::gLayoutTestController->dumpEditingCallbacks() && !done)
        printf("EDITING DELEGATE: doPlatformCommand:%s\n", command ? command : "");
    return m_acceptsEditing;
}

void EditingDelegate::webViewDidBeginEditing()
{
    if (::gLayoutTestController->dumpEditingCallbacks() && !done)
        printf("EDITING DELEGATE: webViewDidBeginEditing:WebViewDidBeginEditingNotification\n");
}

void EditingDelegate::webViewDidChange()
{
    if (::gLayoutTestController->dumpEditingCallbacks() && !done)
        printf("EDITING DELEGATE: webViewDidChange:WebViewDidChangeNotification\n");
}

void EditingDelegate::webViewDidEndEditing()
{
    if (::gLayoutTestController->dumpEditingCallbacks() && !done)
        printf("EDITING DELEGATE: webViewDidEndEditing:WebViewDidEndEditingNotification\n");
}

void EditingDelegate::webViewDidChangeTypingStyle()
{
    if (::gLayoutTestController->dumpEditingCallbacks() && !done)
        printf("EDITING DELEGATE: webViewDidChangeTypingStyle:WebViewDidChangeTypingStyleNotification\n");
}

void EditingDelegate::webViewDidChangeSelection()
{
    if (::gLayoutTestController->dumpEditingCallbacks() && !done)
        printf("EDITING DELEGATE: webViewDidChangeSelection:WebViewDidChangeSelectionNotification\n");
}
