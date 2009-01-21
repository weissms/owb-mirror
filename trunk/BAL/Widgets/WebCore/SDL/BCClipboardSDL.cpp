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
#include "ClipboardSDL.h"

#include "NotImplemented.h"
#include "StringHash.h"

#include "Editor.h"

#include <cstdio>

namespace WKAL {
PassRefPtr<Clipboard> Editor::newGeneralClipboard(ClipboardAccessPolicy policy)
{
    printf("Editor::newGeneralClipboard\n");
    return ClipboardBal::create(policy, false);
}

ClipboardBal::ClipboardBal(ClipboardAccessPolicy policy, bool forDragging)
    : Clipboard(policy, forDragging)
{
    printf("ClipboardBal::ClipboardBal\n");
    NotImplemented();
}

ClipboardBal::~ClipboardBal()
{
    printf("ClipboardBal::~ClipboardBa\n");
    NotImplemented();
}

void ClipboardBal::clearData(const String&)
{
    printf("ClipboardBal::clearData\n");
    NotImplemented();
}

void ClipboardBal::clearAllData()
{
    printf("ClipboardBal::clearAllDat\n");
    NotImplemented();
}

String ClipboardBal::getData(const String&, bool &success) const
{
    printf("ClipboardBal::getData\n");
    NotImplemented();
    success = false;
    return String();
}

bool ClipboardBal::setData(const String&, const String&)
{
    printf("ClipboardBal::setData\n");
    NotImplemented();
    return false;
}

HashSet<String> ClipboardBal::types() const
{
    printf("ClipboardBal::types\n");
    NotImplemented();
    return HashSet<String>();
}

IntPoint ClipboardBal::dragLocation() const
{
    printf("ClipboardBal::dragLocation\n");
    NotImplemented();
    return IntPoint(0, 0);
}

CachedImage* ClipboardBal::dragImage() const
{
    printf("ClipboardBal::dragImage\n");
    NotImplemented();
    return 0;
}

void ClipboardBal::setDragImage(CachedImage*, const IntPoint&)
{
    printf("ClipboardBal::setDragImage\n");
    NotImplemented();
}

Node* ClipboardBal::dragImageElement()
{
    printf("ClipboardBal::dragImageElement\n");
    NotImplemented();
    return 0;
}

void ClipboardBal::setDragImageElement(Node*, const IntPoint&)
{
    printf("ClipboardBal::setDragImageElement\n");
    NotImplemented();
}

DragImageRef ClipboardBal::createDragImage(IntPoint&) const
{
    printf("ClipboardBal::createDragImage\n");
    NotImplemented();
    return 0;
}

void ClipboardBal::declareAndWriteDragImage(Element*, const KURL&, const String&, Frame*)
{
    printf("ClipboardBal::declareAndWriteDragImage\n");
    NotImplemented();
}

void ClipboardBal::writeURL(const KURL&, const String&, Frame*)
{
    printf("ClipboardBal::writeURL\n");
    NotImplemented();
}

void ClipboardBal::writeRange(Range*, Frame*)
{
    printf("ClipboardBal::writeRange\n");
    NotImplemented();
}

bool ClipboardBal::hasData()
{
    printf("ClipboardBal::hasData\n");
    NotImplemented();
    return false;
}

}
