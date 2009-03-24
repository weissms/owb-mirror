/*
 * Copyright (C) 2009 Joerg Strohmayer.
 * Copyright (C) 2008 Pleyo.  All rights reserved.
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
#include "ContextMenu.h"
#include "ContextMenuController.h"
#include "CString.h"
#include "IntRect.h"

#include <proto/intuition.h>
#include <proto/popupmenu.h>

#include <cstdio>

extern char* utf8ToAmiga(const char* utf8);

namespace WKAL {

// TODO: ref-counting correctness checking.
// See http://bugs.webkit.org/show_bug.cgi?id=16115

ContextMenu::ContextMenu(const HitTestResult& result)
    : m_hitTestResult(result)
{
    m_platformDescription = IIntuition->NewObject(IPopupMenu->POPUPMENU_GetClass(), NULL, TAG_DONE);
}

ContextMenu::~ContextMenu()
{
    IIntuition->DisposeObject(m_platformDescription);
}

void ContextMenu::appendItem(ContextMenuItem& item)
{
    void *amigaItem = 0;
    BalMenu* subMenu = 0;

    PlatformMenuItemDescription itemDescription = item.releasePlatformDescription();
    ContextMenuItemType itemType = itemDescription.type;
    if (SubmenuType == itemType) {
        subMenu = itemDescription.subMenu;

        // Disabled because, except for right-to-left writing in text input,
        // there nothing usable on AmigaOS 4.x in the sub menus.
        IIntuition->DisposeObject(subMenu);
        return;
    }

    char* title = utf8ToAmiga(itemDescription.title.utf8().data());
    amigaItem = IIntuition->NewObject(IPopupMenu->POPUPMENU_GetItemClass(), NULL,
                                      PMIA_Title, SeparatorType == itemType ? (const char *)~0 : title ?: "",
                                      PMIA_UserData, itemDescription.action,
                                      SubmenuType == itemType ? PMIA_SubMenu : TAG_IGNORE, subMenu,
                                      PMIA_CheckIt, CheckableActionType == itemType,
                                      PMIA_Checked, itemDescription.checked,
                                      PMIA_Hidden, !itemDescription.enabled,
                                      TAG_DONE);
    free(title);

    if (amigaItem)
        IIntuition->IDoMethod((Object*)m_platformDescription, OM_ADDMEMBER, amigaItem);
}

void ContextMenu::setPlatformDescription(PlatformMenuDescription menu)
{
    m_platformDescription = menu;
}

PlatformMenuDescription ContextMenu::platformDescription() const
{
    return m_platformDescription;
}

PlatformMenuDescription ContextMenu::releasePlatformDescription()
{
    PlatformMenuDescription description = m_platformDescription;
    m_platformDescription = 0;

    return description;
}

}
