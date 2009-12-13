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
#include "FrameView.h"
#include "HostWindow.h"
#include "PopupMenu.h"
#include "CString.h"

#include <proto/intuition.h>
#include <proto/popupmenu.h>

extern char* utf8ToAmiga(const char* utf8);

namespace WebCore {

PopupMenu::PopupMenu(PopupMenuClient* client)
    : m_popupClient(client)
{
}

PopupMenu::~PopupMenu()
{
}

void PopupMenu::show(const IntRect& r, FrameView* v, int index)
{
    BalWidget* widget = client()->hostWindow()->platformPageClient();
    if (!widget)
        return;

    IntPoint windowRect = v->contentsToWindow(r.location());

    int itemCount = client()->listSize();
    if (itemCount > 25 * 25)
        return;

    Object *menu, *items[itemCount], *subMenu[25], *subMenuItems[25];
    String subMenuTitles[25];
    memset(subMenu, 0, sizeof(subMenu));

    menu = (Object*)IIntuition->NewObject(IPopupMenu->POPUPMENU_GetClass(), NULL,
                                          PMA_Left, windowRect.x() + widget->offsetx,
                                          PMA_Top, windowRect.y() + widget->offsety,
                                          TAG_DONE);
    if (!menu)
        return;

    bool gotAllItems = true;

    if (itemCount <= 25) {
        for (int i = 0; i < itemCount; ++i) {
            char* text = utf8ToAmiga(client()->itemText(i).utf8().data());
            items[i] = (Object*)IIntuition->NewObject(IPopupMenu->POPUPMENU_GetItemClass(), NULL,
                                                      PMIA_Title, client()->itemIsSeparator(i) ? (const char *)~0 : text ?: "",
                                                      PMIA_Hidden, !client()->itemIsEnabled(i),
                                                      PMIA_FillPen, client()->itemIsSelected(i),
                                                      TAG_DONE);
            free(text);
            if (!items[i])
                gotAllItems = false;
            else
                IIntuition->IDoMethod(menu, OM_ADDMEMBER, items[i]);
        }
    }
    else {
        for (int i = 0; i < (itemCount + 24) / 25; i++) {
            subMenu[i] = (Object*)IIntuition->NewObject(IPopupMenu->POPUPMENU_GetClass(), NULL, TAG_DONE);
            if (!subMenu[i])
                gotAllItems = false;
            else {
                int j;
                for (j = 0; i * 25 + j < itemCount && j < 25 ; j++) {
                    char* text = utf8ToAmiga(client()->itemText(i * 25 + j).utf8().data());
                    items[i * 25 + j] = (Object*)IIntuition->NewObject(IPopupMenu->POPUPMENU_GetItemClass(), NULL,
                                                                       PMIA_Title, client()->itemIsSeparator(i * 25 + j) ? (const char *)~0 : text ?: "",
                                                                       PMIA_Hidden, !client()->itemIsEnabled(i * 25 + j),
                                                                       PMIA_FillPen, client()->itemIsSelected(i * 25 + j),
                                                                       TAG_DONE);
                    free(text);
                    if (!items[i * 25 + j])
                        gotAllItems = false;
                    else
                        IIntuition->IDoMethod(subMenu[i], OM_ADDMEMBER, items[i * 25 + j]);
                }

                subMenuTitles[i] = client()->itemText(i * 25);
                if (j > 0) {
                    subMenuTitles[i] += " ... ";
                    subMenuTitles[i] += client()->itemText(i * 25 + j - 1);
                }
                char* text = utf8ToAmiga(subMenuTitles[i].utf8().data());
                subMenuItems[i] = (Object*)IIntuition->NewObject(IPopupMenu->POPUPMENU_GetItemClass(), NULL,
                                                                 PMIA_Title, text ?: "...",
                                                                 PMIA_SubMenu, subMenu[i],
                                                                 TAG_DONE);
                free(text);
                if (!subMenuItems[i])
                    gotAllItems = false;
                else
                    IIntuition->IDoMethod(menu, OM_ADDMEMBER, subMenuItems[i]);
            }
        }
    }

    if (gotAllItems) {
        Object* selected = (Object*)IIntuition->IDoMethod(menu, PM_OPEN, widget->window);
        for (int i = 0; i < itemCount; ++i)
            if (selected == items[i]) {
                client()->setTextFromItem(i);
                client()->valueChanged(i);
                break;
            }

        client()->popupDidHide();
    }

    IIntuition->DisposeObject(menu);
}

void PopupMenu::hide()
{
}

void PopupMenu::updateFromElement()
{
}

bool PopupMenu::itemWritingDirectionIsNatural()
{
    return false;
}

}
