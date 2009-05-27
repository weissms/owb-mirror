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
#include "WebWindowConfirm.h"
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

#define MIN 40

using namespace WebCore;
using namespace std;

static int fontSize = 20;

WebWindowConfirm::WebWindowConfirm(const char* text, WebView *view)
    : m_text(text)
    , m_buttonOk("OK")
    , m_buttonCancel("Cancel")
    , m_button(false)
    , m_stateLeft(false)
    , m_stateRigth(false)
    , m_view(view)
    , isPainted(false)
    , m_value(false)
    , m_confirm(false)
    , isThemable(false)
{
    m_mainSurface = m_view->viewWindow();
    setMainWindow(m_view);
}

WebWindowConfirm::~WebWindowConfirm()
{
    if (m_surface)
        SDL_FreeSurface(m_surface);
}

void WebWindowConfirm::onExpose(BalEventExpose event)
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
    ctx.fillRect(IntRect(0, 0, m_surface->w, m_surface->h), Color(0xA0A0A0A0)/*Color::gray*/);
    ctx.endTransparencyLayer();

    RefPtr<WebCore::Image> cornerhl = Image::loadPlatformResource("/Alert/cornerhl");
    if (cornerhl) {
        RefPtr<WebCore::Image> cornerhr = Image::loadPlatformResource("/Alert/cornerhr");
        RefPtr<WebCore::Image> cornerbl = Image::loadPlatformResource("/Alert/cornerbl");
        RefPtr<WebCore::Image> cornerbr = Image::loadPlatformResource("/Alert/cornerbr");
        RefPtr<WebCore::Image> borderh = Image::loadPlatformResource("/Alert/borderh");
        RefPtr<WebCore::Image> borderb = Image::loadPlatformResource("/Alert/borderb");
        RefPtr<WebCore::Image> borderl = Image::loadPlatformResource("/Alert/borderl");
        RefPtr<WebCore::Image> borderr = Image::loadPlatformResource("/Alert/borderr");
        RefPtr<WebCore::Image> bg = Image::loadPlatformResource("/Alert/background");
        RefPtr<WebCore::Image> buttonOK = Image::loadPlatformResource("/Alert/buttonOK");
        RefPtr<WebCore::Image> buttonCancel = Image::loadPlatformResource("/Alert/buttonCancel");
        if (!cornerhr->isNull() && !cornerbl->isNull() && !cornerbr->isNull() && !borderh->isNull() && !borderb->isNull() && !borderl->isNull() && !borderr->isNull() && !bg->isNull() && !buttonOK->isNull() && !buttonCancel->isNull()) {
            isThemable = true;
            fontSize = 13;

            // Create font
            FontDescription fontDescription;
            // this is normally computed by CSS and fixes the minimum size
            fontDescription.setComputedSize(fontSize);
            Font font(fontDescription,0,0);
            // update m_fontList
            // needed or else Assertion `m_fontList' will failed.
            font.update(0);

            int size = (font.width(TextRun(m_text.c_str())) / bg->width()) + 1;

            if (size < MIN)
                size = MIN;

            // Draw Hborder
            IntPoint startPos((m_surface->w - (cornerhl->width() * size)) / 2, (m_surface->h / 2) - 30);
            ctx.drawImage(cornerhl.get(), startPos);
            IntPoint m = startPos;
            for (int i = 0; i < size - 1; i++) {
                m.move(borderh->width(), 0);
                ctx.drawImage(borderh.get(), m);
            }
            m.move(borderh->width(), 0);
            ctx.drawImage(cornerhr.get(), m);

            IntPoint startText = startPos;
            startText.move(borderh->width(), cornerhl->height() * 2);

            // Draw Text Backgroung + rigth and left border
            for(int j=0; j < 2; j++) {
                startPos.move(0, cornerhl->height());
                ctx.drawImage(borderl.get(), startPos);
                m = startPos;
                for (int i = 0; i < size - 1; i++) {
                    m.move(bg->width(), 0);
                    ctx.drawImage(bg.get(), m);
                }
                m.move(bg->width(), 0);
                ctx.drawImage(borderr.get(), m);
            }

            // Draw BackGround Button
            startPos.move(0, cornerhl->height());
            ctx.drawImage(borderl.get(), startPos);
            m = startPos;
            for (int i = 0; i < size - 1; i++) {
                m.move(bg->width(), 0);
                ctx.drawImage(bg.get(), m);
            }
            m.move(bg->width(), 0);
            ctx.drawImage(borderr.get(), m);

            m_buttonOKRect = IntRect((m_surface->w / 2) - (buttonOK->width()) - 10 , m.y(), buttonOK->width(), buttonOK->height());
            m_buttonCancelRect = IntRect((m_surface->w / 2), m.y(), buttonCancel->width(), buttonCancel->height());

            // Draw BBorder
            startPos.move(0, cornerbl->height());
            ctx.drawImage(cornerbl.get(), startPos);
            m = startPos;
            for (int i = 0; i < size - 1; i++) {
                m.move(bg->width(), 0);
                ctx.drawImage(borderb.get(), m);
            }
            m.move(bg->width(), 0);
            ctx.drawImage(cornerbr.get(), m);
            
            // Draw button
            IntRect buttonOKRect = m_buttonOKRect;
            IntRect buttonCancelRect = m_buttonCancelRect;
            ctx.drawImage(buttonOK.get(), buttonOKRect.location());
            ctx.drawImage(buttonCancel.get(), buttonCancelRect.location());

            // Draw Text
/*            FontDescription fontDescription;
            // this is normally computed by CSS and fixes the minimum size
            fontDescription.setComputedSize(fontSize);
            Font font(fontDescription,0,0);
            // update m_fontList
            // needed or else Assertion `m_fontList' will failed.
            font.update(0);*/
        
            TextRun textrun(m_text.c_str());
            ctx.drawText(font, textrun, startText);


            SDL_BlitSurface(m_surface, &sdlSrc, m_mainSurface, &sdlSrc);
            SDL_BlitSurface(m_mainSurface, &sdlSrc, SDL_GetVideoSurface(), &sdlDest);
            if (SDL_GetVideoSurface()->flags & SDL_DOUBLEBUF)
                SDL_Flip(SDL_GetVideoSurface());
            else
                SDL_UpdateRect(SDL_GetVideoSurface(), sdlDest.x, sdlDest.y, sdlDest.w, sdlDest.h);
            ctx.restore();
            return;
        }
    }
    
    // Paint Window background
    ctx.fillRect(IntRect(0, (m_surface->h / 2) - (fontSize * 2), m_surface->w, fontSize * 6), Color(0xFF0000CC));

    // Draw Text
    FontDescription fontDescription;
    // this is normally computed by CSS and fixes the minimum size
    fontDescription.setComputedSize(fontSize);
    Font font(fontDescription,0,0);
    // update m_fontList
    // needed or else Assertion `m_fontList' will failed.
    font.update(0);

    TextRun textrun(m_text.c_str());
    ctx.drawText(font, textrun, IntPoint(((m_surface->w / 2 ) - (textrun.length() * fontSize / 3)), m_surface->h / 2));

    // draw button
    m_stateLeft = false;
    m_stateRigth = false;
    drawButton();

    // Blit and update
    SDL_BlitSurface(m_surface, &sdlSrc, m_mainSurface, &sdlSrc);
    SDL_BlitSurface(m_mainSurface, &sdlSrc, SDL_GetVideoSurface(), &sdlDest);
    if (SDL_GetVideoSurface()->flags & SDL_DOUBLEBUF)
        SDL_Flip(SDL_GetVideoSurface());
    else
        SDL_UpdateRect(SDL_GetVideoSurface(), sdlDest.x, sdlDest.y, sdlDest.w, sdlDest.h);
    ctx.restore();
}

void WebWindowConfirm::drawButton()
{
    if (isThemable)
        return;

    GraphicsContext ctx(m_surface);
    ctx.save();
    // Draw left button
    if (!m_stateLeft) {
        ctx.setFillColor(Color::darkGray);
        ctx.drawRect(IntRect(m_surface->w/2 - 100, (m_surface->h / 2) + (fontSize * 2), 80, 40));
        ctx.setFillColor(Color::gray);
        ctx.drawRect(IntRect(m_surface->w/2 - 100, (m_surface->h / 2) + (fontSize * 2), 77, 37));
    } else {
        ctx.setFillColor(Color::gray);
        ctx.drawRect(IntRect(m_surface->w/2 - 100, (m_surface->h / 2) + (fontSize * 2), 80, 40));
        ctx.setFillColor(Color::darkGray);
        ctx.drawRect(IntRect(m_surface->w/2 - 100, (m_surface->h / 2) + (fontSize * 2), 77, 37));
    }

    FontDescription fontDescription;
    // this is normally computed by CSS and fixes the minimum size
    fontDescription.setComputedSize(fontSize);
    Font font(fontDescription,0,0);
    // update m_fontList
    // needed or else Assertion `m_fontList' will failed.
    font.update(0);

    ctx.setFillColor(Color::black);
    TextRun textrun(m_buttonOk.c_str());
    ctx.drawText(font, textrun, IntPoint((m_surface->w / 2 ) - 85, (m_surface->h / 2) + (fontSize * 2) + 25));

    // Draw rigth button

    if (!m_stateRigth) {
        ctx.setFillColor(Color::darkGray);
        ctx.drawRect(IntRect(m_surface->w/2 + 40, (m_surface->h / 2) + (fontSize * 2), 80, 40));
        ctx.setFillColor(Color::gray);
        ctx.drawRect(IntRect(m_surface->w/2 + 40, (m_surface->h / 2) + (fontSize * 2), 77, 37));
    } else {
        ctx.setFillColor(Color::gray);
        ctx.drawRect(IntRect(m_surface->w/2 + 40, (m_surface->h / 2) + (fontSize * 2), 80, 40));
        ctx.setFillColor(Color::darkGray);
        ctx.drawRect(IntRect(m_surface->w/2 + 40, (m_surface->h / 2) + (fontSize * 2), 77, 37));
    }

    ctx.setFillColor(Color::black);
    TextRun textrun2(m_buttonCancel.c_str());
    ctx.drawText(font, textrun2, IntPoint((m_surface->w / 2 ) + 45, (m_surface->h / 2) + (fontSize * 2) + 25));

    ctx.restore();
}

void WebWindowConfirm::updateButton()
{
    if (isThemable)
        return;

    SDL_Rect sdlSrc, sdlDest;
    if (!m_button)
        sdlSrc.x = sdlDest.x = m_surface->w/2 - 100;
    else
        sdlSrc.x = sdlDest.x = m_surface->w/2 + 40;
    sdlSrc.y = sdlDest.y = (m_surface->h / 2) + (fontSize * 2);
    sdlDest.x += m_view->frameRect().x;
    sdlDest.y += m_view->frameRect().y;
    sdlSrc.w = sdlDest.w = 80;
    sdlSrc.h = sdlDest.h = 40;
    SDL_BlitSurface(m_surface, &sdlSrc, m_mainSurface, &sdlSrc);
    SDL_BlitSurface(m_mainSurface, &sdlSrc, SDL_GetVideoSurface(), &sdlDest);
    if (SDL_GetVideoSurface()->flags & SDL_DOUBLEBUF)
        SDL_Flip(SDL_GetVideoSurface());
    else
        SDL_UpdateRect(SDL_GetVideoSurface(), sdlDest.x, sdlDest.y, sdlDest.w, sdlDest.h);
}

void WebWindowConfirm::onKeyDown(BalEventKey event)
{
    if (!m_surface)
        return;

    switch (event.keysym.sym) {
    case SDLK_RETURN:
    {
        m_confirm = true;
        m_value = true;
        hide();
        m_view->addToDirtyRegion(IntRect(0, 0, m_surface->w, m_surface->h));
        SDL_Event ev;
        ev.type = SDL_VIDEOEXPOSE;
        SDL_PushEvent(&ev);
        return;
    }
    default:
        return;
    }
}

void WebWindowConfirm::onKeyUp(BalEventKey event)
{
    if (!m_surface)
        return;
}

void WebWindowConfirm::onMouseMotion(BalEventMotion event)
{
    if (!m_surface)
        return;

    if (isThemable) {
        IntRect buttonOKRect = m_buttonOKRect;
        IntRect buttonCancelRect = m_buttonCancelRect;
        if ((event.x > buttonOKRect.x() + m_view->frameRect().x && event.x < buttonOKRect.right() + m_view->frameRect().x) && (event.y > buttonOKRect.y() + m_view->frameRect().y && event.y < buttonOKRect.bottom() + m_view->frameRect().y)) {
            if (!m_stateLeft) {
                m_button = false;
                m_stateLeft = true;
                drawButton();
                updateButton();
            }
        } else if ((event.x > buttonCancelRect.x() + m_view->frameRect().x && event.x < buttonCancelRect.right() + m_view->frameRect().x) && (event.y > buttonCancelRect.y() + m_view->frameRect().y && event.y < buttonCancelRect.bottom() + m_view->frameRect().y)) {
            if (!m_stateRigth) {
                m_button = true;
                m_stateRigth = true;
                drawButton();
                updateButton();
            }
        } 
    } else if ((event.x > (m_surface->w/2 - 100) + m_view->frameRect().x && event.x < (m_surface->w/2 - 20) + m_view->frameRect().x) && ( event.y > (m_surface->h / 2) + (fontSize * 2) + m_view->frameRect().y && event.y < (m_surface->h / 2) + (fontSize * 2) + 40 + m_view->frameRect().y)) {
        if (!m_stateLeft) {
            m_button = false;
            m_stateLeft = true;
            drawButton();
            updateButton();
        }
    } else {
        if ((event.x > (m_surface->w/2 + 40) + m_view->frameRect().x && event.x < (m_surface->w/2 + 120) + m_view->frameRect().x) && ( event.y > (m_surface->h / 2) + (fontSize * 2) + m_view->frameRect().y && event.y < (m_surface->h / 2) + (fontSize * 2) + 40 + m_view->frameRect().y)) {
            if (!m_stateRigth) {
                m_button = true;
                m_stateRigth = true;
                drawButton();
                updateButton();
            }
        } else {
            if (m_stateLeft) {
                m_button = false;
                m_stateLeft = false;
                drawButton();
                updateButton();
            }
            if (m_stateRigth) {
                m_button = true;
                m_stateRigth = false;
                drawButton();
                updateButton();
            }
        }
    }
}

void WebWindowConfirm::onMouseButtonDown(BalEventButton event)
{
    if (!m_surface)
        return;

    if (isThemable) {
        IntRect buttonOKRect = m_buttonOKRect;
        IntRect buttonCancelRect = m_buttonCancelRect;
        if ((event.x > buttonOKRect.x() + m_view->frameRect().x && event.x < buttonOKRect.right() + m_view->frameRect().x) && (event.y > buttonOKRect.y() + m_view->frameRect().y && event.y < buttonOKRect.bottom() + m_view->frameRect().y)) {
            hide();
            m_view->addToDirtyRegion(IntRect(0, 0, m_surface->w, m_surface->h));
            SDL_Event ev;
            ev.type = SDL_VIDEOEXPOSE;
            SDL_PushEvent(&ev);
            m_confirm = true;
            m_value = true;
        } else if ((event.x > buttonCancelRect.x() + m_view->frameRect().x && event.x < buttonCancelRect.right() + m_view->frameRect().x) && (event.y > buttonCancelRect.y() + m_view->frameRect().y && event.y < buttonCancelRect.bottom() + m_view->frameRect().y)) {
            hide();
            m_view->addToDirtyRegion(IntRect(0, 0, m_surface->w, m_surface->h));
            SDL_Event ev;
            ev.type = SDL_VIDEOEXPOSE;
            SDL_PushEvent(&ev);
            m_confirm = true;
            m_value = false;
        }
    } else if ((event.x > (m_surface->w/2 - 100) + m_view->frameRect().x && event.x < (m_surface->w/2 - 20) + m_view->frameRect().x) && ( event.y > (m_surface->h / 2) + (fontSize * 2) + m_view->frameRect().y && event.y < (m_surface->h / 2) + (fontSize * 2) + 40 + m_view->frameRect().y)) {
        hide();
        m_view->addToDirtyRegion(IntRect(0, 0, m_surface->w, m_surface->h));
        SDL_Event ev;
        ev.type = SDL_VIDEOEXPOSE;
        SDL_PushEvent(&ev);
        m_confirm = true;
        m_value = true;
    } else {
        if ((event.x > (m_surface->w/2 + 40) + m_view->frameRect().x && event.x < (m_surface->w/2 + 120) + m_view->frameRect().x) && ( event.y > (m_surface->h / 2) + (fontSize * 2) + m_view->frameRect().y && event.y < (m_surface->h / 2) + (fontSize * 2) + 40 + m_view->frameRect().y)) {
            hide();
            m_view->addToDirtyRegion(IntRect(0, 0, m_surface->w, m_surface->h));
            SDL_Event ev;
            ev.type = SDL_VIDEOEXPOSE;
            SDL_PushEvent(&ev);
            m_confirm = true;
            m_value = false;
        }
    }
}

void WebWindowConfirm::onMouseButtonUp(BalEventButton event)
{
    if (!m_surface)
        return;
}

void WebWindowConfirm::onScroll(BalEventScroll event)
{
    if (!m_surface)
        return;
}

void WebWindowConfirm::onResize(BalResizeEvent event)
{
    if (!m_surface)
        return;
}

void WebWindowConfirm::onQuit(BalQuitEvent)
{
    if (!m_surface)
        return;
}

void WebWindowConfirm::onUserEvent(BalUserEvent)
{
    if (!m_surface)
        return;
}

bool WebWindowConfirm::value()
{
    return m_value;
}

