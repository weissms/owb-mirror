/*
 * Copyright (C) 2008 Joerg Strohmayer
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

#include DEEPSEE_INCLUDE

DS_INIT_DEEPSEE_FRAMEWORK();

#include "BCGraphicsContext.h"
#include <Api/WebFrame.h>
#include <Api/WebView.h>
#include "Page.h"
#include "Frame.h"
#include "FrameView.h"
#include "SDL.h"
#include "Timer.h"
#include "SharedTimer.h"
#include "MainThread.h"
#include <unistd.h>

#include "WebBackForwardList.h"
#include <FrameLoader.h>
#define INTUITION_PREFERENCES_H
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <intuition/gui.h>
#include <intuition/icclass.h>
#include <reaction/reaction.h>
#include <proto/bevel.h>
#include <images/bevel.h>
#include <proto/bitmap.h>
#include <images/bitmap.h>
#include <proto/button.h>
#include <gadgets/button.h>
#include <proto/label.h>
#include <images/label.h>
#include <proto/layout.h>
#include <gadgets/layout.h>
#include <proto/string.h>
#include <reaction/reaction_macros.h>
#undef String

using namespace WebCore;
static WebView *webView;
static bool isExposed = false;

#include <cstdio>

static BalWidget *s_owbwindow = NULL;
static struct Hook *window_backfill_hook;
// FIXME: make home and search URL configurable again
static char *g_amiga_home = (char *)"http://amigans.net/"; //"about:blank";
static char *g_amiga_search = (char *)"http://google.com/search?q=%s";

BalWidget *createAmigaWindow(int w, int h)
{
    SDL_Surface* surface;
    Uint32 rmask, gmask, bmask, amask;
    rmask = 0x00ff0000;
    gmask = 0x0000ff00;
    bmask = 0x000000ff;
    amask = 0xff000000;
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32,
                                   rmask, gmask, bmask, amask);

    if (!surface)
        return 0;

    BalWidget *owbwindow = (BalWidget *)calloc(sizeof (BalWidget), 1);
    if(!owbwindow) {
        SDL_FreeSurface(surface);
        return 0;
    }

    owbwindow->surface = surface;

    owbwindow->window = IIntuition->OpenWindowTags(NULL,
                                                   WA_SmartRefresh, TRUE,
                                                   WA_NoCareRefresh, TRUE,
                                                   WA_InnerWidth, w,
                                                   WA_InnerHeight, h,
                                                   WA_MinWidth, 640,
                                                   WA_MinHeight, 480,
                                                   WA_MaxWidth, ~0UL,
                                                   WA_MaxHeight, ~0UL,
                                                   WA_Title, "Origyn Web Browser",
                                                   WA_ScreenTitle, "Origyn Web Browser", // not supported?
                                                   WA_ReportMouse, TRUE,
                                                   WA_SizeGadget, TRUE,
//                                                   WA_SizeBRight, TRUE,
                                                   WA_SizeBBottom, TRUE,
                                                   WA_DragBar, TRUE,
                                                   WA_DepthGadget, TRUE,
                                                   WA_CloseGadget, TRUE,
                                                   WA_RMBTrap, TRUE,
                                                   WA_Hidden, TRUE,
                                                   WA_PubScreenName, "OrigynWebBrowser",
                                                   WA_PubScreenFallBack, TRUE,
                                                   WA_IDCMP, IDCMP_MOUSEBUTTONS
                                                           | IDCMP_MOUSEMOVE
                                                           | IDCMP_CLOSEWINDOW
                                                           | IDCMP_RAWKEY
                                                           | IDCMP_VANILLAKEY
                                                           | IDCMP_EXTENDEDMOUSE
                                                           | IDCMP_IDCMPUPDATE
                                                           | IDCMP_NEWSIZE,
                                                   TAG_DONE);
    if (!owbwindow->window) {
        SDL_FreeSurface(surface);
        free(owbwindow);
        return 0;
    }

    APTR old_proc_window = IDOS->SetProcWindow((APTR)-1);
    struct DrawInfo *drinfo = IIntuition->GetScreenDrawInfo(owbwindow->window->WScreen);
    window_backfill_hook = IIntuition->ObtainIBackFill(drinfo, BT_WINDOWBG, IDS_NORMAL, 0);
    APTR images[4];
    images[0] = BitMapObject,
                    BITMAP_Screen, owbwindow->window->WScreen,
                    BITMAP_Masking, TRUE,
                    BITMAP_Transparent, TRUE,
                    BITMAP_SourceFile, "TBImages:nav_west",
                    BITMAP_SelectSourceFile, "TBImages:nav_west_s",
                    BITMAP_DisabledSourceFile, "TBImages:nav_west_g",
                BitMapEnd;
    images[1] = BitMapObject,
                    BITMAP_Screen, owbwindow->window->WScreen,
                    BITMAP_Masking, TRUE,
                    BITMAP_Transparent, TRUE,
                    BITMAP_SourceFile, "TBImages:nav_east",
                    BITMAP_SelectSourceFile, "TBImages:nav_east_s",
                    BITMAP_DisabledSourceFile, "TBImages:nav_east_g",
                 BitMapEnd;
    images[2] = BitMapObject,
                    BITMAP_Screen, owbwindow->window->WScreen,
                    BITMAP_Masking, TRUE,
                    BITMAP_Transparent, TRUE,
                    BITMAP_SourceFile, "TBImages:stop",
                    BITMAP_SelectSourceFile, "TBImages:stop_s",
                    BITMAP_DisabledSourceFile, "TBImages:stop_g",
                BitMapEnd;
    images[3] = BitMapObject,
                    BITMAP_Screen, owbwindow->window->WScreen,
                    BITMAP_Masking, TRUE,
                    BITMAP_Transparent, TRUE,
                    BITMAP_SourceFile, "TBImages:searchweb",
                    BITMAP_SelectSourceFile, "TBImages:searchweb_s",
                    BITMAP_DisabledSourceFile, "TBImages:searchweb_g",
                BitMapEnd;
    images[4] = BitMapObject,
                    BITMAP_Screen, owbwindow->window->WScreen,
                    BITMAP_Masking, TRUE,
                    BITMAP_Transparent, TRUE,
                    BITMAP_SourceFile, "TBImages:home",
                    BITMAP_SelectSourceFile, "TBImages:home_s",
                    BITMAP_DisabledSourceFile, "TBImages:home_g",
                BitMapEnd;

    owbwindow->gad_toolbar = (struct Gadget *)VLayoutObject,
                                                  GA_Left, owbwindow->window->BorderLeft,
                                                  GA_Top, owbwindow->window->BorderTop,
                                                  GA_RelWidth, -(owbwindow->window->BorderLeft + owbwindow->window->BorderRight),
                                                  GA_NoFilterWheel, TRUE,
                                                  ICA_TARGET, ICTARGET_IDCMP,
                                                  LAYOUT_LayoutBackFill, window_backfill_hook,
                                                  LAYOUT_DeferLayout, TRUE,
                                                  LAYOUT_AddChild, HLayoutObject,
                                                      LAYOUT_AddChild, owbwindow->gad_back = (struct Gadget *)ButtonObject,
                                                          GA_ID,        1, 
                                                          GA_RelVerify, TRUE,
                                                          images[0] ? BUTTON_RenderImage : TAG_IGNORE, images[0],
                                                          images[0] ? TAG_IGNORE : BUTTON_AutoButton, BAG_LFARROW,
                                                          GA_Disabled, TRUE,
                                                      ButtonEnd,
                                                      CHILD_WeightedWidth, 0,
                                                      LAYOUT_AddChild, owbwindow->gad_forward = (struct Gadget *)ButtonObject,
                                                          GA_ID,        2, 
                                                          GA_RelVerify, TRUE,
                                                          images[1] ? BUTTON_RenderImage : TAG_IGNORE, images[1],
                                                          images[1] ? TAG_IGNORE : BUTTON_AutoButton, BAG_RTARROW,
                                                          GA_Disabled, TRUE,
                                                      ButtonEnd,
                                                      CHILD_WeightedWidth, 0,
                                                      LAYOUT_AddChild, owbwindow->gad_stop = (struct Gadget *)ButtonObject,
                                                          GA_ID,        3, 
                                                          GA_RelVerify, TRUE,
                                                          images[2] ? BUTTON_RenderImage : GA_Text, images[2] ?: "Stop",
                                                          GA_TabCycle, NULL == images[2],
                                                          GA_Disabled, TRUE,
                                                      ButtonEnd,
                                                      CHILD_WeightedWidth, 0,
#if 0
                                                      LAYOUT_AddChild, Button("Refresh", 4),
                                                      CHILD_WeightedWidth, 0,
#endif
                                                      LAYOUT_AddChild, ButtonObject,
                                                          GA_ID,        5, 
                                                          GA_RelVerify, TRUE,
                                                          images[4] ? BUTTON_RenderImage : GA_Text, images[4] ?: "Home",
                                                          GA_TabCycle, NULL == images[4],
                                                      ButtonEnd,
                                                      CHILD_WeightedWidth, 0,
                                                      LAYOUT_AddChild, owbwindow->gad_url = (struct Gadget *)StringObject,
                                                          STRINGA_TextVal, "",
                                                          STRINGA_MaxChars, 2000,
                                                          GA_ID, 6,
                                                          GA_RelVerify, TRUE,
                                                          GA_TabCycle, TRUE,
                                                      StringEnd,
                                                      CHILD_Label, LabelObject,
                                                          LABEL_DrawInfo, drinfo,
                                                      LabelEnd,
                                                      LAYOUT_AddChild, StringObject,
                                                          STRINGA_TextVal, "",
                                                          STRINGA_MaxChars, 500,
                                                          GA_ID, 7,
                                                          GA_RelVerify, TRUE,
                                                          GA_TabCycle, TRUE,
                                                      StringEnd,
                                                      CHILD_Label, LabelObject,
                                                          LABEL_DrawInfo, drinfo,
                                                          LABEL_DisposeImage, NULL != images[3],
                                                          images[3] ? LABEL_Image : TAG_IGNORE, images[3],
                                                          images[3] ? TAG_IGNORE : LABEL_Text, "Search",
                                                      LabelEnd,
                                                      CHILD_WeightedWidth, 40,
                                                  LayoutEnd,
                                                  CHILD_WeightedHeight, 0,

                                                  LAYOUT_AddImage, BevelObject,
                                                      BEVEL_Style, BVS_SBAR_VERT,
                                                  BevelEnd,
                                                  CHILD_WeightedHeight, 0,
                                              LayoutEnd;

    IIntuition->FreeScreenDrawInfo(owbwindow->window->WScreen, drinfo);
    IDOS->SetProcWindow(old_proc_window);

    if (!owbwindow->gad_toolbar) {
        IIntuition->DisposeObject(images[0]);
        IIntuition->DisposeObject(images[1]);
        IIntuition->DisposeObject(images[2]);
        IIntuition->DisposeObject(images[3]);
        IIntuition->DisposeObject(images[4]);
        IIntuition->ReleaseIBackFill(window_backfill_hook);
        window_backfill_hook = NULL;
        IIntuition->CloseWindow(owbwindow->window);
        SDL_FreeSurface(surface);
        free(owbwindow);
        return 0;
    }

    struct LayoutLimits layoutlimits;
    ILayout->LayoutLimits(owbwindow->gad_toolbar, &layoutlimits, NULL, owbwindow->window->WScreen);
    owbwindow->offsetx = owbwindow->window->BorderLeft;
    owbwindow->offsety = owbwindow->window->BorderTop + layoutlimits.MinHeight + 4;

    IIntuition->AddGadget(owbwindow->window, owbwindow->gad_toolbar, (uint16)~0);
    IIntuition->RefreshGList(owbwindow->gad_toolbar, owbwindow->window, NULL, -1);

    IIntuition->SetWindowAttrs(owbwindow->window, 
                               WA_InnerHeight, h + owbwindow->offsety,
                               WA_Hidden, FALSE,
                               WA_Activate, TRUE,
                               TAG_DONE);

    return owbwindow;
}

void closeAmigaWindow(BalWidget *owbwindow)
{
    if (!owbwindow)
        return;

    IIntuition->CloseWindow(owbwindow->window);
    IIntuition->DisposeObject(owbwindow->gad_toolbar);
    if (window_backfill_hook) {
       IIntuition->ReleaseIBackFill(window_backfill_hook);
       window_backfill_hook = NULL;
    }
    SDL_FreeSurface(owbwindow->surface);
    free(owbwindow);
}

void waitEvent()
{
    if (!s_owbwindow)
        return;

    bool quit = false;
    while (!quit) {
        struct IntuiMessage *im = (struct IntuiMessage *)IExec->GetMsg(s_owbwindow->window->UserPort);
        if (im) {
            switch (im->Class) {
            case IDCMP_CLOSEWINDOW:
                quit = true;
            break;

            case IDCMP_MOUSEBUTTONS:
                switch (im->Code & ~IECODE_UP_PREFIX) {
                case IECODE_LBUTTON:
                case IECODE_RBUTTON:
                case IECODE_MBUTTON:
                    if (im->Code & IECODE_UP_PREFIX) {
                        struct IntuiMessage im2 = *im;
                        im2.MouseX -= s_owbwindow->offsetx;
                        im2.MouseY -= s_owbwindow->offsety;
                        webView->onMouseButtonUp(im2);
                    } else {
                        struct IntuiMessage im2 = *im;
                        im2.MouseX -= s_owbwindow->offsetx;
                        im2.MouseY -= s_owbwindow->offsety;
                        webView->onMouseButtonDown(im2);
                    }
                break;
                }
            break;

            case IDCMP_MOUSEMOVE:
            {
                struct IntuiMessage im2 = *im;
                im2.MouseX -= s_owbwindow->offsetx;
                im2.MouseY -= s_owbwindow->offsety;
                webView->onMouseMotion(im2);
            }
            break;

            case IDCMP_EXTENDEDMOUSE:
            {
                struct IntuiMessage im2 = *im;
                im2.MouseX -= s_owbwindow->offsetx;
                im2.MouseY -= s_owbwindow->offsety;
                webView->onScroll(im2);
                im2.Class = IDCMP_MOUSEMOVE;
                im2.Code = 0;
                im2.Qualifier = 0;
                webView->onMouseMotion(im2);
            }
            break;

            case IDCMP_RAWKEY:
            {
                struct IntuiMessage im2 = *im;
                im2.MouseX -= s_owbwindow->offsetx;
                im2.MouseY -= s_owbwindow->offsety;
                if (im->Code & IECODE_UP_PREFIX)
                    webView->onKeyUp(im2);
                else
                    webView->onKeyDown(im2);
            }
            break;

            case IDCMP_VANILLAKEY:
            {
                struct IntuiMessage im2 = *im;
                im2.MouseX -= s_owbwindow->offsetx;
                im2.MouseY -= s_owbwindow->offsety;
                webView->onKeyDown(im2);
            }
            break;

            case IDCMP_NEWSIZE:
            {
                uint32 width = 800, height = 600;
                IIntuition->GetWindowAttr(s_owbwindow->window, WA_InnerWidth,  &width,  sizeof(width));
                IIntuition->GetWindowAttr(s_owbwindow->window, WA_InnerHeight, &height, sizeof(height));

                height -= s_owbwindow->offsety - s_owbwindow->window->BorderTop;

                SDL_FreeSurface(s_owbwindow->surface);
                Uint32 rmask, gmask, bmask, amask;
                rmask = 0x00ff0000;
                gmask = 0x0000ff00;
                bmask = 0x000000ff;
                amask = 0xff000000;
                s_owbwindow->surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32,
                                                            rmask, gmask, bmask, amask);

                struct AmigaOWBResizeEvent re = {width, height};
                webView->onResize(re);
            }
            break;

            case IDCMP_IDCMPUPDATE:
            {
                struct TagItem *tags = (struct TagItem *)im->IAddress;
                if (IUtility->FindTagItem(LAYOUT_RequestRefresh, tags))
                    IIntuition->RefreshWindowFrame(s_owbwindow->window);
                else if (IUtility->FindTagItem(LAYOUT_RequestLayout, tags))
                    ILayout->RethinkLayout(s_owbwindow->gad_toolbar, s_owbwindow->window, NULL, FALSE);
                else if (IUtility->FindTagItem(LAYOUT_RelVerify, tags)) {
                    uint32 gid = IUtility->GetTagData(GA_ID, 0, tags);
                    WebFrame* frame = webView->mainFrame();

                    switch (gid) {
                    case 1: // back
                        webView->goBack();
                        s_owbwindow->expose = true;
                    break;

                    case 2: // forward
                        webView->goForward();
                        s_owbwindow->expose = true;
                    break;

                    case 3: // stop
                        webView->mainFrame()->impl()->loader()->stopAllLoaders();
                    break;

#if 0
                    case 4: // refresh
                    break;
#endif

                    case 5: // Home
                    {
                        webView->mainFrame()->loadURL(g_amiga_home);
                    }
                    break;

                    case 6: // URL
                    {
                        struct Gadget *gad = (struct Gadget *)IUtility->GetTagData(LAYOUT_RelAddress, 0, tags);
                        uint32 tab = IUtility->GetTagData(LAYOUT_TabVerify, FALSE, tags);
                        if (gad && !tab) {
                            STRPTR url = NULL;
                            IIntuition->GetAttr(STRINGA_TextVal, gad, (uint32 *)(void *)&url);
                            if (url && *url) {
                                char *colon = strchr(url, ':');
                                if (!colon || colon - url > 5) {
                                    TEXT http_url[2000 + strlen("http://")];
                                    snprintf(http_url, sizeof(http_url), "http://%s", url);
                                    webView->mainFrame()->loadURL(http_url);
                                } else
                                    webView->mainFrame()->loadURL(url);
                            }
                        }
                    }
                    break;

                    case 7: // Search
                    {
                        struct Gadget *gad = (struct Gadget *)IUtility->GetTagData(LAYOUT_RelAddress, 0, tags);
                        uint32 tab = IUtility->GetTagData(LAYOUT_TabVerify, FALSE, tags);
                        if (gad && !tab) {
                            STRPTR search = NULL;
                            IIntuition->GetAttr(STRINGA_TextVal, gad, (uint32 *)(void *)&search);
                            if (search && *search) {
                                TEXT url[600];
                                snprintf(url, sizeof(url), g_amiga_search, search);
                                webView->mainFrame()->loadURL(url);
                            }
                        }
                    }
                    break;

                    default:
                        fprintf(stderr, "GADGETUP, id = %lu\n", gid);
                    break;
                    }
                }
            }
            break;

            default:
                fprintf(stderr, "%s IDCMP class 0x%08lx not handled\n", __PRETTY_FUNCTION__, im->Class);
            break;
            }

            IExec->ReplyMsg(&im->ExecMessage);
        } else if (s_owbwindow->expose) {
            s_owbwindow->expose = false;
            SDL_ExposeEvent ev;
            webView->onExpose(ev);
        } else {
            WTF::dispatchFunctionsFromMainThread();
            WebCore::fireTimerIfNeeded();

            uint32 olddisabled = FALSE, newdisabled;
            IIntuition->GetAttr(GA_Disabled, s_owbwindow->gad_back, &olddisabled);
            newdisabled = webView->backForwardList()->backItem() ? FALSE : TRUE;
            if (olddisabled != newdisabled)
                IIntuition->RefreshSetGadgetAttrs(s_owbwindow->gad_back, s_owbwindow->window, NULL,
                                                  GA_Disabled, newdisabled,
                                                  TAG_DONE);

            IIntuition->GetAttr(GA_Disabled, s_owbwindow->gad_forward, &olddisabled);
            newdisabled = webView->backForwardList()->forwardItem() ? FALSE : TRUE;
            if (olddisabled != newdisabled)
                IIntuition->RefreshSetGadgetAttrs(s_owbwindow->gad_forward, s_owbwindow->window, NULL,
                                                  GA_Disabled, newdisabled,
                                                  TAG_DONE);

            IIntuition->GetAttr(GA_Disabled, s_owbwindow->gad_stop, &olddisabled);
            newdisabled = webView->isLoading() ? FALSE : TRUE;
            if (olddisabled != newdisabled)
                IIntuition->RefreshSetGadgetAttrs(s_owbwindow->gad_stop, s_owbwindow->window, NULL,
                                                  GA_Disabled, newdisabled,
                                                  TAG_DONE);

            usleep(10000);
        }
    }
}

int main (int argc, char* argv[])
{
    webView = WebView::createInstance();
    webView->parseConfigFile();
    IntRect rect = webView->frameRect();

     if (rect.isEmpty()) {
        s_owbwindow = createAmigaWindow(800, 600);
        IntRect clientRect(0, 0, 800, 600);
        webView->initWithFrame(clientRect,"", "");
    } else {
        s_owbwindow = createAmigaWindow(rect.width(), rect.height());
        webView->initWithFrame(rect, "", "");
    }

    webView->setViewWindow(s_owbwindow);

 
    char* uri = (char*) (argc > 1 ? argv[1] : "http://www.google.com/");
    webView->mainFrame()->loadURL(uri);

    /*if(webView->canZoomPageIn()) {
        printf("canZoomPageIn\n");
        webView->zoomPageIn();
    }*/
    /*if(webView->canMakeTextLarger()) {
        printf("canMakeTextLarger\n");
        webView->makeTextLarger();
    }*/

    waitEvent();

    closeAmigaWindow(s_owbwindow);

    delete webView;
    DS_INST_DUMP_CURRENT(IOcout);
    DS_CLEAN_DEEPSEE_FRAMEWORK();
    return 0;
}
