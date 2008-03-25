/*
 * Copyright (C) 2006, 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2007 Collabora, Ltd. All rights reserved.
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

#ifdef __OWB__
#include "PluginViewGtk.h"
#endif

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
#include "Page.h"
#include "PlatformMouseEvent.h"
#include "PluginDebug.h"
#include "PluginPackage.h"
#include "PluginStream.h"
#include "RenderLayer.h"
#include "Settings.h"
#include "kjs_binding.h"
#include "kjs_proxy.h"
#include "kjs_window.h"
#include "npruntime_impl.h"
#include "runtime_root.h"
#include <kjs/JSLock.h>
#include <kjs/value.h>

using KJS::ExecState;
using KJS::Interpreter;
using KJS::JSLock;
using KJS::JSObject;
using KJS::JSValue;
using KJS::UString;

using std::min;

namespace WebCore {

using namespace EventNames;
using namespace HTMLNames;

class PluginRequest {
public:
    PluginRequest(const FrameLoadRequest& frameLoadRequest, bool sendNotification, void* notifyData)
        : m_frameLoadRequest(frameLoadRequest)
        , m_notifyData(notifyData)
        , m_sendNotification(sendNotification) { }
public:
    const FrameLoadRequest& frameLoadRequest() const { return m_frameLoadRequest; }
    void* notifyData() const { return m_notifyData; }
    bool sendNotification() const { return m_sendNotification; }
private:
    FrameLoadRequest m_frameLoadRequest;
    void* m_notifyData;
    bool m_sendNotification;
    // FIXME: user gesture
};

static const double MessageThrottleTimeInterval = 0.001;

class PluginMessageThrottler {
public:
    PluginMessageThrottler(PluginView* pluginView)
        : m_pluginView(pluginView)
        , m_back(0), m_front(0)
        , m_messageThrottleTimer(this, &PluginMessageThrottler::messageThrottleTimerFired)
    {
        // Initialize the free list with our inline messages
        for (unsigned i = 0; i < NumInlineMessages - 1; i++)
            m_inlineMessages[i].next = &m_inlineMessages[i + 1];
        m_inlineMessages[NumInlineMessages - 1].next = 0;
        m_freeInlineMessages = &m_inlineMessages[0];
    }

    ~PluginMessageThrottler()
    {
        PluginMessage* next;
    
        for (PluginMessage* message = m_front; message; message = next) {
            next = message->next;
            freeMessage(message);
        }        
    }
    
    void appendMessage(XID hWnd, guint msg, guint wParam, glong lParam) 
    {
        PluginMessage* message = allocateMessage();

        message->hWnd = hWnd;
        message->msg = msg;
        message->wParam = wParam;
        message->lParam = lParam;
        message->next = 0;

        if (m_back)
            m_back->next = message;
        m_back = message;
        if (!m_front)
            m_front = message;

        if (!m_messageThrottleTimer.isActive())
            m_messageThrottleTimer.startOneShot(MessageThrottleTimeInterval);
    }

private:
    struct PluginMessage {
        XID hWnd;
        guint msg;
        guint wParam;
        glong lParam;

        struct PluginMessage* next;
    };
    
    void messageThrottleTimerFired(Timer<PluginMessageThrottler>*)
    {
        PluginMessage* message = m_front;
        m_front = m_front->next;
        if (message == m_back)
            m_back = 0;

	//        ::CallWindowProc(m_pluginView->pluginWndProc(), message->hWnd, message->msg, message->wParam, message->lParam);

        freeMessage(message);

        if (m_front)
            m_messageThrottleTimer.startOneShot(MessageThrottleTimeInterval);
    }

    PluginMessage* allocateMessage()
    {
        PluginMessage *message;

        if (m_freeInlineMessages) {
            message = m_freeInlineMessages;
            m_freeInlineMessages = message->next;
        } else
            message = new PluginMessage;

        return message;
    }

    bool isInlineMessage(PluginMessage* message) 
    {
        return message >= &m_inlineMessages[0] && message <= &m_inlineMessages[NumInlineMessages - 1];
    }

    void freeMessage(PluginMessage* message) 
    {
        if (isInlineMessage(message)) {
            message->next = m_freeInlineMessages;
            m_freeInlineMessages = message;
        } else
            delete message;
    }

    PluginView* m_pluginView;
    PluginMessage* m_back;
    PluginMessage* m_front;

    static const int NumInlineMessages = 4;
    PluginMessage m_inlineMessages[NumInlineMessages];
    PluginMessage* m_freeInlineMessages;

    Timer<PluginMessageThrottler> m_messageThrottleTimer;
};

static String scriptStringIfJavaScriptURL(const KURL& url)
{
    if (!url.url().startsWith("javascript:", false))
        return String();

    // This returns an unescaped string
    return KURL::decode_string(url.url().mid(11));
}

PluginView* PluginView::s_currentPluginView = 0;

const wchar_t* kWebPluginViewWindowClassName = L"WebPluginView";
const wchar_t* kWebPluginViewProperty = L"WebPluginViewProperty";

static const char* MozillaUserAgent = "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1) Gecko/20061010 Firefox/2.0";

void PluginView::updateWindow() const
{
    if (!parent())
        return;

    ASSERT(parent()->isFrameView());
    FrameView* frameView = static_cast<FrameView*>(parent());

    IntRect oldWindowRect = m_windowRect;
    IntRect oldClipRect = m_clipRect;

    m_windowRect = IntRect(frameView->contentsToWindow(frameGeometry().location()), frameGeometry().size());
    m_clipRect = windowClipRect();
    m_clipRect.move(-m_windowRect.x(), -m_windowRect.y());

    if (m_window && (m_windowRect != oldWindowRect || m_clipRect != oldClipRect))
        m_pluginViewPrivate->updateWindow(m_windowRect, m_needsXEmbed);
}

IntRect PluginView::windowClipRect() const
{
    // Start by clipping to our bounds.
    IntRect clipRect(m_windowRect);
    
    // Take our element and get the clip rect from the enclosing layer and frame view.
    RenderLayer* layer = m_element->renderer()->enclosingLayer();
    FrameView* parentView = m_element->document()->view();
    clipRect.intersect(parentView->windowClipRectForLayer(layer, true));

    return clipRect;
}

void PluginView::setFrameGeometry(const IntRect& rect)
{
    if (m_element->document()->printing())
        return;

    if (rect != frameGeometry())
        Widget::setFrameGeometry(rect);

    if (parent())
        init();

    updateWindow();
    setNPWindowRect(rect);
}

void PluginView::geometryChanged() const
{
    //Widget::geometryChanged();
    updateWindow();
}

void PluginView::setFocus()
{
    //if (XID window = m_window)
    //SetFocus(window);

    Widget::setFocus();
}

void PluginView::show()
{
    m_isVisible = true;

    //if (XID window = m_window)
    //ShowWindow(window, SW_SHOWNA);

    Widget::show();
}

void PluginView::hide()
{
    m_isVisible = false;

    //if (XID window = m_window)
    //ShowWindow(window, SW_HIDE);

    Widget::hide();
}

void PluginView::paintMissingPluginIcon(GraphicsContext* context, const IntRect& rect)
{
    static Image* nullPluginImage;
    if (!nullPluginImage)
        nullPluginImage = Image::loadPlatformResource("nullPlugin");

    IntRect imageRect(frameGeometry().x(), frameGeometry().y(), nullPluginImage->width(), nullPluginImage->height());

    int xOffset = (frameGeometry().width() - imageRect.width()) / 2;
    int yOffset = (frameGeometry().height() - imageRect.height()) / 2;

    imageRect.move(xOffset, yOffset);

    if (!rect.intersects(imageRect))
        return;

    context->save();
    context->clip(windowClipRect());
    //context->drawImage(nullPluginImage, imageRect.location());
    context->drawImage(nullPluginImage->nativeImageForCurrentFrame(), imageRect.location());
    context->restore();
}

void PluginView::paint(GraphicsContext* context, const IntRect& rect)
{
    if (!m_isStarted) {
        // Draw the "missing plugin" image
        paintMissingPluginIcon(context, rect);
        return;
    }

    if (m_isWindowed || context->paintingDisabled())
        return;

    NPEvent npEvent;
    /* Need to synthesize Xevents here */

    m_npWindow.type = NPWindowTypeDrawable;

    ASSERT(parent()->isFrameView());

    if (m_plugin->pluginFuncs()->event) {
        KJS::JSLock::DropAllLocks dropAllLocks;
        m_plugin->pluginFuncs()->event(m_instance, &npEvent);
    }

    setNPWindowRect(frameGeometry());

    if (m_plugin->pluginFuncs()->event) {
        KJS::JSLock::DropAllLocks dropAllLocks;
        m_plugin->pluginFuncs()->event(m_instance, &npEvent);
    }
}

void PluginView::handleKeyboardEvent(KeyboardEvent* event)
{
    NPEvent npEvent;

    /* Synthesize an XEvent and pass it on */
    /*
    npEvent.wParam = event->keyCode();    

    if (event->type() == keydownEvent) {
        npEvent.event = WM_KEYDOWN;
        npEvent.lParam = 0;
    } else if (event->type() == keyupEvent) {
        npEvent.event = WM_KEYUP;
        npEvent.lParam = 0x8000;
    }
    */

    KJS::JSLock::DropAllLocks dropAllLocks;
    if (!m_plugin->pluginFuncs()->event(m_instance, &npEvent))
        event->setDefaultHandled();
}

//extern bool ignoreNextSetCursor;

void PluginView::handleMouseEvent(MouseEvent* event)
{
    NPEvent npEvent;

    /* Synthesize an XEvent to pass on */
    IntPoint p = static_cast<FrameView*>(parent())->contentsToWindow(IntPoint(event->pageX(), event->pageY()));

    /*
    npEvent.lParam = MAKELPARAM(p.x(), p.y());
    npEvent.wParam = 0;

    if (event->ctrlKey())
        npEvent.wParam |= MK_CONTROL;
    if (event->shiftKey())
        npEvent.wParam |= MK_SHIFT;

    if (event->type() == mousemoveEvent ||
        event->type() == mouseoutEvent || 
        event->type() == mouseoverEvent) {
        npEvent.event = WM_MOUSEMOVE;
        if (event->buttonDown())
            switch (event->button()) {
                case LeftButton:
                    npEvent.wParam |= MK_LBUTTON;
                    break;
                case MiddleButton:
                    npEvent.wParam |= MK_MBUTTON;
                    break;
                case RightButton:
                    npEvent.wParam |= MK_RBUTTON;
                break;
            }
    }
    else if (event->type() == mousedownEvent) {
        switch (event->button()) {
            case 0:
                npEvent.event = WM_LBUTTONDOWN;

                // Focus the plugin
                m_parentFrame->document()->setFocusedNode(m_element);
                break;
            case 1:
                npEvent.event = WM_MBUTTONDOWN;
                break;
            case 2:
                npEvent.event = WM_RBUTTONDOWN;
                break;
        }
    } else if (event->type() == mouseupEvent) {
        switch (event->button()) {
            case 0:
                npEvent.event = WM_LBUTTONUP;
                break;
            case 1:
                npEvent.event = WM_MBUTTONUP;
                break;
            case 2:
                npEvent.event = WM_RBUTTONUP;
                break;
        }
    } else
        return;
    */

    KJS::JSLock::DropAllLocks dropAllLocks;
    if (!m_plugin->pluginFuncs()->event(m_instance, &npEvent))
        event->setDefaultHandled();

    // Currently, Widget::setCursor is always called after this function in EventHandler.cpp
    // and since we don't want that we set ignoreNextSetCursor to true here to prevent that.
    //ignoreNextSetCursor = true;     
}

void PluginView::handleEvent(Event* event)
{
    if (!m_plugin || m_isWindowed)
        return;

    if (event->isMouseEvent())
        handleMouseEvent(static_cast<MouseEvent*>(event));
    else if (event->isKeyboardEvent())
        handleKeyboardEvent(static_cast<KeyboardEvent*>(event));
}

void PluginView::setParent(BAL::BTScrollView* parent)
{
    Widget::setParent(parent);
}

void PluginView::setNPWindowRect(const IntRect& rect)
{
    if (!m_isStarted)
        return;

    if (!parent())
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

    if (m_plugin->pluginFuncs()->setwindow) {
        KJS::JSLock::DropAllLocks dropAllLocks;
        m_plugin->pluginFuncs()->setwindow(m_instance, &m_npWindow);

        if (!m_isWindowed)
            return;

        ASSERT(m_window);
    }

    m_pluginViewPrivate->setNPWindowRect(rect, m_needsXEmbed);
}

bool PluginView::start()
{
    if (m_isStarted)
        return false;

    ASSERT(m_plugin);
    ASSERT(m_plugin->pluginFuncs()->newp);

    NPError npErr;
    PluginView::setCurrentPluginView(this);
    {
        KJS::JSLock::DropAllLocks dropAllLocks;
        npErr = m_plugin->pluginFuncs()->newp((NPMIMEType)m_mimeType.data(), m_instance, m_mode, m_paramCount, m_paramNames, m_paramValues, NULL);
    }
    PluginView::setCurrentPluginView(0);

    if (npErr != NPERR_NO_ERROR)
        return false;

    m_isStarted = true;

    if (!m_url.isEmpty() && !m_loadManually) {
        FrameLoadRequest frameLoadRequest;
        frameLoadRequest.resourceRequest().setHTTPMethod("GET");
        frameLoadRequest.resourceRequest().setURL(m_url);
        load(frameLoadRequest, false, 0);
    }

    return true;
}

void PluginView::stop()
{
    if (!m_isStarted)
        return;

    HashSet<RefPtr<PluginStream> > streams = m_streams;
    HashSet<RefPtr<PluginStream> >::iterator end = streams.end();
    for (HashSet<RefPtr<PluginStream> >::iterator it = streams.begin(); it != end; ++it) {
        (*it)->stop();
        disconnectStream((*it).get());
    }

    ASSERT(m_streams.isEmpty());

    m_isStarted = false;

    KJS::JSLock::DropAllLocks dropAllLocks;

    // Destroy the plugin
    m_plugin->pluginFuncs()->destroy(m_instance, 0);

    m_instance->pdata = 0;
}

void PluginView::setCurrentPluginView(PluginView* pluginView)
{
    s_currentPluginView = pluginView;
}

PluginView* PluginView::currentPluginView()
{
    return s_currentPluginView;
}

static char* createUTF8String(const String& str)
{
    CString cstr = str.utf8();
    char* result = reinterpret_cast<char*>(fastMalloc(cstr.length() + 1));

    strncpy(result, cstr.data(), cstr.length() + 1);

    return result;
}

static void freeStringArray(char** stringArray, int length)
{
    if (!stringArray)
        return;

    for (int i = 0; i < length; i++)
        fastFree(stringArray[i]);

    fastFree(stringArray);
}

static bool getString(KJSProxy* proxy, JSValue* result, String& string)
{
    if (!proxy || !result || result->isUndefined())
        return false;
    JSLock lock;

    //ExecState* exec = proxy->globalObject()->globalExec();
    ExecState* exec = proxy->interpreter()->globalExec();
    UString ustring = result->toString(exec);
    exec->clearException();
    
    string = ustring;
    return true;
}

void PluginView::performRequest(PluginRequest* request)
{
    KURL requestURL = request->frameLoadRequest().resourceRequest().url();
    String jsString = scriptStringIfJavaScriptURL(requestURL);

    if (jsString.isNull()) {
        m_parentFrame->loader()->urlSelected(request->frameLoadRequest(), 0, false, true);

        // FIXME: <rdar://problem/4807469> This should be sent when the document has finished loading
        if (request->sendNotification()) {
            KJS::JSLock::DropAllLocks dropAllLocks;
            m_plugin->pluginFuncs()->urlnotify(m_instance, requestURL.url().utf8(), NPRES_DONE, request->notifyData());
        }

        return;
    }

    // Targeted JavaScript requests are only allowed on the frame that contains the JavaScript plugin
    // and this has been made sure in ::load.
    ASSERT(request->frameLoadRequest().frameName().isEmpty() || m_parentFrame->tree()->find(request->frameLoadRequest().frameName()) == m_parentFrame);
    
    // Executing a script can cause the plugin view to be destroyed, so we keep a reference to the parent frame.
    RefPtr<Frame> parentFrame =  m_parentFrame;
    JSValue* result = m_parentFrame->loader()->executeScript(jsString.deprecatedString(), true);

    if (request->frameLoadRequest().frameName().isNull()) {
        String resultString;

        if (!getString(parentFrame->scriptProxy(), result, resultString))
            return;

        CString cstr = resultString.utf8();
        RefPtr<PluginStream> stream = new PluginStream(this, parentFrame.get(), request->frameLoadRequest().resourceRequest(), request->sendNotification(), request->notifyData());
        m_streams.add(stream);
        stream->sendJavaScriptStream(requestURL, cstr);
    }
}

void PluginView::requestTimerFired(Timer<PluginView>* timer)
{
    ASSERT(timer == &m_requestTimer);
    ASSERT(m_requests.size() > 0);

    PluginRequest* request = m_requests[0];
    m_requests.remove(0);
    
    // Schedule a new request before calling performRequest since the call to
    // performRequest can cause the plugin view to be deleted.
    if (m_requests.size() > 0)
        m_requestTimer.startOneShot(0);

    performRequest(request);
}

void PluginView::scheduleRequest(PluginRequest* request)
{
    m_requests.append(request);
    m_requestTimer.startOneShot(0);
}

NPError PluginView::load(const FrameLoadRequest& frameLoadRequest, bool sendNotification, void* notifyData)
{
    ASSERT(frameLoadRequest.resourceRequest().httpMethod() == "GET" || frameLoadRequest.resourceRequest().httpMethod() == "POST");

    KURL url = frameLoadRequest.resourceRequest().url();
    
    if (url.isEmpty())
        return NPERR_INVALID_URL;

    String target = frameLoadRequest.frameName();

    // don't let a plugin start any loads if it is no longer part of a document that is being 
    // displayed unless the loads are in the same frame as the plugin.
    if (m_parentFrame->loader()->documentLoader() != m_parentFrame->loader()->activeDocumentLoader() &&
        (target.isNull() || m_parentFrame->tree()->find(target) != m_parentFrame))
        return NPERR_GENERIC_ERROR;

    String jsString = scriptStringIfJavaScriptURL(url);
    if (!jsString.isNull()) {
        Settings* settings = m_parentFrame->settings();
        if (!settings || !settings->isJavaScriptEnabled()) {
            // Return NPERR_GENERIC_ERROR if JS is disabled. This is what Mozilla does.
            return NPERR_GENERIC_ERROR;
        } else if (target.isNull() && m_mode == NP_FULL) {
            // Don't allow a JavaScript request from a standalone plug-in that is self-targetted
            // because this can cause the user to be redirected to a blank page (3424039).
            return NPERR_INVALID_PARAM;
        }
    }

    if (!jsString.isNull() || !target.isNull()) {
        if (!jsString.isNull() && !target.isNull() && m_parentFrame->tree()->find(target) != m_parentFrame) {
            // For security reasons, only allow JS requests to be made on the frame that contains the plug-in.
            return NPERR_INVALID_PARAM;
        }

        PluginRequest* request = new PluginRequest(frameLoadRequest, sendNotification, notifyData);
        scheduleRequest(request);
    } else {
        PluginStream* stream = new PluginStream(this, m_parentFrame, frameLoadRequest.resourceRequest(), sendNotification, notifyData);
        m_streams.add(stream);

        stream->start();
    }

    return NPERR_NO_ERROR;
}

static KURL makeURL(const KURL& baseURL, const char* relativeURLString)
{
    DeprecatedString urlString = DeprecatedString::fromLatin1(relativeURLString);

    // Strip return characters
    urlString.replace('\n', "");
    urlString.replace('\r', "");

    return KURL(baseURL, urlString);
}

NPError PluginView::getURLNotify(const char* url, const char* target, void* notifyData)
{
    FrameLoadRequest frameLoadRequest;

    frameLoadRequest.setFrameName(target);
    frameLoadRequest.resourceRequest().setHTTPMethod("GET");
    frameLoadRequest.resourceRequest().setURL(makeURL(m_baseURL, url));

    return load(frameLoadRequest, true, notifyData);
}

NPError PluginView::getURL(const char* url, const char* target)
{
    FrameLoadRequest frameLoadRequest;

    frameLoadRequest.setFrameName(target);
    frameLoadRequest.resourceRequest().setHTTPMethod("GET");
    frameLoadRequest.resourceRequest().setURL(makeURL(m_baseURL, url));

    return load(frameLoadRequest, false, 0);
}

static inline bool startsWithBlankLine(const Vector<char>& buffer)
{
    return buffer.size() > 0 && buffer[0] == '\n';
}

static inline int locationAfterFirstBlankLine(const Vector<char>& buffer)
{
    const char* bytes = buffer.data();
    unsigned length = buffer.size();

    for (unsigned i = 0; i < length - 4; i++) {
        // Support for Acrobat. It sends "\n\n".
        if (bytes[i] == '\n' && bytes[i + 1] == '\n')
            return i + 2;
        
        // Returns the position after 2 CRLF's or 1 CRLF if it is the first line.
        if (bytes[i] == '\r' && bytes[i + 1] == '\n') {
            i += 2;
            if (i == 2)
                return i;
            else if (bytes[i] == '\n')
                // Support for Director. It sends "\r\n\n" (3880387).
                return i + 1;
            else if (bytes[i] == '\r' && bytes[i + 1] == '\n')
                // Support for Flash. It sends "\r\n\r\n" (3758113).
                return i + 2;
        }
    }

    return -1;
}

static inline const char* findEOL(const char* bytes, unsigned length)
{
    // According to the HTTP specification EOL is defined as
    // a CRLF pair. Unfortunately, some servers will use LF
    // instead. Worse yet, some servers will use a combination
    // of both (e.g. <header>CRLFLF<body>), so findEOL needs
    // to be more forgiving. It will now accept CRLF, LF or
    // CR.
    //
    // It returns NULL if EOLF is not found or it will return
    // a pointer to the first terminating character.
    for (unsigned i = 0; i < length; i++) {
        if (bytes[i] == '\n')
            return bytes + i;
        if (bytes[i] == '\r') {
            // Check to see if spanning buffer bounds
            // (CRLF is across reads). If so, wait for
            // next read.
            if (i + 1 == length)
                break;

            return bytes + i;
        }
    }

    return 0;
}

static inline String capitalizeRFC822HeaderFieldName(const String& name)
{
    bool capitalizeCharacter = true;
    String result;

    for (unsigned i = 0; i < name.length(); i++) {
        UChar c;

        if (capitalizeCharacter && name[i] >= 'a' && name[i] <= 'z')
            c = toupper(name[i]);
        else if (!capitalizeCharacter && name[i] >= 'A' && name[i] <= 'Z')
            c = tolower(name[i]);
        else
            c = name[i];

        if (name[i] == '-')
            capitalizeCharacter = true;
        else
            capitalizeCharacter = false;

        result.append(c);
    }

    return result;
}

static inline HTTPHeaderMap parseRFC822HeaderFields(const Vector<char>& buffer, unsigned length)
{
    const char* bytes = buffer.data();
    const char* eol;
    String lastKey;
    HTTPHeaderMap headerFields;

    // Loop ove rlines until we're past the header, or we can't find any more end-of-lines
    while ((eol = findEOL(bytes, length))) {
        const char* line = bytes;
        int lineLength = eol - bytes;
        
        // Move bytes to the character after the terminator as returned by findEOL.
        bytes = eol + 1;
        if ((*eol == '\r') && (*bytes == '\n'))
            bytes++; // Safe since findEOL won't return a spanning CRLF.

        length -= (bytes - line);
        if (lineLength == 0)
            // Blank line; we're at the end of the header
            break;
        else if (*line == ' ' || *line == '\t') {
            // Continuation of the previous header
            if (lastKey.isNull()) {
                // malformed header; ignore it and continue
                continue;
            } else {
                // Merge the continuation of the previous header
                String currentValue = headerFields.get(lastKey);
                String newValue = DeprecatedString::fromLatin1(line, lineLength);

                headerFields.set(lastKey, currentValue + newValue);
            }
        } else {
            // Brand new header
            const char* colon;
            for (colon = line; *colon != ':' && colon != eol; colon++) {
                // empty loop
            }
            if (colon == eol) 
                // malformed header; ignore it and continue
                continue;
            else {
                lastKey = capitalizeRFC822HeaderFieldName(DeprecatedString::fromLatin1(line, colon - line));
                String value;

                for (colon++; colon != eol; colon++) {
                    if (*colon != ' ' && *colon != '\t')
                        break;
                }
                if (colon == eol)
                    value = "";
                else
                    value = DeprecatedString::fromLatin1(colon, eol - colon);

                String oldValue = headerFields.get(lastKey);
                if (!oldValue.isNull()) {
                    String tmp = oldValue;
                    tmp += ", ";
                    tmp += value;
                    value = tmp;
                }

                headerFields.set(lastKey, value);
            }
        }
    }

    return headerFields;
}

NPError PluginView::handlePost(const char* url, const char* target, uint32 len, const char* buf, bool file, void* notifyData, bool sendNotification, bool allowHeaders)
{
    if (!url || !len || !buf)
        return NPERR_INVALID_PARAM;

    FrameLoadRequest frameLoadRequest;

    HTTPHeaderMap headerFields;
    Vector<char> buffer;
    
    if (file) {
        String filename = DeprecatedString::fromLatin1(buf, len);

        if (filename.startsWith("file:///"))
            filename = filename.substring(8);

        // Get file info
        if (!g_file_test ((filename.utf8()).data(), (GFileTest)(G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR)))
            return NPERR_FILE_NOT_FOUND;

	//FIXME - read the file data into buffer
        FILE* fileHandle = fopen((filename.utf8()).data(), "r");
        
        if (fileHandle == NULL)
            return NPERR_FILE_NOT_FOUND;

        //buffer.resize();

        int bytesRead = fread(buffer.data(), 1, 0, fileHandle);

        fclose(fileHandle);

        if (bytesRead <= 0)
            return NPERR_FILE_NOT_FOUND;

	return NPERR_NO_ERROR;
    } else {
        buffer.resize(len);
        memcpy(buffer.data(), buf, len);
    }

    const char* postData = buffer.data();
    int postDataLength = buffer.size();
    
    if (allowHeaders) {
        if (startsWithBlankLine(buffer)) {
            postData++;
            postDataLength--;
        } else {
            int location = locationAfterFirstBlankLine(buffer);
            if (location != -1) {
                // If the blank line is somewhere in the middle of the buffer, everything before is the header
                headerFields = parseRFC822HeaderFields(buffer, location);
                unsigned dataLength = buffer.size() - location;

                // Sometimes plugins like to set Content-Length themselves when they post,
                // but WebFoundation does not like that. So we will remove the header
                // and instead truncate the data to the requested length.
                String contentLength = headerFields.get("Content-Length");

                if (!contentLength.isNull())
                    dataLength = min(contentLength.toInt(), (int)dataLength);
                headerFields.remove("Content-Length");

                postData += location;
                postDataLength = dataLength;
            }
        }
    }

    frameLoadRequest.resourceRequest().setHTTPMethod("POST");
    frameLoadRequest.resourceRequest().setURL(makeURL(m_baseURL, url));
    frameLoadRequest.resourceRequest().addHTTPHeaderFields(headerFields);
    frameLoadRequest.resourceRequest().setHTTPBody(PassRefPtr<FormData>(new FormData(postData, postDataLength)));
    frameLoadRequest.setFrameName(target);

    return load(frameLoadRequest, sendNotification, notifyData);
}

NPError PluginView::postURLNotify(const char* url, const char* target, uint32 len, const char* buf, NPBool file, void* notifyData)
{
    return handlePost(url, target, len, buf, file, notifyData, true, true);
}

NPError PluginView::postURL(const char* url, const char* target, uint32 len, const char* buf, NPBool file)
{
    // As documented, only allow headers to be specified via NPP_PostURL when using a file.
    return handlePost(url, target, len, buf, file, 0, false, file);
}

NPError PluginView::newStream(NPMIMEType type, const char* target, NPStream** stream)
{
    //notImplemented();
    // Unsupported
    return NPERR_GENERIC_ERROR;
}

int32 PluginView::write(NPStream* stream, int32 len, void* buffer)
{
    //notImplemented();
    // Unsupported
    return -1;
}

NPError PluginView::destroyStream(NPStream* stream, NPReason reason)
{
    PluginStream* browserStream = static_cast<PluginStream*>(stream->ndata);

    if (!stream || PluginStream::ownerForStream(stream) != m_instance)
        return NPERR_INVALID_INSTANCE_ERROR;

    browserStream->cancelAndDestroyStream(reason);
    return NPERR_NO_ERROR;
}

const char* PluginView::userAgent()
{
    if (m_quirks & PluginQuirkWantsMozillaUserAgent)
        return MozillaUserAgent;

    if (m_userAgent.isNull())
        m_userAgent = m_parentFrame->loader()->userAgent(m_url).utf8();
    return m_userAgent.data();
}

void PluginView::status(const char* message)
{
    String s = DeprecatedString::fromLatin1(message);

    if (Page* page = m_parentFrame->page())
        page->chrome()->setStatusbarText(m_parentFrame, s);
}

NPError PluginView::getValue(NPNVariable variable, void* value)
{
    switch (variable) {
        case NPNVxtAppContext: {
            if (!m_needsXEmbed) {
                *(void **)value = m_pluginViewPrivate->getContext();

                return NPERR_NO_ERROR;
            } else
                return NPERR_GENERIC_ERROR;
        }

        case NPNVToolkit: {
            *((unsigned long *)value) = m_pluginViewPrivate->nvToolkit();
            return NPERR_NO_ERROR;
        }

        case NPNVSupportsXEmbedBool: {
            *((unsigned long *)value) = m_pluginViewPrivate->supportsXEmbed();
            return NPERR_NO_ERROR;
        }

        case NPNVWindowNPObject: {
            NPObject* windowScriptObject = m_parentFrame->windowScriptNPObject();

            // Return value is expected to be retained, as described here: <http://www.mozilla.org/projects/plugin/npruntime.html>
            if (windowScriptObject)
                _NPN_RetainObject(windowScriptObject);

            void** v = (void**)value;
            *v = windowScriptObject;
            
            return NPERR_NO_ERROR;
        }

        case NPNVPluginElementNPObject: {
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

        case NPNVnetscapeWindow: {
            void* w = reinterpret_cast<void*>(value);
            *((XID *)w) = m_pluginViewPrivate->nvNetscapeWindow(frameGeometry());

            return NPERR_NO_ERROR;
        }
        default:
            return NPERR_GENERIC_ERROR;
    }
}

NPError PluginView::setValue(NPPVariable variable, void* value)
{
    switch (variable) {
        case NPPVpluginWindowBool:
            m_isWindowed = value;
            return NPERR_NO_ERROR;
        case NPPVpluginTransparentBool:
            m_isTransparent = value;
            return NPERR_NO_ERROR;
        default:
            //notImplemented();
            return NPERR_GENERIC_ERROR;
    }
}

void PluginView::invalidateTimerFired(Timer<PluginView>* timer)
{
    ASSERT(timer == &m_invalidateTimer);

    for (unsigned i = 0; i < m_invalidRects.size(); i++)
        Widget::invalidateRect(m_invalidRects[i]);
    m_invalidRects.clear();
}


void PluginView::invalidateRect(NPRect* rect)
{
    if (!rect) {
        invalidate();
        return;
    }

    IntRect r(rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top);

    if (m_isWindowed) {
        /* FIXME - Invalidate the rectangle
        RECT invalidRect(r);
        InvalidateRect(m_window, &invalidRect, FALSE);
        */
    } else {
        if (m_quirks & PluginQuirkThrottleInvalidate) {
            m_invalidRects.append(r);
            if (!m_invalidateTimer.isActive())
                m_invalidateTimer.startOneShot(0.001);
        } else
            Widget::invalidateRect(r);
    }
}

void PluginView::invalidateRegion(NPRegion region)
{
    if (m_isWindowed)
        return;

    /* FIXME - Invalidate the region
    RECT r;

    if (GetRgnBox(region, &r) == 0) {
        invalidate();
        return;
    }

    Widget::invalidateRect(r);
    */
}

void PluginView::forceRedraw()
{
    /* FIXME - Force a redraw
    if (m_isWindowed)
        ::UpdateWindow(m_window);
    else
        ::UpdateWindow(containingWindow());
    */
}

KJS::Bindings::Instance* PluginView::bindingInstance()
{
    NPObject* object = 0;

    if (!m_plugin || !m_plugin->pluginFuncs()->getvalue)
        return 0;

    NPError npErr;
    {
        KJS::JSLock::DropAllLocks dropAllLocks;
        npErr = m_plugin->pluginFuncs()->getvalue(m_instance, NPPVpluginScriptableNPObject, &object);
    }

    if (npErr != NPERR_NO_ERROR || !object)
        return 0;

    // FIXME - error: incomplete type ‘KJS::Bindings::Instance’ used in nested name specifier
    // error: incomplete type ‘KJS::Bindings::Instance’ used in nested name specifier
    //RefPtr<KJS::Bindings::RootObject> root = m_parentFrame->createRootObject(this, m_parentFrame->scriptProxy()->interpreter());
    //KJS::Bindings::Instance *instance = KJS::Bindings::Instance::createBindingForLanguageInstance(KJS::Bindings::Instance::CLanguage, object, root.release());

    //_NPN_ReleaseObject(object);

    //return instance;
    return NULL;
}

PluginView::~PluginView()
{
    stop();

    deleteAllValues(m_requests);

    freeStringArray(m_paramNames, m_paramCount);
    freeStringArray(m_paramValues, m_paramCount);

    m_parentFrame->cleanupScriptObjectsForPlugin(this);

    if(m_pluginViewPrivate)
        delete m_pluginViewPrivate;

    if (m_plugin && !(m_quirks & PluginQuirkDontUnloadPlugin))
        m_plugin->unload();
}

void PluginView::disconnectStream(PluginStream* stream)
{
    ASSERT(m_streams.contains(stream));

    m_streams.remove(stream);
}

void PluginView::determineQuirks(const String& mimeType)
{
    // The flash plugin only requests windowless plugins if we return a mozilla user agent
    if (mimeType == "application/x-shockwave-flash") {
        m_quirks |= PluginQuirkWantsMozillaUserAgent;
        m_quirks |= PluginQuirkThrottleInvalidate;
        m_quirks |= PluginQuirkThrottleWMUserPlusOneMessages;
    }

    // The WMP plugin sets its size on the first NPP_SetWindow call and never updates its size, so
    // call SetWindow when the plugin view has a correct size
    if (m_plugin->name().contains("Microsoft") && m_plugin->name().contains("Windows Media")) {
        m_quirks |= PluginQuirkDeferFirstSetWindowCall;

        // Windowless mode does not work at all with the WMP plugin so just remove that parameter 
        // and don't pass it to the plug-in.
        m_quirks |= PluginQuirkRemoveWindowlessVideoParam;
    }

    // The DivX plugin sets its size on the first NPP_SetWindow call and never updates its size, so
    // call SetWindow when the plugin view has a correct size
    if (mimeType == "video/divx")
        m_quirks |= PluginQuirkDeferFirstSetWindowCall;

    // FIXME: This is a workaround for a problem in our NPRuntime bindings; if a plug-in creates an
    // NPObject and passes it to a function it's not possible to see what root object that NPObject belongs to.
    // Thus, we don't know that the object should be invalidated when the plug-in instance goes away.
    // See <rdar://problem/5487742>.
    if (mimeType == "application/x-silverlight")
        m_quirks |= PluginQuirkDontUnloadPlugin;

    // Prevent the Real plugin from calling the Window Proc recursively, causing the stack to overflow.
    if (mimeType == "audio/x-pn-realaudio-plugin")
        m_quirks |= PluginQuirkDontCallWndProcForSameMessageRecursively;
}

void PluginView::setParameters(const Vector<String>& paramNames, const Vector<String>& paramValues)
{
    ASSERT(paramNames.size() == paramValues.size());

    unsigned size = paramNames.size();
    unsigned paramCount = 0;

    m_paramNames = reinterpret_cast<char**>(fastMalloc(sizeof(char*) * size));
    m_paramValues = reinterpret_cast<char**>(fastMalloc(sizeof(char*) * size));

    for (unsigned i = 0; i < size; i++) {
        if ((m_quirks & PluginQuirkRemoveWindowlessVideoParam) && equalIgnoringCase(paramNames[i], "windowlessvideo"))
            continue;

        m_paramNames[paramCount] = createUTF8String(paramNames[i]);
        m_paramValues[paramCount] = createUTF8String(paramValues[i]);

        paramCount++;
    }

    m_paramCount = paramCount;
}

PluginView::PluginView(Frame* parentFrame, const IntSize& size, PluginPackage* plugin, Element* element, const KURL& url, const Vector<String>& paramNames, const Vector<String>& paramValues, const String& mimeType, bool loadManually)
    : m_plugin(plugin)
    , m_parentFrame(parentFrame)
    , m_element(element)
    , m_isStarted(false)
    , m_url(url)
    , m_baseURL(m_parentFrame->loader()->completeURL(m_parentFrame->document()->baseURL()))
    , m_status(PluginStatusLoadedSuccessfully)
    , m_requestTimer(this, &PluginView::requestTimerFired)
    , m_invalidateTimer(this, &PluginView::invalidateTimerFired)
    , m_paramNames(0)
    , m_paramValues(0)
    , m_quirks(0)
    , m_window(0)
    , m_isWindowed(true)
    , m_isTransparent(false)
    , m_isVisible(false)
    , m_haveInitialized(false)
    , m_lastMessage(0)
    , m_isCallingPluginWndProc(false)
    , m_loadManually(loadManually)
    , m_manualStream(0)
    , m_needsXEmbed(false)
{
    m_pluginViewPrivate = new PluginViewPrivate();

    if (!m_plugin) {
        m_status = PluginStatusCanNotFindPlugin;
        return;
    }

    m_instance = &m_instanceStruct;
    m_instance->ndata = this;

    m_mimeType = mimeType.utf8();
    determineQuirks(mimeType);

    setParameters(paramNames, paramValues);

    m_mode = m_loadManually ? NP_FULL : NP_EMBED;

    resize(size);
}

void PluginView::init()
{
    if (m_haveInitialized)
        return;
    m_haveInitialized = true;

    if (!m_plugin) {
        ASSERT(m_status == PluginStatusCanNotFindPlugin);
        return;
    }

    if (!m_plugin->load()) {
        m_plugin = 0;
        m_status = PluginStatusCanNotLoadPlugin;
        return;
    }

    if (!start()) {
        m_status = PluginStatusCanNotLoadPlugin;
        return;
    }

    if (m_plugin->pluginFuncs()->getvalue)
        m_plugin->pluginFuncs()->getvalue(m_instance, NPPVpluginNeedsXEmbed, &m_needsXEmbed);

    m_pluginViewPrivate->setNeedsXEmbed(m_needsXEmbed);
    m_pluginViewPrivate->init(m_needsXEmbed, &m_npWindow, m_isWindowed, &m_window, frameGeometry());
    
    if (!(m_quirks & PluginQuirkDeferFirstSetWindowCall))
        setNPWindowRect(frameGeometry());

    m_status = PluginStatusLoadedSuccessfully;

}

void PluginView::didReceiveResponse(const ResourceResponse& response)
{
    ASSERT(!m_manualStream);

    m_manualStream = new PluginStream(this, m_parentFrame, m_parentFrame->loader()->activeDocumentLoader()->request(), false, 0);
    m_manualStream->setLoadManually(true);

    m_manualStream->didReceiveResponse(0, response);
}

void PluginView::didReceiveData(const char* data, int length)
{
    ASSERT(m_manualStream);
    
    m_manualStream->didReceiveData(0, data, length);
}

void PluginView::didFinishLoading()
{
    ASSERT(m_manualStream);

    m_manualStream->didFinishLoading(0);
}

void PluginView::didFail(const ResourceError& error)
{
    ASSERT(m_manualStream);

    m_manualStream->didFail(0, error);
}

} // namespace WebCore
