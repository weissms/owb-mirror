/*
 * Copyright (C) 2010 Pleyo.  All rights reserved.
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
#ifndef HistoryDelegate_h
#define HistoryDelegate_h

#include <WebKit.h>

class HistoryDelegate : public WebHistoryDelegate {

public:
    static TransferSharedPtr<HistoryDelegate> createInstance() {
        return new HistoryDelegate();
    }

    virtual ~HistoryDelegate()
    {
    }

    virtual void didNavigateWithNavigationData(WebView* webView, WebNavigationData* navigationData, WebFrame* webFrame);
    virtual void didPerformClientRedirectFromURL(WebView* webView, const char* sourceURL, const char* destinationURL, WebFrame* webFrame);
    virtual void didPerformServerRedirectFromURL(WebView* webView, const char* sourceURL, const char* destinationURL, WebFrame* webFrame);
    virtual void updateHistoryTitle(WebView* webView, const char* title, const char* url);
    virtual void populateVisitedLinksForWebView(WebView* webView);
private:
    HistoryDelegate()
    {
    }
};

#endif //HistoryDelegate_h
