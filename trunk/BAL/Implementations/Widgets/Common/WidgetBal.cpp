/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com
 * Copyright (C) 2007 Pleyo
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
#include "WidgetBal.h"

#include "BIGraphicsContext.h"
#include "BALConfiguration.h"
#include "Font.h"
#include "BTLogHelper.h"
#include "Cursor.h"

namespace WebCore {
class Cursor;
}

namespace BAL
{


BTWidget::BTWidget()
        : data(new WidgetPrivate)
{
    data->drawable = 0;
}

BTWidget::~BTWidget()
{
    delete data;
}

BIWindow * BTWidget::drawable() const
{
    return data->drawable;
}

void BTWidget::setDrawable(BIWindow *drawable)
{
    data->drawable = drawable;
}

void BTWidget::setClient(WidgetClient* c)
{
    data->client = c;
}

WidgetClient* BTWidget::client() const
{
    return data->client;
}

IntRect BTWidget::frameGeometry() const
{
    return data->geometry;
}

bool BTWidget::hasFocus() const
{
    BALNotImplemented();
    return false;
}

void BTWidget::setFocus()
{
    BALNotImplemented();
}

void BTWidget::clearFocus()
{
      BALNotImplemented();
}

const Font& BTWidget::font() const
{
    return data->font;
}

void BTWidget::setFont(const Font& font)
{
    data->font = font;
}

void BTWidget::setCursor(const Cursor&)
{
    // commented : occurs too often !
    //BALNotImplemented();
}

void BTWidget::show()
{
    BALNotImplemented();
}

void BTWidget::hide()
{
    BALNotImplemented();
}

void BTWidget::setFrameGeometry(const IntRect &/*r*/)
{
    BALNotImplemented();
}

void BTWidget::setEnabled(bool)
{
    BALNotImplemented();
}
bool BTWidget::isEnabled() const
{
    BALNotImplemented();
    return false;
}
int BTWidget::x() const { BALNotImplemented(); return 0; }
int BTWidget::y() const { BALNotImplemented(); return 0; }
int BTWidget::width() const { BALNotImplemented(); return 0; }
int BTWidget::height() const { BALNotImplemented(); return 0; }
IntSize BTWidget::size() const
{
    BALNotImplemented();
    return IntSize();
}
void BTWidget::resize(int, int)
{
    BALNotImplemented();
}
void BTWidget::resize(const IntSize&)
{
    BALNotImplemented();
}
IntPoint BTWidget::pos() const
{
    BALNotImplemented();
    return IntPoint();
}
void BTWidget::move(int, int)
{
    BALNotImplemented();
}
void BTWidget::move(const IntPoint&)
{
    BALNotImplemented();
}

void BTWidget::paint(GraphicsContext*, const IntRect&)
{
    BALNotImplemented();
}
Widget::FocusPolicy BTWidget::focusPolicy() const
{
    BALNotImplemented();
    return NoFocus;
}
Cursor BTWidget::cursor()
{
    BALNotImplemented();
    return Cursor();
}
GraphicsContext* BTWidget::lockDrawingFocus()
{
    BALNotImplemented();
    return 0;
}
void BTWidget::unlockDrawingFocus(GraphicsContext*)
{
     BALNotImplemented();
}
void BTWidget::enableFlushDrawing()
{
    BALNotImplemented();
}
void BTWidget::disableFlushDrawing()
{
     BALNotImplemented();
}
void BTWidget::setIsSelected(bool)
{
     BALNotImplemented();
}
bool BTWidget::isFrameView() const
{
     BALNotImplemented();
     return false;
}

void BTWidget::invalidate()
{
    BALNotImplemented();
}

void BTWidget::invalidateRect(const IntRect& r)
{
    BALNotImplemented();
}

void BTWidget::removeFromParent()
{
     BALNotImplemented();
}

IntRect BTWidget::windowClipRect() const
{
    BALNotImplemented();
    return IntRect();
}
}
