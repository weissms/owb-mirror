/*
 * Copyright (C) 2009 Joerg Strohmayer.
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
#include "Pasteboard.h"

#include "CString.h"
#include "DocumentFragment.h"
#include "Frame.h"
#include "PlatformString.h"
#include "Image.h"
#include "Range.h"
#include "RenderImage.h"
#include "SelectionController.h"
#include "KURL.h"
#include "markup.h"
#include <proto/datatypes.h>
#include <proto/iffparse.h>
#include <proto/intuition.h>
#include <datatypes/textclass.h>
#include <datatypes/pictureclass.h>
#ifndef ID_CSET
    #define ID_CSET MAKE_ID('C','S','E','T')
#endif

struct IFFCodeSet
{
    uint32 CodeSet;
    uint32 Reserved[7];
};

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
//    printf("Pasteboard::generalPasteboard\n");
    static Pasteboard* pasteboard = new Pasteboard();
    return pasteboard;
}

Pasteboard::Pasteboard()
{
//    printf("Pasteboard::Pasteboard\n");
    NotImplemented();
}

Pasteboard::~Pasteboard()
{
    printf("Pasteboard::~Pasteboard\n");
    delete m_helper;
}

void Pasteboard::setHelper(PasteboardHelper* helper)
{
    printf("Pasteboard::setHelper\n");
    m_helper = helper;
}

static void writeUTF8(const char* data, size_t len)
{
    IFFHandle *ih;

    bool copied = false;

    if (data && len && (ih = IIFFParse->AllocIFF())) {
        if (ClipboardHandle *ch = IIFFParse->OpenClipboard(PRIMARY_CLIP)) {
            ih->iff_Stream = (uint32)ch;
            IIFFParse->InitIFFasClip(ih);

            if (0 == IIFFParse->OpenIFF(ih, IFFF_WRITE)) {
                if (0 == IIFFParse->PushChunk(ih, ID_FTXT, ID_FORM, IFFSIZE_UNKNOWN)) {
                    const IFFCodeSet cs = { 106, { 0, 0, 0, 0, 0, 0, 0 } };

                    if (0 == IIFFParse->PushChunk(ih, 0, ID_CSET, sizeof(cs))
                     && IIFFParse->WriteChunkBytes(ih, &cs, sizeof(cs)) > 0
                     && 0 == IIFFParse->PopChunk(ih)
                     && 0 == IIFFParse->PushChunk(ih, 0, ID_CHRS, IFFSIZE_UNKNOWN)) {
                        if (IIFFParse->WriteChunkBytes(ih, data, len) > 0)
                            copied = true;
                        IIFFParse->PopChunk(ih);
                    }
                    IIFFParse->PopChunk(ih);
                }
                IIFFParse->CloseIFF(ih);
            }
            IIFFParse->CloseClipboard(ch);
        }
        IIFFParse->FreeIFF(ih);
    }

    if (!copied)
        IIntuition->DisplayBeep(NULL);
}

void Pasteboard::writeSelection(Range* selectedRange, bool canSmartCopyOrDelete, Frame* frame)
{
    CString utf8 = frame->selectedText().utf8(); //selectedRange->text().utf8();
    const char *data = utf8.data();
    size_t len = utf8.length();

    writeUTF8(data, len);
}

void Pasteboard::writeURL(const KURL& url, const String&, Frame* frame)
{
    CString utf8 = url.prettyURL().utf8();
    const char *data = utf8.data();
    size_t len = utf8.length();

    writeUTF8(data, len);
}

void Pasteboard::writeImage(Node* node, const KURL&, const String& title)
{
    bool copied = false;

    RenderImage* renderer = static_cast<RenderImage*>(node->renderer());
    CachedImage* cachedImage = static_cast<CachedImage*>(renderer->cachedImage());
    WebCore::Image* image = cachedImage->image();

    Object* dto = IDataTypes->NewDTObject(NULL,
                                          DTA_SourceType, DTST_RAM,
                                          DTA_GroupID, GID_PICTURE,
                                          PDTA_DestMode, PMODE_V43,
                                          TAG_DONE);
    if (dto) {
        BitMapHeader *bmh;
        if (IDataTypes->GetDTAttrs(dto, PDTA_BitMapHeader, &bmh, TAG_DONE)) {
            bmh->bmh_Width = image->width();
            bmh->bmh_Height = image->height();
            bmh->bmh_Depth = 32;
            bmh->bmh_Masking = mskHasAlpha;

            IDataTypes->SetDTAttrs(dto, NULL, NULL,
                                   DTA_ObjName, title.utf8().data(),
                                   DTA_NominalHoriz, image->width(),
                                   DTA_NominalVert, image->height(),
                                   PDTA_SourceMode, PMODE_V43,
                                   TAG_DONE);

            cairo_surface_t* nativeImage = image->nativeImageForCurrentFrame();
            if (nativeImage)
                if (IIntuition->IDoMethod(dto, PDTM_WRITEPIXELARRAY,
                                          cairo_image_surface_get_data(nativeImage),
                                          PBPAFMT_ARGB,
                                          cairo_image_surface_get_stride(nativeImage),
                                          0, 0,
                                          image->width(), image->height()))
                    if (IDataTypes->DoDTMethod(dto, NULL, NULL, DTM_COPY, NULL))
                        copied = true;
        }
        IDataTypes->DisposeDTObject(dto);
    }

    if (!copied)
        IIntuition->DisplayBeep(NULL);
}

void Pasteboard::clear()
{
//    printf("Pasteboard::clear\n");
}

bool Pasteboard::canSmartReplace()
{
//    printf("Pasteboard::canSmartReplace\n");
    NotImplemented();
    return false;
}

PassRefPtr<DocumentFragment> Pasteboard::documentFragment(Frame* frame, PassRefPtr<Range> context,
                                                          bool allowPlainText, bool& chosePlainText)
{
    if (allowPlainText) {
        String text = plainText(frame);

        chosePlainText = true;
        RefPtr<DocumentFragment> fragment = createFragmentFromText(context.get(), text);
        if (fragment)
            return fragment.release();
    }

    printf("Pasteboard::documentFragment\n");
    return 0;
}

static void copycollection(String &str, CollectionItem* ci, uint32 codeSet)
{
    if (ci->ci_Next)
        copycollection(str, ci->ci_Next, codeSet);

    if (106 == codeSet) // UTF-8
        str.append(String().fromUTF8((const char *)ci->ci_Data, ci->ci_Size));
    else if (0 == codeSet || 3 == codeSet || 4 == codeSet) // US-ASCII or ISO-8859-1
        str.append(String((const char *)ci->ci_Data, ci->ci_Size));
    else { // unsupported charset
        STRPTR data = (STRPTR )ci->ci_Data;
        TEXT text[ci->ci_Size];

        for (int i = 0 ; i < ci->ci_Size ; i++)
            text[i] = data[i] <= 127 ? data[i] : '?';

        str.append(String(text, ci->ci_Size));
    }
}

String Pasteboard::plainText(Frame* frame)
{
    String result;

    if (IFFHandle *ih = IIFFParse->AllocIFF()) {
        if (ClipboardHandle *ch = IIFFParse->OpenClipboard(PRIMARY_CLIP)) {
            ih->iff_Stream = (uint32)ch;
            IIFFParse->InitIFFasClip(ih);

            if (0 == IIFFParse->OpenIFF(ih, IFFF_READ)) {
                const int32 chunks[4] = { ID_FTXT, ID_CHRS, ID_FTXT, ID_CSET };

                if (0 == IIFFParse->CollectionChunks(ih, chunks, 2)
                 && 0 == IIFFParse->StopOnExit(ih, ID_FTXT, ID_FORM))
                    while (true)
                        if (IFFERR_EOC == IIFFParse->ParseIFF(ih, IFFPARSE_SCAN)) {
                            IFFCodeSet cs;
                            CollectionItem *ci;

                            cs.CodeSet = 0;

                            ci = IIFFParse->FindCollection(ih, ID_FTXT, ID_CSET);
                            if (ci && sizeof(cs) == ci->ci_Size)
                                memcpy(&cs, ci->ci_Data, sizeof(cs));

                            ci = IIFFParse->FindCollection(ih, ID_FTXT, ID_CHRS);
                            if (ci)
                                copycollection(result, ci, cs.CodeSet);
                        }
                        else
                            break;

                IIFFParse->CloseIFF(ih);
            }
            IIFFParse->CloseClipboard(ch);
        }
        IIFFParse->FreeIFF(ih);
    }

    return result;
}

}
