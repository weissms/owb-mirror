/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2007 Pleyo
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include "config.h"
#include "BALConfiguration.h"
#include "FileChooser.h"

#ifdef OWB_ICON_SUPPORT
#include "Icon.h"
#endif //OWB_ICON_SUPPORT

namespace WebCore {

FileChooser::FileChooser(FileChooserClient* client, const String& filename)
    : m_client(client)
    , m_filename(filename)
{
}

FileChooser::~FileChooser()
{
}

void FileChooser::openFileChooser(Document*)
{
    BALNotImplemented();
}

String FileChooser::basenameForWidth(const Font&, int width) const
{
    BALNotImplemented();
    return String();
}

}

// vim: ts=4 sw=4 et
