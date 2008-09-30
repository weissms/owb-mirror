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
#include "PopupMenu.h"

#include "CString.h"
#include "FrameView.h"
#include "Font.h"
#include "FontDescription.h"
#include "FloatRect.h"
#include "FontSelector.h"
#include "PlatformScreen.h"
#include "PlatformString.h"
#include "RenderStyle.h"
#include "ObserverServiceData.h"
#if PLATFORM(AMIGAOS4)
#include "cairo.h"
#else
#include "SDL.h"
#endif

using std::min;

namespace WKAL {

// Default Window animation duration in milliseconds
static const int defaultAnimationDuration = 200;
// Maximum height of a popup window
static const int maxPopupHeight = 320;

static const int popupWindowAlphaPercent = 95;

const int optionSpacingMiddle = 1;
const int popupWindowBorderWidth = 1;

// FIXME: Remove this as soon as practical.
static inline bool isASCIIPrintable(unsigned c)
{
    return c >= 0x20 && c <= 0x7E;
}

PopupMenu::PopupMenu(PopupMenuClient* client)
    : RefCounted<PopupMenu>(0)
    , m_popupClient(client)
    , m_popup(0)
    , m_scrollBar(0)
    , m_wasClicked(false)
    , m_itemHeight(0)
    , m_scrollOffset(0)
    , m_wheelDelta(0)
    , m_focusedIndex(0)
    , m_scrollbarCapturingMouse(false)
{
}

PopupMenu::~PopupMenu()
{
    if (m_popup)
#if PLATFORM(AMIGAOS4)
        cairo_surface_destroy(m_popup);
#else
        SDL_FreeSurface(m_popup);
#endif
}

void PopupMenu::show(const IntRect& r, FrameView* v, int index)
{
    ASSERT(client());
    
    calculatePositionAndSize(r, v);
    if (clientRect().isEmpty())
        return;

    OWBAL::ObserverServiceData::createObserverService()->notifyObserver("PopupMenuShow", "", this);
}

int PopupMenu::focusedIndex() const
{
    return m_focusedIndex;
}

bool PopupMenu::setFocusedIndex(int i, bool hotTracking)
{
    return true;
}

bool PopupMenu::scrollToRevealSelection()
{
    return true;
}

void PopupMenu::hide()
{
    OWBAL::ObserverServiceData::createObserverService()->notifyObserver("PopupMenuHide", "", this);
}

void PopupMenu::updateFromElement()
{
}

bool PopupMenu::itemWritingDirectionIsNatural()
{
    return false;
}

void PopupMenu::menuItemActivated(BalMenuItem* item, PopupMenu* that)
{
    ASSERT(that->client());

}

void PopupMenu::menuUnmapped(BalWidget*, PopupMenu* that)
{
    ASSERT(that->client());
    that->client()->hidePopup();
}

void PopupMenu::menuPositionFunction(BalMenu*, int* x, int* y, bool* pushIn, PopupMenu* that)
{
    *x = that->m_menuPosition.x();
    *y = that->m_menuPosition.y();
    *pushIn = true;
}

void PopupMenu::menuRemoveItem(BalWidget* widget, PopupMenu* that)
{
    ASSERT(that->m_popup);

}

IntRect PopupMenu::windowRect()
{
    return m_windowRect;
}

const int endOfLinePadding = 2;
void PopupMenu::calculatePositionAndSize(const IntRect& r, FrameView* v)
{
    IntRect rScreenCoords(v->contentsToWindow(r.location()), r.size());
    rScreenCoords.setY(rScreenCoords.y() + rScreenCoords.height());
    
    m_itemHeight = rScreenCoords.height();

    int itemCount = client()->listSize();
    int naturalHeight = m_itemHeight * itemCount;

    int popupWidth = 0;
    for (int i = 0; i < itemCount; ++i) {
        String text = client()->itemText(i);
        if (text.isEmpty())
            continue;

        Font itemFont = client()->clientStyle()->font();
        if (client()->itemIsLabel(i)) {
            FontDescription d = itemFont.fontDescription();
            d.setWeight(d.bolderWeight());
            itemFont = Font(d, itemFont.letterSpacing(), itemFont.wordSpacing());
            itemFont.update(m_popupClient->fontSelector());
        }

        popupWidth = max(popupWidth, itemFont.width(TextRun(text.characters(), text.length())));
    }
    
    rScreenCoords.setHeight(naturalHeight);
    rScreenCoords.setWidth(popupWidth + 10);

    m_windowRect = rScreenCoords;
}

void PopupMenu::invalidateItem(int index)
{
    if (!m_popup)
        return;

    IntRect damageRect(clientRect());
    damageRect.setY(m_itemHeight * (index - m_scrollOffset));
    damageRect.setHeight(m_itemHeight);
    if (m_scrollBar)
        damageRect.setWidth(damageRect.width() - m_scrollBar->frameRect().width());

    IntRect r = damageRect;
    //::InvalidateRect(m_popup, &r, TRUE);
}

IntRect PopupMenu::clientRect() const
{
    IntRect clientRect = m_windowRect;
    clientRect.inflate(-popupWindowBorderWidth);
    clientRect.setLocation(IntPoint(0, 0));
    return clientRect;
}

int PopupMenu::visibleItems() const
{
    return clientRect().height() / m_itemHeight;
}

IntRect PopupMenu::windowClipRect() const
{
    return m_windowRect;
}

void PopupMenu::valueChanged(Scrollbar* scrollBar)
{
}

}
