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
#include "Widget.h"

#include "Cursor.h"
#include "FrameView.h"
#include "GraphicsContext.h"
#include "IntRect.h"
#include "RenderObject.h"
#include "SDL.h"

namespace WKAL {

typedef SDL_Cursor BalCursor;

//TODO : redo the WidgetPrivate on SDL
class WidgetPrivate {
public:
    BalWidget* widget;
    WidgetClient* client;
    IntRect frameRect;

    ScrollView* parent;
    BalWidget* containingWindow;
    bool suppressInvalidation;
    BalCursor* cursor;
    bool enabled;

    BalDrawable* balDrawable() const
    {
        return  0;
    }
};

Widget::Widget()
    : data(new WidgetPrivate)
{
    data->widget = 0;
    data->parent = 0;
    data->containingWindow = 0;
    data->suppressInvalidation = false;
    data->cursor = 0;
}

BalWidget* Widget::balWidget() const
{
    return data->widget;
}

void Widget::setBalWidget(BalWidget* widget)
{
    data->widget = widget;
}

Widget::~Widget()
{
    ASSERT(!parent());
    delete data;
}

void Widget::setContainingWindow(PlatformWidget containingWindow)
{
    data->containingWindow = containingWindow;
}

PlatformWidget Widget::containingWindow() const
{
    return data->containingWindow;
}

void Widget::setClient(WidgetClient* c)
{
    data->client = c;
}

WidgetClient* Widget::client() const
{
    return data->client;
}

IntRect Widget::frameGeometry() const
{
//     printf("frameGeometry this = %p %d %d %d %d\n",this, data->frameRect.x(), data->frameRect.y(), data->frameRect.width(), data->frameRect.height());
    return data->frameRect;
}

void Widget::setFrameGeometry(const IntRect& r)
{
    data->frameRect = r;
}

void Widget::setParent(ScrollView* v)
{
    data->parent = v;
}

ScrollView* Widget::parent() const
{
    return data->parent;
}

void Widget::setFocus()
{
    NotImplemented();
}

Cursor Widget::cursor()
{
    return Cursor(data->cursor);
}

void Widget::setCursor(const Cursor& cursor)
{
}

void Widget::show()
{
    NotImplemented();
}

void Widget::hide()
{
    NotImplemented();
}

void Widget::setEnabled(bool shouldEnable)
{
    data->enabled = shouldEnable;
}

bool Widget::isEnabled() const
{
    return data->enabled;
}

void Widget::removeFromParent()
{
    if (parent())
        parent()->removeChild(this);
}

void Widget::paint(GraphicsContext* context, const IntRect &r)
{
    if (!balWidget())
        return;

    if (!context->balExposeEvent())
        return;
    
    printf("Widget::paint(\n");
    BalWidget* widget = balWidget();
    SDL_Event ev;
    ev.type = SDL_VIDEOEXPOSE;
    SDL_PushEvent(&ev);
}

void Widget::setIsSelected(bool)
{
    NotImplemented();
}

void Widget::invalidate()
{
    invalidateRect(IntRect(0, 0, width(), height()));
}

void Widget::invalidateRect(const IntRect& rect)
{
    //printf("invalidateRect x=%d y=%d w=%d h=%d\n", rect.x(), rect.y(), rect.width(), rect.height());
    if (data->suppressInvalidation)
        return;

    if (!parent()) {
        if (isFrameView())
            static_cast<FrameView*>(this)->addToDirtyRegion(rect);
        return;
    }

    // Get the root widget.
    ScrollView* outermostView = parent();
    while (outermostView && outermostView->parent())
        outermostView = outermostView->parent();
    if (!outermostView)
        return;

    IntRect windowRect = convertToContainingWindow(rect);
    outermostView->addToDirtyRegion(windowRect);
}

IntPoint Widget::convertToContainingWindow(const IntPoint& point) const
{
    IntPoint windowPoint = point;
    for (const Widget *parentWidget = parent(), *childWidget = this;
         parentWidget;
         childWidget = parentWidget, parentWidget = parentWidget->parent())
        windowPoint = parentWidget->convertChildToSelf(childWidget, windowPoint);
    return windowPoint;
}

IntPoint Widget::convertFromContainingWindow(const IntPoint& point) const
{
    IntPoint widgetPoint = point;
    for (const Widget *parentWidget = parent(), *childWidget = this;
         parentWidget;
         childWidget = parentWidget, parentWidget = parentWidget->parent())
        widgetPoint = parentWidget->convertSelfToChild(childWidget, widgetPoint);
    return widgetPoint;
}

IntRect Widget::convertToContainingWindow(const IntRect& rect) const
{
    IntRect convertedRect = rect;
    convertedRect.setLocation(convertToContainingWindow(convertedRect.location()));
    return convertedRect;
}

IntPoint Widget::convertChildToSelf(const Widget* child, const IntPoint& point) const
{
    return IntPoint(point.x() + child->x(), point.y() + child->y());
}

IntPoint Widget::convertSelfToChild(const Widget* child, const IntPoint& point) const
{
    return IntPoint(point.x() - child->x(), point.y() - child->y());
}

bool Widget::suppressInvalidation() const
{
    return data->suppressInvalidation;
}

void Widget::setSuppressInvalidation(bool suppress)
{
    data->suppressInvalidation = suppress;
}

void Widget::geometryChanged() const
{
}

bool Widget::isPluginView() const 
{
   return false; 
}

void Widget::handleEvent(Event*) 
{
}

}
