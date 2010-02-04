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

#include "PolicyDelegate.h"

#include "DumpRenderTree.h"
#include "LayoutTestController.h"
#include <string>

using std::wstring;
using std::string;

string dumpPath(DOMNode* node)
{
    string result;

    string name = node->nodeName();
    result += name;

    DOMNode* parent = node->parentNode();
    if (parent)
        result += " > " + dumpPath(parent);

    return result;
}

TransferSharedPtr<PolicyDelegate> PolicyDelegate::createInstance()
{
    return new PolicyDelegate();
}

PolicyDelegate::PolicyDelegate()
    : m_permissiveDelegate(false)
    , m_controllerToNotifyDone(0)
{
}

void PolicyDelegate::decidePolicyForNavigationAction(
        /* [in] */ WebView *webView,
        /* [in] */ WebNavigationAction *actionInformation,
        /* [in] */ WebMutableURLRequest *request,
        /* [in] */ WebFrame *frame,
        /* [in] */ WebFramePolicyListener *listener)
{
    string url = request->URL();
    WebNavigationType navType = actionInformation->type();
    
    //wstring wurl = urlSuitableForTestResult(wstring(url, SysStringLen(url)));

    string typeDescription;
    switch (navType) {
        case WebNavigationTypeLinkClicked:
            typeDescription = "link clicked";
            break;
        case WebNavigationTypeFormSubmitted:
            typeDescription = "form submitted";
            break;
        case WebNavigationTypeBackForward:
            typeDescription = "back/forward";
            break;
        case WebNavigationTypeReload:
            typeDescription = "reload";
            break;
        case WebNavigationTypeFormResubmitted:
            typeDescription = "form resubmitted";
            break;
        case WebNavigationTypeOther:
            typeDescription = "other";
            break;
        default:
            typeDescription = "illegal value";
    }

    string newUrl;
    if (!url.c_str() || url.find("file://") == string::npos)
        newUrl = url;
    else {
        size_t pos = url.find_last_of("/");
        newUrl = url.substr(pos + 1);
    }

    string message = "Policy delegate: attempt to load " + newUrl + " with navigation type '" + typeDescription + "'";

    WebHitTestResults *results = actionInformation->webHitTestResults();
    if (results) {
        DOMNode* originatingNode = results->domNode();
        if (originatingNode)
            message += " originating from " + dumpPath(originatingNode);
    }

    printf("%s\n", message.c_str());

    if (m_permissiveDelegate)
        listener->use();
    else
        listener->ignore();

    if (m_controllerToNotifyDone) {
        m_controllerToNotifyDone->notifyDone();
        m_controllerToNotifyDone = 0;
    }
}


void PolicyDelegate::unableToImplementPolicyWithError(
        /* [in] */ WebView *webView,
        /* [in] */ WebError *error,
        /* [in] */ WebFrame *frame)
{
    string domainMessage = error->domain();

    int code = error->code();
    
    string frameNameMessage = frame->name();
    
    printf("Policy delegate: unable to implement policy with error domain '%s', error code %d, in frame '%s'", domainMessage.c_str(), code, frameNameMessage.c_str());
}

bool PolicyDelegate::decidePolicyForMIMEType(WebView *webView, const char* type, WebMutableURLRequest *request, WebFrame *frame, WebFramePolicyListener *listener) 
{ 
    return false; 
}
    
