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

#ifndef WebScriptDebugServer_H
#define WebScriptDebugServer_H



/**
 *  @file  WebScriptDebugServer.h
 *  WebScriptDebugServer description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"
#include <JavaScriptDebugListener.h>
#include <JavaScriptDebugListener.h>
#include "PlatformString.h"

namespace WebCore {
    class Page;
}

class WebView;
class WebDataSource;
class WebFrame;
class WebScriptCallFrame;

class WebScriptDebugListener {
public:
    /**
     * did load main resource for data source
     */
    void didLoadMainResourceForDataSource(WebView* view, WebDataSource* dataSource) {};

    /**
     * did parse source
     */
    void didParseSource(WebView* view, WebCore::String sourceCode, unsigned baseLineNumber, WebCore::String url, int sourceID, WebFrame* forWebFrame) {};

    /**
     * failed to parse source
     */
    void failedToParseSource(WebView* view, WebCore::String sourceCode, unsigned baseLineNumber, WebCore::String url, WebCore::String error, WebFrame* forWebFrame) {};

    /**
     * did enter call frame
     */
    void didEnterCallFrame(WebView* view, WebScriptCallFrame* frame, int sourceID, int lineNumber, WebFrame* forWebFrame) {};

    /**
     * will execute statement
     */
    void willExecuteStatement(WebView* view, WebScriptCallFrame* frame, int sourceID, int lineNumber, WebFrame* forWebFrame) {};

    /**
     * will leave call frame
     */
    void willLeaveCallFrame(WebView* view, WebScriptCallFrame* frame, int sourceID, int lineNumber, WebFrame* forWebFrame) {};

    /**
     * exception was raised
     */
    void exceptionWasRaised(WebView* view, WebScriptCallFrame* frame, int sourceID, int lineNumber, WebFrame* forWebFrame) {};

    /**
     * server did die
     */
    void serverDidDie() {};
};

class WebScriptDebugServer { //: public WebCore::JavaScriptDebugListener {
public:

    /**
     * create a new instance of WebScriptDebugServer
     */
    static WebScriptDebugServer* createInstance();

    /**
     * get the shared WebScriptDebugServer
     */
    static WebScriptDebugServer* sharedWebScriptDebugServer();


    /**
     * WebScriptDebugServer destructor
     */
    virtual ~WebScriptDebugServer();


    /**
     * add listener
     */
    virtual void addListener(WebScriptDebugListener*);

    /**
     * remove listener
     */
    virtual void removeListener(WebScriptDebugListener*);

    /**
     * step
     */
    virtual void step();

    /**
     * pause
     */
    virtual void pause();

    /**
     * resume
     */
    virtual void resume();

    /**
     * isPaused
     */
    virtual bool isPaused();

private:

    /**
     * WebScriptDebugServer constructor
     */
    WebScriptDebugServer();


    bool m_paused;
    bool m_step;
    bool m_callingListeners;
};

#endif
