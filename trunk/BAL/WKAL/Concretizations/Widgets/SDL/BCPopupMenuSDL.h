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


#ifndef PopupMenu_h
#define PopupMenu_h

#include <wtf/RefCounted.h>

#include "IntRect.h"
#include "PopupMenuClient.h"
#include "ScrollBar.h"
#include "ScrollbarClient.h"
#include <wtf/PassRefPtr.h>
#include "BALBase.h"

#include <wtf/HashMap.h>

namespace WKAL {

class FrameView;
class PlatformScrollbar;

class PopupMenu : public RefCounted<PopupMenu>, private ScrollbarClient
{
public:
    static PassRefPtr<PopupMenu> create(PopupMenuClient* client) { return new PopupMenu(client); }
    ~PopupMenu();
    
    void disconnectClient() { m_popupClient = 0; }

    void show(const IntRect&, FrameView*, int index);
    void hide();

    void updateFromElement();
    
    PopupMenuClient* client() const { return m_popupClient; }

    static bool itemWritingDirectionIsNatural();
    
    IntRect windowRect();

protected:
    PopupMenu(PopupMenuClient* client);

    //ScrollBarClient
    virtual void valueChanged(Scrollbar*);
    virtual IntRect windowClipRect() const;
    virtual bool isActive() const { return true; }
private:
    PopupMenuClient* m_popupClient;
    
    IntPoint m_menuPosition;
    BalMenu* m_popup;
    HashMap<BalWidget*, int> m_indexMap;
    static void menuItemActivated(BalMenuItem* item, PopupMenu*);
    static void menuUnmapped(BalWidget*, PopupMenu*);
    static void menuPositionFunction(BalMenu*, int*, int*, bool*, PopupMenu*);
    static void menuRemoveItem(BalWidget*, PopupMenu*);
    void calculatePositionAndSize(const IntRect&, FrameView*);
    void invalidateItem(int index);
    bool setFocusedIndex(int i, bool hotTracking = false);
    int focusedIndex() const;
    IntRect clientRect() const;
    int visibleItems() const;
    bool scrollToRevealSelection();

    RefPtr<PlatformScrollbar> m_scrollBar;
    bool m_wasClicked;
    IntRect m_windowRect;
    int m_itemHeight;
    int m_scrollOffset;
    int m_wheelDelta;
    int m_focusedIndex;
    bool m_scrollbarCapturingMouse;
};

}

#endif
