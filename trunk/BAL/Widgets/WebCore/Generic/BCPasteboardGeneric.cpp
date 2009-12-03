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
#include "Logging.h"
#include "Pasteboard.h"

#include "CString.h"
#include "DocumentFragment.h"
#include "Frame.h"
#include "Image.h"
#include "KURL.h"
#include "NotImplemented.h"
#include "markup.h"
#include "PlatformString.h"
#include "Range.h"
#include "RenderImage.h"

#include <cstdio>

namespace WebCore {

/* FIXME: we must get rid of this and use the enum in webkitwebview.h someway */
typedef enum
{
    WEBKIT_WEB_VIEW_TARGET_INFO_HTML = - 1,
    WEBKIT_WEB_VIEW_TARGET_INFO_TEXT = - 2
} WebKitWebViewTargetInfo;

class PasteboardSelectionData {
public:
    PasteboardSelectionData(char* text, char* markup)
        : m_text(text)
        , m_markup(markup) { }

    ~PasteboardSelectionData() {
        free(m_text);
        free(m_markup);
    }

    const char* text() const { return m_text; }
    const char* markup() const { return m_markup; }

private:
    char* m_text;
    char* m_markup;
};

Pasteboard* Pasteboard::generalPasteboard()
{
    static Pasteboard* pasteboard = new Pasteboard();
    return pasteboard;
}

Pasteboard::Pasteboard()
: m_data(0)
{
    balNotImplemented();
}

Pasteboard::~Pasteboard()
{
    delete m_data;
}

void Pasteboard::writeSelection(Range* selectedRange, bool canSmartCopyOrDelete, Frame* frame)
{
    char* text = strdup(frame->selectedText().utf8().data());
    char* markup = strdup(createMarkup(selectedRange, 0, AnnotateForInterchange).utf8().data());
    if (m_data)
        delete m_data;
    m_data = new PasteboardSelectionData(text, markup);
}

void Pasteboard::writePlainText(const String&)
{
    balNotImplemented();
}

void Pasteboard::writeURL(const KURL& url, const String&, Frame* frame)
{

}

void Pasteboard::writeImage(Node* node, const KURL&, const String&)
{
    balNotImplemented();
}

void Pasteboard::clear()
{
}

bool Pasteboard::canSmartReplace()
{
    balNotImplemented();
    return false;
}

PassRefPtr<DocumentFragment> Pasteboard::documentFragment(Frame* frame, PassRefPtr<Range> context,
                                                          bool allowPlainText, bool& chosePlainText)
{
    if (!m_data)
        return 0;

    chosePlainText = false;
    String html = m_data->markup();
    if (!html.isEmpty()) {
        RefPtr<DocumentFragment> fragment = createFragmentFromMarkup(frame->document(), html, "");
        if (fragment)
            return fragment.release();
    }


    if (!allowPlainText)
        return 0;

    String text = String::fromUTF8(m_data->text());

    chosePlainText = true;
    RefPtr<DocumentFragment> fragment = createFragmentFromText(context.get(), text);
    if (fragment)
        return fragment.release();

    return 0;
}

String Pasteboard::plainText(Frame* frame)
{
    if (!m_data)
        return String();
    return String::fromUTF8(m_data->text());
}

}
