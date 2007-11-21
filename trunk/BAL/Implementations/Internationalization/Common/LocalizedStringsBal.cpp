/*
 * Copyright (C) 2006, 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com 
 * Copyright (C) 2007 Holger Hans Peter Freyther
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"

#include "LocalizedStrings.h"
#include "PlatformString.h"

namespace WebCore {

String defaultLanguage()
{
    return "en";
}

String submitButtonDefaultLabel()
{
    return String::fromUTF8("Submit");
}

String inputElementAltText()
{ 
    return String::fromUTF8("Submit");
}

String resetButtonDefaultLabel()
{ 
    return String::fromUTF8("Reset");
}

String searchableIndexIntroduction()
{
    return String::fromUTF8("Searchable Index");
}

String fileButtonChooseFileLabel()
{
    return String::fromUTF8("Choose File");
}

String fileButtonNoFileSelectedLabel()
{
    return String::fromUTF8("No file selected");
}

String contextMenuItemTagOpenLinkInNewWindow()
{
    return String::fromUTF8("Open Link in New Window");
}

String contextMenuItemTagDownloadLinkToDisk()
{
    return String::fromUTF8("Download Linked File");
}

String contextMenuItemTagCopyLinkToClipboard()
{
    return String::fromUTF8("Copy Link");
}

String contextMenuItemTagOpenImageInNewWindow()
{
    return String::fromUTF8("Open Image in New Window");
}

String contextMenuItemTagDownloadImageToDisk()
{
    return String::fromUTF8("Download Image");
}

String contextMenuItemTagCopyImageToClipboard()
{
    return String::fromUTF8("Copy Image");
}

String contextMenuItemTagOpenFrameInNewWindow()
{
    return String::fromUTF8("Open Frame in New Window");
}

String contextMenuItemTagCopy()
{
    return String::fromUTF8("Copy");
}

String contextMenuItemTagGoBack()
{
    return String::fromUTF8("Back");
}

String contextMenuItemTagGoForward()
{
    return String::fromUTF8("Forward");
}

String contextMenuItemTagStop()
{
    return String::fromUTF8("Stop");
}

String contextMenuItemTagReload()
{
    return String::fromUTF8("Reload");
}

String contextMenuItemTagCut()
{
    return String::fromUTF8("Cut");
}

String contextMenuItemTagPaste()
{
    return String::fromUTF8("Paste");
}

String contextMenuItemTagNoGuessesFound()
{
    return String::fromUTF8("No Guesses Found");
}

String contextMenuItemTagIgnoreSpelling()
{
    return String::fromUTF8("Ignore Spelling");
}

String contextMenuItemTagLearnSpelling()
{
    return String::fromUTF8("Learn Spelling");
}

String contextMenuItemTagSearchWeb()
{
    return String::fromUTF8("Search with MSN");
}

String contextMenuItemTagLookUpInDictionary()
{
    return String::fromUTF8("Look Up in Dictionary");
}

String contextMenuItemTagOpenLink()
{
    return String::fromUTF8("Open Link");
}

String contextMenuItemTagIgnoreGrammar()
{
    return String::fromUTF8("Ignore Grammar");
}

String contextMenuItemTagSpellingMenu()
{
    return String::fromUTF8("Spelling and Grammar");
}

String contextMenuItemTagShowSpellingPanel(bool show)
{
    return String::fromUTF8(show ? "Show Spelling and Grammar" : "Hide Spelling and Grammar");
}

String contextMenuItemTagCheckSpelling()
{
    return String::fromUTF8("Check Document Now");
}

String contextMenuItemTagCheckSpellingWhileTyping()
{
    return String::fromUTF8("Check Spelling While Typing");
}

String contextMenuItemTagCheckGrammarWithSpelling()
{
    return String::fromUTF8("Check Grammar With Spelling");
}

String contextMenuItemTagFontMenu()
{
    return String::fromUTF8("Font");
}

String contextMenuItemTagBold()
{
    return String::fromUTF8("Bold");
}

String contextMenuItemTagItalic()
{
    return String::fromUTF8("Italic");
}

String contextMenuItemTagUnderline()
{
    return String::fromUTF8("Underline");
}

String contextMenuItemTagOutline()
{
    return String::fromUTF8("Outline");
}

String contextMenuItemTagWritingDirectionMenu()
{
    return String::fromUTF8("Write Direction");
}

String contextMenuItemTagDefaultDirection()
{
    return String::fromUTF8("Default");
}

String contextMenuItemTagLeftToRight()
{
    return String::fromUTF8("Left to Right");
}

String contextMenuItemTagRightToLeft()
{
    return String::fromUTF8("Right to Left");
}

String contextMenuItemTagInspectElement()
{
    return String::fromUTF8("Inspect Element");
}

String searchMenuNoRecentSearchesText()
{
    return String::fromUTF8("No recent searches");
}

String searchMenuRecentSearchesText()
{
    return String::fromUTF8("Recent searches");
}

String searchMenuClearRecentSearchesText()
{
    return String::fromUTF8("Clear recent searches");
}

String unknownFileSizeText()
{
    return String::fromUTF8("Unknown");
}

}
