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
#include "PluginPackageGtk.h"

#include "CString.h"
#include "DeprecatedString.h"
#include "npruntime_impl.h"
#include "PluginDebug.h"

namespace WebCore {

PluginPackagePrivate::~PluginPackagePrivate()
{
    ASSERT(!m_isLoaded);
}

PluginPackagePrivate::PluginPackagePrivate()
    : m_module(0)
    , m_isLoaded(false)
    , m_loadCount(0)
{
}

bool PluginPackagePrivate::fetchInfo(String path, String &name, String &description)
{
    if (!load(path))
        return false;

    const gchar* types = m_NP_GetMIMEDescription();
    gchar** mimeDescs = g_strsplit(types, ";", -1);
    for (int i = 0; mimeDescs[i] && mimeDescs[i][0]; i++) {
        gchar** mimeData = g_strsplit(mimeDescs[i], ":", 3);

        String description = String::fromUTF8(mimeData[2]);
        gchar** extensions = g_strsplit(mimeData[1], ",", -1);

        Vector<String> extVector;
        for (int j = 0; extensions[j]; j++)
            extVector.append(String::fromUTF8(extensions[j]));

        m_mimeToExtensions.add(mimeData[0], extVector);
        m_mimeToDescriptions.add(mimeData[0], description);

        g_strfreev(extensions);
        g_strfreev(mimeData);
    }
    g_strfreev(mimeDescs);

    char* buffer = 0;
    NPError err = m_NPP_GetValue(0, NPPVpluginNameString, &buffer);
    if (err == NPERR_NO_ERROR)
        name = buffer;

    buffer = 0;
    err = m_NPP_GetValue(0, NPPVpluginDescriptionString, &buffer);
    if (err == NPERR_NO_ERROR)
        description = buffer;

    unload();
    return true;
}

bool PluginPackagePrivate::load(String path)
{
    if (m_isLoaded) {
        m_loadCount++;
        return true;
    }

    m_module = g_module_open((path.utf8()).data(), G_MODULE_BIND_LOCAL);
    if (!m_module)
        return false;

    m_isLoaded = true;

    NP_InitializeFuncPtr NP_Initialize;
    NPError npErr;

    g_module_symbol(m_module, "NP_Initialize", (void**)&NP_Initialize);
    g_module_symbol(m_module, "NP_GetMIMEDescription", (void**)&m_NP_GetMIMEDescription);
    g_module_symbol(m_module, "NP_Shutdown", (void**)&m_NPP_Shutdown);
    g_module_symbol(m_module, "NP_GetValue", (void**)&m_NPP_GetValue);

    if (!NP_Initialize || !m_NP_GetMIMEDescription || !m_NPP_Shutdown || !m_NPP_GetValue)
        goto abort;

    memset(&m_pluginFuncs, 0, sizeof(m_pluginFuncs));
    m_pluginFuncs.size = sizeof(m_pluginFuncs);

    m_browserFuncs.size = sizeof (m_browserFuncs);
    m_browserFuncs.version = NP_VERSION_MINOR;
    m_browserFuncs.geturl = NPN_GetURL;
    m_browserFuncs.posturl = NPN_PostURL;
    m_browserFuncs.requestread = NPN_RequestRead;
    m_browserFuncs.newstream = NPN_NewStream;
    m_browserFuncs.write = NPN_Write;
    m_browserFuncs.destroystream = NPN_DestroyStream;
    m_browserFuncs.status = NPN_Status;
    m_browserFuncs.uagent = NPN_UserAgent;
    m_browserFuncs.memalloc = NPN_MemAlloc;
    m_browserFuncs.memfree = NPN_MemFree;
    m_browserFuncs.memflush = NPN_MemFlush;
    m_browserFuncs.reloadplugins = NPN_ReloadPlugins;
    m_browserFuncs.geturlnotify = NPN_GetURLNotify;
    m_browserFuncs.posturlnotify = NPN_PostURLNotify;
    m_browserFuncs.getvalue = NPN_GetValue;
    m_browserFuncs.setvalue = NPN_SetValue;
    m_browserFuncs.invalidaterect = NPN_InvalidateRect;
    m_browserFuncs.invalidateregion = NPN_InvalidateRegion;
    m_browserFuncs.forceredraw = NPN_ForceRedraw;
    m_browserFuncs.getJavaEnv = NPN_GetJavaEnv;
    m_browserFuncs.getJavaPeer = NPN_GetJavaPeer;
    m_browserFuncs.pushpopupsenabledstate = NPN_PushPopupsEnabledState;
    m_browserFuncs.poppopupsenabledstate = NPN_PopPopupsEnabledState;

    m_browserFuncs.releasevariantvalue = _NPN_ReleaseVariantValue;
    m_browserFuncs.getstringidentifier = _NPN_GetStringIdentifier;
    m_browserFuncs.getstringidentifiers = _NPN_GetStringIdentifiers;
    m_browserFuncs.getintidentifier = _NPN_GetIntIdentifier;
    m_browserFuncs.identifierisstring = _NPN_IdentifierIsString;
    m_browserFuncs.utf8fromidentifier = _NPN_UTF8FromIdentifier;
    m_browserFuncs.createobject = _NPN_CreateObject;
    m_browserFuncs.retainobject = _NPN_RetainObject;
    m_browserFuncs.releaseobject = _NPN_ReleaseObject;
    m_browserFuncs.invoke = _NPN_Invoke;
    m_browserFuncs.invokeDefault = _NPN_InvokeDefault;
    m_browserFuncs.evaluate = _NPN_Evaluate;
    m_browserFuncs.getproperty = _NPN_GetProperty;
    m_browserFuncs.setproperty = _NPN_SetProperty;
    m_browserFuncs.removeproperty = _NPN_RemoveProperty;
    m_browserFuncs.hasproperty = _NPN_HasMethod;
    m_browserFuncs.hasmethod = _NPN_HasProperty;
    m_browserFuncs.setexception = _NPN_SetException;
    m_browserFuncs.enumerate = _NPN_Enumerate;

    npErr = NP_Initialize(&m_browserFuncs, &m_pluginFuncs);
    if (npErr != NPERR_NO_ERROR)
        goto abort;

    m_loadCount++;
    return true;

abort:
    unloadWithoutShutdown();
    return false;
}

void PluginPackagePrivate::unload()
{
    if (!m_isLoaded)
        return;

    if (--m_loadCount > 0)
        return;

    m_NPP_Shutdown();

    unloadWithoutShutdown();
}

void PluginPackagePrivate::unloadWithoutShutdown()
{
    if (!m_isLoaded)
        return;

    ASSERT(m_loadCount == 0);
    ASSERT(m_module);

    g_module_close (m_module);

    m_isLoaded = false;
}


}
