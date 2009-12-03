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
#include <runtime/JSLock.h>
#include <runtime/JSValue.h>

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
    // TODO
    if (!parent() || !m_isWindowed)
        return;

    ASSERT(parent()->isFrameView());

    // As we are a non visual plugin, there's no visual to draw
    return;
}

void PluginView::setFocus()
{
    // no visual to focus
}

void PluginView::show()
{
    // no visual to update
}

void PluginView::hide()
{
}

void PluginView::paint(GraphicsContext* context, const IntRect& rect)
{
    // no visual to paint
}

void PluginView::handleKeyboardEvent(KeyboardEvent* event)
{
    NPEvent npEvent;

    /* FIXME: Synthesize an XEvent to pass through */

    JSC::JSLock::DropAllLocks dropAllLocks(false);
    if (!m_plugin->pluginFuncs()->event(m_instance, &npEvent))
        event->setDefaultHandled();
}

void PluginView::handleMouseEvent(MouseEvent* event)
{
    NPEvent npEvent;

    if (!m_isWindowed)
      return;

    /* FIXME: Synthesize an XEvent to pass through */
    IntPoint p = static_cast<FrameView*>(parent())->contentsToWindow(IntPoint(event->pageX(), event->pageY()));

    JSC::JSLock::DropAllLocks dropAllLocks(false);
    if (!m_plugin->pluginFuncs()->event(m_instance, &npEvent))
        event->setDefaultHandled();
}

void PluginView::setParent(ScrollView* parent)
{
    Widget::setParent(parent);

    if (parent)
        init();
    else {
        if (!platformPluginWidget())
            return;
    }
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
        JSC::JSLock::DropAllLocks dropAllLocks(false);
        setCallingPlugin(true);
        m_plugin->pluginFuncs()->setwindow(m_instance, &m_npWindow);
        setCallingPlugin(false);
        PluginView::setCurrentPluginView(0);

        if (!m_isWindowed)
            return;

        // no view : may be null
        // ASSERT(platformPluginWidget());
    }
}

void PluginView::setParentVisible(bool visible)
{
    if (isParentVisible() == visible)
        return;

    Widget::setParentVisible(visible);

    if (isSelfVisible() && platformPluginWidget()) {
        // no visual to show
    }
}

void PluginView::invalidateRegion(NPRegion)
{ 
    notImplemented();
}

NPError PluginView::handlePostReadFile(Vector<char>& buffer, uint32 len, const char* buf)
{
    String filename(buf, len);

    if (filename.startsWith("file:///"))
        filename = filename.substring(8);

    //FIXME - read the file data into buffer
    FILE* fileHandle = fopen((filename.utf8()).data(), "r");

    if (fileHandle == 0)
        return NPERR_FILE_NOT_FOUND;

    int bytesRead = fread(buffer.data(), 1, 0, fileHandle);

    fclose(fileHandle);

    if (bytesRead <= 0)
        return NPERR_FILE_NOT_FOUND;

    return NPERR_NO_ERROR;
}


NPError PluginView::getValueStatic(NPNVariable variable, void* value)
{
    switch (variable) {
    case NPNVToolkit:
#if PLATFORM(GTK)
        *static_cast<uint32*>(value) = 2;
#else
        *static_cast<uint32*>(value) = 0;
#endif
        return NPERR_NO_ERROR;

    case NPNVSupportsXEmbedBool:
#if PLATFORM(X11)
        *static_cast<NPBool*>(value) = true;
#else
        *static_cast<NPBool*>(value) = false;
#endif
        return NPERR_NO_ERROR;

    case NPNVjavascriptEnabledBool:
        *static_cast<NPBool*>(value) = true;
        return NPERR_NO_ERROR;

    default:
        return NPERR_GENERIC_ERROR;
    }
}

NPError PluginView::getValue(NPNVariable variable, void* value)
{
        switch (variable) {
    case NPNVxDisplay:
#if PLATFORM(X11)
/*        if (m_needsXEmbed)
            *(void **)value = (void *)GDK_DISPLAY();
        else
            *(void **)value = (void *)GTK_XTBIN(platformPluginWidget())->xtclient.xtdisplay;*/
        return NPERR_NO_ERROR;
#else
        return NPERR_GENERIC_ERROR;
#endif

#if PLATFORM(X11)
    case NPNVxtAppContext:
        if (!m_needsXEmbed) {
//            *(void **)value = XtDisplayToApplicationContext (GTK_XTBIN(platformPluginWidget())->xtclient.xtdisplay);

            return NPERR_NO_ERROR;
        } else
            return NPERR_GENERIC_ERROR;
#endif
#if ENABLE(NETSCAPE_PLUGIN_API)
        case NPNVWindowNPObject: {
            if (m_isJavaScriptPaused)
                return NPERR_GENERIC_ERROR;

            NPObject* windowScriptObject = m_parentFrame->script()->windowScriptNPObject();

            // Return value is expected to be retained, as described here: <http://www.mozilla.org/projects/plugin/npruntime.html>
            if (windowScriptObject)
                _NPN_RetainObject(windowScriptObject);

            void** v = (void**)value;
            *v = windowScriptObject;

            return NPERR_NO_ERROR;
        }

        case NPNVPluginElementNPObject: {
            if (m_isJavaScriptPaused)
                return NPERR_GENERIC_ERROR;

            NPObject* pluginScriptObject = 0;

            if (m_element->hasTagName(appletTag) || m_element->hasTagName(embedTag) || m_element->hasTagName(objectTag))
                pluginScriptObject = static_cast<HTMLPlugInElement*>(m_element)->getNPObject();

            // Return value is expected to be retained, as described here: <http://www.mozilla.org/projects/plugin/npruntime.html>
            if (pluginScriptObject)
                _NPN_RetainObject(pluginScriptObject);

            void** v = (void**)value;
            *v = pluginScriptObject;

            return NPERR_NO_ERROR;
        }
#endif

        case NPNVnetscapeWindow: {
#if PLATFORM(X11)
            //void* w = reinterpret_cast<void*>(value);
//            *((XID *)w) = GDK_WINDOW_XWINDOW(m_parentFrame->view()->hostWindow()->platformWindow()->window);
#endif
/*#ifdef GDK_WINDOWING_WIN32
            HGDIOBJ* w = reinterpret_cast<HGDIOBJ*>(value);
            *w = GDK_WINDOW_HWND(m_parentFrame->view()->hostWindow()->platformWindow()->window);
#endif*/
            return NPERR_NO_ERROR;
        }

        default:
            return getValueStatic(variable, value);
    }
}

void PluginView::invalidateRect(const IntRect& rect)
{
    invalidateWindowlessPluginRect(rect);
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
