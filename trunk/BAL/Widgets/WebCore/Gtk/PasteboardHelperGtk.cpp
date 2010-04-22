/*
 *  Copyright (C) 2007 Luca Bruno <lethalman88@gmail.com>
 *  Copyright (C) 2009 Holger Hans Peter Freyther
 *  Copyright (C) 2010 Martin Robinson <mrobinson@webkit.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "config.h"
#include "PasteboardHelperGtk.h"

#include "DataObjectGtk.h"
#include "FocusController.h"
#include "Frame.h"
#include "Page.h"
#include <gtk/gtk.h>

using namespace WebCore;

namespace WebKit {

PasteboardHelperGtk::PasteboardHelperGtk()
{
    initializeTargetList();
}

PasteboardHelperGtk::~PasteboardHelperGtk()
{
}

guint PasteboardHelperGtk::getIdForTargetType(PasteboardTargetType type)
{
    if (type == TargetTypeMarkup)
        return -1;

    return -2;
}

bool PasteboardHelperGtk::usePrimarySelectionClipboard(GtkWidget* widget)
{
    //return webkit_web_view_use_primary_for_paste(WEBKIT_WEB_VIEW((widget)));
    return false;
}

}
