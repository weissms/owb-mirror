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
/**
 * @file BIResourceHandle.h
 *
 * Interface of Transfer Job
 */
#ifndef BIResourceHandle_H_
#define BIResourceHandle_H_

#include "AuthenticationChallenge.h"
#include "HTTPHeaderMap.h"
#include <Shared.h>
#include <SharedBuffer.h>
#include <wtf/OwnPtr.h>

namespace WebCore {
class AuthenticationChallenge;
class Credential;
class FormData;
class Frame;
class KURL;
class ResourceError;
class ResourceHandleClient;
class ResourceHandleInternal;
class ResourceRequest;
class ResourceResponse;
class SharedBuffer;
class SubresourceLoader;
class SubresourceLoaderClient;
}

using WebCore::FormData;
using WebCore::Frame;
using WebCore::HTTPHeaderMap;
using WebCore::ResourceError;
using WebCore::ResourceHandleClient;
using WebCore::ResourceRequest;
using WebCore::ResourceResponse;
using WebCore::Shared;
using WebCore::KURL;
using WebCore::String;


namespace BAL {

class BIResourceHandle : public Shared<BIResourceHandle> {

public:
    virtual ~BIResourceHandle() {}
    /**
     * Concrete Constructor will set BIResourceHandleClient*, method, and url.
     * If need, it will set form postData.
     */

    /**
     * returns the url
     */
    virtual KURL url() const = 0;

    /**
     * returns the method
     */
    virtual WebCore::String method() const = 0;

    /**
     * returns the post data
     */
    virtual PassRefPtr<FormData> postData() const = 0;

    /**
     * returns the client
     */
    virtual ResourceHandleClient* client() const = 0;
    virtual void setClient(ResourceHandleClient*) = 0;

    /**
     * test if ResourceHandle loads is blocked
     */
    static bool loadsBlocked();

    /**
     * clear authentification
     */
    virtual void clearAuthentication() = 0;

    /**
     * cancel the loading
     */
    virtual void cancel() = 0;

    /**
     * load resource synchronously
     */
    static void loadResourceSynchronously(const ResourceRequest&, ResourceError&, ResourceResponse&, Vector<char>& data);

    /**
     * test if the resource will load from cache 
     */
    static bool willLoadFromCache(ResourceRequest&);

    /**
     * return the bufferData
     */
    virtual PassRefPtr<WebCore::SharedBuffer> bufferedData() = 0;

    /**
     * test if the ResourceHandle supports buffered data
     */
    static bool supportsBufferedData();

    /**
     * set if the ResourceHandle defers loading
     */
    virtual void setDefersLoading(bool) = 0;
    /**
     * get request headers
     */
    virtual const HTTPHeaderMap& requestHeaders() const = 0;

    /**
     * get request
     */
    virtual const ResourceRequest& request() const = 0;

    /**
     * create the ResourceHandle
     */
    static PassRefPtr<BIResourceHandle> create(const WebCore::ResourceRequest&, ResourceHandleClient*, Frame*, bool defersLoading, bool shouldContentSniff, bool mightDownloadFromHandle = false);
  };

}

#endif // BIResourceHandle_H_
