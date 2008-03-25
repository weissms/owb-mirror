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
#include "PluginDatabase.h"

#ifdef __OWB__
#include "PluginDatabaseGtk.h"
#endif

#include "PluginPackage.h"
#include "PluginView.h"

namespace WebCore {

PluginDatabase* PluginDatabase::installedPlugins()
{
    static PluginDatabase* plugins = 0;
    
    if (!plugins) {
        plugins = new PluginDatabase;
        plugins->setPluginPaths(PluginDatabase::defaultPluginPaths());
        plugins->refresh();
    }

    return plugins;
}

PluginDatabase::PluginDatabase()
:m_pluginDatabasePrivate(new PluginDatabasePrivate())
{
}

PluginDatabase::~PluginDatabase()
{
    if(m_pluginDatabasePrivate)
        delete m_pluginDatabasePrivate;
    m_pluginDatabasePrivate=0;
}

void PluginDatabase::addExtraPluginPath(const String& path)
{
    m_pluginPaths.append(path);
    refresh();
}

bool PluginDatabase::refresh()
{   
    PluginSet newPlugins;

    bool pluginSetChanged = false;

    // Create a new set of plugins
    newPlugins = getPluginsInPaths();

    if (!m_plugins.isEmpty()) {
        m_registeredMIMETypes.clear();

        PluginSet pluginsToUnload = m_plugins;

        PluginSet::const_iterator end = newPlugins.end();
        for (PluginSet::const_iterator it = newPlugins.begin(); it != end; ++it)
            pluginsToUnload.remove(*it);

        end = m_plugins.end();
        for (PluginSet::const_iterator it = m_plugins.begin(); it != end; ++it)
            newPlugins.remove(*it);

        // Unload plugins
        end = pluginsToUnload.end();
        for (PluginSet::const_iterator it = pluginsToUnload.begin(); it != end; ++it)
            m_plugins.remove(*it);

        // Add new plugins
        end = newPlugins.end();
        for (PluginSet::const_iterator it = newPlugins.begin(); it != end; ++it)
            m_plugins.add(*it);

        pluginSetChanged = !pluginsToUnload.isEmpty() || !newPlugins.isEmpty();
    } else {
        m_plugins = newPlugins;
        PluginSet::const_iterator end = newPlugins.end();
        for (PluginSet::const_iterator it = newPlugins.begin(); it != end; ++it)
            m_plugins.add(*it);

        pluginSetChanged = !newPlugins.isEmpty();
    }

    // Register plug-in MIME types
    PluginSet::const_iterator end = m_plugins.end();
    for (PluginSet::const_iterator it = m_plugins.begin(); it != end; ++it) {
        // Get MIME types
        MIMEToDescriptionsMap::const_iterator map_end = (*it)->mimeToDescriptions().end();
        for (MIMEToDescriptionsMap::const_iterator map_it = (*it)->mimeToDescriptions().begin(); map_it != map_end; ++map_it) {
            m_registeredMIMETypes.add(map_it->first);
        }
    }

    return pluginSetChanged;
}

Vector<PluginPackage*> PluginDatabase::plugins() const
{
    Vector<PluginPackage*> result;

    PluginSet::const_iterator end = m_plugins.end();
    for (PluginSet::const_iterator it = m_plugins.begin(); it != end; ++it)
        result.append((*it).get());

    return result;
}

PluginSet PluginDatabase::getPluginsInPaths() const
{
    return m_pluginDatabasePrivate->getPluginsInPaths(m_pluginPaths);
}

static inline Vector<int> parseVersionString(const String& versionString)
{
    Vector<int> version;

    unsigned startPos = 0;
    unsigned endPos;
    
    while (startPos < versionString.length()) {
        for (endPos = startPos; endPos < versionString.length(); ++endPos)
            if (versionString[endPos] == '.' || versionString[endPos] == '_')
                break;

        int versionComponent = versionString.substring(startPos, endPos - startPos).toInt();
        version.append(versionComponent);

        startPos = endPos + 1;
    }

    return version;
}

// This returns whether versionA is higher than versionB
static inline bool compareVersions(const Vector<int>& versionA, const Vector<int>& versionB)
{
    for (unsigned i = 0; i < versionA.size(); i++) {
        if (i >= versionB.size())
            return true;

        if (versionA[i] > versionB[i])
            return true;
        else if (versionA[i] < versionB[i])
            return false;
    }

    // If we come here, the versions are either the same or versionB has an extra component, just return false
    return false;
}

bool PluginDatabase::isMIMETypeRegistered(const String& mimeType) const
{
    return !mimeType.isNull() && m_registeredMIMETypes.contains(mimeType);
}

PluginPackage* PluginDatabase::pluginForMIMEType(const String& mimeType)
{
    String key = mimeType.lower();

    PluginSet::const_iterator end = m_plugins.end();
    for (PluginSet::const_iterator it = m_plugins.begin(); it != end; ++it) {
        if ((*it)->mimeToDescriptions().contains(key))
            return (*it).get();
    }

    return 0;
}

PluginPackage* PluginDatabase::pluginForExtension(const String& extension)
{
    PluginSet::const_iterator end = m_plugins.end();
    for (PluginSet::const_iterator it = m_plugins.begin(); it != end; ++it) {
        MIMEToExtensionsMap::const_iterator mime_end = (*it)->mimeToExtensions().end();

        for (MIMEToExtensionsMap::const_iterator mime_it = (*it)->mimeToExtensions().begin(); mime_it != mime_end; ++mime_it) {
            const Vector<String>& extensions = mime_it->second;

            for (unsigned i = 0; i < extensions.size(); i++) {
                if (extensions[i] == extension)
                    return (*it).get();
            }
        }
    }

    return 0;
}

PluginPackage* PluginDatabase::findPlugin(const KURL& url, const String& mimeType)
{   
    PluginPackage* plugin = 0;

    if (!mimeType.isNull())
        plugin = pluginForMIMEType(mimeType);
    
    if (!plugin) {
        String path = url.path();
        String extension = path.substring(path.reverseFind('.') + 1);

        plugin = pluginForExtension(extension);

        // FIXME: if no plugin could be found, query xdgmime for the mime type 
        // corresponding to the extension.
    }

    return plugin;
}

PluginView* PluginDatabase::createPluginView(Frame* parentFrame, const IntSize& size, Element* element, const KURL& url, const Vector<String>& paramNames, const Vector<String>& paramValues, const String& mimeType, bool loadManually)
{
    PluginPackage* plugin = findPlugin(url, mimeType);
    
    // No plugin was found, try refreshing the database and searching again
    if (!plugin && refresh())
        plugin = findPlugin(url, mimeType);
        
    return new PluginView(parentFrame, size, plugin, element, url, paramNames, paramValues, mimeType, loadManually);
}

}
