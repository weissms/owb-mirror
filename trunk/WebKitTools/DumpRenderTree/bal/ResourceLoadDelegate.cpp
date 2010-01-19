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

#include "ResourceLoadDelegate.h"

#include "DumpRenderTree.h"
#include "LayoutTestController.h"
#include <sstream>
#include <vector>
#include <map>
#include <string>

using std::string;
using std::iostream;

string urlSuitable(string& url)
{
    if (!url.c_str() || url.find("file://") == string::npos)
        return url;

    string newUrl;
    size_t pos = url.rfind("resources");
    if (pos == string::npos) {
        size_t p = url.find_last_of("/"); 
        newUrl = url.substr(p + 1);
    } else
        newUrl = url.substr(pos);
    return newUrl;
}

string stringFromInt(int i)
{
    std::ostringstream ss;
    ss << i;
    return ss.str();
}

typedef map<unsigned long, string> IdentifierMap;

IdentifierMap& urlMap()
{
    static IdentifierMap urlMap;

    return urlMap;
}

static string descriptionSuitableForTestResult(unsigned long identifier)
{
    IdentifierMap::iterator it = urlMap().find(identifier);
    
    if (it == urlMap().end())
        return "<unknown>";

    return urlSuitable(it->second);
}

static string descriptionSuitableForTestResult(WebMutableURLRequest* request)
{
    if (!request)
        return "(null)";

    string requestUrl = request->URL();
    
    string url = urlSuitable(requestUrl);
    
    string requestMainDocumentURL = request->mainDocumentURL();
    
    string mainDocumentURL = urlSuitable(requestMainDocumentURL);

    return "<NSURLRequest URL " + url + ", main document URL " + mainDocumentURL + ">";
}

static string descriptionSuitableForTestResult(WebURLResponse* response)
{
    if (!response)
        return "(null)";

    string responseUrl = response->URL();
    
    string url = urlSuitable(responseUrl);

    return "<NSURLResponse " + url + ">";
}

static string descriptionSuitableForTestResult(WebError* error, unsigned long identifier)
{
    string result = "<NSError ";

    const char* errorDomain = error->domain();

    string domain = errorDomain;

    int code = error->code();

    if (domain == "CFURLErrorDomain") {
        domain = "NSURLErrorDomain";

        // Convert kCFURLErrorUnknown to NSURLErrorUnknown
        if (code == -998)
            code = -1;
    } else if (domain == "kCFErrorDomainWinSock") {
        domain = "NSURLErrorDomain";

        // Convert the winsock error code to an NSURLError code.
/*        if (code == WSAEADDRNOTAVAIL)
            code = -1004; // NSURLErrorCannotConnectToHose;*/
    }

    result += "domain " + domain;
    result += ", code " + stringFromInt(code);

    string errorFailingURL = error->failingURL();

    string failingURL;
    
    // If the error doesn't have a failing URL, we fake one by using the URL the resource had 
    // at creation time. This seems to work fine for now.
    // See <rdar://problem/5064234> CFErrors should have failingURL key.
    if (!errorFailingURL.empty())
        failingURL = errorFailingURL;
    else
        failingURL = descriptionSuitableForTestResult(identifier);

    result += ", failing URL \"" + urlSuitable(failingURL) + "\">";

    return result;
}

TransferSharedPtr<ResourceLoadDelegate> ResourceLoadDelegate::createInstance()
{
    return new ResourceLoadDelegate();
}

ResourceLoadDelegate::ResourceLoadDelegate()
{
}

ResourceLoadDelegate::~ResourceLoadDelegate()
{
}

void ResourceLoadDelegate::identifierForInitialRequest(WebView* webView, WebMutableURLRequest* request, WebDataSource* dataSource, unsigned long identifier)
{ 
    if (!done && gLayoutTestController->dumpResourceLoadCallbacks())
        urlMap()[identifier] = request->URL();
}

WebMutableURLRequest* ResourceLoadDelegate::willSendRequest(WebView* webView, unsigned long identifier, WebMutableURLRequest* request, WebURLResponse* redirectResponse, WebDataSource* dataSource)
{
    if (!done && gLayoutTestController->dumpResourceLoadCallbacks())
        printf("%s - willSendRequest %s redirectResponse %s\n", descriptionSuitableForTestResult(identifier).c_str(), descriptionSuitableForTestResult(request).c_str(), descriptionSuitableForTestResult(redirectResponse).c_str());

    if (!done && gLayoutTestController->willSendRequestReturnsNull())
        return 0;

    if (!done && gLayoutTestController->willSendRequestReturnsNullOnRedirect() && redirectResponse) {
        printf("Returning null for this redirect\n");
        return 0;
    }

    const char* url = request->URL();
    const char* host = request->host();
    const char* protocol = request->protocol();
    if (host
        && (!strcmp(protocol,"http") || !strcmp(protocol,"https"))
        && strcmp(host,"127.0.0.1")
        && strcmp(host,"255.255.255.255") // used in some tests that expect to get back an error
        && strcmp(host,"localhost")) {
        printf("Blocked access to external URL %s\n", url);
        return 0;
    }

    for(size_t i = 0; i < disallowedURLs.size(); ++i) {
        if (!strcmp(url, disallowedURLs[i].c_str()))
            return 0;
    }

    return request;
}

void ResourceLoadDelegate::didFinishLoadingFromDataSource(WebView* webView, unsigned long identifier, WebDataSource* dataSource)
{
    if (!done && gLayoutTestController->dumpResourceLoadCallbacks()) {
        printf("%s - didFinishLoading\n", descriptionSuitableForTestResult(identifier).c_str());
        urlMap().erase(identifier);
    }
}
        
void ResourceLoadDelegate::didFailLoadingWithError(WebView* webView, unsigned long identifier, WebError* error, WebDataSource* dataSource)
{
    if (!done && gLayoutTestController->dumpResourceLoadCallbacks()) {
        printf("%s - didFailLoadingWithError: %s\n", descriptionSuitableForTestResult(identifier).c_str(), descriptionSuitableForTestResult(error, identifier).c_str());
        urlMap().erase(identifier);
    }
}

void ResourceLoadDelegate::didReceiveResponse(WebView *webView, unsigned long identifier, WebURLResponse *response, WebDataSource *dataSource) 
{ 
    
    if (!done && gLayoutTestController->dumpResourceLoadCallbacks())
        printf("%s - didReceiveResponse %s\n", descriptionSuitableForTestResult(identifier).c_str(), descriptionSuitableForTestResult(response).c_str());

    if (!done && gLayoutTestController->dumpResourceResponseMIMETypes())
        printf("%s has MIME type %s\n", response->URL(), response->MIMEType());
}
