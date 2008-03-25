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
  * @file BALConfiguration.h
  * OWBAL configuration and object creation management
  *
  * Purpose of this file is to centralize creation of object.
  * All creation returns AN INTERFACE, and NOT a object.
  * This way, you can create an object without a knowledge of its implementation.
  *
  * Methods that starts with *create* create a new instance at each call.
  * Methods that starts with *get* create a singleton.
  *
  */
#ifndef BAL_CONFIGURATION_
#define BAL_CONFIGURATION_

#include <stdio.h>
#include <stdlib.h>

// must use log() to enable deactivation during LayoutTests
#ifdef BAL_LOG
  #define BALNotImplemented() if (!getenv("LAYOUT_TEST") && getenv("NOT_IMPLEMENTED")) { printf("%s:%d:%s() Not implemented\n", __FILE__, __LINE__, __func__); }
#else
  #define BALNotImplemented()
#endif

#include "RGBA32Array.h" // no forwarding due to typedef
#include "BIWindowEvent.h" // need to include because of BIWindowEvent::ENUM_WINDOW use

namespace WebCore {
    class FontDescription;
    class IntSize;
    class IntRect;
    class FormData;
    class KURL;
    class ResourceHandleClient;
    class String;
}

using WebCore::FormData;
using WebCore::KURL;
using WebCore::ResourceHandleClient;

namespace BAL
{
    class BIImageDecoder;
    class BIEventLoop;
    class BIWindow;
    class BTAffineTransform;

    // Decoders
    BIImageDecoder* createBCJPEGImageDecoder();
    BIImageDecoder* createBCPNGImageDecoder();
    BIImageDecoder* createBCGIFImageDecoder();
    BIImageDecoder* createBCBMPImageDecoder();
    void deleteBIImageDecoder( BIImageDecoder* );

    // Events
    class BIKeyboardEvent;
    BIEventLoop* getBIEventLoop();
    void deleteBIEventLoop();
    BIMouseEvent* createBIMouseEvent();
    BIWindowEvent* createBIWindowEvent(BIWindowEvent::ENUM_WINDOW, bool bGain,
       const WebCore::IntRect& rect, const BTWidget*);
    BIKeyboardEvent* createBIKeyboardEvent(  const WebCore::String& aText,
       const WebCore::String& aUnmodifiedText, const WebCore::String& aKeyIdentifier,
       bool bIsKeyUp, bool bShiftKey, bool bCtrlKey, bool bAltKey, bool bMetaKey, bool bIsAutoRepeat,
       int aVKey );
    BIEvent* createBITimerEvent();

    // Window
    class BIWindowManager;
    BIWindowManager* getBIWindowManager();
    void deleteBIWindowManager();

    // Observer
    class BIObserver;
    class BIObserverService;
    BIObserverService* getBIObserverService();
    
    // ResourceHandleManager
    class BIResourceHandleManager;
    BIResourceHandleManager* getBIResourceHandleManager();
    void deleteBIResourceHandleManager();

    // File
    class BIFile;
    BIFile* createBIFile(char* path);
    void deleteBIFile(BIFile* file);

    // Fonts
    class BIFontPlatformDataPrivate;
    class BIGlyphBuffer;
    BIGlyphBuffer* createBIGlyphBuffer();
    void deleteBIGlyphBuffer(BIGlyphBuffer*);
    BIFontPlatformDataPrivate* createBIFontPlatformDataPrivate();
    void deleteBIFontPlatformDataPrivate(BIFontPlatformDataPrivate* data);

    // Graphics
    class BIGraphicsContext;
    class BIGraphicsDevice;
    class BINativeImage;
    class BISurface;
    BIGraphicsContext* createRGBA32GraphicsContext(unsigned width, unsigned height);
    void deleteRGBA32GraphicsContext(BIGraphicsContext*);
    BIGraphicsContext* getBIGraphicsContext();
    BIGraphicsContext* createBIGraphicsContext();
    BIGraphicsContext* createFakeBIGraphicsContext();
    void deleteBIGraphicsContext(BIGraphicsContext*);
    BTAffineTransform &createBiAffineTransform();
    BTAffineTransform &createBiAffineTransform(double a, double b, double c, double d, double tx, double ty);
    BIGraphicsDevice* getBIGraphicsDevice();
    void deleteBIGraphicsDevice();
    
 
    // XML
    class BIXSLT;
    BIXSLT* createBIXSLT();
    class BIXML;
    BIXML* createBIXML();

    //Internationalization
    class BIInternationalization;
    BIInternationalization *getBIInternationalization();
}

// Bridge !
// not in BAL namespace
class BICookieJar;
BICookieJar* getBICookieJar();
void deleteBICookieJar(BICookieJar*);

// Implementation of previous methods will be done
// in the component itself, with this macro.

#define IMPLEMENT_CREATE_DELETE(interfacename, classname) \
    namespace BAL { \
        interfacename* create##interfacename() { \
            return new classname(); \
        } \
        void delete##interfacename(interfacename* p) { \
            delete p; \
        } \
    }

#define IMPLEMENT_GET_DELETE(interfacename, classname) \
    namespace BAL { \
        static interfacename* __##classname = 0; \
        interfacename* get##interfacename() { \
            if (__##classname) \
                return __##classname; \
            else \
                return __##classname = new classname(); \
        } \
        void delete##interfacename() { \
            if (__##classname) { \
                delete __##classname; \
                __##classname = NULL; \
            } \
        } \
    }

#endif
