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
#include "IntRect.h"

namespace WebCore {
class Cursor;
}

namespace BAL
{


BTWidget::BTWidget()
        : data(new WidgetPrivate)
{
}

BTWidget::~BTWidget()
{
    delete data;
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
    return IntRect(data->x, data->y, data->width, data->height);
}

void BTWidget::setFocus()
{
    BALNotImplemented();
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

void BTWidget::setFrameGeometry(const IntRect &r)
{
    move(r.x(), r.y());
    resize(r.width(), r.height());
}

void BTWidget::setEnabled(bool isEnabled)
{
    BALNotImplemented();
    data->m_isEnabled = isEnabled;
}

bool BTWidget::isEnabled() const
{
    BALNotImplemented();
    return data->m_isEnabled;
}

int BTWidget::x() const { return data->x; }
int BTWidget::y() const { return data->y; }
int BTWidget::width() const { return data->width; }
int BTWidget::height() const { return data->height; }

IntSize BTWidget::size() const
{
    return IntSize(width(), height());
}

void BTWidget::resize(int w, int h)
{
    data->width = w;
    data->height = h;
}

void BTWidget::resize(const IntSize& size)
{
    data->width = size.width();
    data->height = size.height();
}

IntPoint BTWidget::pos() const
{
    return IntPoint(x(), y());
}

void BTWidget::move(int x, int y)
{
    if (data->m_parent) {
        // we are in absolute coords in the frame, so take care of parents
        data->x = data->m_parent->x() + x;
        data->y = data->m_parent->y() + y;
    } else {
        data->x = x;
        data->y = y;
    }
}

void BTWidget::move(const IntPoint& point)
{
    BALNotImplemented();
    if (data->m_parent) {
        data->x = data->m_parent->x() + point.x();
        data->y = data->m_parent->y() + point.y();
    } else {
        data->x = point.x();
        data->y = point.y();
    }
}

void BTWidget::paint(GraphicsContext*, const IntRect&)
{
    BALNotImplemented();
}

Cursor BTWidget::cursor()
{
    BALNotImplemented();
    return Cursor();
}

void BTWidget::setIsSelected(bool)
{
     BALNotImplemented();
}

bool BTWidget::isFrameView() const
{
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
     data->m_parent->removeChild(this);
     data->m_parent = NULL;
}

IntRect BTWidget::windowClipRect() const
{
    BALNotImplemented();
    return IntRect();
}

void BTWidget::setParent(BTScrollView* parent)
{
    data->m_parent = parent;
}

BTScrollView* BTWidget::parent() const
{
    return data->m_parent;
}

}


