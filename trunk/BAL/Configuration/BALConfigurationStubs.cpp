/*
 * Copyright (C) 2007 Pleyo.  All rights reserved.
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
 
 /**
 * @file BALConfigurationStubs.cpp
 *
 * Empty all the OWBAL implementation
 */
#include "BALConfiguration.h"

namespace BAL
{
    class BIImageDecoder;
    class BIEventLoop;
    class BIWindow;
    class FormData;

    // Decoders
    BIImageDecoder* createBCJPEGImageDecoder() { return 0; }
    BIImageDecoder* createBCPNGImageDecoder() { return 0; }
    BIImageDecoder* createBCGIFImageDecoder() { return 0; }
    void deleteBIImageDecoder( BIImageDecoder* ) { }

    // Events
    BIEventLoop* createBCEventLoop() { return 0; } //FIXME should start with "get", it's a singleton
    void deleteBIEventLoop( BIEventLoop* ) {}

    // Window
    BIWindow* createBCWindowBal(int x, int y, int width, int height, int depth) { return 0; }

    // Graphics
    class BIGraphicsContext;
    class BINativeImage;
    BIGraphicsContext* createRGBA32GraphicsContext(unsigned width, unsigned height) { return 0; }
    void deleteRGBA32GraphicsContext(BIGraphicsContext*){}
    BINativeImage* createBCNativeImage( const RGBA32Buffer&, const WebCore::IntSize& ) { return 0; }

    // ResourceHandleManager
    class BIResourceHandleManager;
    class BIResourceHandle;
    BIResourceHandleManager* getBIResourceHandleManager( ) { return 0; }
    // BIResourceHandle* createBCResourceHandle removed because of a static create

    // Fonts
    class BIFontCache;
    class BIFontData;
    class FontPlatformData;
    class BIGlyphBuffer;
    BIFontCache* getBIFontCache() { return 0; }
    BIGlyphBuffer* createBIGlyphBuffer() { return 0; }
    void deleteBIGlyphBuffer(BIGlyphBuffer*){}
    BIFontData* createBIFontData(const FontPlatformData& f) { return 0; }
    void deleteBIFontData(BIFontData* data) { }

    // Graphics
    class BIGraphicsContext;
    BIGraphicsContext* getBIGraphicsContext() { return 0; }
    BIGraphicsContext* createBIGraphicsContext() { return 0; }
    void deleteBIGraphicsContext(BIGraphicsContext*){}

    BIKeyboardEvent* createBIKeyboardEvent(  const WebCore::String& aText,
       const WebCore::String& aUnmodifiedText, const WebCore::String& aKeyIdentifier,
       bool bIsKeyUp, bool bShiftKey, bool bCtrlKey, bool bAltKey, bool bMetaKey, bool bIsAutoRepeat,
       int aVKey ) { return 0; }

}


#include "../Implementations/Widgets/Generic/WindowBal.h"
class BIPainter;
BAL::WindowBal::WindowBal(int, int, int, int, int) : m_timer(this, 0) {}
BAL::WindowBal::~WindowBal() {}
int BAL::WindowBal::x() {}
int BAL::WindowBal::y() {}
int BAL::WindowBal::width() {}
int BAL::WindowBal::height() {}
int BAL::WindowBal::depth() {}
void BAL::WindowBal::redraw(const WebCore::IntRect&) {}
void BAL::WindowBal::setPainter(BIPainter*) {}

#include "../Interfaces/TextField.h"
void WebCore::TextField::setAutoSaveName(WebCore::String const&) {}
void WebCore::TextField::setPlaceholderString(WebCore::String const&) {}
void WebCore::TextField::setEdited(bool) {}
void WebCore::TextField::setText(WebCore::String const&) {}
void WebCore::TextField::setWritingDirection(WebCore::TextDirection) {}
void WebCore::TextField::setMaxResults(int) {}
void WebCore::TextField::setReadOnly(bool) {}
void WebCore::TextField::selectAll() {}
void WebCore::TextField::addSearchResult() {}
WebCore::String WebCore::TextField::selectedText() const {}
void WebCore::TextField::setColors(WebCore::Color const&, WebCore::Color const&) {}
void WebCore::TextField::setAlignment(WebCore::HorizontalAlignment) {}
bool WebCore::TextField::edited() const {}
void WebCore::TextField::setLiveSearch(bool) {}
bool WebCore::TextField::hasSelectedText() const {}
int WebCore::TextField::cursorPosition() const {}
WebCore::IntSize WebCore::TextField::sizeForCharacterWidth(int) const {}
int WebCore::TextField::maxLength() const {}
void WebCore::TextField::setMaxLength(int) {}
void WebCore::TextField::setCursorPosition(int) {}
void WebCore::TextField::setSelection(int, int) {}
WebCore::String WebCore::TextField::text() const {}
int WebCore::TextField::selectionStart() const {}

#include "CharsetNames.h"

const char* WebCore::charsetNameFromTextEncodingID(WebCore::TextEncodingID) { return 0; }
WebCore::TextEncodingID WebCore::textEncodingIDFromCharsetName(char const*, WebCore::TextEncodingFlags*) { }

