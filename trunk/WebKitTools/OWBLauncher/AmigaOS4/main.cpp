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

#include "config.h"
#include "BCGraphicsContext.h"
#include <Api/WebFrame.h>
#include <Api/WebView.h>
#include "CString.h"
#include "Page.h"
#include "Editor.h"
#include "FocusController.h"
#include "Frame.h"
#include "FrameView.h"
#include "cairo.h"
#include "cairo-amigaos.h"
#include "SelectionController.h"
#include "SharedTimer.h"
#include "Timer.h"
#include "MainThread.h"
#include <unistd.h>

#include "WebBackForwardList.h"
#include <FrameLoader.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/icon.h>
#include <proto/iffparse.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/wb.h>
#include <datatypes/textclass.h>
#include <intuition/gui.h>
#include <intuition/icclass.h>
#include <libraries/keymap.h>
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
#include <proto/fuelgauge.h>
#include <gadgets/fuelgauge.h>
#include <reaction/reaction_macros.h>
#include <proto/gadtools.h>
#undef String
#include <cstdio>

#ifndef ID_CSET
    #define ID_CSET MAKE_ID('C','S','E','T')
#endif

struct IFFCodeSet
{
    uint32 CodeSet;
    uint32 Reserved[7];
};

using namespace WebCore;
using std::max;

enum gadgetIDs
{
    GID_Back = 1,
    GID_Forward,
    GID_Stop,
    GID_Reload,
    GID_Home,
    GID_URL,
    GID_Search,
    GID_VBar,
    GID_HBar,
    GID_Iconify
};

enum menuIDs
{
    MID_NewWindow = 1,
    MID_Close,
    MID_Quit,
    MID_ZoomIn,
    MID_ZoomReset,
    MID_ZoomOut,
    MID_TextLarger,
    MID_TextReset,
    MID_TextSmaller,
    MID_Redraw,
    MID_Iconify,
    MID_Cut,
    MID_Copy,
    MID_Paste,
    MID_SelectAll,
    MID_CopyHTML,
    MID_CopyLatin1
};

AmigaConfig amigaConfig;
MsgPort *sharedPort, *appIconPort;
BalWidget *firstWindow;
DiskObject *icon;
static bool getInnerSize = false;
Task *mainTask;
uint32 sharedTimerSignal;
static int32 sharedTimerSignalBit;

static NewMenu Menus[] =
{
    Title((STRPTR)"Project"),
        Item((STRPTR)"New window",   (STRPTR)"N", MID_NewWindow),
        Item((STRPTR)"Close window", (STRPTR)"K", MID_Close),
        ItemBar,
        Item((STRPTR)"Iconify",      (STRPTR)"I", MID_Iconify),
        ItemBar,
        Item((STRPTR)"Quit",         (STRPTR)"Q", MID_Quit),
    Title((STRPTR)"Edit"),
        Item((STRPTR)"Cut",          (STRPTR)"X", MID_Cut),
        Item((STRPTR)"Copy",         (STRPTR)"C", MID_Copy),
//        Item((STRPTR)"Copy as HTML",        NULL, MID_CopyHTML),
        Item((STRPTR)"Copy as ISO-8859-1",  NULL, MID_CopyLatin1),
        Item((STRPTR)"Paste",        (STRPTR)"V", MID_Paste),
        Item((STRPTR)"Select all",   (STRPTR)"A", MID_SelectAll),
    Title((STRPTR)"View"),
        Item((STRPTR)"Zoom in",      (STRPTR)"+", MID_ZoomIn),
        Item((STRPTR)"Zoom reset",   (STRPTR)"0", MID_ZoomReset),
        Item((STRPTR)"Zoom out",     (STRPTR)"-", MID_ZoomOut),
        ItemBar,
        Item((STRPTR)"Text larger",  (STRPTR)"1", MID_TextLarger),
        Item((STRPTR)"Text reset",   (STRPTR)"2", MID_TextReset),
        Item((STRPTR)"Text smaller", (STRPTR)"3", MID_TextSmaller),
        ItemBar,
        Item((STRPTR)"Redraw",       (STRPTR)"R", MID_Redraw),
    EndMenu
};

#define FULLMENUNUM_ZoomIn      FULLMENUNUM(2, 0, 0)
#define FULLMENUNUM_ZoomReset   FULLMENUNUM(2, 1, 0)
#define FULLMENUNUM_ZoomOut     FULLMENUNUM(2, 2, 0)
#define FULLMENUNUM_TextLarger  FULLMENUNUM(2, 4, 0)
#define FULLMENUNUM_TextReset   FULLMENUNUM(2, 5, 0)
#define FULLMENUNUM_TextSmaller FULLMENUNUM(2, 6, 0)



static void initAmigaConfig(void)
{
    static bool isInitialized = false;
    if (!isInitialized) {
        amigaConfig.homeURL = "http://amigans.net/";
        amigaConfig.searchURL = "http://google.com/search?q=%s";
        amigaConfig.width = 800;
        amigaConfig.height = 600;
        amigaConfig.minFontSize = 9;
        amigaConfig.fontXDPI = 80;
        amigaConfig.fontYDPI = 72;
        amigaConfig.unicodeFontName = "Bitstream Cyberbit";

        isInitialized = true;

        static TEXT progname[512];
        bool gotprogname = false;

        if (IDOS->Cli()) {
            TEXT temp[8192];

            if (IDOS->GetProgramName(temp, sizeof(temp))) {
                strcpy(progname, "PROGDIR:");
                strlcat(progname, IDOS->FilePart(temp), sizeof(progname));
                gotprogname = true;
            }
        } else
        {
            strlcpy(progname, IExec->FindTask(NULL)->tc_Node.ln_Name, sizeof(progname));
            gotprogname = true;
        }

        icon = gotprogname ? IIcon->GetDiskObject(progname) : NULL;
        if (icon) {
            STRPTR found;

            found = IIcon->FindToolType(icon->do_ToolTypes, "HOME");
            if (found)
                amigaConfig.homeURL = strdup(found);

            found = IIcon->FindToolType(icon->do_ToolTypes, "SEARCH");
            if (found)
                amigaConfig.searchURL = strdup(found);

            found = IIcon->FindToolType(icon->do_ToolTypes, "WIDTH");
            if (found)
                amigaConfig.width = atoi(found);

            found = IIcon->FindToolType(icon->do_ToolTypes, "HEIGHT");
            if (found)
                amigaConfig.height = atoi(found);

            found = IIcon->FindToolType(icon->do_ToolTypes, "MINFONTSIZE");
            if (found)
                amigaConfig.minFontSize = atoi(found);

            found = IIcon->FindToolType(icon->do_ToolTypes, "FONTXDPI");
            if (found)
                amigaConfig.fontXDPI = atoi(found);

            found = IIcon->FindToolType(icon->do_ToolTypes, "FONTYDPI");
            if (found)
                amigaConfig.fontYDPI = atoi(found);

            found = IIcon->FindToolType(icon->do_ToolTypes, "UNICODEFONT");
            if (found)
                amigaConfig.unicodeFontName = strdup(found);

            found = IIcon->FindToolType(icon->do_ToolTypes, "WINDOWLEFT");
            if (found)
                amigaConfig.windowleft = atoi(found);

            found = IIcon->FindToolType(icon->do_ToolTypes, "WINDOWTOP");
            if (found) {
                amigaConfig.windowtop = atoi(found);
                if (0 == amigaConfig.windowtop)
                    amigaConfig.windowtop = -1;
            }

            found = IIcon->FindToolType(icon->do_ToolTypes, "WINDOWWIDTH");
            if (found)
                amigaConfig.windowwidth = atoi(found);

            found = IIcon->FindToolType(icon->do_ToolTypes, "WINDOWHEIGHT");
            if (found)
                amigaConfig.windowheight = atoi(found);


            amigaConfig.height      = max(amigaConfig.height, 400);
            amigaConfig.width       = max(amigaConfig.width, 640);
            amigaConfig.minFontSize = max(amigaConfig.minFontSize, 5);
            amigaConfig.fontXDPI    = max(amigaConfig.fontXDPI,10);
            amigaConfig.fontYDPI    = max(amigaConfig.fontYDPI,10);

            icon->do_Type = 0;

            if (!amigaConfig.windowwidth || !amigaConfig.windowheight)
                getInnerSize = true;
        }
    }
}

static bool initTimer(void)
{
    mainTask = IExec->FindTask(NULL);
    sharedTimerSignalBit = IExec->AllocSignal(-1);
    if (-1 == sharedTimerSignalBit)
        return false;

    sharedTimerSignal = 1 << sharedTimerSignalBit;

    return true;
}

static void cleanupTimer(void)
{
    IExec->FreeSignal(sharedTimerSignalBit);
}

static void closeAmigaWindowWithoutSurface(BalWidget *owbwindow)
{
    if (owbwindow->window->MenuStrip) {
        Menu *menustrip = owbwindow->window->MenuStrip;
        IIntuition->ClearMenuStrip(owbwindow->window);
        IGadTools->FreeMenus(menustrip);
    }
    IIntuition->CloseWindow(owbwindow->window);
    owbwindow->window = 0;
    IIntuition->DisposeObject(owbwindow->gad_vbar);
    owbwindow->gad_vbar = 0;
    IIntuition->DisposeObject(owbwindow->gad_hbar);
    owbwindow->gad_hbar = 0;
    IIntuition->DisposeObject(owbwindow->gad_toolbar);
    owbwindow->gad_toolbar = 0;
    IIntuition->DisposeObject(owbwindow->img_iconify);
    owbwindow->img_iconify = 0;
    IIntuition->DisposeObject(owbwindow->gad_iconify);
    owbwindow->gad_iconify = 0;
    if (owbwindow->backfill_hook) {
        IIntuition->ReleaseIBackFill(owbwindow->backfill_hook);
        owbwindow->backfill_hook = 0;
    }
}

void closeAmigaWindow(BalWidget *owbwindow)
{
    if (!owbwindow)
        return;

    if (owbwindow->window)
        closeAmigaWindowWithoutSurface(owbwindow);

    if (owbwindow->appicon)
        IWorkbench->RemoveAppIcon(owbwindow->appicon);

    if (owbwindow->cr)
        cairo_destroy(owbwindow->cr);

    if (owbwindow->surface)
        cairo_surface_destroy(owbwindow->surface);

    if (firstWindow == owbwindow)
        firstWindow = owbwindow->next;
    else {
        BalWidget *window = firstWindow;
        while (window && window->next != owbwindow)
            window = window->next;
        if (window)
            window->next = owbwindow->next;
    }

    delete owbwindow->webView;
    free(owbwindow);
}

static bool openAmigaWindow(BalWidget *owbwindow)
{
    int w = amigaConfig.width;
    int h = amigaConfig.height;
    int windowX = amigaConfig.windowleft;
    int windowY = amigaConfig.windowtop;
    int windowW = amigaConfig.windowwidth;
    int windowH = amigaConfig.windowheight;

    owbwindow->window = IIntuition->OpenWindowTags(NULL,
                                                   WA_Left, windowX,
                                                   windowY ? WA_Top : TAG_IGNORE, windowY,
                                                   windowW ? WA_Width : WA_InnerWidth , windowW ?: w,
                                                   windowH ? WA_Height : WA_InnerHeight, windowH ?: h,
                                                   WA_MinWidth, 640,
                                                   WA_MinHeight, 480,
                                                   WA_MaxWidth, ~0UL,
                                                   WA_MaxHeight, ~0UL,
                                                   WA_SmartRefresh, TRUE,
                                                   WA_NoCareRefresh, TRUE,
                                                   WA_Title, "Origyn Web Browser",
                                                   WA_ScreenTitle, "Origyn Web Browser",
                                                   WA_ReportMouse, TRUE,
                                                   WA_SizeGadget, TRUE,
                                                   WA_SizeBRight, TRUE,
                                                   WA_SizeBBottom, TRUE,
                                                   WA_DragBar, TRUE,
                                                   WA_DepthGadget, TRUE,
                                                   WA_CloseGadget, TRUE,
                                                   WA_Hidden, TRUE,
                                                   WA_PubScreenName, "OrigynWebBrowser",
                                                   WA_PubScreenFallBack, TRUE,
                                                   WA_UserPort, sharedPort,
                                                   WA_NewLookMenus, TRUE,
                                                   WA_IDCMP, IDCMP_MOUSEBUTTONS
                                                           | IDCMP_MOUSEMOVE
                                                           | IDCMP_CLOSEWINDOW
                                                           | IDCMP_RAWKEY
                                                           | IDCMP_VANILLAKEY
                                                           | IDCMP_EXTENDEDMOUSE
                                                           | IDCMP_IDCMPUPDATE
                                                           | IDCMP_NEWSIZE
                                                           | IDCMP_MENUPICK
                                                           | IDCMP_GADGETUP,
                                                   TAG_DONE);
    if (!owbwindow->window)
        return false;

    owbwindow->window->UserData = (BYTE *)owbwindow;

    APTR old_proc_window = IDOS->SetProcWindow((APTR)-1);
    DrawInfo *drinfo = IIntuition->GetScreenDrawInfo(owbwindow->window->WScreen);
    owbwindow->backfill_hook = IIntuition->ObtainIBackFill(drinfo, BT_WINDOWBG, IDS_NORMAL, 0);
    APTR images[6];
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
    images[5] = BitMapObject,
                    BITMAP_Screen, owbwindow->window->WScreen,
                    BITMAP_Masking, TRUE,
                    BITMAP_Transparent, TRUE,
                    BITMAP_SourceFile, "TBImages:reload",
                    BITMAP_SelectSourceFile, "TBImages:reload_s",
                    BITMAP_DisabledSourceFile, "TBImages:reload_g",
                BitMapEnd;

    owbwindow->gad_toolbar = (Gadget *)VLayoutObject,
                                           GA_Left, owbwindow->window->BorderLeft,
                                           GA_Top, owbwindow->window->BorderTop,
                                           GA_RelWidth, -(owbwindow->window->BorderLeft + owbwindow->window->BorderRight),
                                           GA_NoFilterWheel, TRUE,
                                           ICA_TARGET, ICTARGET_IDCMP,
                                           LAYOUT_LayoutBackFill, owbwindow->backfill_hook,
                                           LAYOUT_DeferLayout, TRUE,
                                           LAYOUT_AddChild, HLayoutObject,
                                               LAYOUT_AddChild, owbwindow->gad_back = (Gadget *)ButtonObject,
                                                   GA_ID, GID_Back, 
                                                   GA_RelVerify, TRUE,
                                                   images[0] ? BUTTON_RenderImage : TAG_IGNORE, images[0],
                                                   images[0] ? TAG_IGNORE : BUTTON_AutoButton, BAG_LFARROW,
                                                   GA_Disabled, TRUE,
                                               ButtonEnd,
                                               CHILD_WeightedWidth, 0,
                                               LAYOUT_AddChild, owbwindow->gad_forward = (Gadget *)ButtonObject,
                                                   GA_ID, GID_Forward, 
                                                   GA_RelVerify, TRUE,
                                                   images[1] ? BUTTON_RenderImage : TAG_IGNORE, images[1],
                                                   images[1] ? TAG_IGNORE : BUTTON_AutoButton, BAG_RTARROW,
                                                   GA_Disabled, TRUE,
                                               ButtonEnd,
                                               CHILD_WeightedWidth, 0,
                                               LAYOUT_AddChild, owbwindow->gad_stop = (Gadget *)ButtonObject,
                                                   GA_ID, GID_Stop, 
                                                   GA_RelVerify, TRUE,
                                                   images[2] ? BUTTON_RenderImage : GA_Text, images[2] ?: "Stop",
                                                   GA_TabCycle, NULL == images[2],
                                                   GA_Disabled, TRUE,
                                               ButtonEnd,
                                               CHILD_WeightedWidth, 0,
                                               LAYOUT_AddChild, ButtonObject,
                                                   GA_ID, GID_Reload, 
                                                   GA_RelVerify, TRUE,
                                                   images[5] ? BUTTON_RenderImage : GA_Text, images[5] ?: "Reload",
                                                   GA_TabCycle, NULL == images[5],
                                               ButtonEnd,
                                               CHILD_WeightedWidth, 0,
                                               LAYOUT_AddChild, ButtonObject,
                                                   GA_ID, GID_Home, 
                                                   GA_RelVerify, TRUE,
                                                   images[4] ? BUTTON_RenderImage : GA_Text, images[4] ?: "Home",
                                                   GA_TabCycle, NULL == images[4],
                                               ButtonEnd,
                                               CHILD_WeightedWidth, 0,
                                               LAYOUT_AddChild, owbwindow->gad_url = (Gadget *)StringObject,
                                                   STRINGA_TextVal, owbwindow->url,
                                                   STRINGA_MaxChars, 2000,
                                                   GA_ID, GID_URL,
                                                   GA_RelVerify, TRUE,
                                                   GA_TabCycle, TRUE,
                                               StringEnd,
                                               CHILD_Label, LabelObject,
                                                   LABEL_DrawInfo, drinfo,
                                               LabelEnd,
                                               LAYOUT_AddChild, owbwindow->gad_search = (Gadget *)StringObject,
                                                   STRINGA_TextVal, owbwindow->search,
                                                   STRINGA_MaxChars, 500,
                                                   GA_ID, GID_Search,
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

                                           LAYOUT_AddChild, owbwindow->gad_fuelgauge = (Gadget *)FuelGaugeObject,
                                               FUELGAUGE_Min, 0,
                                               FUELGAUGE_Max, 1000,
                                               FUELGAUGE_Level, 0,
                                               FUELGAUGE_Ticks, 0,
                                               FUELGAUGE_Percent, FALSE,
                                           FuelGaugeEnd,
                                           CHILD_WeightedHeight, 0,

                                           LAYOUT_AddImage, BevelObject,
                                               BEVEL_Style, BVS_SBAR_VERT,
                                           BevelEnd,
                                           CHILD_WeightedHeight, 0,
                                       LayoutEnd;

    owbwindow->gad_vbar = (Gadget *)IIntuition->NewObject(NULL, "scrollergclass",
                                                          GA_ID, GID_VBar,
                                                          GA_RightBorder, TRUE,
                                                          GA_RelVerify, TRUE,
                                                          GA_Immediate, TRUE,
                                                          GA_FollowMouse, TRUE,
                                                          GA_DrawInfo, drinfo,
                                                          PGA_ArrowDelta, 20,
                                                          ICA_TARGET, ICTARGET_IDCMP,
                                                          TAG_END);

    owbwindow->gad_hbar = (Gadget *)IIntuition->NewObject(NULL, "scrollergclass",
                                                          GA_ID, GID_HBar,
                                                          GA_BottomBorder, TRUE,
                                                          GA_RelVerify, TRUE,
                                                          GA_Immediate, TRUE,
                                                          GA_FollowMouse, TRUE,
                                                          GA_DrawInfo, drinfo,
                                                          PGA_Freedom, FREEHORIZ,
                                                          PGA_ArrowDelta, 20,
                                                          ICA_TARGET, ICTARGET_IDCMP,
                                                          TAG_END);

    if (owbwindow->gad_toolbar && icon) {
        uint32 img_size = (owbwindow->window->WScreen->Flags & SCREENHIRES) ? SYSISIZE_MEDRES : SYSISIZE_LOWRES;
        uint32 img_height = owbwindow->window->WScreen->Font->ta_YSize + owbwindow->window->WScreen->WBorTop + 1;

        owbwindow->img_iconify = IIntuition->NewObject(NULL, "sysiclass",
                                                       SYSIA_Which, ICONIFYIMAGE,
                                                       SYSIA_Size, img_size,
                                                       SYSIA_DrawInfo, drinfo,
                                                       IA_Height, img_height,
                                                       TAG_DONE);
        if (owbwindow->img_iconify)
            owbwindow->gad_iconify = (Gadget *)IIntuition->NewObject(NULL, "buttongclass",
                                                                     GA_Image, owbwindow->img_iconify,
                                                                     GA_ID, GID_Iconify,
                                                                     GA_RelVerify, TRUE,
                                                                     GA_Titlebar, TRUE,
                                                                     GA_RelRight, 0,
                                                                     TAG_DONE);
    }

    IIntuition->FreeScreenDrawInfo(owbwindow->window->WScreen, drinfo);
    IDOS->SetProcWindow(old_proc_window);

    if (!owbwindow->gad_toolbar) {
        IIntuition->DisposeObject(images[0]);
        IIntuition->DisposeObject(images[1]);
        IIntuition->DisposeObject(images[2]);
        IIntuition->DisposeObject(images[3]);
        IIntuition->DisposeObject(images[4]);
        IIntuition->DisposeObject(images[5]);
        IIntuition->DisposeObject(owbwindow->gad_vbar);
        IIntuition->DisposeObject(owbwindow->gad_hbar);
        IIntuition->ReleaseIBackFill(owbwindow->backfill_hook);
        IIntuition->CloseWindow(owbwindow->window);
        return false;
    }

    LayoutLimits layoutlimits;
    ILayout->LayoutLimits(owbwindow->gad_toolbar, &layoutlimits, NULL, owbwindow->window->WScreen);
    owbwindow->offsetx = owbwindow->window->BorderLeft;
    owbwindow->offsety = owbwindow->window->BorderTop + layoutlimits.MinHeight;

    IIntuition->AddGadget(owbwindow->window, owbwindow->gad_toolbar, (uint16)~0);
    if (owbwindow->gad_vbar)
        IIntuition->AddGadget(owbwindow->window, owbwindow->gad_vbar, (uint16)~0);
    if (owbwindow->gad_hbar)
        IIntuition->AddGadget(owbwindow->window, owbwindow->gad_hbar, (uint16)~0);
    if (owbwindow->gad_iconify)
        IIntuition->AddGadget(owbwindow->window, owbwindow->gad_iconify, (uint16)~0);
    IIntuition->RefreshGList(owbwindow->gad_toolbar, owbwindow->window, NULL, -1);

    IIntuition->SetWindowAttrs(owbwindow->window, 
                               windowH ? TAG_IGNORE : WA_InnerHeight, h + layoutlimits.MinHeight,
                               WA_Hidden, FALSE,
                               WA_Activate, TRUE,
                               TAG_DONE);

    APTR vi = IGadTools->GetVisualInfoA(owbwindow->window->WScreen, NULL);
    if (vi) {
        Menu *menustrip = IGadTools->CreateMenusA(Menus, NULL);
        if (menustrip) {
            if (!IGadTools->LayoutMenus(menustrip, vi,
                                        GTMN_NewLookMenus, TRUE,
                                        TAG_DONE)) {
                IGadTools->FreeMenus(menustrip);
                menustrip = NULL;
            }
        }

        if (menustrip)
             IIntuition->SetMenuStrip(owbwindow->window, menustrip);

        IGadTools->FreeVisualInfo(vi);
    }

    if (windowW || windowH) {
        IIntuition->GetWindowAttrs(owbwindow->window,
                                   WA_InnerWidth,  &amigaConfig.width,
                                   WA_InnerHeight, &amigaConfig.height,
                                   TAG_DONE);
        amigaConfig.height -= owbwindow->offsety - owbwindow->window->BorderTop;
        amigaConfig.windowwidth = amigaConfig.windowheight = 0;
    }

    return true;
}

BalWidget *createAmigaWindow(WebView *webView)
{
    if (!webView)
        return 0;

    BalWidget *owbwindow = (BalWidget *)calloc(sizeof (BalWidget), 1);
    if(!owbwindow)
        return 0;

    if (!openAmigaWindow(owbwindow)) {
        free(owbwindow);
        return 0;
    }

    owbwindow->webView = webView;

    if (!firstWindow)
        firstWindow = owbwindow;
    else {
        BalWidget *window = firstWindow;
        while (window->next)
            window = window->next;
        window->next = owbwindow;
    }

    owbwindow->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, amigaConfig.width, amigaConfig.height);
    if (!owbwindow->surface) {
        closeAmigaWindow(owbwindow);
        return 0;
    }

    owbwindow->cr = cairo_create(owbwindow->surface);
    if (!owbwindow->cr) {
        closeAmigaWindow(owbwindow);
        return 0;
    }

    return owbwindow;
}

static void iconifyWindow(BalWidget *owbwindow)
{
    if (!owbwindow->appicon) {
        STRPTR text;
        IIntuition->GetAttr(STRINGA_TextVal, owbwindow->gad_url, (uint32 *)(void *)&text);
        strlcpy(owbwindow->url, text, sizeof(owbwindow->url));
        IIntuition->GetAttr(STRINGA_TextVal, owbwindow->gad_search, (uint32 *)(void *)&text);
        strlcpy(owbwindow->search, text, sizeof(owbwindow->search));

        owbwindow->appicon = IWorkbench->AddAppIcon(0, (uint32)owbwindow,
                                                    "OWB", appIconPort,
                                                    0, icon, TAG_DONE);
        if (owbwindow->appicon)
            closeAmigaWindowWithoutSurface(owbwindow);
    }
}

static void copyToClipboard(WebView* webView, bool html, bool latin1)
{
    Frame* frame = core(webView)->focusController()->focusedOrMainFrame();
    if (!frame)
        return;

    IFFHandle *ih;

    String selectedText;
    if (html) {
       SelectionController* controller = frame->selection();
       if (controller)
           selectedText = controller->toRange()->toHTML();
    }
    else
       selectedText = frame->selectedText();

    CString text = latin1 ? selectedText.latin1() : selectedText.utf8();
    const char *data = text.data();
    size_t len = text.length();

    bool copied = false;

    if (data && len && (ih = IIFFParse->AllocIFF())) {
        if (ClipboardHandle *ch = IIFFParse->OpenClipboard(PRIMARY_CLIP)) {
            ih->iff_Stream = (uint32)ch;
            IIFFParse->InitIFFasClip(ih);

            if (0 == IIFFParse->OpenIFF(ih, IFFF_WRITE)) {
                if (0 == IIFFParse->PushChunk(ih, ID_FTXT, ID_FORM, IFFSIZE_UNKNOWN)) {
                    const IFFCodeSet cs = { latin1 ? 4 : 106, { 0, 0, 0, 0, 0, 0, 0 } };

                    if (0 == IIFFParse->PushChunk(ih, 0, ID_CSET, sizeof(cs))
                     && IIFFParse->WriteChunkBytes(ih, &cs, sizeof(cs)) > 0
                     && 0 == IIFFParse->PopChunk(ih)
                     && 0 == IIFFParse->PushChunk(ih, 0, ID_CHRS, IFFSIZE_UNKNOWN)) {
                        if (IIFFParse->WriteChunkBytes(ih, data, len) > 0)
                            copied = true;
                        IIFFParse->PopChunk(ih);
                    }
                    IIFFParse->PopChunk(ih);
                }
                IIFFParse->CloseIFF(ih);
            }
            IIFFParse->CloseClipboard(ch);
        }
        IIFFParse->FreeIFF(ih);
    }

    if (copied)
        frame->selection()->setSelection(Selection(frame->selection()->end(), SEL_DEFAULT_AFFINITY), false, false, true);
    else
        IIntuition->DisplayBeep(NULL);
}

static void waitEvent(void)
{
    uint32 signalSet = (1L << sharedPort->mp_SigBit) | sharedTimerSignal | (1L << appIconPort->mp_SigBit);

    while (firstWindow) {
        IntuiMessage *im;

        uint32 signals = IExec->Wait(signalSet);

        while ((im = (IntuiMessage *)IExec->GetMsg(sharedPort))) {
            BalWidget *owbwindow = (BalWidget *)im->IDCMPWindow->UserData;
            WebView *webView = 0;

            if (owbwindow)
                webView = owbwindow->webView;

            if (webView)
                switch (im->Class) {
                case IDCMP_CLOSEWINDOW:
                    IExec->ReplyMsg(&im->ExecMessage);
                    closeAmigaWindow(owbwindow);
                    continue;
                break;

                case IDCMP_MOUSEBUTTONS:
                    switch (im->Code & ~IECODE_UP_PREFIX) {
                    case IECODE_LBUTTON:
                    case IECODE_RBUTTON:
                    case IECODE_MBUTTON:
                    {
                        IntuiMessage im2 = *im;
                        im2.MouseX -= owbwindow->offsetx;
                        im2.MouseY -= owbwindow->offsety;
                        if (im->Code & IECODE_UP_PREFIX)
                            webView->onMouseButtonUp(im2);
                        else
                            webView->onMouseButtonDown(im2);
                    }
                    break;

                    case IECODE_4TH_BUTTON:
                        webView->goBack();
                        owbwindow->expose = true;
                    break;

                    case IECODE_5TH_BUTTON:
                        webView->goForward();
                        owbwindow->expose = true;
                    break;
                    }
                break;

                case IDCMP_MOUSEMOVE:
                {
                    IntuiMessage im2 = *im;
                    im2.MouseX -= owbwindow->offsetx;
                    im2.MouseY -= owbwindow->offsety;
                    webView->onMouseMotion(im2);
                }
                break;

                case IDCMP_EXTENDEDMOUSE:
                {
                    IntuiMessage im2 = *im;
                    im2.MouseX -= owbwindow->offsetx;
                    im2.MouseY -= owbwindow->offsety;
                    webView->onScroll(im2);
                    im2.Class = IDCMP_MOUSEMOVE;
                    im2.Code = 0;
                    im2.Qualifier = 0;
                    webView->onMouseMotion(im2);
                }
                break;

                case IDCMP_RAWKEY:
                {
                    IntuiMessage im2 = *im;
                    im2.MouseX -= owbwindow->offsetx;
                    im2.MouseY -= owbwindow->offsety;
                    if (im->Code & IECODE_UP_PREFIX)
                        webView->onKeyUp(im2);
                    else
                        webView->onKeyDown(im2);
                }
                break;

                case IDCMP_VANILLAKEY:
                {
                    IntuiMessage im2 = *im;
                    im2.MouseX -= owbwindow->offsetx;
                    im2.MouseY -= owbwindow->offsety;
                    webView->onKeyDown(im2);
                }
                break;

                case IDCMP_NEWSIZE:
                {
                    int32 width = 800, height = 600;
                    IIntuition->GetWindowAttrs(owbwindow->window,
                                               WA_InnerWidth,  &width,
                                               WA_InnerHeight, &height,
                                               TAG_DONE);
                    height -= owbwindow->offsety - owbwindow->window->BorderTop;

                    if (getInnerSize) {
                        amigaConfig.width = width;
                        amigaConfig.height = height;
                        getInnerSize = false;
                    }

                    Frame* frame = core(webView->mainFrame());
                    if (!frame || frame->view()->visibleWidth() != width || frame->view()->visibleHeight() != height) {
                        cairo_surface_t *newsurface;
                        cairo_t *newcr = 0;

                        newsurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
                        if (newsurface) {
                            newcr = cairo_create(newsurface);
                            if (!newcr)
                                cairo_surface_destroy(newsurface);
                        }

                        if (newcr) {
                            cairo_destroy(owbwindow->cr);
                            cairo_surface_destroy(owbwindow->surface);

                            owbwindow->cr = newcr;
                            owbwindow->surface = newsurface;

                            AmigaOWBResizeEvent re = {width, height};
                            webView->onResize(re);
                        }
                    }

                    if (frame && frame->view() && frame->view()->visibleWidth() == width && frame->view()->visibleHeight() == height) {
                        webView->addToDirtyRegion(IntRect(0, 0, width, height));
                        owbwindow->expose = true;
                    }
                }
                break;

                case IDCMP_IDCMPUPDATE:
                {
                    TagItem *tags = (TagItem *)im->IAddress;
                    if (IUtility->FindTagItem(LAYOUT_RequestRefresh, tags))
                        IIntuition->RefreshWindowFrame(owbwindow->window);
                    else if (IUtility->FindTagItem(LAYOUT_RequestLayout, tags))
                        ILayout->RethinkLayout(owbwindow->gad_toolbar, owbwindow->window, NULL, FALSE);
                    else if (IUtility->FindTagItem(LAYOUT_RelVerify, tags)) {
                        gadgetIDs gid = (gadgetIDs)IUtility->GetTagData(GA_ID, 0, tags);

                        switch (gid) {
                        case GID_Back:
                            webView->goBack();
                            owbwindow->expose = true;
                        break;

                        case GID_Forward:
                            webView->goForward();
                            owbwindow->expose = true;
                        break;

                        case GID_Stop:
                            webView->mainFrame()->impl()->loader()->stopAllLoaders();
                        break;

                        case GID_Reload:
                            webView->mainFrame()->reload();
                        break;

                        case GID_Home:
                            webView->mainFrame()->loadURL(amigaConfig.homeURL);
                        break;

                        case GID_URL:
                        {
                            Gadget *gad = (Gadget *)IUtility->GetTagData(LAYOUT_RelAddress, 0, tags);
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

                        case GID_Search:
                        {
                            Gadget *gad = (Gadget *)IUtility->GetTagData(LAYOUT_RelAddress, 0, tags);
                            uint32 tab = IUtility->GetTagData(LAYOUT_TabVerify, FALSE, tags);
                            if (gad && !tab) {
                                STRPTR search = NULL;
                                IIntuition->GetAttr(STRINGA_TextVal, gad, (uint32 *)(void *)&search);
                                if (search && *search) {
                                    TEXT url[600];
                                    snprintf(url, sizeof(url), amigaConfig.searchURL, search);
                                    webView->mainFrame()->loadURL(url);
                                }
                            }
                        }
                        break;

                        default:
                            fprintf(stderr, "GADGETUP, id = %d\n", gid);
                        break;
                        }
                    }
                    else {
                        uint32 gid = IUtility->GetTagData(GA_ID, 0, tags);
                        if (GID_VBar == gid) {
                            if (owbwindow->gad_vbar) {
                                int top = 0;
                                IIntuition->GetAttr(PGA_Top, owbwindow->gad_vbar, (uint32 *)(void *)&top);
                                IntPoint diff(0, top - webView->scrollOffset().y());
                                if (diff.y())
                                    webView->scrollBy(diff);
                            }
                        }
                        else if (GID_HBar == gid) {
                            if (owbwindow->gad_hbar) {
                                int left = 0;
                                IIntuition->GetAttr(PGA_Top, owbwindow->gad_hbar, (uint32 *)(void *)&left);
                                IntPoint diff(left - webView->scrollOffset().x(), 0);
                                if (diff.x())
                                    webView->scrollBy(diff);
                            }
                        }
                        else
                           fprintf(stderr, "gid = %lu, code 0x%04x, qual 0x%04x not handed\n", gid, im->Code, im->Qualifier);
                    }
                }
                break;

                case IDCMP_MENUPICK:
                {
                    MenuItem *item = IIntuition->ItemAddress(owbwindow->window->MenuStrip, im->Code);
                    if (item) {
                        menuIDs id = (menuIDs)(uint32)GTMENUITEM_USERDATA(item);

                        switch (id) {
                        case MID_NewWindow:
                        {
                            WebView *newWebView = WebView::createInstance();
                            BalWidget *newowbwindow = createAmigaWindow(newWebView);

                            if (newowbwindow) {
                                IntRect clientRect(0, 0, amigaConfig.width, amigaConfig.height);
                                newWebView->initWithFrame(clientRect, "", "");

                                newWebView->setViewWindow(newowbwindow);

                                newWebView->mainFrame()->loadURL("about:blank");
                            }
                            else
                                delete newWebView;
                        }
                        break;

                        case MID_Close:
                            IExec->ReplyMsg(&im->ExecMessage);
                            closeAmigaWindow(owbwindow);
                            continue;
                        break;

                        case MID_Quit:
                            IExec->ReplyMsg(&im->ExecMessage);
                            while (firstWindow)
                                closeAmigaWindow(firstWindow);
                            return;
                        break;

                        case MID_ZoomIn:
                            if (webView->canZoomPageIn())
                                webView->zoomPageIn();
                        break;

                        case MID_ZoomReset:
                            if (webView->canResetPageZoom())
                                webView->resetPageZoom();
                        break;

                        case MID_ZoomOut:
                            if (webView->canZoomPageOut())
                                webView->zoomPageOut();
                        break;

                        case MID_TextLarger:
                            if (webView->canMakeTextLarger())
                                webView->makeTextLarger();
                        break;

                        case MID_TextReset:
                            if (webView->canMakeTextStandardSize())
                                webView->makeTextStandardSize();
                        break;

                        case MID_TextSmaller:
                            if (webView->canMakeTextSmaller())
                                webView->makeTextSmaller();
                        break;

                        case MID_Redraw:
                        {
                            Frame* frame = core(webView->mainFrame());
                            if (frame->view()) {
                                webView->addToDirtyRegion(IntRect(0, 0, frame->view()->visibleWidth(), frame->view()->visibleHeight()));
                                owbwindow->expose = true;
                            }
                        }
                        break;

                        case MID_Iconify:
                            IExec->ReplyMsg(&im->ExecMessage);
                            iconifyWindow(owbwindow);
                            continue;
                        break;

                        case MID_Cut:
                            webView->cut();
                        break;

                        case MID_Copy:
                        case MID_CopyHTML:
                        case MID_CopyLatin1:
                            copyToClipboard(webView, MID_CopyHTML == id, MID_CopyLatin1 == id);
                        break;

                        case MID_Paste:
                            webView->paste();
                        break;

                        case MID_SelectAll:
                            webView->executeCoreCommandByName("SelectAll", "");
                        break;
                        }
                    }
                }
                break;

                case IDCMP_GADGETUP:
                    if (GID_Iconify == ((Gadget *)im->IAddress)->GadgetID) {
                        IExec->ReplyMsg(&im->ExecMessage);
                        iconifyWindow(owbwindow);
                        continue;
                    }
                break;

                default:
                    fprintf(stderr, "%s IDCMP class 0x%08lx not handled\n", __PRETTY_FUNCTION__, im->Class);
                break;
                }

            IExec->ReplyMsg(&im->ExecMessage);
        }

        if (signals & sharedTimerSignal) {
            WTF::dispatchFunctionsFromMainThread();
            WebCore::fireTimerIfNeeded();
        }

        BalWidget *owbwindow = firstWindow;
        while (owbwindow) {
            WebView *webView = owbwindow->webView;

            if (owbwindow->expose) {
                owbwindow->expose = false;
                BalEventExpose ev = 0;
                webView->onExpose(ev);
                IExec->Signal(mainTask, 1L << sharedPort->mp_SigBit);
            }
            else if (owbwindow->window) {
                uint32 olddisabled = FALSE, newdisabled;
                IIntuition->GetAttr(GA_Disabled, owbwindow->gad_back, &olddisabled);
                olddisabled = !!olddisabled;
                newdisabled = webView->backForwardList()->backItem() ? FALSE : TRUE;
                if (olddisabled != newdisabled)
                    IIntuition->RefreshSetGadgetAttrs(owbwindow->gad_back, owbwindow->window, NULL,
                                                      GA_Disabled, newdisabled,
                                                      TAG_DONE);

                IIntuition->GetAttr(GA_Disabled, owbwindow->gad_forward, &olddisabled);
                olddisabled = !!olddisabled;
                newdisabled = webView->backForwardList()->forwardItem() ? FALSE : TRUE;
                if (olddisabled != newdisabled)
                    IIntuition->RefreshSetGadgetAttrs(owbwindow->gad_forward, owbwindow->window, NULL,
                                                      GA_Disabled, newdisabled,
                                                      TAG_DONE);

                IIntuition->GetAttr(GA_Disabled, owbwindow->gad_stop, &olddisabled);
                olddisabled = !!olddisabled;
                newdisabled = webView->isLoading() ? FALSE : TRUE;
                if (olddisabled != newdisabled)
                    IIntuition->RefreshSetGadgetAttrs(owbwindow->gad_stop, owbwindow->window, NULL,
                                                      GA_Disabled, newdisabled,
                                                      TAG_DONE);

                
                if (owbwindow->gad_fuelgauge) {
                    uint32 oldprogress = 0;
                    IIntuition->GetAttr(FUELGAUGE_Level, owbwindow->gad_fuelgauge, &oldprogress);
                    uint32 newprogress = (uint32)(webView->estimatedProgress() * 1000);
                    if (oldprogress != newprogress)
                        IIntuition->RefreshSetGadgetAttrs(owbwindow->gad_fuelgauge, owbwindow->window, NULL,
                                                          FUELGAUGE_Level, newprogress,
                                                          TAG_DONE);
                }

                if (owbwindow->gad_vbar) {
                    uint32 top, total, visible;
                    IIntuition->GetAttrs(owbwindow->gad_vbar,
                                         PGA_Top, &top,
                                         PGA_Total, &total,
                                         PGA_Visible, &visible,
                                         TAG_DONE);
                    uint32 newtop = core(webView->mainFrame())->view()->contentsY();
                    uint32 newtotal = core(webView->mainFrame())->view()->contentsHeight();
                    uint32 newvisible = core(webView->mainFrame())->view()->visibleHeight();
                    if (top != newtop || total != newtotal || visible != newvisible)
                        IIntuition->RefreshSetGadgetAttrs(owbwindow->gad_vbar, owbwindow->window, NULL,
                                                          PGA_Top, newtop,
                                                          PGA_Total, newtotal,
                                                          PGA_Visible, newvisible,
                                                          TAG_DONE);
                }

                if (owbwindow->gad_hbar) {
                    uint32 top, total, visible;
                    IIntuition->GetAttrs(owbwindow->gad_hbar,
                                         PGA_Top, &top,
                                         PGA_Total, &total,
                                         PGA_Visible, &visible,
                                         TAG_DONE);
                    uint32 newtop = core(webView->mainFrame())->view()->contentsX();
                    uint32 newtotal = core(webView->mainFrame())->view()->contentsWidth();
                    uint32 newvisible = core(webView->mainFrame())->view()->visibleWidth();
                    if (top != newtop || total != newtotal || visible != newvisible)
                        IIntuition->RefreshSetGadgetAttrs(owbwindow->gad_hbar, owbwindow->window, NULL,
                                                          PGA_Top, newtop,
                                                          PGA_Total, newtotal,
                                                          PGA_Visible, newvisible,
                                                          TAG_DONE);
                }

                if (webView->canResetPageZoom()) {
                    IIntuition->OnMenu(owbwindow->window, FULLMENUNUM_ZoomReset);

                    IIntuition->OffMenu(owbwindow->window, FULLMENUNUM_TextLarger);
                    IIntuition->OffMenu(owbwindow->window, FULLMENUNUM_TextSmaller);
                    IIntuition->OffMenu(owbwindow->window, FULLMENUNUM_TextReset);

                    if (webView->canZoomPageIn())
                        IIntuition->OnMenu(owbwindow->window, FULLMENUNUM_ZoomIn);
                    else
                        IIntuition->OffMenu(owbwindow->window, FULLMENUNUM_ZoomIn);

                    if (webView->canZoomPageOut())
                        IIntuition->OnMenu(owbwindow->window, FULLMENUNUM_ZoomOut);
                    else
                        IIntuition->OffMenu(owbwindow->window, FULLMENUNUM_ZoomOut);
                }
                else {
                    IIntuition->OffMenu(owbwindow->window, FULLMENUNUM_ZoomReset);

                    if (webView->canMakeTextStandardSize()) {
                        IIntuition->OnMenu(owbwindow->window, FULLMENUNUM_TextReset);

                        IIntuition->OffMenu(owbwindow->window, FULLMENUNUM_ZoomIn);
                        IIntuition->OffMenu(owbwindow->window, FULLMENUNUM_ZoomReset);
                        IIntuition->OffMenu(owbwindow->window, FULLMENUNUM_ZoomOut);

                        if (webView->canMakeTextLarger())
                            IIntuition->OnMenu(owbwindow->window, FULLMENUNUM_TextLarger);
                        else
                            IIntuition->OffMenu(owbwindow->window, FULLMENUNUM_TextLarger);

                        if (webView->canMakeTextSmaller())
                            IIntuition->OnMenu(owbwindow->window, FULLMENUNUM_TextSmaller);
                        else
                            IIntuition->OffMenu(owbwindow->window, FULLMENUNUM_TextSmaller);
                    }
                    else {
                        IIntuition->OffMenu(owbwindow->window, FULLMENUNUM_TextReset);

                        if (webView->canZoomPageIn())
                            IIntuition->OnMenu(owbwindow->window, FULLMENUNUM_ZoomIn);
                        else
                            IIntuition->OffMenu(owbwindow->window, FULLMENUNUM_ZoomIn);

                        if (webView->canZoomPageOut())
                            IIntuition->OnMenu(owbwindow->window, FULLMENUNUM_ZoomOut);
                        else
                            IIntuition->OffMenu(owbwindow->window, FULLMENUNUM_ZoomOut);

                        if (webView->canMakeTextLarger())
                            IIntuition->OnMenu(owbwindow->window, FULLMENUNUM_TextLarger);
                        else
                            IIntuition->OffMenu(owbwindow->window, FULLMENUNUM_TextLarger);

                        if (webView->canMakeTextSmaller())
                            IIntuition->OnMenu(owbwindow->window, FULLMENUNUM_TextSmaller);
                        else
                            IIntuition->OffMenu(owbwindow->window, FULLMENUNUM_TextSmaller);
                    }
                }
            }

            owbwindow = owbwindow->next;
        }

        while (AppMessage* am = (AppMessage *)IExec->GetMsg(appIconPort)) {
            if (0 == am->am_NumArgs) {
                BalWidget *owbwindow = (BalWidget *)am->am_UserData;
                WebView *webView = 0;

                if (owbwindow)
                    webView = owbwindow->webView;
               
                if (webView && owbwindow->appicon && openAmigaWindow(owbwindow)) {
                    IWorkbench->RemoveAppIcon(owbwindow->appicon);
                    owbwindow->appicon = NULL;

                    Frame* frame = core(webView->mainFrame());
                    if (frame->view()) {
                        webView->addToDirtyRegion(IntRect(0, 0, frame->view()->visibleWidth(), frame->view()->visibleHeight()));
                        owbwindow->expose = true;
                    }
                }
            }

            IExec->ReplyMsg((Message*)am);
	}
    }
}

int main (int argc, char* argv[])
{
    if (!initTimer())
       return RETURN_ERROR;

    initAmigaConfig();

    sharedPort = (MsgPort *)IExec->AllocSysObjectTags(ASOT_PORT, TAG_DONE);
    if (sharedPort) {
        appIconPort = (MsgPort *)IExec->AllocSysObjectTags(ASOT_PORT, TAG_DONE);
        if (appIconPort) {
            WebView *webView = WebView::createInstance();
            BalWidget *owbwindow = createAmigaWindow(webView);

            if (owbwindow) {
                IntRect clientRect(0, 0, amigaConfig.width, amigaConfig.height);
                webView->initWithFrame(clientRect, "", "");

                webView->setViewWindow(owbwindow);

                webView->mainFrame()->loadURL(argc > 1 ? argv[1] : amigaConfig.homeURL);

                waitEvent();
            }
            else
                delete webView;

            IExec->FreeSysObject(ASOT_PORT, appIconPort);
        }
        IExec->FreeSysObject(ASOT_PORT, sharedPort);
    }

    cleanupTimer();

    if (icon)
        IIcon->FreeDiskObject(icon);

    DS_INST_DUMP_CURRENT(IOcout);
    DS_CLEAN_DEEPSEE_FRAMEWORK();
    return 0;
}

