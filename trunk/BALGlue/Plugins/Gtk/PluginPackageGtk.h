/*
 * Copyright (C) 2006, 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2007 Collabora, Ltd. All rights reserved.
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

#ifndef PluginPackageGtk_H
#define PluginPackageGtk_H

#include <gmodule.h>

//#include "RefCounted.h"
#include "Shared.h"
#include "StringHash.h"
#include "PlatformString.h"
#include "npfunctions.h"
#include <wtf/HashMap.h>

#undef NP_InitializeFuncPtr

namespace WebCore {
    typedef HashMap<String, String> MIMEToDescriptionsMap;
    typedef HashMap<String, Vector<String> > MIMEToExtensionsMap;

    class PluginPackagePrivate{
    public:
        PluginPackagePrivate();
        ~PluginPackagePrivate();

        const MIMEToDescriptionsMap& mimeToDescriptions() const { return m_mimeToDescriptions; }
        const MIMEToExtensionsMap& mimeToExtensions() const { return m_mimeToExtensions; }

        const NPPluginFuncs* pluginFuncs() const { return &m_pluginFuncs; }
        bool fetchInfo(String path, String &name, String &description);
        bool load(String path);
        void unload();
        void unloadWithoutShutdown();
    private:

        bool m_isLoaded;
        int m_loadCount;

        MIMEToDescriptionsMap m_mimeToDescriptions;
        MIMEToExtensionsMap m_mimeToExtensions;

        GModule* m_module;

        NP_GetMIMEDescriptionFuncPtr m_NP_GetMIMEDescription;
        NPP_ShutdownProcPtr m_NPP_Shutdown;
        NPP_GetValueProcPtr m_NPP_GetValue;
        NPPluginFuncs m_pluginFuncs;
        NPNetscapeFuncs m_browserFuncs;
    };

} // namespace WebCore

#endif
