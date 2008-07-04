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

#ifndef WebMutableURLRequest_H
#define WebMutableURLRequest_H


/**
 *  @file  WebMutableURLRequest.h
 *  WebMutableURLRequest description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"
#include <ResourceRequest.h>
#include <ResourceRequest.h>
#include <ResourceRequestBase.h>

namespace WebCore
{
    class FormData;
    class String;
}

class HTTPHeaderPropertyBag;

class WebMutableURLRequest
{
public:

    /**
     * create a new instance of WebMutableURLRequest
     */
    static WebMutableURLRequest* createInstance();

    /**
     * create a new instance of WebMutableURLRequest
     */
    static WebMutableURLRequest* createInstance(WebMutableURLRequest* req);

    /**
     * create a new instance of WebMutableURLRequest
     */
    static WebMutableURLRequest* createInstance(const WebCore::ResourceRequest&);


    /**
     * create an immutable instance of WebMutableURLRequest
     */
    static WebMutableURLRequest* createImmutableInstance();

    /**
     * create an immutable instance of WebMutableURLRequest
     */
    static WebMutableURLRequest* createImmutableInstance(const WebCore::ResourceRequest&);
protected:

    /**
     *  WebMutableURLRequest constructor
     */
    WebMutableURLRequest(bool isMutable);

public:

    /**
     * WebMutableURLRequest destructor
     */
    virtual ~WebMutableURLRequest();

    /**
     * request with URL 
     */
    virtual void requestWithURL( WebCore::String theURL, WebCore::ResourceRequestCachePolicy cachePolicy, double timeoutInterval);

    /**
     * get all HTTP header fields 
     */
    virtual HTTPHeaderPropertyBag* allHTTPHeaderFields();

    /**
     * get cachePolicy 
     */
    virtual WebCore::ResourceRequestCachePolicy cachePolicy();

    /**
     * get HTTP body
     */
    virtual WebCore::String HTTPBody();

    /**
     * get HTTP body stream
     */
    virtual WebCore::String HTTPBodyStream();

    /**
     * get HTTP method
     */
    virtual WebCore::String HTTPMethod();

    /**
     * get HTTP should handle cookies
     */
    virtual bool HTTPShouldHandleCookies();

    /**
     * initialise WebMutableURLRequest with URL 
     */
    virtual void initWithURL(WebCore::String url, WebCore::ResourceRequestCachePolicy cachePolicy, double timeoutInterval);

    /**
     * get main document URL
     */
    virtual WebCore::String mainDocumentURL();

    /**
     * get timeout interval
     */
    virtual double timeoutInterval();

    /**
     * get URL
     */
    virtual WebCore::String URL();

    /**
     * get value for HTTP header field
     */
    virtual WebCore::String valueForHTTPHeaderField(WebCore::String field);

    /**
     * test if the WebMutableURLRequest is empty
     */
    virtual bool isEmpty();


    /**
     * add value
     */
    virtual void addValue(WebCore::String value, WebCore::String field);

    /**
     * set all HTTP header fields
     */
    virtual void setAllHTTPHeaderFields(HTTPHeaderPropertyBag *headerFields);

    /**
     * set cache policy
     */
    virtual void setCachePolicy(WebCore::ResourceRequestCachePolicy policy);

    /**
     * set HTTP body
     */
    virtual void setHTTPBody(WebCore::String data);

    /**
     * set HTTP body stream
     */
    virtual void setHTTPBodyStream(WebCore::String data);

    /**
     * set HTTP method
     */
    virtual void setHTTPMethod(WebCore::String method);

    /**
     * set HTTP should handle cookies
     */
    virtual void setHTTPShouldHandleCookies(bool handleCookies);

    /**
     * set main document URL
     */
    virtual void setMainDocumentURL(WebCore::String theURL);

    /**
     * set timeout interval
     */
    virtual void setTimeoutInterval(double timeoutInterval);

    /**
     * set URL
     */
    virtual void setURL(WebCore::String theURL);

    /**
     * set value
     */
    virtual void setValue(WebCore::String value, WebCore::String field);

    /**
     * set allows any HTTPS certificate
     */
    virtual void setAllowsAnyHTTPSCertificate();


    /**
     * setClientCertificate
     */
    //virtual void setClientCertificate(OLE_HANDLE cert);


    /**
     * set form data
     */
    void setFormData(const PassRefPtr<WebCore::FormData> data);
    
    /**
     * get form data
     */
    const PassRefPtr<WebCore::FormData> formData() const;
    

    /**
     * add HTTP header fields
     */
    void addHTTPHeaderFields(const WebCore::HTTPHeaderMap& headerFields);
    
    /**
     * get resource request
     */
    const WebCore::HTTPHeaderMap& httpHeaderFields() const;

    const WebCore::ResourceRequest& resourceRequest() const;
protected:
    bool m_isMutable;
    WebCore::ResourceRequest m_request;
};

#endif
