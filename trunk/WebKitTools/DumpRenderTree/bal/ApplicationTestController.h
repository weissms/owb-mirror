/*
 * Copyright (C) 2009 Pleyo. All rights reserved.
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
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY PLEYO AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#ifndef ApplicationTestController_h
#define ApplicationTestController_h

#include "WebKit.h"
#include <JavaScriptCore/JSObjectRef.h>

#if PLATFORM(BAL)

typedef enum {
    ChannelConfigType,
} ConfigFileType;

class ApplicationTestController {
public:
    ApplicationTestController();
    ~ApplicationTestController();

    void makeWindowObject(JSContextRef context, JSObjectRef windowObject, JSValueRef* exception);

    void fillConfigurationFromFile(char*, ConfigFileType);

    bool showChildApplications() { return m_showChildApplications; }
    void setShowChildApplications(bool showChildApplications) { m_showChildApplications = showChildApplications; }

private:
    static JSClassRef getJSClass();
    static JSStaticValue* staticValues();
    static JSStaticFunction* staticFunctions();

    bool m_showChildApplications;
};

#endif // PLATFORM(BAL)

#endif // ApplicationTestController_h
