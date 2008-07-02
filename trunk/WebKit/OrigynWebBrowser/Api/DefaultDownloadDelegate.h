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
#ifndef DefaultDownloadDelegate_h
#define DefaultDownloadDelegate_h


/**
 *  @file  DefaultDownloadDelegate.h
 *  DefaultDownloadDelegate description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"
#include <wtf/HashSet.h>
#include <wtf/HashSet.h>
#include "PlatformString.h"

class WebError;
class WebMutableURLResponse;
class WebMutableURLRequest;
class WebDownload;
class WebURLAuthenticationChallenge;

class DefaultDownloadDelegate
{
public:

    /**
     * get shared instance
     * @param[out]: DefaultDownloadDelegate
     * @code
     * DefaultDownloadDelegate *d = DefaultDownloadDelegate::sharedInstance();
     * @endcode
     */
    static DefaultDownloadDelegate* sharedInstance();

    /**
     * create a new instance of DefaultDownloadDelegate
     * @param[out]: DefaultDownloadDelegate
     * @code
     * DefaultDownloadDelegate *d = DefaultDownloadDelegate::sharedInstance();
     * @endcode
     */
    static DefaultDownloadDelegate* createInstance();
private:

    /**
     *  DefaultDownloadDelegate default constructor
     */
    DefaultDownloadDelegate();
public:

    /**
     * DefaultDownloadDelegate destructor
     */
    virtual ~DefaultDownloadDelegate();

    /**
     * set destination of download with the suggested filename
     * Not Implemented
     * @param[in]: WebDownload
     * @param[in]: filename
     * @code
     * d->decideDestinationWithSuggestedFilename(download, filename);
     * @endcode
     */
    virtual void decideDestinationWithSuggestedFilename(WebDownload *download, WebCore::String filename);

    /**
     * did cancel authentication challenge
     * Not Implemented
     * @param[in]: WebDownload
     * @param[in]: WebURLAuthenticationChallenge
     * @code
     * d->didCancelAuthenticationChallenge(download, challenge);
     * @endcode
     */
    virtual void didCancelAuthenticationChallenge(WebDownload* download, WebURLAuthenticationChallenge* challenge);

    /**
     * did create destination
     * Not Implemented
     * @param[in]: WebDownload
     * @param[in]: destination
     * @code
     * d->didCreateDestination(download, destination);
     * @endcode
     */
    virtual void didCreateDestination(WebDownload* download,  WebCore::String destination);

    /**
     * did fail with error
     * Not Implemented
     * @param[in]: WebDownload
     * @param[in]: Error
     * @code
     * d->didFailWithError(download, error);
     * @endcode
     */
    virtual void didFailWithError(WebDownload* download, WebError* error);

    /**
     * did receive authentication challenge
     * Not Implemented
     * @param[in]: WebDownload
     * @param[in]: WebURLAuthenticationChallenge
     * @code
     * d->didReceiveAuthenticationChallenge(download, challenge);
     * @endcode
     */
    virtual void didReceiveAuthenticationChallenge(WebDownload* download, WebURLAuthenticationChallenge* challenge);

    /**
     * did receive data of length
     * Not Implemented
     * @param[in]: WebDownload
     * @param[in]: length
     * @code
     * d->didReceiveDataOfLength(download, length);
     * @endcode
     */
    virtual void didReceiveDataOfLength(WebDownload* download, unsigned length);

    /**
     * did receive response
     * Not Implemented
     * @param[in]: WebDownload
     * @param[in]: WebMutableURLResponse
     * @code
     * d->didReceiveResponse(download, response);
     * @endcode
     */
    virtual void didReceiveResponse(WebDownload* download, WebMutableURLResponse* response);

    /**
     * should decode source data of MIMEType
     * Not Implemented
     * @param[in]: download
     * @param[in]: encodingType
     * @code
     * bool s = d->shouldDecodeSourceDataOfMIMEType(download, encodingType);
     * @endcode
     */
    virtual bool shouldDecodeSourceDataOfMIMEType(WebDownload* download, WebCore::String encodingType);

    /**
     * will resume with response
     * Not Implemented
     * @param[in]: WebDownload
     * @param[in]: WebMutableURLResponse
     * @param[in]: long long
     * @code
     * d->willResumeWithResponse(download, response, fromByte);
     * @endcode
     */
    virtual void willResumeWithResponse(WebDownload* download, WebMutableURLResponse* response, long long fromByte);

    /**
     * will send request
     * Not Implemented
     * @param[in]: WebDownload
     * @param[in]: WebMutableURLRequest
     * @param[in]: WebMutableURLResponse
     * @param[out]: WebMutableURLRequest
     * @code
     * WebMutableURLRequest *w = d->willSendRequest(download, request, redirectResponse);
     * @endcode
     */
    virtual WebMutableURLRequest* willSendRequest(WebDownload* download, WebMutableURLRequest* request,  WebMutableURLResponse* redirectResponse);

    /**
     * did begin
     * @param[in]: WebDownload
     * @code
     * d->didBegin(download);
     * @endcode
     */
    virtual void didBegin(WebDownload* download);

    /**
     * did finish
     * @param[in]: WebDownload
     * @code
     * d->didFinish(download);
     * @endcode
     */
    virtual void didFinish(WebDownload* download);

    // DefaultDownloadDelegate

    /**
     * register download
     * @param[in]: WebDownload
     * @code
     * d->registerDownload(download);
     * @endcode
     */
    void registerDownload(WebDownload*);

    /**
     * unregister download
     * @param[in]: WebDownload
     * @code
     * d->unregisterDownload(download);
     * @endcode
     */
    void unregisterDownload(WebDownload*);
protected:
    HashSet<WebDownload*> m_downloads;
};

#endif
