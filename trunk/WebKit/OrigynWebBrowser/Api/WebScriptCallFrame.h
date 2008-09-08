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

#ifndef WebScriptCallFrame_h
#define WebScriptCallFrame_h


/**
 *  @file  WebScriptCallFrame.h
 *  WebScriptCallFrame description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"
#include "PropertyNameArray.h"
#include "PropertyNameArray.h"

namespace JSC {
    class ExecState;
    class JSValue;
    class UString;
}

namespace WebCore {
    class String;
}

class WebScriptCallFrame {
public:

    /**
     * create a new instance of WebScriptCallFrame
     */
    static WebScriptCallFrame* createInstance(JSC::ExecState*);

private:

    /**
     * WebScriptCallFrame constructor
     */
    WebScriptCallFrame(JSC::ExecState*);

public:

    /**
     * WebScriptCallFrame destructor
     */
    virtual ~WebScriptCallFrame();

    /**
     *  get caller 
     */
    virtual WebScriptCallFrame* caller();

    /**
     * get functionName 
     */
    virtual WebCore::String functionName();

    /**
     * string by evaluating javascript from string
     */
    virtual WebCore::String stringByEvaluatingJavaScriptFromString(WebCore::String script);

    /**
     * get variable names
     */
    virtual JSC::PropertyNameArray variableNames();

    /**
     * get value for variable
     */
    virtual WebCore::String valueForVariable(WebCore::String key);


    /**
     * get value by evaluating java script from string
     */
    virtual JSC::JSValue* valueByEvaluatingJavaScriptFromString(WebCore::String script);

    /**
     * get state
     */
    virtual JSC::ExecState* state() const { return m_state; }


    /**
     * convert jsValue to string
     */
    static JSC::UString jsValueToString(JSC::ExecState*, JSC::JSValue*);

private:

    JSC::ExecState* m_state;
};

#endif
