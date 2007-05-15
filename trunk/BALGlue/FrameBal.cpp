/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com
 * All rights reserved.
 * Copyright (C) 2007 Pleyo.  All rights reserved.
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
#include "BALConfiguration.h"
#include "FrameBal.h"

#include "BIEventLoop.h"
#include "BIWindow.h"
#include "BIWindowEvent.h"
#include "BTLogHelper.h"
#include "CString.h"
#include "Document.h"
#include "EventHandler.h"
#include "FrameLoader.h"
#include "FrameLoaderClientBal.h"
#include "FramePrivate.h"
#include "FrameView.h"
#include "Page.h"
#include "PlatformWheelEvent.h"
#include "RenderTreeAsText.h"
#include "SelectionController.h"
#include "Settings.h"
#include "TypingCommand.h"

// This function loads resources from WebKit
// This does not belong here and I'm not sure where
// it should go
// I don't know what the plans or design is
// for none code resources
Vector<char> loadResourceIntoArray(const char* resourceName)
{
    Vector<char> resource;
    //if (strcmp(resourceName,"missingImage") == 0) {
    //}
    return resource;
}

namespace WebCore {

FrameBal::FrameBal(BIWindow* win, Page* p, HTMLFrameOwnerElement* ownerElement, FrameLoaderClientBal* frameLoader)
    : Frame(p, ownerElement, frameLoader)
    , m_drawable(win)
{
    Settings* settings = new Settings;
    settings->setLoadsImagesAutomatically(true);
    settings->setMinimumFontSize(5);
    settings->setMinimumLogicalFontSize(5);
    settings->setShouldPrintBackgrounds(true);
#ifdef __OWB_JS__
    settings->setJavaScriptEnabled(true);
#else
    settings->setJavaScriptEnabled(false);
#endif

    settings->setPluginsEnabled(true);
    settings->setDefaultFixedFontSize(14);
    settings->setDefaultFontSize(14);

    setSettings(settings);
    frameLoader->setFrame(this);

    FrameView* view = new FrameView(this);
    setView(view);
    view->setDrawable(win);
    win->setPainter(this);
}

FrameBal::~FrameBal()
{
    loader()->cancelAndClear();
}

void Frame::print()
{
    BALNotImplemented();
}

void Frame::issueTransposeCommand()
{
    BALNotImplemented();
}

void Frame::respondToChangedSelection(WebCore::Selection const&, bool)
{
    // FIXME: If we want continous spell checking, we need to implement this.
}

void Frame::cleanupPlatformScriptObjects()
{
}

bool Frame::isCharacterSmartReplaceExempt(UChar, bool)
{
    // no smart replace
    return true;
}

DragImageRef Frame::dragImageForSelection()
{
    return 0;
}

void Frame::setNeedsReapplyStyles()
{
    BALNotImplemented();
}

}
