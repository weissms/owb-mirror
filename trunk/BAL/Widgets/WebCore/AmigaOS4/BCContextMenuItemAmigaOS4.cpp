/*
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
#include "ContextMenuItem.h"
#include "CString.h"
#include "Logging.h"
#include "NotImplemented.h"

#include <cstdio>

#define WEBKIT_CONTEXT_MENU_ACTION "webkit-context-menu"

namespace WKAL {

// Extract the ActionType from the menu item
ContextMenuItem::ContextMenuItem(BalMenuItem* item)
    : m_platformDescription()
{
    printf("ContextMenuItem::ContextMenuItem\n");
}

ContextMenuItem::ContextMenuItem(ContextMenu*)
{
    printf("ContextMenuItem::ContextMenuItem\n");
    balNotImplemented();
}

ContextMenuItem::ContextMenuItem(ContextMenuItemType type, ContextMenuAction action, const String& title, ContextMenu* subMenu)
{
    m_platformDescription.type = type;
    m_platformDescription.action = action;
    m_platformDescription.title = title;

    if (subMenu)
        setSubMenu(subMenu);
}

ContextMenuItem::~ContextMenuItem()
{
}

BalMenuItem* ContextMenuItem::createNativeMenuItem(const PlatformMenuItemDescription& menu)
{
    printf("ContextMenuItem::createNativeMenuItem\n");
    return 0;
}

PlatformMenuItemDescription ContextMenuItem::releasePlatformDescription()
{
    PlatformMenuItemDescription description = m_platformDescription;
    m_platformDescription = PlatformMenuItemDescription();
    return description;
}

ContextMenuItemType ContextMenuItem::type() const
{
    return m_platformDescription.type;
}

void ContextMenuItem::setType(ContextMenuItemType type)
{
    printf("ContextMenuItem::setType\n");
    m_platformDescription.type = type;
}

ContextMenuAction ContextMenuItem::action() const
{
    return m_platformDescription.action;
}

void ContextMenuItem::setAction(ContextMenuAction action)
{
    printf("ContextMenuItem::setAction\n");
    m_platformDescription.action = action;
}

String ContextMenuItem::title() const
{
    return m_platformDescription.title;
}

void ContextMenuItem::setTitle(const String& title)
{
    m_platformDescription.title = title;
}

PlatformMenuDescription ContextMenuItem::platformSubMenu() const
{
    printf("ContextMenuItem::platformSubMenu\n");
    return m_platformDescription.subMenu;
}

void ContextMenuItem::setSubMenu(ContextMenu* menu)
{
    m_platformDescription.subMenu = menu->releasePlatformDescription();
    m_platformDescription.type = SubmenuType;
}

void ContextMenuItem::setChecked(bool shouldCheck)
{
    printf("ContextMenuItem::setChecked\n");
    m_platformDescription.checked = shouldCheck;
}

void ContextMenuItem::setEnabled(bool shouldEnable)
{
    printf("ContextMenuItem::setEnabled\n");
    m_platformDescription.enabled = shouldEnable;
}

}
