/*
 * Copyright (C) 2004, 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2005, 2006 Michael Emmel mike.emmel@gmail.com 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "ResourceHandle.h"

#include "DocLoader.h"
#ifndef __OWB__
#include "NotImplemented.h"
#endif
#include "ResourceHandleInternal.h"
#ifdef __OWB__
#include "BIResourceHandleManager.h"
#else
#include "ResourceHandleManager.h"
#endif

namespace WebCore {

ResourceHandleInternal::~ResourceHandleInternal()
{
    free(m_url);
    if (m_customHeaders)
        curl_slist_free_all(m_customHeaders);
    if (m_file)
        fclose(m_file);
}

#ifdef __OWB__
}
namespace BAL {
#endif //__OWB__

ResourceHandle::~ResourceHandle()
{
    cancel();
}

bool ResourceHandle::start(Frame* frame)
{
    ASSERT(frame);
    ref();
#ifdef __OWB__
    getBIResourceHandleManager()->add(this);
#else
    ResourceHandleManager::sharedInstance()->add(this);
#endif
    return true;
}

void ResourceHandle::cancel()
{
#ifdef __OWB__
    getBIResourceHandleManager()->cancel(this);
#else
    ResourceHandleManager::sharedInstance()->cancel(this);
#endif
}

PassRefPtr<SharedBuffer> ResourceHandle::bufferedData()
{
    return 0;
}

bool ResourceHandle::supportsBufferedData()
{
    return false;
}

void ResourceHandle::setDefersLoading(bool defers)
{
    d->m_defersLoading = defers;
    //notImplemented();
}

bool ResourceHandle::willLoadFromCache(ResourceRequest&)
{
    //notImplemented();
    return false;
}

bool ResourceHandle::loadsBlocked()
{
    //notImplemented();
    return false;
}

void ResourceHandle::loadResourceSynchronously(const ResourceRequest&, ResourceError&, ResourceResponse&, Vector<char>&, Frame*)
{
    //notImplemented();
}

} // namespace WebCore
