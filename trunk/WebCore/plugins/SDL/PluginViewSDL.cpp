/*
 * Copyright (C) 2006, 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2008 Collabora Ltd. All rights reserved.
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
#include "PluginView.h"

#include "Document.h"
#include "DocumentLoader.h"
#include "Element.h"
#include "EventNames.h"
#include "FrameLoader.h"
#include "FrameLoadRequest.h"
#include "FrameTree.h"
#include "Frame.h"
#include "FrameView.h"
#include "GraphicsContext.h"
#include "Image.h"
#include "HTMLNames.h"
#include "HTMLPlugInElement.h"
#include "KeyboardEvent.h"
#include "MouseEvent.h"
#include "NotImplemented.h"
#include "Page.h"
#include "PlatformMouseEvent.h"
#include "PluginDebug.h"
#include "PluginPackage.h"
#include "RenderLayer.h"
#include "Settings.h"
#include "JSDOMBinding.h"
#include "ScriptController.h"
#include "npruntime_impl.h"
#include "runtime.h"
#include "runtime_root.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

using JSC::ExecState;
using JSC::Interpreter;
using JSC::JSObject;
using JSC::UString;

using std::min;

using namespace WTF;

namespace WebCore {

using namespace HTMLNames;

void PluginView::updatePluginWidget()
{
    if (!parent() || !m_isWindowed)
        return;

    ASSERT(parent()->isFrameView());
    FrameView* frameView = static_cast<FrameView*>(parent());

    IntRect oldWindowRect = m_windowRect;
    IntRect oldClipRect = m_clipRect;

    m_windowRect = IntRect(frameView->contentsToWindow(frameRect().location()), frameRect().size());
    m_clipRect = windowClipRect();
    m_clipRect.move(-m_windowRect.x(), -m_windowRect.y());
}

void PluginView::setFocus()
{
}

void PluginView::show()
{
}

void PluginView::hide()
{
}

/*void PluginView::paintMissingPluginIcon(GraphicsContext* context, const IntRect& rect)
{
    
    if (m_status !=  PluginStatusLoadedSuccessfully) {
        static PassRefPtr<WebCore::Image> image;
        if (!image)
            image = Image::loadPlatformResource("missing-plugin");
        
        IntRect imageRect(x(), y(), image->width(), image->height());
        int xOffset = (width() - imageRect.width()) / 2;
        int yOffset = (height() - imageRect.height()) / 2;
        imageRect.move(xOffset, yOffset);
        
        context->save();
        context->clip(windowClipRect());
        context->drawImage(image.get(), imageRect.location());
        context->restore();
    } else {
        static RefPtr<Image> nullPluginImage;
        if (!nullPluginImage)
            nullPluginImage = Image::loadPlatformResource("nullPlugin");

        IntRect imageRect(x(), y(), nullPluginImage->width(), nullPluginImage->height());

        int xOffset = (width() - imageRect.width()) / 2;
        int yOffset = (height() - imageRect.height()) / 2;

        imageRect.move(xOffset, yOffset);

        if (!rect.intersects(imageRect))
            return;

        context->save();
        context->clip(windowClipRect());
        context->drawImage(nullPluginImage.get(), imageRect.location());
        context->restore();
    }
}*/

void PluginView::paint(GraphicsContext* context, const IntRect& rect)
{
    if (!m_isStarted || m_status !=  PluginStatusLoadedSuccessfully) {
        // Draw the "missing plugin" image
        paintMissingPluginIcon(context, rect);
        return;
    }
}

void PluginView::handleKeyboardEvent(KeyboardEvent* event)
{
    NPEvent npEvent;
    
    //FIXME: Synthesize a SDLEvent to pass through
    if (!m_plugin->pluginFuncs()->event(m_instance, &npEvent))
        event->setDefaultHandled();
}

void PluginView::handleMouseEvent(MouseEvent* event)
{
    NPEvent npEvent;

    if (!m_isWindowed)
        return;

    //FIXME: Synthesize a SDLEvent to pass through
    IntPoint p = static_cast<FrameView*>(parent())->contentsToWindow(IntPoint(event->pageX(), event->pageY()));

    if (!m_plugin->pluginFuncs()->event(m_instance, &npEvent))
        event->setDefaultHandled();
}

void PluginView::setParent(ScrollView* parent)
{
    Widget::setParent(parent);

    if (parent)
        init();
}

void PluginView::setNPWindowRect(const IntRect& rect)
{
    if (!m_isStarted || !parent())
        return;

    IntPoint p = static_cast<FrameView*>(parent())->contentsToWindow(rect.location());
    m_npWindow.x = p.x();
    m_npWindow.y = p.y();

    m_npWindow.width = rect.width();
    m_npWindow.height = rect.height();

    m_npWindow.clipRect.left = 0;
    m_npWindow.clipRect.top = 0;
    m_npWindow.clipRect.right = rect.width();
    m_npWindow.clipRect.bottom = rect.height();

    if (m_npWindow.x < 0 || m_npWindow.y < 0 ||
        m_npWindow.width <= 0 || m_npWindow.height <= 0)
        return;

    if (m_plugin->pluginFuncs()->setwindow) {
        PluginView::setCurrentPluginView(this);
        setCallingPlugin(true);
        m_plugin->pluginFuncs()->setwindow(m_instance, &m_npWindow);
        setCallingPlugin(false);
        PluginView::setCurrentPluginView(0);

        if (!m_isWindowed)
            return;

    }
}

void PluginView::invalidateRegion(NPRegion)
{ 
    notImplemented();
}

NPError PluginView::handlePostReadFile(Vector<char>& buffer, uint32 len, const char* buf)
{
    return NPERR_NO_ERROR;
}

NPError PluginView::getValue(NPNVariable variable, void* value)
{
    return NPERR_NO_ERROR;
}

NPError PluginView::getValueStatic(NPNVariable variable, void* value)
{
    return NPERR_GENERIC_ERROR;
}

void PluginView::invalidateRect(NPRect* rect)
{
    if (!rect) {
        invalidate();
        return;
    }

    IntRect r(rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top);
    Widget::invalidateRect(r);
}

void PluginView::forceRedraw()
{
}

void PluginView::invalidateRect(const IntRect& rect)
{
}

void PluginView::setParentVisible(bool visible)
{
}

bool PluginView::platformStart()
{
    return true;
}

void PluginView::platformDestroy()
{
    notImplemented();
}

void PluginView::halt()
{
    notImplemented();
}

void PluginView::restart()
{
    notImplemented();
}

} // namespace WebCore
