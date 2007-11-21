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
 * @file BTPlugInInfoStore.cpp
 *
 * Plugins manager.
 *
 * @namespace WebCore Core browser from Apple's WebKit
 */
#include "config.h"
#include "PlugInInfoStore.h"
#include "BTLogHelper.h"
#include "DeprecatedString.h"

//For BALNotImplemented
#include "BALConfiguration.h"

namespace WebCore {

WebCore::PluginInfo* PlugInInfoStore::createPluginInfoForPluginAtIndex(unsigned int)
{
    BALNotImplemented();
    return 0;
}

unsigned int PlugInInfoStore::pluginCount() const
{
    // 0 for kjs_navigator, tell we do not support js compatible plugins
    return 0;
}

bool PlugInInfoStore::supportsMIMEType(WebCore::String const& mimetype)
{
    DBGM(MODULE_FACILITIES, mimetype.deprecatedString().ascii());
    if (mimetype == "application/x-origyn-mediaplayer") {
        DBGM(MODULE_FACILITIES, "mime-type '%s' is supported\n",
             mimetype.deprecatedString().ascii());
        return true;
    } else {
        DBGM(MODULE_FACILITIES, "mime-type '%s' is not supported\n",
             mimetype.deprecatedString().ascii());
        return false;
    }
}

void refreshPlugins(bool)
{
}

}
