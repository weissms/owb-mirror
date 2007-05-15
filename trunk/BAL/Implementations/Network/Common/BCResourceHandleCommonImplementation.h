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
 * @file BCResourceHandleJobCommonImplementation.h
 *
 * BAL Transfer Job Common Implementation
 */

#ifndef BCResourceHandleCommonImplementation_H_
#define BCResourceHandleCommonImplementation_H_

#include "AuthenticationChallenge.h"
#include "BTFormData.h"
#include "DeprecatedString.h"
#include "KURL.h"
#include "PlatformString.h"
#include "ResourceRequest.h"
#include <wtf/HashMap.h>

using WebCore::String;
using WebCore::FormData;
using WebCore::ResourceHandleClient;
using WebCore::AuthenticationChallenge;

namespace BC {

/**
 * BAL Transfer Job Common Implementation
 */
    class BCResourceHandleCommonImplementation : Noncopyable {
    public:
        BCResourceHandleCommonImplementation(BAL::BIResourceHandle* loader, const ResourceRequest& request, ResourceHandleClient* c, bool defersLoading, bool mightDownloadFromHandle)
            : m_client(c)
            , m_request(request)
            , status(0)
            , m_defersLoading(defersLoading)
            , m_mightDownloadFromHandle(mightDownloadFromHandle)
        {
        }

        ~BCResourceHandleCommonImplementation() {}

        ResourceHandleClient* client() { return m_client; }
        ResourceHandleClient* m_client;

        ResourceRequest m_request;

        int status;

        bool m_defersLoading;
        bool m_mightDownloadFromHandle;
       AuthenticationChallenge m_currentWebChallenge;
    };

} // namespace BAL

#endif
