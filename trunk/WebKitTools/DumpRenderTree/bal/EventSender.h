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

#ifndef EventSender_h
#define EventSender_h

//class DraggingInfo;
#include <JavaScriptCore/JSObjectRef.h>
#include "WebKit.h"


typedef const struct OpaqueJSContext* JSContextRef;
typedef struct OpaqueJSValue* JSObjectRef;


class EventSenderController {
public:
    EventSenderController();
    ~EventSenderController();

    BalEventMotion makeMouseMotionEvent(bool state, int x, int y);
    void dispatchMouseMotion(BalEventMotion);

    BalEventButton makeMouseButtonEvent(bool up, int x, int y);
    void dispatchMouseButton(BalEventButton);

    BalEventKey makeKeyboardEvent(bool up, bool shift, bool control, bool alt, bool meta, int charCode, bool isVKKey);
    void dispatchKeyboardEvent(BalEventKey);
    int getCharCode(char *);

    void makeWindowObject(JSContextRef context, JSObjectRef windowObject, JSValueRef* exception);

private:
    static JSClassRef getJSClass();
};

void makeEventSenderWindowObject(JSContextRef context, JSObjectRef windowObject, JSValueRef* exception);
JSObjectRef makeEventSender(JSContextRef context);

//extern DraggingInfo* draggingInfo;

#endif
