/*
 * Copyright (C) 2008 Pleyo.  All rights reserved.
 * Copyright (C) 2009 Fabien Coeurjoly
 * Copyright (C) 2009 Stanislaw Szymczyk
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

#include "config.h"
#include "WebDownload.h"
#include "WebDownloadPrivate.h"

#include "CString.h"
#include "WebDownloadDelegate.h"
#include "WebError.h"
#include "WebMutableURLRequest.h"
#include "WebURLAuthenticationChallenge.h"
#include "WebURLCredential.h"
#include "WebURLResponse.h"

#include <sys/stat.h>
#include <sys/types.h>

#include <PlatformString.h>
#include <NotImplemented.h>
#include <ResourceError.h>
#include <ResourceHandle.h>
#include <ResourceRequest.h>
#include <ResourceHandleClient.h>
#include <ResourceResponse.h>
#include <SystemTime.h>
#include <FileIO.h>

#if PLATFORM(AROS)
#include <aros/debug.h>
#endif

using namespace WebCore;

/*****************************************************************************************************/

class DownloadClient : public Noncopyable, public ResourceHandleClient
{
public:
    DownloadClient(WebDownload*);

    void didStart();
    virtual void didReceiveResponse(ResourceHandle*, const WebCore::ResourceResponse&);
    virtual void didReceiveData(ResourceHandle*, const char*, int, int);
    virtual void didFinishLoading(ResourceHandle*);
    virtual void didFail(ResourceHandle*, const ResourceError&);
    virtual void wasBlocked(ResourceHandle*);
    virtual void cannotShowURL(ResourceHandle*);
private:
    WebDownload* m_download;
};

DownloadClient::DownloadClient(WebDownload* download)
        : m_download(download)
{
}

void DownloadClient::didStart()
{
    if (!m_download->downloadDelegate())
        return;

    WebDownloadPrivate* priv = m_download->getWebDownloadPrivate();

    priv->state = WEBKIT_WEB_DOWNLOAD_STATE_STARTED;
    m_download->downloadDelegate()->didBegin(m_download);
}

void DownloadClient::didReceiveResponse(ResourceHandle*, const WebCore::ResourceResponse& response)
{
    if (!m_download->downloadDelegate())
        return;
    
    WebDownloadPrivate* priv = m_download->getWebDownloadPrivate();

    WebURLResponse *webResponse = WebURLResponse::createInstance(response);
    m_download->downloadDelegate()->didReceiveResponse(m_download, webResponse);

    WebCore::String suggestedFilename = webResponse->suggestedFilename();
    if(suggestedFilename.length() == 0)
    {
        suggestedFilename = response.url().string().substring(response.url().pathAfterLastSlash());
    }

    m_download->downloadDelegate()->decideDestinationWithSuggestedFilename(m_download, suggestedFilename.utf8().data());
    delete webResponse;

    // Fail if destination file path is not set
    if(priv->destinationPath == NULL)
    {
        ResourceError resourceError(String(WebKitErrorDomain), WebURLErrorCannotCreateFile, String(), String());
        didFail(priv->resourceHandle.get(), resourceError);
        return;
    }

    priv->outputChannel = new File(priv->destinationPath);

    // Fail if destination file already exists and can't be overwritten
    if(!priv->allowOverwrite && priv->outputChannel->open('r') != -1)
    {
        ResourceError resourceError(String(WebKitErrorDomain), WebURLErrorCannotCreateFile, String(), String());
        didFail(priv->resourceHandle.get(), resourceError);
        return;
    }
    else
        priv->outputChannel->close();

    // Fail if not possible to open destination file to writing
    if(priv->outputChannel->open('w') == -1)
    {
        ResourceError resourceError(String(WebKitErrorDomain), WebURLErrorCannotCreateFile, String(), String());
        didFail(priv->resourceHandle.get(), resourceError);
        return;
    }

    m_download->downloadDelegate()->didCreateDestination(m_download, priv->destinationPath.utf8().data());
}

void DownloadClient::didReceiveData(ResourceHandle*, const char* data, int length, int lengthReceived)
{
    if (!m_download->downloadDelegate())
        return;

    WebDownloadPrivate* priv = m_download->getWebDownloadPrivate();

    m_download->downloadDelegate()->didReceiveDataOfLength(m_download, length);

    // FIXME: handle write errors somehow
    priv->outputChannel->write(data, length);

    priv->currentSize += length;
}

void DownloadClient::didFinishLoading(ResourceHandle*)
{
    if (!m_download->downloadDelegate())
        return;

    WebDownloadPrivate* priv = m_download->getWebDownloadPrivate();

    priv->state = WEBKIT_WEB_DOWNLOAD_STATE_FINISHED;

    priv->outputChannel->close();
    delete priv->outputChannel;
    priv->outputChannel = NULL;

    m_download->downloadDelegate()->didFinish(m_download);
}

void DownloadClient::didFail(ResourceHandle*, const ResourceError& resourceError)
{
    if (!m_download->downloadDelegate())
        return;

    WebDownloadPrivate* priv = m_download->getWebDownloadPrivate();
    priv->state = WEBKIT_WEB_DOWNLOAD_STATE_ERROR;

    priv->resourceHandle->setClient(0);
    priv->resourceHandle->cancel();

    WebError *error = WebError::createInstance(resourceError);
    m_download->downloadDelegate()->didFailWithError(m_download, error);
}

void DownloadClient::wasBlocked(ResourceHandle*)
{
    notImplemented();
}

void DownloadClient::cannotShowURL(ResourceHandle*)
{
    notImplemented();
}

/*********************************************************************************************/

WebDownload::WebDownload()
{
    m_request = 0;
    m_priv = new WebDownloadPrivate();
}

void WebDownload::init(ResourceHandle* handle, const ResourceRequest* request, const WebCore::ResourceResponse* response, TransferSharedPtr<WebDownloadDelegate> delegate)
{
    m_delegate = delegate;

    m_priv->downloadClient = new DownloadClient(this);
    m_priv->state = WEBKIT_WEB_DOWNLOAD_STATE_CREATED;
    m_priv->currentSize = 0;
    m_priv->outputChannel = NULL;

    m_request = WebMutableURLRequest::createInstance(*request);
    m_response = WebURLResponse::createInstance(*response);
    m_priv->resourceHandle = handle;
}

void WebDownload::init(const KURL& url, TransferSharedPtr<WebDownloadDelegate> delegate)
{
    m_delegate = delegate;

    m_request = WebMutableURLRequest::createInstance(ResourceRequest(url));

    m_priv->downloadClient = new DownloadClient(this);
    m_priv->currentSize = 0;
    m_priv->outputChannel = NULL;
    m_priv->state = WEBKIT_WEB_DOWNLOAD_STATE_CREATED;
    m_priv->resourceHandle = NULL;
}

WebDownload::~WebDownload()
{
    if(m_priv->resourceHandle)
    {
        if(m_priv->state == WEBKIT_WEB_DOWNLOAD_STATE_CREATED || m_priv->state == WEBKIT_WEB_DOWNLOAD_STATE_STARTED)
        {
            m_priv->resourceHandle->setClient(0);
            m_priv->resourceHandle->cancel();
        }

        m_priv->resourceHandle.release();
        m_priv->resourceHandle = NULL;
    }

    delete m_priv->downloadClient;

    if(m_priv->outputChannel)
    {
        m_priv->outputChannel->close();
        delete m_priv->outputChannel;
    }

    delete m_priv;

    if(m_delegate)
        m_delegate = 0;
    if(m_request)
        delete m_request;
    if(m_response)
        delete m_response;
}

WebDownload* WebDownload::createInstance()
{
    WebDownload* instance = new WebDownload();
    return instance;
}

WebDownload* WebDownload::createInstance(ResourceHandle* handle, const ResourceRequest* request, const WebCore::ResourceResponse* response, TransferSharedPtr<WebDownloadDelegate> delegate)
{
    WebDownload* instance = new WebDownload();
    instance->init(handle, request, response, delegate);
    return instance;
}

WebDownload* WebDownload::createInstance(const KURL& url, TransferSharedPtr<WebDownloadDelegate> delegate)
{
    WebDownload* instance = new WebDownload();
    instance->init(url, delegate);
    return instance;
}

void WebDownload::initWithRequest(
        /* [in] */ WebMutableURLRequest* request,
        /* [in] */ TransferSharedPtr<WebDownloadDelegate> delegate)
{
    notImplemented();
}

void WebDownload::initToResumeWithBundle(
        /* [in] */ const char* bundlePath,
        /* [in] */ TransferSharedPtr<WebDownloadDelegate> delegate)
{
    notImplemented();
}

bool WebDownload::canResumeDownloadDecodedWithEncodingMIMEType(
        /* [in] */ const char*)
{
    notImplemented();
    return false;
}

void WebDownload::start()
{
    if (m_priv->state != WEBKIT_WEB_DOWNLOAD_STATE_CREATED)
        return;

    if (m_priv->resourceHandle)
    {
        m_priv->resourceHandle->setClient(m_priv->downloadClient);
        m_priv->downloadClient->didStart();
        m_priv->downloadClient->didReceiveResponse(m_priv->resourceHandle.get(), m_response->resourceResponse());
    }
    else
    {
        m_priv->downloadClient->didStart();
        m_priv->resourceHandle = ResourceHandle::create(m_request->resourceRequest(), m_priv->downloadClient, 0, false, false, false);
    }
}

void WebDownload::cancel()
{
    if (!(m_priv->state == WEBKIT_WEB_DOWNLOAD_STATE_CREATED || m_priv->state == WEBKIT_WEB_DOWNLOAD_STATE_STARTED))
        return;

    if (m_priv->resourceHandle)
        m_priv->resourceHandle->cancel();

    m_priv->state = WEBKIT_WEB_DOWNLOAD_STATE_CANCELLED;
}

void WebDownload::cancelForResume()
{
    notImplemented();
}

bool WebDownload::deletesFileUponFailure()
{
    return false;
}

char* WebDownload::bundlePathForTargetPath(const char* targetPath)
{
    notImplemented();
    return NULL;
}

WebMutableURLRequest* WebDownload::request()
{
    return m_request;
}

void WebDownload::setDeletesFileUponFailure(bool deletesFileUponFailure)
{
    notImplemented();
}

void WebDownload::setDestination(const char* path, bool allowOverwrite)
{
    m_priv->destinationPath = path;
    m_priv->allowOverwrite = allowOverwrite;
}

void WebDownload::cancelAuthenticationChallenge(WebURLAuthenticationChallenge*)
{
    notImplemented();
}

void WebDownload::continueWithoutCredentialForAuthenticationChallenge(WebURLAuthenticationChallenge* challenge)
{
    notImplemented();
}

void WebDownload::useCredential(WebURLCredential* credential, WebURLAuthenticationChallenge* challenge)
{
    notImplemented();
}
