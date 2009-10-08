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

#include "config.h"

#include "LocalizedStrings.h"
#include "PlatformString.h"


namespace OWBAL {


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
    return String::fromUTF8("(None)");
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
    return String::fromUTF8("Copy Link Location");
}

String contextMenuItemTagOpenImageInNewWindow()
{
    return String::fromUTF8("Open Image in New Window");
}

String contextMenuItemTagDownloadImageToDisk()
{
    return String::fromUTF8("Save Image As");
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
    static String stockLabel = String::fromUTF8("copy");
    return stockLabel;
}

String contextMenuItemTagDelete()
{
    static String stockLabel = String::fromUTF8("delete");
    return stockLabel;
}

String contextMenuItemTagSelectAll()
{
    static String stockLabel = String::fromUTF8("Select All");
    return stockLabel;
}

String contextMenuItemTagUnicode()
{
    return String::fromUTF8("Insert Unicode Control Character");
}

String contextMenuItemTagInputMethods()
{
    return String::fromUTF8("Input Methods");
}

String contextMenuItemTagGoBack()
{
    static String stockLabel = String::fromUTF8("back");
    return stockLabel;
}

String contextMenuItemTagGoForward()
{
    static String stockLabel = String::fromUTF8("forward");
    return stockLabel;
}

String contextMenuItemTagStop()
{
    static String stockLabel = String::fromUTF8("stop");
    return stockLabel;
}

String contextMenuItemTagReload()
{
    return String::fromUTF8("Reload");
}

String contextMenuItemTagCut()
{
    static String stockLabel = String::fromUTF8("cut");
    return stockLabel;
}

String contextMenuItemTagPaste()
{
    static String stockLabel = String::fromUTF8("paste");
    return stockLabel;
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
    return String::fromUTF8("Search the Web");
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
    static String stockLabel = String::fromUTF8("Bold");;
    return stockLabel;
}

String contextMenuItemTagItalic()
{
    static String stockLabel = String::fromUTF8("Italic");
    return stockLabel;
}

String contextMenuItemTagUnderline()
{
    static String stockLabel = String::fromUTF8("Underline");
    return stockLabel;
}

String contextMenuItemTagOutline()
{
    return String::fromUTF8("Outline");
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

String AXDefinitionListTermText()
{
    return String::fromUTF8("term");
}

String AXDefinitionListDefinitionText()
{
    return String::fromUTF8("definition");
}

String AXButtonActionVerb()
{
    return String();
}

String AXRadioButtonActionVerb()
{
    return String();
}

String AXTextFieldActionVerb()
{
    return String();
}

String AXCheckedCheckBoxActionVerb()
{
    return String();
}

String AXUncheckedCheckBoxActionVerb()
{
    return String();
}

String AXLinkActionVerb()
{
    return String();
}

String unknownFileSizeText()
{
    return String::fromUTF8("Unknown");
}

String imageTitle(const String& filename, const IntSize& size)
{
    return String();
}

String contextMenuItemTagLeftToRight()
{
    return String::fromUTF8("Left to Right context menu item");
}

String contextMenuItemTagDefaultDirection()
{
    return String::fromUTF8("Default writing direction context menu item");
}

String contextMenuItemTagRightToLeft()
{
    return String::fromUTF8("Right to Left context menu item");
}

String contextMenuItemTagWritingDirectionMenu()
{
    return String::fromUTF8("Writing direction context sub-menu item");
}

String contextMenuItemTagTextDirectionMenu()
{
    return String::fromUTF8("Text direction context sub-menu item");
}

String multipleFileUploadText(unsigned numberOfFiles)
{
    return String();
}

}
