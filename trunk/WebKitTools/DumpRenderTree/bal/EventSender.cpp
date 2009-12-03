/*
 * Copyright (C) 2009 Pleyo.  All rights reserved.
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

//#include "config.h"
#include "EventSender.h"

#include "DumpRenderTree.h"

#include <JavaScriptCore/JSRetainPtr.h>
#include <JSBase.h>
#include <JSObjectRef.h>
#include <JSValueRef.h>
#include <JSStringRef.h>
#include <WebKit.h>

#include <sys/time.h> 
#include <unistd.h>
#include <ctype.h>

#if ENABLE(CEHTML)
#include "VKKeyConversion.h"
#endif

static bool down;
static bool dragMode = true;
static int timeOffset;
static unsigned eventStartTime;
struct Point {
    int x;
    int y;
};

static Point lastMousePosition;

static unsigned endOfQueue;
static unsigned startOfQueue;
static bool didDragEnter;

static inline EventSenderController* toEventSenderController(JSObjectRef object)
{
    // FIXME: We should ASSERT that it is the right class here.
    return static_cast<EventSenderController*>(JSObjectGetPrivate(object));
}

static JSValueRef getDragModeCallback(JSContextRef context, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception)
{
    return JSValueMakeBoolean(context, dragMode);
}

static bool setDragModeCallback(JSContextRef context, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception)
{
    dragMode = JSValueToBoolean(context, value);
    return true;
}

static JSValueRef leapForwardCallback(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    /*if (argumentCount > 0) {
        setDelay(endOfQueue, JSValueToNumber(context, arguments[0], exception));
    }*/

    return JSValueMakeUndefined(context);
}

static JSValueRef contextClickCallback(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    EventSenderController* eventSender = toEventSenderController(thisObject);
    down = true;
    eventSender->dispatchMouseButton(eventSender->makeMouseButtonEvent(false, lastMousePosition.x, lastMousePosition.y));

    down = false;
    eventSender->dispatchMouseButton(eventSender->makeMouseButtonEvent(true, lastMousePosition.x, lastMousePosition.y));
    
    return JSValueMakeUndefined(context);
}

static JSValueRef mouseDownCallback(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    down = true;
    EventSenderController* eventSender = toEventSenderController(thisObject);
    eventSender->dispatchMouseButton(eventSender->makeMouseButtonEvent(false, lastMousePosition.x, lastMousePosition.y));

    return JSValueMakeUndefined(context);
}

static JSValueRef mouseUpCallback(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    EventSenderController* eventSender = toEventSenderController(thisObject);
    eventSender->dispatchMouseButton(eventSender->makeMouseButtonEvent(true, lastMousePosition.x, lastMousePosition.y));
    down = false;

    return JSValueMakeUndefined(context);
}

static JSValueRef mouseMoveToCallback(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    if (argumentCount < 2)
        return JSValueMakeUndefined(context);

    lastMousePosition.x = (int)JSValueToNumber(context, arguments[0], exception);
    lastMousePosition.y = (int)JSValueToNumber(context, arguments[1], exception);

    EventSenderController* eventSender = toEventSenderController(thisObject);
    eventSender->dispatchMouseMotion(eventSender->makeMouseMotionEvent(down, lastMousePosition.x, lastMousePosition.y));

    return JSValueMakeUndefined(context);
}

static JSValueRef keyDownCallback(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    if (argumentCount < 1)
        return JSValueMakeUndefined(context);
    EventSenderController* eventSender = toEventSenderController(thisObject);

    static const JSStringRef lengthProperty = JSStringCreateWithUTF8CString("length");

    JSStringRef character = JSValueToStringCopy(context, arguments[0], exception);
    int charCode = 0;
    bool needsShiftKeyModifier = false;
    bool isVKKey = false; // This is only set when CEHTML is activated.
    if (JSStringIsEqualToUTF8CString(character, "leftArrow") 
     || JSStringIsEqualToUTF8CString(character, "rightArrow")
     || JSStringIsEqualToUTF8CString(character, "upArrow")
     || JSStringIsEqualToUTF8CString(character, "downArrow")
     || JSStringIsEqualToUTF8CString(character, "pageUp")
     || JSStringIsEqualToUTF8CString(character, "pageDown")
     || JSStringIsEqualToUTF8CString(character, "home")
     || JSStringIsEqualToUTF8CString(character, "end")
     || JSStringIsEqualToUTF8CString(character, "delete")) {
        charCode = eventSender->getCharCode(JSStringCopyUTF8CString(character));
    } else {
#if ENABLE(CEHTML)
        char vkValue[32];
        JSStringGetUTF8CString(character, vkValue, 32);
        charCode = VKKeyValueFromString(vkValue);
        if (charCode > 0)
            isVKKey = true;
        else
#endif
        {
            charCode = JSStringGetCharactersPtr(character)[0];
            if (isupper(charCode))
                needsShiftKeyModifier = true;
        }
    }
    JSStringRelease(character);

    bool shift = needsShiftKeyModifier;
    bool control = false;
    bool alt = false;
    bool meta = false;
    if (argumentCount > 1) {
        JSObjectRef modifiersArray = JSValueToObject(context, arguments[1], 0);
        if (modifiersArray) {
            int modifiersCount = JSValueToNumber(context, JSObjectGetProperty(context, modifiersArray, lengthProperty, 0), 0);
            for (int i = 0; i < modifiersCount; ++i) {
                JSValueRef value = JSObjectGetPropertyAtIndex(context, modifiersArray, i, 0);
                JSStringRef string = JSValueToStringCopy(context, value, 0);
                if (JSStringIsEqualToUTF8CString(string, "ctrlKey"))
                    control = true;
                else if (JSStringIsEqualToUTF8CString(string, "shiftKey"))
                    shift = true;
                else if (JSStringIsEqualToUTF8CString(string, "altKey"))
                    alt = true;
                else if (JSStringIsEqualToUTF8CString(string, "meta"))
                    meta = true;

                JSStringRelease(string);
            }
        }
    }

    BalEventKey ek = eventSender->makeKeyboardEvent(false, shift, control, alt, meta, charCode, isVKKey);
    eventSender->dispatchKeyboardEvent(ek); 

    return JSValueMakeUndefined(context);
}

static JSValueRef textZoomInCallback(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    WebView* webView = getWebView();
    webView->makeTextLarger();
    return JSValueMakeUndefined(context);
}

static JSValueRef textZoomOutCallback(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    WebView* webView = getWebView();
    webView->makeTextSmaller();
    return JSValueMakeUndefined(context);
}

static JSValueRef zoomPageInCallback(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    WebView* webView = getWebView();
    webView->zoomPageIn();
    return JSValueMakeUndefined(context);
}

static JSValueRef zoomPageOutCallback(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    WebView* webView = getWebView();
    webView->zoomPageOut();
    return JSValueMakeUndefined(context);
}

static JSValueRef clearKillRingCallback(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    return JSValueMakeUndefined(context);
}

JSClassRef EventSenderController::getJSClass()
{
    static JSStaticFunction staticFunctions[] = {
        { "contextClick", contextClickCallback, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete },
        { "mouseDown", mouseDownCallback, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete },
        { "mouseUp", mouseUpCallback, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete },
        { "mouseMoveTo", mouseMoveToCallback, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete },
        { "leapForward", leapForwardCallback, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete },
        { "keyDown", keyDownCallback, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete },
        { "textZoomIn", textZoomInCallback, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete },
        { "textZoomOut", textZoomOutCallback, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete },
        { "zoomPageIn", zoomPageInCallback, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete },
        { "zoomPageOut", zoomPageOutCallback, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete },
        { "clearKillRing", clearKillRingCallback, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete },
        { 0, 0, 0}
    };

    static JSStaticValue staticValues[] = {
        { "dragMode", getDragModeCallback, setDragModeCallback, kJSPropertyAttributeNone },
        { 0, 0, 0, 0 }
    };

    static JSClassDefinition classDefinition = {
        0, kJSClassAttributeNone, "EventSenderController", 0, staticValues, staticFunctions,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    static JSClassRef accessibilityControllerClass = JSClassCreate(&classDefinition);
    return accessibilityControllerClass;
}


EventSenderController::EventSenderController()
{
    down = false;
    dragMode = true;
    struct timezone tz;
    struct timeval val;
    gettimeofday(&val, &tz);
    eventStartTime = val.tv_sec;
    timeOffset = 0;
    lastMousePosition.x = 0;
    lastMousePosition.y = 0;

    endOfQueue = 0;
    startOfQueue = 0;

    didDragEnter = false;
    //draggingInfo = 0;
}

EventSenderController::~EventSenderController()
{
}

void EventSenderController::makeWindowObject(JSContextRef context, JSObjectRef windowObject, JSValueRef* exception)
{
    JSRetainPtr<JSStringRef> eventSenderStr(Adopt, JSStringCreateWithUTF8CString("eventSender"));
    JSValueRef eventSenderObject = JSObjectMake(context, getJSClass(), this);
    JSObjectSetProperty(context, windowObject, eventSenderStr.get(), eventSenderObject, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete, exception);
}

