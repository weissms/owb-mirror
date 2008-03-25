/*
 * Copyright (C) 2007 Pleyo.  All rights reserved.
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
#ifndef BIWINDOW_H_
#define BIWINDOW_H_

namespace WebCore {

    class IntRect;
    class Frame;
    class FrameLoaderClientBal;
    class KURL;

}

namespace BAL {

    class BIPainter;
    class BIEvent;

    class BIWindow {
        public:
            /**
            * event handler
            */
            virtual void handleEvent(BAL::BIEvent *event) = 0;
            /**
            * test if it can go back or forward
            */
            virtual bool canGoBackOrForward(int distance) = 0;
            /**
            * go back or forward
            */
            virtual void goBackOrForward(int distance) = 0;
            /**
            * set the url
            */
            virtual void setURL(const WebCore::KURL& url) = 0;
            virtual WebCore::String executeJavaScript(WebCore::String script) = 0;
            /**
            * stop the loading
            */
            virtual void stop() = 0;
            /**
            * get url
            */
            virtual const WebCore::KURL& URL() = 0;
            /**
            * get the widget
            */
            virtual const BTWidget* widget() const = 0;

            /**
            * get the main Frame
            */
            virtual WebCore::Frame* mainFrame() const = 0;

            /**
            * set frameLoaderClient
            */
            virtual void setFrameLoaderClient(WebCore::FrameLoaderClientBal*) = 0;
            // this is mandatory
            /**
            * BIWindow destructor
            */
            virtual ~BIWindow() {};

    };

}

#define IMPLEMENT_BIWINDOW  \
    public: \
    virtual void handleEvent(BAL::BIEvent *event);\
    virtual bool canGoBackOrForward(int distance); \
    virtual void goBackOrForward(int distance); \
    virtual void setURL(const KURL& url);\
    virtual WebCore::String executeJavaScript(WebCore::String script);\
    virtual void stop();\
    virtual const WebCore::KURL& URL();\
    virtual const BTWidget* widget() const;\
    virtual WebCore::Frame* mainFrame() const; \
    virtual void setFrameLoaderClient(WebCore::FrameLoaderClientBal*);

#endif // BIWINDOW_H
