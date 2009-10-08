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
#include "PtrAndFlags.h"
#include "WebPopup.h"
#include "GraphicsContext.h"
#include "IntRect.h"
#include "Color.h"
#include "TextRun.h"
#include "Font.h"
#include "FontSelector.h"
#include "WebView.h"
#include "WebFrame.h"
#include "Frame.h"
#include "FrameView.h"
#include "Image.h"
#include "RenderTheme.h"
#include "PlatformString.h"
#include "PopupMenu.h"

using namespace WebCore;

static int scrollbarSize = 10;
const int separatorPadding = 4;
const int separatorHeight = 1;

WebPopup::WebPopup(WebCore::PopupMenu* popup, WebView* view)
    : m_view(view)
    , isPainted(false)
    , m_popup(popup)
    , m_scrollNeeded(false)
    , m_scrollIndex(0)
    , m_isInitialized(false)
{
    m_mainSurface = m_view->viewWindow();
    setMainWindow(m_view);
}

WebPopup::~WebPopup()
{
    if (m_surface)
        SDL_FreeSurface(m_surface);
}

void WebPopup::onExpose(BalEventExpose event)
{
    if (!m_surface || isPainted)
        return;

    isPainted = true;

    SDL_Rect sdlSrc, sdlDest;
    sdlSrc.x = 0;
    sdlSrc.y = 0;
    sdlDest.x = m_view->frameRect().x;
    sdlDest.y = m_view->frameRect().y;
    sdlSrc.w = sdlDest.w = m_surface->w;
    sdlSrc.h = sdlDest.h = m_surface->h;

    GraphicsContext ctx(m_surface);
    ctx.setBalExposeEvent(&event);
    ctx.save();

    // Paint background
    ctx.beginTransparencyLayer(0.8);
    ctx.fillRect(IntRect(0, 0, m_surface->w, m_surface->h), Color(0xA0A0A0A0));
    ctx.endTransparencyLayer();
    paintPopup(m_popup->client()->selectedIndex());

    // Blit and update
    SDL_BlitSurface(m_surface, &sdlSrc, m_mainSurface, &sdlSrc);
    SDL_BlitSurface(m_mainSurface, &sdlSrc, SDL_GetVideoSurface(), &sdlDest);
    if (SDL_GetVideoSurface()->flags & SDL_DOUBLEBUF)
        SDL_Flip(SDL_GetVideoSurface());
    else
        SDL_UpdateRect(SDL_GetVideoSurface(), sdlDest.x, sdlDest.y, sdlDest.w, sdlDest.h);
    ctx.restore();
}

void WebPopup::paintScrollbar()
{
    GraphicsContext ctx(m_surface);
    ctx.save();
    int sizeOut = m_popup->windowRect().bottom() - m_surface->h;
    int height = m_popup->windowRect().height() - sizeOut;
    IntRect scrollBg = IntRect(m_popup->windowRect().right(), m_popup->windowRect().y(), scrollbarSize, height);
    ctx.fillRect(scrollBg, Color::white);
    IntPoint p1 = IntPoint(m_popup->windowRect().right() + (scrollbarSize / 2), m_popup->windowRect().y());
    IntPoint p2 = IntPoint(m_popup->windowRect().right() + (scrollbarSize / 2), m_popup->windowRect().bottom() - sizeOut);
    ctx.drawLine(p1, p2);
    
    // Draw Thumb
    int itemCount = m_popup->client()->listSize();
    float per = (float)m_itemNumber / (float)itemCount;
    m_thumb = IntRect(m_popup->windowRect().right(), (int)(m_popup->windowRect().y() + (m_scrollIndex * m_popup->itemHeight() * per)), scrollbarSize, (int)(height * per));
    ctx.fillRect(m_thumb, Color::gray);
    ctx.restore();
}

void WebPopup::paintPopup(int highligth)
{
    GraphicsContext ctx(m_surface);
    ctx.save();
    int itemCount = m_popup->client()->listSize();
    
    if (!m_isInitialized) {
        if (m_popup->windowRect().bottom() > m_surface->h) {
            m_scrollNeeded = true;
            int sizeOut = m_popup->windowRect().bottom() - m_surface->h;
            m_itemNumber = (m_popup->windowRect().height() - sizeOut) / m_popup->itemHeight();
        } else
            m_itemNumber = itemCount;
        m_isInitialized = true;
    }

    for (int i = m_scrollIndex; i < m_itemNumber + m_scrollIndex; ++i) {
        Color optionBackgroundColor, optionTextColor;
        PopupMenuStyle itemStyle = m_popup->client()->itemStyle(i);
        if ((i - m_scrollIndex) == highligth) {
            optionBackgroundColor = RenderTheme::defaultTheme()->activeListBoxSelectionBackgroundColor();
            optionTextColor = RenderTheme::defaultTheme()->activeListBoxSelectionForegroundColor();
        } else {
            optionBackgroundColor = itemStyle.backgroundColor();
            optionTextColor = itemStyle.foregroundColor();
        }
        
        // Draw the background for this menu item
        if (itemStyle.isVisible())
            ctx.fillRect(IntRect(m_popup->windowRect().x(), m_popup->windowRect().y() + (m_popup->itemHeight() * (i - m_scrollIndex)), m_popup->windowRect().width(), m_popup->itemHeight()), optionBackgroundColor);

        if (m_popup->client()->itemIsSeparator(i)) {
            IntRect separatorRect(m_popup->windowRect().x() + separatorPadding, m_popup->windowRect().y() + (m_popup->itemHeight() - separatorHeight) / 2, m_popup->windowRect().width() - 2 * separatorPadding, separatorHeight);
            ctx.fillRect(separatorRect, optionTextColor);
            continue;
        }

        String text = m_popup->client()->itemText(i);
        if (text.isEmpty())
            continue;

        unsigned length = text.length();
        const UChar* string = text.characters();
        TextRun textRun(string, length, false, 0, 0, text.defaultWritingDirection() == WTF::Unicode::RightToLeft);

        ctx.setFillColor(optionTextColor);

        Font itemFont = m_popup->client()->menuStyle().font();
        if (m_popup->client()->itemIsLabel(i)) {
            FontDescription d = itemFont.fontDescription();
            d.setWeight(d.bolderWeight());
            itemFont = Font(d, itemFont.letterSpacing(), itemFont.wordSpacing());
            itemFont.update(m_popup->client()->fontSelector());
        }

        // Draw the item text
        if (itemStyle.isVisible()) {
            int textX = max(m_popup->windowRect().x(), m_popup->windowRect().x() + m_popup->client()->clientPaddingLeft() - m_popup->client()->clientInsetLeft());
            int textY = m_popup->windowRect().y() + (i - m_scrollIndex) * m_popup->itemHeight() + itemFont.ascent() + (m_popup->itemHeight() - itemFont.height()) / 2;
            ctx.drawBidiText(itemFont, textRun, IntPoint(textX, textY));
        }
    }
    
    if (m_scrollNeeded)
        paintScrollbar();
    
    ctx.restore();
}

void WebPopup::updatePopup()
{
    SDL_Rect sdlSrc, sdlDest;
    sdlSrc.x = m_popup->windowRect().x();
    sdlSrc.y = m_popup->windowRect().y();
    sdlDest.x = m_popup->windowRect().x() + m_view->frameRect().x;
    sdlDest.y = m_popup->windowRect().y() + m_view->frameRect().y;
    sdlSrc.w = sdlDest.w = m_popup->windowRect().width() + scrollbarSize;
    sdlSrc.h = sdlDest.h = m_popup->windowRect().height();

    SDL_BlitSurface(m_surface, &sdlSrc, m_mainSurface, &sdlSrc);
    SDL_BlitSurface(m_mainSurface, &sdlSrc, SDL_GetVideoSurface(), &sdlDest);
    if (SDL_GetVideoSurface()->flags & SDL_DOUBLEBUF)
        SDL_Flip(SDL_GetVideoSurface());
    else
        SDL_UpdateRect(SDL_GetVideoSurface(), sdlDest.x, sdlDest.y, sdlDest.w, sdlDest.h);
}

void WebPopup::valueChanged()
{
    GraphicsContext ctx(m_surface);
    ctx.save();
    ctx.fillRect(IntRect(m_popup->windowRect().x(), m_popup->windowRect().y() - m_popup->itemHeight(), m_popup->windowRect().width(), m_popup->itemHeight()), Color(0xFF000000));

    SDL_Rect sdlSrc, sdlDest;
    sdlSrc.x = m_popup->windowRect().x();
    sdlSrc.y = m_popup->windowRect().y() - m_popup->itemHeight();
    sdlDest.x = m_popup->windowRect().x() + m_view->frameRect().x;
    sdlDest.y = m_popup->windowRect().y()  - m_popup->itemHeight() + m_view->frameRect().y;
    sdlSrc.w = sdlDest.w = m_popup->windowRect().width();
    sdlSrc.h = sdlDest.h = m_popup->itemHeight();

    SDL_BlitSurface(m_surface, &sdlSrc, m_mainSurface, &sdlSrc);
    SDL_BlitSurface(m_mainSurface, &sdlSrc, SDL_GetVideoSurface(), &sdlDest);
    if (SDL_GetVideoSurface()->flags & SDL_DOUBLEBUF)
        SDL_Flip(SDL_GetVideoSurface());
    else
        SDL_UpdateRect(SDL_GetVideoSurface(), sdlDest.x, sdlDest.y, sdlDest.w, sdlDest.h);
}

void WebPopup::onKeyDown(BalEventKey event)
{
    if (!m_surface)
        return;

    switch (event.keysym.sym) {
    case SDLK_RETURN:
    {
        m_popup->client()->valueChanged(m_popup->focusedIndex());
        m_popup->client()->setTextFromItem(m_popup->focusedIndex());
        hide();
        m_popup->client()->popupDidHide();
        m_view->addToDirtyRegion(IntRect(0, 0, m_surface->w, m_surface->h));
        SDL_Event ev;
        ev.type = SDL_VIDEOEXPOSE;
        SDL_PushEvent(&ev);
        return;
    }
    case SDLK_ESCAPE:
    {
        hide();
        m_popup->client()->popupDidHide();
        m_view->addToDirtyRegion(IntRect(0, 0, m_surface->w, m_surface->h));
        SDL_Event ev;
        ev.type = SDL_VIDEOEXPOSE;
        SDL_PushEvent(&ev);
        return;
    }
    case SDLK_DOWN:
    {
        if (m_popup->down()) {
            paintPopup(m_popup->focusedIndex());
            updatePopup();
        }
        return;
    }
    case SDLK_UP:
    {
        if (m_popup->up()) {
            paintPopup(m_popup->focusedIndex());
            updatePopup();
        }
        return;
    }
    default:
        return;
    }
}

void WebPopup::onKeyUp(BalEventKey event)
{
    if (!m_surface)
        return;
}

void WebPopup::onMouseMotion(BalEventMotion event)
{
    if (!m_surface)
        return;

    if ((event.x > m_popup->windowRect().x() + m_view->frameRect().x && event.x < m_popup->windowRect().right() + m_view->frameRect().x) && (event.y > m_popup->windowRect().y() + m_view->frameRect().y && event.y < m_popup->windowRect().bottom() + m_view->frameRect().x)) {
        int i = (event.y - m_popup->windowRect().y() - m_view->frameRect().y) / m_popup->itemHeight();

        if (i != m_popup->focusedIndex()) {
            m_popup->setFocusedIndex(i);
            paintPopup(m_popup->focusedIndex());
            updatePopup();
        }
    } else {
        if (m_scrolled && ((event.x > m_thumb.x() + m_view->frameRect().x && event.x < m_thumb.right() + m_view->frameRect().x) && (event.y > m_thumb.y() + m_view->frameRect().y && event.y < m_thumb.bottom() + m_view->frameRect().y))) {
            if (m_position > event.y) {
                m_scrollIndex--;
                if (m_scrollIndex < 0 ) 
                    m_scrollIndex = 0;
            } else {
                int itemCount = m_popup->client()->listSize();
                m_scrollIndex++;
                if (m_scrollIndex + m_itemNumber > itemCount ) 
                    m_scrollIndex = m_scrollIndex--;
            }
            m_position = event.y;
            paintPopup(m_popup->focusedIndex());
            updatePopup();
        }
    }
}

void WebPopup::onMouseButtonDown(BalEventButton event)
{
    if (!m_surface)
        return;

    if ((event.x > m_popup->windowRect().x() + m_view->frameRect().x && event.x < m_popup->windowRect().right() + m_view->frameRect().x) && (event.y > m_popup->windowRect().y() + m_view->frameRect().y && event.y < m_popup->windowRect().bottom() + m_view->frameRect().y)) {
        hide();
        int i = (event.y - m_popup->windowRect().y() - m_view->frameRect().y) / m_popup->itemHeight();
        m_popup->client()->valueChanged(i + m_scrollIndex);
        m_popup->client()->popupDidHide();
        m_view->addToDirtyRegion(IntRect(0, 0, m_surface->w, m_surface->h));
        SDL_Event ev;
        ev.type = SDL_VIDEOEXPOSE;
        SDL_PushEvent(&ev);
        m_popup->client()->setTextFromItem(i + m_scrollIndex);
    } else {
        if ((event.x > m_thumb.x() + m_view->frameRect().x && event.x < m_thumb.right() + m_view->frameRect().x) && (event.y > m_thumb.y() + m_view->frameRect().y && event.y < m_thumb.bottom() + m_view->frameRect().y)) {
            m_scrolled = true;
            m_position = event.y;
        } else {
            hide();
            m_popup->client()->popupDidHide();
            m_view->addToDirtyRegion(IntRect(0, 0, m_surface->w, m_surface->h));
            SDL_Event ev;
            ev.type = SDL_VIDEOEXPOSE;
            SDL_PushEvent(&ev);
            SDL_Event butev;
            butev.type = SDL_MOUSEBUTTONDOWN;
            butev.button = event;
            SDL_PushEvent(&butev);
        }
    }
}

void WebPopup::onMouseButtonUp(BalEventButton event)
{
    if (!m_surface)
        return;

    m_scrolled = false;
}

void WebPopup::onScroll(BalEventScroll event)
{
    if (!m_surface)
        return;

    int itemCount = m_popup->client()->listSize();
    switch (event.button) {
    case SDL_BUTTON_WHEELUP:
        m_scrollIndex--;
        if (m_scrollIndex < 0 ) 
            m_scrollIndex = 0;
        else {
            paintPopup(m_popup->focusedIndex());
            updatePopup();
        }
        break;
    case SDL_BUTTON_WHEELDOWN:
        m_scrollIndex++;
        if (m_scrollIndex + m_itemNumber > itemCount ) 
            m_scrollIndex = m_scrollIndex--;
        else {
            paintPopup(m_popup->focusedIndex());
            updatePopup();
        }
        break;
    }
}

void WebPopup::onResize(BalResizeEvent event)
{
    if (!m_surface)
        return;
}

void WebPopup::onQuit(BalQuitEvent)
{
    if (!m_surface)
        return;
}

void WebPopup::onUserEvent(BalUserEvent)
{
    if (!m_surface)
        return;
}

