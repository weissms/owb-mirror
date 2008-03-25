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

#include "config.h"
#include "PluginPackage.h"

#ifdef __OWB__
#include "PluginPackageGtk.h"
#endif

#include "CString.h"
#include "DeprecatedString.h"
#include "npruntime_impl.h"
#include "PluginDebug.h"

namespace WebCore {

PluginPackage::~PluginPackage()
{
    if(m_pluginPackagePrivate)
        delete m_pluginPackagePrivate;
    m_pluginPackagePrivate = 0;
}

PluginPackage::PluginPackage(const String& path, const time_t& lastModified)
    : m_path(path)
    , m_lastModified(lastModified)
    , m_pluginPackagePrivate(new PluginPackagePrivate())
{
    int pos = m_path.deprecatedString().findRev('/');

    m_fileName = m_path.right(m_path.length() - pos - 1);

}

const MIMEToDescriptionsMap& PluginPackage::mimeToDescriptions() const
{ 
    return m_pluginPackagePrivate->mimeToDescriptions(); 
}
const MIMEToExtensionsMap& PluginPackage::mimeToExtensions() const 
{ 
    return m_pluginPackagePrivate->mimeToExtensions(); 
}

const NPPluginFuncs* PluginPackage::pluginFuncs() const 
{ 
    return m_pluginPackagePrivate->pluginFuncs(); 
}

bool PluginPackage::fetchInfo()
{
    return m_pluginPackagePrivate->fetchInfo(m_path, m_name, m_description);
}

PluginPackage* PluginPackage::createPackage(const String& path, const time_t& lastModified)
{
    PluginPackage* package = new PluginPackage(path, lastModified);

    if (!package->fetchInfo()) {
        delete package;
        return 0;
    }
    
    return package;
}

bool PluginPackage::load()
{
    return m_pluginPackagePrivate->load(m_path);
}

void PluginPackage::unload()
{
    m_pluginPackagePrivate->unload();
}

unsigned PluginPackage::hash() const
{ 
    unsigned hashCodes[2] = {
        m_path.impl()->hash(),
        m_lastModified
    };

    return StringImpl::computeHash(reinterpret_cast<UChar*>(hashCodes), 2 * sizeof(unsigned) / sizeof(UChar));
}

bool PluginPackage::equal(const PluginPackage& a, const PluginPackage& b)
{
    return a.m_description == b.m_description;
}

}
