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

#include "config.h"
#include "GraphicsContext.h"
#include <Api/WebFrame.h>
#include <Api/WebView.h>
#include "CString.h"
#include "Page.h"
#include "DOMImplementation.h"
#include "Editor.h"
#include "FocusController.h"
#include "Frame.h"
#include "FrameView.h"
#include "cairo.h"
#include "SelectionController.h"
#include "SharedTimer.h"
#include "SubstituteData.h"
#include "Timer.h"
#include "MainThread.h"
#include "WebBackForwardList.h"
#include "WebDataSource.h"
#include <FrameLoader.h>
#include <unistd.h>
#include <cstdio>

#include <proto/diskfont.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/icon.h>
#include <proto/iffparse.h>
#include <proto/intuition.h>
#include <proto/locale.h>
#include <proto/timer.h>
#include <proto/utility.h>
#include <proto/wb.h>
#include <datatypes/textclass.h>
#include <diskfont/diskfonttag.h>
#include <intuition/gui.h>
#include <intuition/icclass.h>
#include <libraries/keymap.h>

#include "bookmark.h"

#include "OWB_rev.h"

#define ALL_REACTION_CLASSES
#define ALL_REACTION_MACROS
#include <reaction/reaction.h>
#undef String

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
using std::min;

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
    GID_Iconify,
    GID_Bookmark,
    GID_BookmarkAdd,
    GID_ClickTab
};

enum menuIDs
{
    MID_NewWindow = 1,
    MID_Close,
    MID_Quit,
    MID_ZoomIn,
    MID_ZoomReset,
    MID_ZoomOut,
    MID_Redraw,
    MID_Iconify,
    MID_Cut,
    MID_Copy,
    MID_Paste,
    MID_SelectAll,
    MID_CopyHTML,
    MID_CopyLatin1,
    MID_Source,
    MID_BookMark
};

enum arexxIDs
{
    RXID_OPENURL = 1,
    RXID_STOP,
    RXID_BACK,
    RXID_FORWARD,
    RXID_RELOAD,
    RXID_HOME,
    RXID_SCREENTOFRONT,
    RXID_GETURL,
    RXID_GETTITLE
};

AmigaConfig amigaConfig;
MsgPort *sharedPort, *appIconPort;
BalWidget* firstWindow;
DiskObject* icon;
Task* mainTask;
uint32 sharedTimerSignal;
static int32 sharedTimerSignalBit;
static Object* rxObject;
static uint32 arexxSignal;
static List clickTabList;
static char* startPage;

static NewMenu Menus[] =
{
    Title((STRPTR)"Project"),
        Item((STRPTR)"New window",         (STRPTR)"N", MID_NewWindow),
        Item((STRPTR)"Close window",       (STRPTR)"K", MID_Close),
        ItemBar,
        Item((STRPTR)"Iconify",            (STRPTR)"I", MID_Iconify),
        ItemBar,
        Item((STRPTR)"Quit",               (STRPTR)"Q", MID_Quit),
    Title((STRPTR)"Edit"),
        Item((STRPTR)"Cut",                (STRPTR)"X", MID_Cut),
        Item((STRPTR)"Copy",               (STRPTR)"C", MID_Copy),
//        Item((STRPTR)"Copy as HTML",        NULL, MID_CopyHTML),
        Item((STRPTR)"Copy as ISO-8859-1",  NULL, MID_CopyLatin1),
        Item((STRPTR)"Paste",              (STRPTR)"V", MID_Paste),
        Item((STRPTR)"Select all",         (STRPTR)"A", MID_SelectAll),
    Title((STRPTR)"View"),
        Item((STRPTR)"Zoom in",            (STRPTR)"+", MID_ZoomIn),
        Item((STRPTR)"Zoom reset",         (STRPTR)"0", MID_ZoomReset),
        Item((STRPTR)"Zoom out",           (STRPTR)"-", MID_ZoomOut),
        ItemBar,
        Item((STRPTR)"Redraw",             (STRPTR)"R", MID_Redraw),
        Item((STRPTR)"Source",             (STRPTR)"U", MID_Source),
    Title((STRPTR)"Tools"),
        Item((STRPTR)"External bookmark",  (STRPTR)"B", MID_BookMark),
    EndMenu
};

#define FULLMENUNUM_ZoomIn      FULLMENUNUM(2, 0, 0)
#define FULLMENUNUM_ZoomReset   FULLMENUNUM(2, 1, 0)
#define FULLMENUNUM_ZoomOut     FULLMENUNUM(2, 2, 0)
#define FULLMENUNUM_Source      FULLMENUNUM(2, 5, 0)

extern "C" {
static void rxFunc(ARexxCmd*, RexxMsg*);
static ARexxCmd rxCommands[] =
{
    { (char*)"OPENURL",       RXID_OPENURL,       (void(*)(void))rxFunc, (char*)"URL/A", 0, NULL, 0, 0, NULL},
    { (char*)"STOP",          RXID_STOP,          (void(*)(void))rxFunc, NULL,           0, NULL, 0, 0, NULL},
    { (char*)"GOBACK",        RXID_BACK,          (void(*)(void))rxFunc, NULL,           0, NULL, 0, 0, NULL},
    { (char*)"GOFORWARD",     RXID_FORWARD,       (void(*)(void))rxFunc, NULL,           0, NULL, 0, 0, NULL},
    { (char*)"RELOAD",        RXID_RELOAD,        (void(*)(void))rxFunc, NULL,           0, NULL, 0, 0, NULL},
    { (char*)"GOHOME",        RXID_HOME,          (void(*)(void))rxFunc, NULL,           0, NULL, 0, 0, NULL},
    { (char*)"SCREENTOFRONT", RXID_SCREENTOFRONT, (void(*)(void))rxFunc, NULL,           0, NULL, 0, 0, NULL},
    { (char*)"GETURL",        RXID_GETURL,        (void(*)(void))rxFunc, NULL,           0, NULL, 0, 0, NULL},
    { (char*)"GETTITLE",      RXID_GETTITLE,      (void(*)(void))rxFunc, NULL,           0, NULL, 0, 0, NULL},
    { NULL,                   0,                  NULL,                  NULL,           0, NULL, 0, 0, NULL}
};
}



static void initAmigaConfig(void)
{
    static bool isInitialized = false;
    if (!isInitialized) {
        amigaConfig.homeURL = "http://amigans.net/";
        amigaConfig.searchURL = "http://google.com/search?q=%s";
        amigaConfig.minFontSize = 9;
        amigaConfig.fontXDPI = 84;
        amigaConfig.fontYDPI = 84;
        amigaConfig.unicodeFontName = "Bitstream Cyberbit";
        amigaConfig.windowleft = 0;
        amigaConfig.windowtop = 0;
        amigaConfig.windowwidth = 1024;
        amigaConfig.windowheight = 768;
        amigaConfig.pubScreenName = "OrigynWebBrowser";
        amigaConfig.bookMarkCommand = "OWBAddressBook/AddressBook -%s";
        amigaConfig.cookiePath = "PROGDIR:cookieCollection.txt";
        amigaConfig.fontKerning = false;
        amigaConfig.fontHinter = 3 - 1;
        amigaConfig.tabs = false;

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

            found = IIcon->FindToolType(icon->do_ToolTypes, "CONFIRMQUIT");
            if (found)
                amigaConfig.confirmQuit = true;

            found = IIcon->FindToolType(icon->do_ToolTypes, "PUBSCREEN");
            if (found)
                amigaConfig.pubScreenName = strdup(found);

            found = IIcon->FindToolType(icon->do_ToolTypes, "BOOKMARKCOMMAND");
            if (found)
                amigaConfig.bookMarkCommand = strdup(found);

            found = IIcon->FindToolType(icon->do_ToolTypes, "COOKIES");
            if (found)
                amigaConfig.cookiePath = strdup(found);

            found = IIcon->FindToolType(icon->do_ToolTypes, "FONTKERNING");
            if (found)
                amigaConfig.fontKerning = true;

            found = IIcon->FindToolType(icon->do_ToolTypes, "FONTHINTER");
            if (found)
                amigaConfig.fontHinter = atoi(found) - 1;

            found = IIcon->FindToolType(icon->do_ToolTypes, "CONTEXTMENURMB");
            if (found)
                amigaConfig.contextMenuRMB = true;

            found = IIcon->FindToolType(icon->do_ToolTypes, "TABS");
            if (found)
                amigaConfig.tabs = true;

            found = IIcon->FindToolType(icon->do_ToolTypes, "STARTPAGE");
            if (found)
                startPage = strdup(found);

            amigaConfig.minFontSize = max(amigaConfig.minFontSize, 5);
            amigaConfig.fontXDPI    = max(amigaConfig.fontXDPI,10);
            amigaConfig.fontYDPI    = max(amigaConfig.fontYDPI,10);

            icon->do_Type = 0;
        }


        // AmiUpdate support
        BPTR lock = IDOS->GetProgramDir();
        if (lock) {
            TEXT progpath[16*1024];

            if (IDOS->DevNameFromLock(lock, progpath, sizeof(progpath), DN_FULLPATH)) {
                TEXT old_progpath[16*1024];
                APTR oldwin = IDOS->SetProcWindow((APTR)-1);

                if (-1 == IDOS->GetVar("AppPaths/OWB", old_progpath, sizeof(progpath), LV_VAR | GVF_GLOBAL_ONLY)
                 || strcmp(old_progpath, progpath))
                    IDOS->SetVar("AppPaths/OWB", progpath, -1, LV_VAR | GVF_GLOBAL_ONLY | GVF_SAVE_VAR);

                IDOS->SetProcWindow(oldwin);
            }
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



static void rxFunc(ARexxCmd* cmd, RexxMsg*)
{
    BalWidget* window = firstWindow;
    if (amigaConfig.tabs) {
        ::Node* node;
        IIntuition->GetAttr(CLICKTAB_CurrentNode, window->gad_clicktab, (uint32*)(void*)&node);
        IClickTab->GetClickTabNodeAttrs(node, TNA_UserData, &window, TAG_DONE);
    }
    else
        while (window->next)
            window = window->next;

    char* str = 0;
    if (cmd->ac_ArgList)
        str = (char*)cmd->ac_ArgList[0];

    switch (cmd->ac_ID)  {
    case RXID_OPENURL:
        window->webView->mainFrame()->loadURL(str);
    break;

    case RXID_STOP:
        core(window->webView->mainFrame())->loader()->stopAllLoaders();
    break;

    case RXID_BACK:
        window->webView->goBack();
        window->expose = true;
    break;

    case RXID_FORWARD:
        window->webView->goForward();
        window->expose = true;
    break;

    case RXID_RELOAD:
        window->webView->mainFrame()->reload();
    break;

    case RXID_HOME:
        window->webView->mainFrame()->loadURL(amigaConfig.homeURL);
    break;

    case RXID_SCREENTOFRONT:
        if (window->window)
            IIntuition->ScreenToFront(window->window->WScreen);
    break;

    case RXID_GETURL:
        cmd->ac_Result = (char*)"";

        if (window->webView) {
            static char* prevURL = 0;
            free(prevURL);
            prevURL = cmd->ac_Result = (char*)window->webView->mainFrame()->url();
        }
    break;

    case RXID_GETTITLE:
        cmd->ac_Result = (char*)"";

        if (window->webView) {
            static char* prevTitle = 0;
            free(prevTitle);
            prevTitle = cmd->ac_Result = (char*)window->webView->mainFrameTitle();
        }
    break;
    }
}

void initARexx(void)
{
    rxObject = (Object*)ARexxObject,
                            AREXX_HostName, "OWB",
                            AREXX_Commands, rxCommands,
                        End;

    if (rxObject)
        IIntuition->GetAttr(AREXX_SigMask, rxObject, &arexxSignal);
}

void cleanupARexx(void)
{
    if (rxObject)
        IIntuition->DisposeObject(rxObject);
}



static void closeAmigaWindowWithoutSurface(BalWidget* owbwindow)
{
    if (owbwindow->clickTabNode) {
        ::Node* next = IExec->GetSucc(owbwindow->clickTabNode);
        if (!next)
            next = IExec->GetPred(owbwindow->clickTabNode);

        if (firstWindow) {
            IIntuition->SetAttrs(firstWindow->gad_clicktab,
                                 CLICKTAB_Labels, ~0,
                                 TAG_DONE);
            IExec->Remove(owbwindow->clickTabNode);
        }

        IClickTab->FreeClickTabNode(owbwindow->clickTabNode);
        owbwindow->clickTabNode = 0;

        if (firstWindow) {
            BalWidget* window = firstWindow;
            int32 nodeNumber = 0;
            while (window) {
                if (window->clickTabNode)
                    IClickTab->SetClickTabNodeAttrs(window->clickTabNode, TNA_Number, nodeNumber, TAG_DONE);
                nodeNumber++;
                window = window->next;
            }

            IIntuition->RefreshSetGadgetAttrs(firstWindow->gad_clicktab, firstWindow->window, NULL,
                                              CLICKTAB_Labels, firstWindow->clickTabList,
                                              CLICKTAB_CurrentNode, next,
                                              TAG_DONE);

            BalWidget* activeTab = firstWindow;
            if (next)
                IClickTab->GetClickTabNodeAttrs(next, TNA_UserData, &activeTab, TAG_DONE);
            Frame* frame = core(activeTab->webView->mainFrame());
            if (frame->view()) {
                activeTab->webView->addToDirtyRegion(IntRect(0, 0, frame->view()->visibleWidth(), frame->view()->visibleHeight()));
                activeTab->expose = true;
            }
        }
    }

    if (!amigaConfig.tabs || !firstWindow) {
        if (owbwindow->window->MenuStrip) {
            Menu* menustrip = owbwindow->window->MenuStrip;
            IIntuition->ClearMenuStrip(owbwindow->window);
            IGadTools->FreeMenus(menustrip);
        }
        IIntuition->CloseWindow(owbwindow->window);
        owbwindow->window = 0;
        IIntuition->DisposeObject(owbwindow->gad_vbar);
        owbwindow->gad_vbar = 0;
        IIntuition->DisposeObject(owbwindow->gad_hbar);
        owbwindow->gad_hbar = 0;
        IIntuition->DisposeObject(owbwindow->img_iconify);
        owbwindow->img_iconify = 0;
        IIntuition->DisposeObject(owbwindow->gad_iconify);
        owbwindow->gad_iconify = 0;
        IIntuition->DisposeObject(owbwindow->gad_toolbar);
        owbwindow->gad_toolbar = 0;
    }
    else {
        IIntuition->RefreshSetGadgetAttrs((Gadget*)owbwindow->page, owbwindow->window, NULL,
                                          PAGE_Remove, owbwindow->gad_vlayout,
                                          TAG_DONE);
        owbwindow->gad_vlayout = 0;
        owbwindow->window->UserData = (int8*)firstWindow;
        owbwindow->gad_vbar = 0;
        owbwindow->gad_hbar = 0;
        owbwindow->img_iconify = 0;
        owbwindow->gad_iconify = 0;
        owbwindow->gad_toolbar = 0;
        owbwindow->gad_clicktab = 0;
        owbwindow->gad_fuelgauge = 0;
        owbwindow->window = 0;
    }

    owbwindow->gad_status = 0;

    if (amigaConfig.tabs && firstWindow)
        firstWindow->backfill_hook = owbwindow->backfill_hook;
    else
        IIntuition->ReleaseIBackFill(owbwindow->backfill_hook);
    owbwindow->backfill_hook = 0;
    IIntuition->DisposeObject(owbwindow->img_back);
    owbwindow->img_back = 0;
    IIntuition->DisposeObject(owbwindow->img_forward);
    owbwindow->img_forward = 0;
    IIntuition->DisposeObject(owbwindow->img_stop);
    owbwindow->img_stop = 0;
    IIntuition->DisposeObject(owbwindow->img_home);
    owbwindow->img_home = 0;
    IIntuition->DisposeObject(owbwindow->img_reload);
    owbwindow->img_reload = 0;
    IIntuition->DisposeObject(owbwindow->img_bookmark);
    owbwindow->img_bookmark = 0;
    IIntuition->DisposeObject(owbwindow->img_bookmarkadd);
    owbwindow->img_bookmarkadd = 0;
}

void closeAmigaWindow(BalWidget* owbwindow)
{
    if (!owbwindow)
        return;

    if (firstWindow == owbwindow)
        firstWindow = owbwindow->next;
    else {
        BalWidget* window = firstWindow;
        while (window && window->next != owbwindow)
            window = window->next;
        if (window)
            window->next = owbwindow->next;
    }

    if (owbwindow->window)
        closeAmigaWindowWithoutSurface(owbwindow);

    if (amigaConfig.tabs && firstWindow) {
        ILayout->FlushLayoutDomainCache(firstWindow->gad_toolbar);
        ILayout->RethinkLayout(firstWindow->gad_toolbar, firstWindow->window, NULL, TRUE);
        LayoutLimits layoutlimits;
        ILayout->LayoutLimits(firstWindow->gad_toolbar, &layoutlimits, NULL, firstWindow->window->WScreen);
        layoutlimits.MinWidth  += firstWindow->window->BorderLeft + firstWindow->window->BorderRight;
        layoutlimits.MinHeight += firstWindow->window->BorderTop  + firstWindow->window->BorderBottom;
        layoutlimits.MinHeight = max(layoutlimits.MinHeight, (uint16)360);
        firstWindow->window->MinWidth  = layoutlimits.MinWidth;
        firstWindow->window->MinHeight = layoutlimits.MinHeight;
    }

    if ((!amigaConfig.tabs || !firstWindow) && owbwindow->appicon)
        IWorkbench->RemoveAppIcon(owbwindow->appicon);

    if (owbwindow->cr)
        cairo_destroy(owbwindow->cr);

    if (owbwindow->surface)
        cairo_surface_destroy(owbwindow->surface);

    delete owbwindow->webView;

    if ((!amigaConfig.tabs || !firstWindow) && owbwindow->bookmark)
        amiga_bookmark_exit((OWBBookMark*)owbwindow->bookmark);

    free(owbwindow);
}

extern "C" {
void* createTBImage(const char* name, ::Screen *screen)
{
    char normal[2][128];
    char selected[2][128];
    char ghosted[2][128];
    void* result = 0;

    snprintf(normal[0],   sizeof(normal[0]),   "PROGDIR:Resources/ToolbarImages/%s",   name);
    snprintf(selected[0], sizeof(selected[0]), "PROGDIR:Resources/ToolbarImages/%s_s", name);
    snprintf(ghosted[0],  sizeof(ghosted[0]),  "PROGDIR:Resources/ToolbarImages/%s_g", name);
    snprintf(normal[1],   sizeof(normal[1]),   "TBImages:%s",   name);
    snprintf(selected[1], sizeof(selected[1]), "TBImages:%s_s", name);
    snprintf(ghosted[1],  sizeof(ghosted[1]),  "TBImages:%s_g", name);

    for (int i = 0 ; i < 2 && !result ; i++)
        result = BitMapObject,
            BITMAP_Screen, screen,
            BITMAP_Masking, TRUE,
            BITMAP_Transparent, TRUE,
            BITMAP_SourceFile, normal[i],
            BITMAP_SelectSourceFile, selected[i],
            BITMAP_DisabledSourceFile, ghosted[i],
        BitMapEnd;

    return result;
}
};

static char* screenName;
static bool openAmigaWindow(BalWidget* owbwindow, bool first, int32 nodeNumber)
{
    static TEXT fallbackTextFontName[256];
    static TextAttr fallbackTextAttr = { fallbackTextFontName, 0, 0, 0 };

    if (!screenName)
        asprintf(&screenName, "Origyn Web Browser %s", &VERSTAG[11]);

    int windowX = amigaConfig.windowleft;
    int windowY = amigaConfig.windowtop;
    int windowW = amigaConfig.windowwidth;
    int windowH = amigaConfig.windowheight;

    if (owbwindow->width) {
        windowX = owbwindow->left;
        windowY = owbwindow->top;
        if (0 == windowY)
            windowY = -1;
        windowW = owbwindow->width;
        windowH = owbwindow->height;
    }

    if (first)
        IExec->NewList(&clickTabList);
    owbwindow->clickTabList = &clickTabList;

    Window *window;
    if (first || !amigaConfig.tabs)
        window = IIntuition->OpenWindowTags(NULL,
                                            WA_Left, windowX,
                                            windowY ? WA_Top : TAG_IGNORE, windowY,
                                            WA_Width, windowW,
                                            WA_Height, windowH,
                                            WA_MinWidth, 480,
                                            WA_MinHeight, 360,
                                            WA_MaxWidth, ~0UL,
                                            WA_MaxHeight, ~0UL,
                                            WA_SmartRefresh, TRUE,
                                            WA_NoCareRefresh, TRUE,
                                            WA_Title, amigaConfig.tabs ? "Origyn Web Browser" : owbwindow->title,
                                            WA_ScreenTitle, screenName,
                                            WA_ReportMouse, TRUE,
                                            WA_SizeGadget, TRUE,
                                            WA_SizeBRight, TRUE,
                                            WA_SizeBBottom, TRUE,
                                            WA_DragBar, TRUE,
                                            WA_DepthGadget, TRUE,
                                            WA_CloseGadget, TRUE,
                                            WA_Activate, TRUE,
                                            WA_PubScreenName, amigaConfig.pubScreenName,
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
                                                    | IDCMP_GADGETUP
                                                    | (amigaConfig.contextMenuRMB ? IDCMP_MENUVERIFY : 0),
                                            WA_NoMenuKeyVerify, TRUE,
                                            TAG_DONE);
    else
        window = firstWindow->window;

    if (!window)
        return false;

    owbwindow->window = window;

    IIntuition->ScreenToFront(window->WScreen);

    window->UserData = (int8*)owbwindow;

    APTR old_proc_window = IDOS->SetProcWindow((APTR)-1);
    DrawInfo* drinfo = IIntuition->GetScreenDrawInfo(window->WScreen);
    if (first || !amigaConfig.tabs)
        owbwindow->backfill_hook = IIntuition->ObtainIBackFill(drinfo, BT_WINDOWBG, IDS_NORMAL, 0);
    owbwindow->img_back        = createTBImage("nav_west",       window->WScreen);
    owbwindow->img_forward     = createTBImage("nav_east",       window->WScreen);
    owbwindow->img_stop        = createTBImage("stop",           window->WScreen);
    owbwindow->img_search      = createTBImage("searchweb",      window->WScreen);
    owbwindow->img_home        = createTBImage("home",           window->WScreen);
    owbwindow->img_reload      = createTBImage("reload",         window->WScreen);
    owbwindow->img_bookmark    = createTBImage("addressbook",    window->WScreen);
    owbwindow->img_bookmarkadd = createTBImage("addressbookadd", window->WScreen);

    if (!fallbackTextFontName[0]) {
        strcpy(fallbackTextFontName, "RAfallback.font");
        fallbackTextAttr.ta_YSize = 8;
        TextAttr* ta = 0;
        IIntuition->GetGUIAttrs(NULL, drinfo, GUIA_FallbackTextAttr, &ta, TAG_DONE);
        if (ta) {
            strlcpy(fallbackTextAttr.ta_Name, ta->ta_Name, sizeof(fallbackTextFontName));
            fallbackTextAttr.ta_YSize = ta->ta_YSize;
        }
    }

    owbwindow->gad_vlayout = (Gadget*)
        VLayoutObject,
            LAYOUT_AddChild, HLayoutObject,
                LAYOUT_AddChild, owbwindow->gad_back = (Gadget*)ButtonObject,
                    GA_ID, GID_Back,
                    GA_RelVerify, TRUE,
                    owbwindow->img_back ? BUTTON_RenderImage : TAG_IGNORE, owbwindow->img_back,
                    owbwindow->img_back ? TAG_IGNORE : BUTTON_AutoButton, BAG_LFARROW,
                    GA_Disabled, TRUE,
                ButtonEnd,
                CHILD_WeightedWidth, 0,
                LAYOUT_AddChild, owbwindow->gad_forward = (Gadget*)ButtonObject,
                    GA_ID, GID_Forward,
                    GA_RelVerify, TRUE,
                    owbwindow->img_forward ? BUTTON_RenderImage : TAG_IGNORE, owbwindow->img_forward,
                    owbwindow->img_forward ? TAG_IGNORE : BUTTON_AutoButton, BAG_RTARROW,
                    GA_Disabled, TRUE,
                ButtonEnd,
                CHILD_WeightedWidth, 0,
                LAYOUT_AddChild, owbwindow->gad_stop = (Gadget*)ButtonObject,
                    GA_ID, GID_Stop,
                    GA_RelVerify, TRUE,
                    owbwindow->img_stop ? BUTTON_RenderImage : GA_Text, owbwindow->img_stop ?: "Stop",
                    GA_TabCycle, NULL == owbwindow->img_stop,
                    GA_Disabled, TRUE,
                ButtonEnd,
                CHILD_WeightedWidth, 0,
                LAYOUT_AddChild, ButtonObject,
                    GA_ID, GID_Reload,
                    GA_RelVerify, TRUE,
                    owbwindow->img_reload ? BUTTON_RenderImage : GA_Text, owbwindow->img_reload ?: "Reload",
                    GA_TabCycle, NULL == owbwindow->img_reload,
                ButtonEnd,
                CHILD_WeightedWidth, 0,
                LAYOUT_AddChild, ButtonObject,
                    GA_ID, GID_Home,
                    GA_RelVerify, TRUE,
                    owbwindow->img_home ? BUTTON_RenderImage : GA_Text, owbwindow->img_home ?: "Home",
                    GA_TabCycle, NULL == owbwindow->img_home,
                ButtonEnd,
                CHILD_WeightedWidth, 0,
                LAYOUT_AddChild, owbwindow->gad_url = (Gadget*)StringObject,
                    STRINGA_TextVal, owbwindow->url,
                    STRINGA_MaxChars, 2000,
                    GA_ID, GID_URL,
                    GA_RelVerify, TRUE,
                    GA_TabCycle, TRUE,
                StringEnd,
                CHILD_Label, LabelObject,
                    LABEL_DrawInfo, drinfo,
                LabelEnd,
                LAYOUT_AddChild, owbwindow->gad_bookmarkadd = (Gadget*)ButtonObject,
                    GA_ID, GID_BookmarkAdd,
                    GA_RelVerify, TRUE,
                    owbwindow->img_bookmarkadd ? BUTTON_RenderImage : GA_Text, owbwindow->img_bookmarkadd ?: "Add",
                    GA_TabCycle, NULL == owbwindow->img_bookmarkadd,
                ButtonEnd,
                CHILD_WeightedWidth, 0,
                LAYOUT_AddChild, owbwindow->gad_bookmark = (Gadget*)ButtonObject,
                    GA_ID, GID_Bookmark,
                    GA_RelVerify, TRUE,
                    owbwindow->img_bookmark ? BUTTON_RenderImage : GA_Text, owbwindow->img_bookmark ?: "Bookmark",
                    GA_TabCycle, NULL == owbwindow->img_bookmark,
                ButtonEnd,
                CHILD_WeightedWidth, 0,
                LAYOUT_AddChild, owbwindow->gad_search = (Gadget*)StringObject,
                    STRINGA_TextVal, owbwindow->search,
                    STRINGA_MaxChars, 500,
                    GA_ID, GID_Search,
                    GA_RelVerify, TRUE,
                    GA_TabCycle, TRUE,
                StringEnd,
                CHILD_Label, LabelObject,
                    LABEL_DrawInfo, drinfo,
                    LABEL_DisposeImage, NULL != owbwindow->img_search,
                    owbwindow->img_search ? LABEL_Image : TAG_IGNORE, owbwindow->img_search,
                    owbwindow->img_search ? TAG_IGNORE : LABEL_Text, "Search",
                LabelEnd,
                CHILD_WeightedWidth, 40,
            LayoutEnd,
            CHILD_WeightedHeight, 0,

            LAYOUT_AddImage, BevelObject,
                BEVEL_Style, BVS_NONE, //BVS_SBAR_VERT,
            BevelEnd,
            CHILD_WeightedHeight, 0,

            LAYOUT_AddChild, owbwindow->gad_webview = (Gadget*)SpaceObject,
                GA_ReadOnly, TRUE,
                SPACE_Transparent, TRUE,
            SpaceEnd,

            LAYOUT_AddChild, owbwindow->gad_hlayout = (Gadget*)HLayoutObject,
                LAYOUT_BevelStyle, BVS_DISPLAY,
                LAYOUT_AddChild, owbwindow->gad_status = (Gadget*)ButtonObject,
                    GA_Underscore, 0,
                    GA_TextAttr, &fallbackTextAttr,
                    GA_ReadOnly, TRUE,
                    BUTTON_BevelStyle, BVS_NONE,
                    BUTTON_Transparent, TRUE,
                    BUTTON_Justification, BCJ_LEFT,
                ButtonEnd,
                CHILD_WeightedWidth, 80,
                LAYOUT_AddChild, owbwindow->gad_fuelgauge = (Gadget*)FuelGaugeObject,
                    FUELGAUGE_Min, 0,
                    FUELGAUGE_Max, 1000,
                    FUELGAUGE_Level, 0,
                    FUELGAUGE_Ticks, 0,
                    FUELGAUGE_Percent, FALSE,
                    FUELGAUGE_Justification, FGJ_CENTER,
                    GA_TextAttr, &fallbackTextAttr,
                FuelGaugeEnd,
                CHILD_WeightedWidth, 20,
            LayoutEnd,
            CHILD_WeightedHeight, 0,
        LayoutEnd;

    if (!owbwindow->gad_vlayout) {
        IIntuition->DisposeObject(owbwindow->img_back);
        IIntuition->DisposeObject(owbwindow->img_forward);
        IIntuition->DisposeObject(owbwindow->img_stop);
        IIntuition->DisposeObject(owbwindow->img_search);
        IIntuition->DisposeObject(owbwindow->img_home);
        IIntuition->DisposeObject(owbwindow->img_reload);
        IIntuition->DisposeObject(owbwindow->img_bookmark);
        IIntuition->DisposeObject(owbwindow->img_bookmarkadd);
        IIntuition->ReleaseIBackFill(owbwindow->backfill_hook);
        if (first || !amigaConfig.tabs)
            IIntuition->CloseWindow(window);
        return false;
    }

    LayoutLimits layoutlimits;
    ILayout->LayoutLimits(owbwindow->gad_vlayout, &layoutlimits, NULL, window->WScreen);

    if (first || !amigaConfig.tabs) {
        owbwindow->gad_vbar = (Gadget*)IIntuition->NewObject(NULL, "scrollergclass",
                                                             GA_ID, GID_VBar,
                                                             GA_RightBorder, TRUE,
                                                             GA_RelVerify, TRUE,
                                                             GA_Immediate, TRUE,
                                                             GA_FollowMouse, TRUE,
                                                             GA_DrawInfo, drinfo,
                                                             PGA_ArrowDelta, 20,
                                                             ICA_TARGET, ICTARGET_IDCMP,
                                                             TAG_END);

        owbwindow->gad_hbar = (Gadget*)IIntuition->NewObject(NULL, "scrollergclass",
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
    }

    if (amigaConfig.tabs) {
        if (nodeNumber < 0) {
            nodeNumber = 0;
            if (!first) {
                BalWidget* widget = firstWindow;
                while (widget) {
                    nodeNumber++;
                    widget = widget->next;
                }
            }
        }

        owbwindow->clickTabNode = IClickTab->AllocClickTabNode(TNA_Text, owbwindow->title, TNA_UserData, owbwindow, TNA_Number, nodeNumber, TAG_DONE);

        if (first) {
            if (owbwindow->clickTabNode)
                IExec->AddTail(owbwindow->clickTabList, owbwindow->clickTabNode);

            owbwindow->gad_toolbar = (Gadget*)VLayoutObject,
                                                  GA_Left, window->BorderLeft,
                                                  GA_Top, window->BorderTop,
                                                  GA_RelWidth, -(window->BorderLeft + window->BorderRight),
                                                  GA_RelHeight, -(window->BorderTop + window->BorderBottom),
                                                  GA_NoFilterWheel, TRUE,
                                                  ICA_TARGET, ICTARGET_IDCMP,
                                                  LAYOUT_LayoutBackFill, owbwindow->backfill_hook,
                                                  LAYOUT_DeferLayout, TRUE,
                                                  LAYOUT_AddChild, owbwindow->gad_clicktab = (Gadget*)ClickTabObject,
                                                      GA_ID, GID_ClickTab,
                                                      CLICKTAB_Labels, owbwindow->clickTabList,
                                                      CLICKTAB_LabelTruncate, TRUE,
                                                      CLICKTAB_EvenSize, FALSE,
                                                      CLICKTAB_CurrentNode, owbwindow->clickTabNode,
                                                      CLICKTAB_PageGroup, owbwindow->page = (Object*)PageObject,
                                                          PAGE_Add, owbwindow->gad_vlayout,
                                                      PageEnd,
                                                      ICA_TARGET, ICTARGET_IDCMP,
                                                  ClickTabEnd,
                                              LayoutEnd;

        }
        else {
            owbwindow->page = firstWindow->page;
            owbwindow->gad_toolbar = firstWindow->gad_toolbar;
            owbwindow->gad_clicktab = firstWindow->gad_clicktab;
            owbwindow->gad_vbar = firstWindow->gad_vbar;
            owbwindow->gad_hbar = firstWindow->gad_hbar;
            owbwindow->gad_iconify = firstWindow->gad_iconify;
            owbwindow->img_iconify = firstWindow->img_iconify;

            IIntuition->SetAttrs(owbwindow->gad_clicktab,
                                 CLICKTAB_Labels, ~0,
                                 TAG_DONE);
            if (owbwindow->clickTabNode)
                IExec->AddTail(owbwindow->clickTabList, owbwindow->clickTabNode);
            IIntuition->RefreshSetGadgetAttrs((Gadget*)owbwindow->page, window, NULL,
                                              PAGE_Add, owbwindow->gad_vlayout,
                                              TAG_DONE);
            IIntuition->RefreshSetGadgetAttrs(owbwindow->gad_clicktab, window, NULL,
                                              CLICKTAB_Labels, owbwindow->clickTabList,
                                              CLICKTAB_CurrentNode, owbwindow->clickTabNode,
                                              TAG_DONE);

            ILayout->FlushLayoutDomainCache(owbwindow->gad_toolbar);
            ILayout->RethinkLayout(owbwindow->gad_toolbar, window, NULL, TRUE);
        }
    }
    else
        owbwindow->gad_toolbar = (Gadget*)VLayoutObject,
                                              GA_Left, window->BorderLeft,
                                              GA_Top, window->BorderTop,
                                              GA_RelWidth, -(window->BorderLeft + window->BorderRight),
                                              GA_RelHeight, -(window->BorderTop + window->BorderBottom),
                                              GA_NoFilterWheel, TRUE,
                                              ICA_TARGET, ICTARGET_IDCMP,
                                              LAYOUT_LayoutBackFill, owbwindow->backfill_hook,
                                              LAYOUT_DeferLayout, TRUE,
                                              LAYOUT_AddChild, owbwindow->page = (Object*)PageObject,
                                                  PAGE_Add, owbwindow->gad_vlayout,
                                              PageEnd,
                                          LayoutEnd;

    if (first || !amigaConfig.tabs)
        if (owbwindow->gad_toolbar && icon) {
            uint32 img_size = (window->WScreen->Flags & SCREENHIRES) ? SYSISIZE_MEDRES : SYSISIZE_LOWRES;
            uint32 img_height = window->WScreen->Font->ta_YSize + window->WScreen->WBorTop + 1;

            owbwindow->img_iconify = IIntuition->NewObject(NULL, "sysiclass",
                                                           SYSIA_Which, ICONIFYIMAGE,
                                                           SYSIA_Size, img_size,
                                                           SYSIA_DrawInfo, drinfo,
                                                           IA_Height, img_height,
                                                           TAG_DONE);
            if (owbwindow->img_iconify)
                owbwindow->gad_iconify = (Gadget*)IIntuition->NewObject(NULL, "buttongclass",
                                                                        GA_Image, owbwindow->img_iconify,
                                                                        GA_ID, GID_Iconify,
                                                                        GA_RelVerify, TRUE,
                                                                        GA_Titlebar, TRUE,
                                                                        GA_RelRight, 0,
                                                                        TAG_DONE);
        }

    IIntuition->FreeScreenDrawInfo(window->WScreen, drinfo);
    IDOS->SetProcWindow(old_proc_window);

    if (!owbwindow->gad_toolbar) {
        if (first || !amigaConfig.tabs) {
            IIntuition->DisposeObject(owbwindow->gad_iconify);
            IIntuition->DisposeObject(owbwindow->img_iconify);
            IIntuition->DisposeObject(owbwindow->gad_vbar);
            IIntuition->DisposeObject(owbwindow->gad_hbar);
        }
        IIntuition->DisposeObject(owbwindow->img_back);
        IIntuition->DisposeObject(owbwindow->img_forward);
        IIntuition->DisposeObject(owbwindow->img_stop);
        IIntuition->DisposeObject(owbwindow->img_search);
        IIntuition->DisposeObject(owbwindow->img_home);
        IIntuition->DisposeObject(owbwindow->img_reload);
        IIntuition->DisposeObject(owbwindow->img_bookmark);
        IIntuition->DisposeObject(owbwindow->img_bookmarkadd);
        IIntuition->ReleaseIBackFill(owbwindow->backfill_hook);
        if (owbwindow->clickTabNode) {
            IExec->Remove(owbwindow->clickTabNode);
            IClickTab->FreeClickTabNode(owbwindow->clickTabNode);
        }
        if (first || !amigaConfig.tabs)
            IIntuition->CloseWindow(window);
        return false;
    }

    ILayout->LayoutLimits(owbwindow->gad_toolbar, &layoutlimits, NULL, window->WScreen);
    layoutlimits.MinWidth  += window->BorderLeft + window->BorderRight;
    layoutlimits.MinHeight += window->BorderTop  + window->BorderBottom;
    layoutlimits.MinHeight = max(layoutlimits.MinHeight, (uint16)360);
    if (layoutlimits.MinWidth > window->Width || layoutlimits.MinHeight > window->Height)
        IIntuition->ChangeWindowBox(window,
                                    window->LeftEdge,
                                    window->TopEdge,
                                    max(layoutlimits.MinWidth, (uint16)window->Width),
                                    max(layoutlimits.MinHeight, (uint16)window->Height));
    window->MinWidth  = layoutlimits.MinWidth;
    window->MinHeight = layoutlimits.MinHeight;

    if (first || !amigaConfig.tabs) {
        IIntuition->AddGadget(window, owbwindow->gad_toolbar, (uint16)~0);

        if (owbwindow->gad_vbar)
            IIntuition->AddGadget(window, owbwindow->gad_vbar, (uint16)~0);
        if (owbwindow->gad_hbar)
            IIntuition->AddGadget(window, owbwindow->gad_hbar, (uint16)~0);
        if (owbwindow->gad_iconify)
            IIntuition->AddGadget(window, owbwindow->gad_iconify, (uint16)~0);

        IIntuition->RefreshGList(owbwindow->gad_toolbar, window, NULL, -1);
    }

    if (first || !amigaConfig.tabs) {
        IBox* ibox;
        IIntuition->GetAttr(SPACE_AreaBox, owbwindow->gad_webview, (uint32*)(void*)&ibox);
        owbwindow->offsetx = ibox->Left;
        owbwindow->offsety = ibox->Top;
        owbwindow->webViewWidth = ibox->Width;
        owbwindow->webViewHeight = ibox->Height;
    }
    else {
        owbwindow->offsetx = firstWindow->offsetx;
        owbwindow->offsety = firstWindow->offsety;
        owbwindow->webViewWidth = firstWindow->webViewWidth;
        owbwindow->webViewHeight = firstWindow->webViewHeight;
    }

    if (first || !amigaConfig.tabs) {
        APTR vi = IGadTools->GetVisualInfoA(window->WScreen, NULL);
        if (vi) {
            Menu* menustrip = IGadTools->CreateMenusA(Menus, NULL);
            if (menustrip) {
                if (!IGadTools->LayoutMenus(menustrip, vi,
                                            GTMN_NewLookMenus, TRUE,
                                            TAG_DONE)) {
                    IGadTools->FreeMenus(menustrip);
                    menustrip = NULL;
                }
            }

            if (menustrip)
                 IIntuition->SetMenuStrip(window, menustrip);

            IGadTools->FreeVisualInfo(vi);
        }
    }

    return true;
}

BalWidget* createAmigaWindow(WebView* webView)
{
    if (!webView)
        return 0;

    BalWidget* owbwindow = (BalWidget*)calloc(sizeof (BalWidget), 1);
    if(!owbwindow)
        return 0;

    strcpy(owbwindow->title, amigaConfig.tabs ? "(unnamed)" : "Origyn Web Browser");

    if (!openAmigaWindow(owbwindow, !firstWindow, -1)) {
        free(owbwindow);
        return 0;
    }

    owbwindow->webView = webView;

    if (!amigaConfig.tabs || !firstWindow)
        owbwindow->bookmark = amiga_bookmark_init(owbwindow->window, sharedPort->mp_SigBit, webView);
    else
        owbwindow->bookmark = firstWindow->bookmark;

    if (!firstWindow)
        firstWindow = owbwindow;
    else {
        BalWidget* window = firstWindow;
        while (window->next)
            window = window->next;
        window->next = owbwindow;
    }

    owbwindow->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, owbwindow->webViewWidth, owbwindow->webViewHeight);
    if (!owbwindow->surface) {
        closeAmigaWindow(owbwindow);
        return 0;
    }

    owbwindow->cr = cairo_create(owbwindow->surface);
    if (!owbwindow->cr) {
        closeAmigaWindow(owbwindow);
        return 0;
    }

    IIntuition->GetAttr(AREXX_HostName, rxObject, (uint32*)(void*)&owbwindow->arexxPortName);

    return owbwindow;
}

static void iconifyWindow(BalWidget* owbwindow)
{
    if (amigaConfig.tabs) {
        AppIcon* appicon = IWorkbench->AddAppIcon(0, (uint32)owbwindow,
                                                  "OWB", appIconPort,
                                                  0, icon, TAG_DONE);

        BalWidget* window = firstWindow;
        while (appicon && window) {
            STRPTR text;
            IIntuition->GetAttr(STRINGA_TextVal, window->gad_url, (uint32*)(void*)&text);
            strlcpy(window->url, text, sizeof(window->url));
            IIntuition->GetAttr(STRINGA_TextVal, window->gad_search, (uint32*)(void*)&text);
            strlcpy(window->search, text, sizeof(window->search));
            IIntuition->GetWindowAttrs(window->window,
                                       WA_Left, &window->left,
                                       WA_Top, &window->top,
                                       WA_Width, &window->width,
                                       WA_Height, &window->height,
                                       TAG_DONE);

            window->appicon = appicon;

            if (window->next)
                closeAmigaWindowWithoutSurface(window);
            else {
                BalWidget* oldFirst = firstWindow;
                firstWindow = 0;
                closeAmigaWindowWithoutSurface(window);
                firstWindow = oldFirst;
            }

            window = window->next;
        }
    } else
        if (!owbwindow->appicon) {
            STRPTR text;
            IIntuition->GetAttr(STRINGA_TextVal, owbwindow->gad_url, (uint32*)(void*)&text);
            strlcpy(owbwindow->url, text, sizeof(owbwindow->url));
            IIntuition->GetAttr(STRINGA_TextVal, owbwindow->gad_search, (uint32*)(void*)&text);
            strlcpy(owbwindow->search, text, sizeof(owbwindow->search));
            IIntuition->GetWindowAttrs(owbwindow->window,
                                       WA_Left, &owbwindow->left,
                                       WA_Top, &owbwindow->top,
                                       WA_Width, &owbwindow->width,
                                       WA_Height, &owbwindow->height,
                                       TAG_DONE);

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

    IFFHandle* ih;

    String selectedText;
    if (html) {
       SelectionController* controller = frame->selection();
       if (controller)
           selectedText = controller->toRange()->toHTML();
    }
    else
       selectedText = frame->selectedText();

    CString text = latin1 ? selectedText.latin1() : selectedText.utf8();
    const char* data = text.data();
    size_t len = text.length();

    bool copied = false;

    if (data && len && (ih = IIFFParse->AllocIFF())) {
        if (ClipboardHandle* ch = IIFFParse->OpenClipboard(PRIMARY_CLIP)) {
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

static bool confirmQuit(BalWidget* owbwindow)
{
    bool quit = true;

    if (amigaConfig.confirmQuit) {
        Object* requester = (Object*)RequesterObject,
                                         REQ_CharSet, 4,
                                         REQ_TitleText, "OWB",
                                         REQ_BodyText, "Quit OWB?",
                                     End;
        if (requester) {
            struct Requester dummyRequester;

            if (owbwindow->window) {
                IIntuition->InitRequester(&dummyRequester);
                IIntuition->Request(&dummyRequester, owbwindow->window);
                IIntuition->SetWindowPointer(owbwindow->window, WA_BusyPointer, TRUE, WA_PointerDelay, TRUE, TAG_DONE);
            }

            if (0 == OpenRequester(requester, owbwindow->window))
                quit = false;

            if (owbwindow->window) {
                IIntuition->SetWindowPointer(owbwindow->window, WA_BusyPointer, FALSE, TAG_DONE);
                IIntuition->EndRequest(&dummyRequester, owbwindow->window);
            }

            IIntuition->DisposeObject(requester);
        }
    }

    return quit;
}

static void resizeWebView(BalWidget* owbwindow, WebView* webView, IBox* ibox)
{
    int width = owbwindow->webViewWidth = ibox->Width;
    int height = owbwindow->webViewHeight = ibox->Height;

    Frame* frame = core(webView->mainFrame());
    if (!frame || frame->view()->visibleWidth() != width || frame->view()->visibleHeight() != height) {
        cairo_surface_t* newsurface;
        cairo_t* newcr = 0;

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

static void waitEvent(void)
{
    uint32 signalSet = (1L << sharedPort->mp_SigBit) | sharedTimerSignal | (1L << appIconPort->mp_SigBit) | arexxSignal;

    while (firstWindow) {
        IntuiMessage* im;

        uint32 signals = IExec->Wait(signalSet);

        if (signals & arexxSignal)
            RA_HandleRexx(rxObject);

        while ((im = (IntuiMessage*)IExec->GetMsg(sharedPort))) {
            BalWidget* owbwindow = (BalWidget*)im->IDCMPWindow->UserData;
            if (amigaConfig.tabs) {
                ::Node* node;
                IIntuition->GetAttr(CLICKTAB_CurrentNode, owbwindow->gad_clicktab, (uint32*)(void*)&node);
                IClickTab->GetClickTabNodeAttrs(node, TNA_UserData, &owbwindow, TAG_DONE);
            }

            WebView* webView = 0;
            if (owbwindow)
                webView = owbwindow->webView;

            if (webView)
                switch (im->Class) {
                case IDCMP_CLOSEWINDOW:
                {
                    bool close = true;

                    IExec->ReplyMsg(&im->ExecMessage);

                    if (firstWindow == owbwindow && !owbwindow->next)
                        close = confirmQuit(owbwindow);

                    if (close)
                        closeAmigaWindow(owbwindow);

                    continue;
                }
                break;

                case IDCMP_MOUSEBUTTONS:
                    switch (im->Code & ~IECODE_UP_PREFIX) {
                    case IECODE_LBUTTON:
                    case IECODE_RBUTTON:
                    case IECODE_MBUTTON:
                        if (im->MouseX >= owbwindow->offsetx
                         && im->MouseX <  owbwindow->offsetx + owbwindow->webViewWidth
                         && im->MouseY >= owbwindow->offsety
                         && im->MouseY <  owbwindow->offsety + owbwindow->webViewHeight) {
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

                    BalEventExpose ev = 0;
                    webView->onExpose(ev);

                    im2.Class = IDCMP_MOUSEMOVE;
                    im2.Code = 0;
                    im2.Qualifier = 0;
                    webView->onMouseMotion(im2);
                }
                break;

                case IDCMP_RAWKEY:
                    if (!(im->Code & IECODE_UP_PREFIX)) {
                        IntuiMessage im2 = *im;
                        im2.MouseX -= owbwindow->offsetx;
                        im2.MouseY -= owbwindow->offsety;

                        webView->onKeyDown(im2);
                        im2.Code |= IECODE_UP_PREFIX;
                        webView->onKeyUp(im2);
                    }
                    else
                        fprintf(stderr, "IDCMP_RAWKEY code %02x\n", im->Code);
                break;

                case IDCMP_VANILLAKEY:
                {
                    IntuiMessage im2 = *im;
                    im2.MouseX -= owbwindow->offsetx;
                    im2.MouseY -= owbwindow->offsety;
                    webView->onKeyDown(im2);

                    if (im->Code & 0xFFFFFF00)
                        fprintf(stderr, "IDCMP_VANILLAKEY code %02x\n", im->Code);

                    // fake KeyUp event with impossible code
                    im2.Code |= 0x100;
                    webView->onKeyUp(im2);
                }
                break;

                case IDCMP_NEWSIZE:
                {
                    IIntuition->RefreshGList(owbwindow->gad_toolbar, owbwindow->window, NULL, -1);
                    IBox* ibox;
                    IIntuition->GetAttr(SPACE_AreaBox, owbwindow->gad_webview, (uint32*)(void*)&ibox);
                    if (amigaConfig.tabs) {
                        BalWidget* owbwindow = firstWindow;

                        while (owbwindow) {
                            resizeWebView(owbwindow, owbwindow->webView, ibox);
                            owbwindow = owbwindow->next;
                        }
                    }
                    else
                        resizeWebView(owbwindow, webView, ibox);
                }
                break;

                case IDCMP_IDCMPUPDATE:
                {
                    TagItem* tags = (TagItem*)im->IAddress;
                    if (IUtility->FindTagItem(LAYOUT_RequestRefresh, tags))
                        IIntuition->RefreshWindowFrame(owbwindow->window);
                    else if (IUtility->FindTagItem(LAYOUT_RequestLayout, tags))
                        ILayout->RethinkLayout(owbwindow->gad_toolbar, owbwindow->window, NULL, FALSE);
                    else if (IUtility->FindTagItem(LAYOUT_RelVerify, tags)) {
                        gadgetIDs gid = (gadgetIDs)IUtility->GetTagData(GA_ID, 0, tags);

                        switch (gid) {
                        case GID_Back:
                        {
                            webView->goBack();
                            owbwindow->expose = true;
                            char* url = (char*)webView->mainFrame()->url();
                            if (ILayout->SetPageGadgetAttrs(owbwindow->gad_url, owbwindow->page,
                                                            owbwindow->window, NULL,
                                                            STRINGA_TextVal, url,
                                                            TAG_DONE))
                                ILayout->RefreshPageGadget(owbwindow->gad_url, owbwindow->page, owbwindow->window, NULL);
                            free(url);
                        }
                        break;

                        case GID_Forward:
                        {
                            webView->goForward();
                            owbwindow->expose = true;
                            char* url = (char*)webView->mainFrame()->url();
                            if (ILayout->SetPageGadgetAttrs(owbwindow->gad_url, owbwindow->page,
                                                            owbwindow->window, NULL,
                                                            STRINGA_TextVal, url,
                                                            TAG_DONE))
                                ILayout->RefreshPageGadget(owbwindow->gad_url, owbwindow->page, owbwindow->window, NULL);
                            free(url);
                        }
                        break;

                        case GID_Stop:
                            core(webView->mainFrame())->loader()->stopAllLoaders();
                        break;

                        case GID_Reload:
                            webView->mainFrame()->reload();
                        break;

                        case GID_Home:
                            webView->mainFrame()->loadURL(amigaConfig.homeURL);
                        break;

                        case GID_URL:
                        {
                            Gadget* gad = (Gadget*)IUtility->GetTagData(LAYOUT_RelAddress, 0, tags);
                            uint32 tab = IUtility->GetTagData(LAYOUT_TabVerify, FALSE, tags);
                            if (gad && !tab) {
                                STRPTR url = NULL;
                                IIntuition->GetAttr(STRINGA_TextVal, gad, (uint32*)(void*)&url);
                                if (url && *url) {
                                    char* colon = strchr(url, ':');
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
                            Gadget* gad = (Gadget*)IUtility->GetTagData(LAYOUT_RelAddress, 0, tags);
                            uint32 tab = IUtility->GetTagData(LAYOUT_TabVerify, FALSE, tags);
                            if (gad && !tab) {
                                STRPTR search = NULL;
                                IIntuition->GetAttr(STRINGA_TextVal, gad, (uint32*)(void*)&search);
                                if (search && *search) {
                                    TEXT url[600];
                                    snprintf(url, sizeof(url), amigaConfig.searchURL, search);
                                    webView->mainFrame()->loadURL(url);
                                }
                            }
                        }
                        break;

                        case GID_Bookmark:
                        {
                            if (owbwindow->bookmark) {
                                OWBBookMark* bm = (OWBBookMark*)owbwindow->bookmark;
                                bm->owbwindow = owbwindow->window;
                                bm->webView = webView;
                                amiga_bookmark_open(bm);
                            }
                        }
                        break;

                        case GID_BookmarkAdd:
                        {
                            STRPTR url = NULL;
                            IIntuition->GetAttr(STRINGA_TextVal, owbwindow->gad_url, (uint32*)(void*)&url);
                            if (url && *url && owbwindow->bookmark) {
                                OWBBookMark* bm = (OWBBookMark*)owbwindow->bookmark;
                                bm->owbwindow = owbwindow->window;
                                bm->webView = webView;
                                amiga_bookmark_add(bm, url);
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
                                IIntuition->GetAttr(PGA_Top, owbwindow->gad_vbar, (uint32*)(void*)&top);
                                IntPoint diff(0, top - webView->scrollOffset().y);
                                if (diff.y()) {
                                    webView->scrollBy(diff);
                                    BalEventExpose ev = 0;
                                    webView->onExpose(ev);
                                }
                            }
                        }
                        else if (GID_HBar == gid) {
                            if (owbwindow->gad_hbar) {
                                int left = 0;
                                IIntuition->GetAttr(PGA_Top, owbwindow->gad_hbar, (uint32*)(void*)&left);
                                IntPoint diff(left - webView->scrollOffset().x, 0);
                                if (diff.x()) {
                                    webView->scrollBy(diff);
                                    BalEventExpose ev = 0;
                                    webView->onExpose(ev);
                                }
                            }
                        }
                        else if (GID_ClickTab == gid) {
                            BalWidget* activeTab = owbwindow;
                            ::Node* node;
                            IIntuition->GetAttr(CLICKTAB_CurrentNode, owbwindow->gad_clicktab, (uint32*)(void*)&node);
                            IClickTab->GetClickTabNodeAttrs(node, TNA_UserData, &activeTab, TAG_DONE);

                            Frame* frame = core(activeTab->webView->mainFrame());
                            if (frame->view()) {
                                activeTab->webView->addToDirtyRegion(IntRect(0, 0, frame->view()->visibleWidth(), frame->view()->visibleHeight()));
                                activeTab->expose = true;
                            }
                        }
                        else
                           fprintf(stderr, "gid = %lu, code 0x%04x, qual 0x%04x not handed\n", gid, im->Code, im->Qualifier);
                    }
                }
                break;

                case IDCMP_MENUPICK:
                {
                    MenuItem* item = IIntuition->ItemAddress(owbwindow->window->MenuStrip, im->Code);
                    if (item) {
                        menuIDs id = (menuIDs)(uint32)GTMENUITEM_USERDATA(item);

                        switch (id) {
                        case MID_NewWindow:
                        {
                            WebView* newWebView = WebView::createInstance();
                            BalWidget* newowbwindow = createAmigaWindow(newWebView);

                            if (newowbwindow) {
                                BalRectangle clientRect = {0, 0, newowbwindow->webViewWidth, newowbwindow->webViewHeight};
                                newWebView->initWithFrame(clientRect, "", "");

                                newWebView->setViewWindow(newowbwindow);

                                static char* content = 0;
                                if (!content)
                                    asprintf(&content, "%s%s%s", "<html><head><title>Origyn Web Browser</title><head><body><table width=\"100%%\" height=\"100%%\"><tr align=\"center\"><td align=\"center\"><h1>Origyn Web Browser</h1><h4><a href=\"http://strohmayer.org/owb/\">AmigaOS 4.x version ", &VERSTAG[11], "</a></h4></table></body></html>");

                                newWebView->mainFrame()->loadHTMLString(content ?: "", "about:");
                            }
                            else
                                delete newWebView;
                        }
                        break;

                        case MID_Close:
                        {
                            bool close = true;

                            IExec->ReplyMsg(&im->ExecMessage);

                            if (firstWindow == owbwindow && !owbwindow->next)
                                close = confirmQuit(owbwindow);

                            if (close)
                                closeAmigaWindow(owbwindow);

                            continue;
                        }
                        break;

                        case MID_Quit:
                        {
                            IExec->ReplyMsg(&im->ExecMessage);

                            if (confirmQuit(owbwindow)) {
                                while (firstWindow)
                                    closeAmigaWindow(firstWindow);
                                return;
                            }

                            continue;
                        }
                        break;

                        case MID_ZoomIn:
                            webView->zoomPageIn();
                        break;

                        case MID_ZoomReset:
                            if (webView->canResetPageZoom())
                                webView->resetPageZoom();
                        break;

                        case MID_ZoomOut:
                            webView->zoomPageOut();
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

                        case MID_Source:
                        {
                            WebView* newWebView = WebView::createInstance();
                            BalWidget* newowbwindow = createAmigaWindow(newWebView);

                            if (newowbwindow) {
                                BalRectangle clientRect = {0, 0, newowbwindow->webViewWidth, newowbwindow->webViewHeight};
                                newWebView->initWithFrame(clientRect, "", "");

                                newWebView->setViewWindow(newowbwindow);

                                newWebView->mainFrame()->loadURL("about:blank");

                                newWebView->setInViewSourceMode(true);

                                Frame* frame = core(newWebView->mainFrame());
                                if (frame) {
                                    String mimeType = frame->loader()->responseMIMEType();

                                    if (!mimeType)
                                        mimeType = "text/html";


                                    KURL baseKURL(frame->loader()->url());
                                    KURL failingKURL("");

                                    ResourceRequest request(baseKURL);
                                    SubstituteData substituteData(webView->mainFrame()->dataSource()->data(), mimeType, frame->loader()->encoding(), failingKURL);

                                    frame->loader()->load(request, substituteData);
                                }

                                char* url = (char*)webView->mainFrame()->url();
                                if (ILayout->SetPageGadgetAttrs(newowbwindow->gad_url, newowbwindow->page,
                                                                newowbwindow->window, NULL,
                                                                STRINGA_TextVal, url,
                                                                TAG_DONE))
                                    ILayout->RefreshPageGadget(newowbwindow->gad_url, newowbwindow->page, newowbwindow->window, NULL);
                                free(url);
                            }
                            else
                                delete newWebView;
                        }
                        break;

                        case MID_BookMark:
                        {
                            TEXT command[1024];
                            TEXT pubScreenName[MAXPUBSCREENNAME+1];
                            BPTR progdir = IDOS->GetProgramDir();

                            pubScreenName[0] = 0;
                            if (owbwindow->window)
                                IIntuition->GetScreenAttr(owbwindow->window->WScreen, SA_PubName, pubScreenName, MAXPUBSCREENNAME+1);
                            IUtility->SNPrintf(command, sizeof(command), amigaConfig.bookMarkCommand, owbwindow->arexxPortName, pubScreenName);
                            IDOS->SystemTags(command,
                                             SYS_Asynch, TRUE,
                                             SYS_Input, IDOS->DupFileHandle(IDOS->Input()),
                                             SYS_Output, IDOS->DupFileHandle(IDOS->Output()),
                                             SYS_Error, IDOS->DupFileHandle(IDOS->ErrorOutput()),
                                             NP_CloseError, TRUE,
                                             NP_CurrentDir, IDOS->DupLock(IDOS->GetProgramDir()),
                                             NP_Name, "OWB Bookmark");
                        }
                        break;
                        }
                    }
                }
                break;

                case IDCMP_MENUVERIFY:
                    if (MENUHOT == im->Code
                     && im->MouseX >= owbwindow->offsetx
                     && im->MouseX <  owbwindow->offsetx + owbwindow->webViewWidth
                     && im->MouseY >= owbwindow->offsety
                     && im->MouseY <  owbwindow->offsety + owbwindow->webViewHeight) {
                         IntuiMessage im2 = *im;
                         im->Code = MENUCANCEL;
                         IExec->ReplyMsg(&im->ExecMessage);
                         im2.Class = IDCMP_MOUSEBUTTONS;
                         im2.Code = IECODE_RBUTTON;
                         im2.MouseX -= owbwindow->offsetx;
                         im2.MouseY -= owbwindow->offsety;
                         webView->onMouseButtonDown(im2);
                         continue;
                     }
                break;

                case IDCMP_GADGETUP:
                    if (GID_Iconify == ((Gadget*)im->IAddress)->GadgetID) {
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

        BalWidget* owbwindow = firstWindow;

        if (owbwindow && (signals & sharedTimerSignal))
            owbwindow->webView->fireWebKitEvents();

        while (owbwindow) {
            WebView* webView = owbwindow->webView;

            if (owbwindow->bookmark)
                amiga_bookmark_eventloop((OWBBookMark*)owbwindow->bookmark);

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
                    if (ILayout->SetPageGadgetAttrs(owbwindow->gad_back, owbwindow->page,
                                                    owbwindow->window, NULL,
                                                    GA_Disabled, newdisabled,
                                                    TAG_DONE))
                        ILayout->RefreshPageGadget(owbwindow->gad_back, owbwindow->page, owbwindow->window, NULL);

                IIntuition->GetAttr(GA_Disabled, owbwindow->gad_forward, &olddisabled);
                olddisabled = !!olddisabled;
                newdisabled = webView->backForwardList()->forwardItem() ? FALSE : TRUE;
                if (olddisabled != newdisabled)
                    if (ILayout->SetPageGadgetAttrs(owbwindow->gad_forward, owbwindow->page,
                                                    owbwindow->window, NULL,
                                                    GA_Disabled, newdisabled,
                                                    TAG_DONE))
                        ILayout->RefreshPageGadget(owbwindow->gad_forward, owbwindow->page, owbwindow->window, NULL);

                IIntuition->GetAttr(GA_Disabled, owbwindow->gad_stop, &olddisabled);
                olddisabled = !!olddisabled;
                newdisabled = webView->isLoading() ? FALSE : TRUE;
                if (olddisabled != newdisabled) {
                    if (ILayout->SetPageGadgetAttrs(owbwindow->gad_stop, owbwindow->page,
                                                    owbwindow->window, NULL,
                                                    GA_Disabled, newdisabled,
                                                    TAG_DONE))
                        ILayout->RefreshPageGadget(owbwindow->gad_stop, owbwindow->page, owbwindow->window, NULL);
                }

                BalWidget* activeTab = owbwindow;
                if (amigaConfig.tabs) {
                    ::Node* node;
                    IIntuition->GetAttr(CLICKTAB_CurrentNode, owbwindow->gad_clicktab, (uint32*)(void*)&node);
                    IClickTab->GetClickTabNodeAttrs(node, TNA_UserData, &activeTab, TAG_DONE);
                }

                if (activeTab == owbwindow) {
                    if (owbwindow->gad_vbar) {
                        uint32 top, total, visible;
                        IIntuition->GetAttrs(owbwindow->gad_vbar,
                                             PGA_Top, &top,
                                             PGA_Total, &total,
                                             PGA_Visible, &visible,
                                             TAG_DONE);
                        uint32 newtop = core(webView->mainFrame())->view()->visibleContentRect().y();
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
                        uint32 newtop = core(webView->mainFrame())->view()->visibleContentRect().x();
                        uint32 newtotal = core(webView->mainFrame())->view()->contentsWidth();
                        uint32 newvisible = core(webView->mainFrame())->view()->visibleWidth();
                        if (top != newtop || total != newtotal || visible != newvisible)
                            IIntuition->RefreshSetGadgetAttrs(owbwindow->gad_hbar, owbwindow->window, NULL,
                                                              PGA_Top, newtop,
                                                              PGA_Total, newtotal,
                                                              PGA_Visible, newvisible,
                                                              TAG_DONE);
                    }

                    if (webView->canResetPageZoom())
                        IIntuition->OnMenu(owbwindow->window, FULLMENUNUM_ZoomReset);
                    else
                        IIntuition->OffMenu(owbwindow->window, FULLMENUNUM_ZoomReset);

                    if (webView->inViewSourceMode())
                        IIntuition->OffMenu(owbwindow->window, FULLMENUNUM_Source);
                    else
                        IIntuition->OnMenu(owbwindow->window, FULLMENUNUM_Source);
                }
            }

            owbwindow = owbwindow->next;
        }

        while (AppMessage* am = (AppMessage*)IExec->GetMsg(appIconPort)) {
            if (0 == am->am_NumArgs) {
                if (amigaConfig.tabs) {
                    BalWidget* owbwindow = firstWindow;
                    AppIcon* appicon = firstWindow->appicon;
                    int32 nodeNumber = 0;
                    while (owbwindow) {
                        WebView* webView = owbwindow->webView;

                        if (webView && owbwindow->appicon && openAmigaWindow(owbwindow, owbwindow == firstWindow, nodeNumber)) {
                            owbwindow->appicon = 0;

                            Frame* frame = core(webView->mainFrame());
                            if (frame->view()) {
                                webView->addToDirtyRegion(IntRect(0, 0, frame->view()->visibleWidth(), frame->view()->visibleHeight()));
                                owbwindow->expose = true;
                            }
                        }

                        nodeNumber++;
                        owbwindow = owbwindow->next;
                    }

                    IWorkbench->RemoveAppIcon(appicon);
                }
                else {
                    BalWidget* owbwindow = (BalWidget*)am->am_UserData;
                    WebView* webView = 0;

                    if (owbwindow)
                        webView = owbwindow->webView;

                    if (webView && owbwindow->appicon && openAmigaWindow(owbwindow, false, -1)) {
                        IWorkbench->RemoveAppIcon(owbwindow->appicon);
                        owbwindow->appicon = 0;

                        Frame* frame = core(webView->mainFrame());
                        if (frame->view()) {
                            webView->addToDirtyRegion(IntRect(0, 0, frame->view()->visibleWidth(), frame->view()->visibleHeight()));
                            owbwindow->expose = true;
                        }
                    }
                }
            }

            IExec->ReplyMsg((Message*)am);
        }
    }
}

extern "C" {
void amiga_bookmark_showpage(struct OWBBookMark* bm)
{
    WebView* webView = (WebView*)bm->webView;
    BalWidget* owbwindow = firstWindow;

    while (owbwindow) {
       if (owbwindow->webView == webView) {
           webView->mainFrame()->loadURL(bm->urllink);
           break;
       }

       owbwindow = owbwindow->next;
    }
}
}

static struct Locale* locale;
static uint32* unicode_map;

uint32 amigaToUnicodeChar(uint32 c)
{
    if (unicode_map && c <= 255 && unicode_map[c] <= 65535)
        return unicode_map[c];

    return c;
}

char* utf8ToAmiga(const char* utf8)
{
    String string = String::fromUTF8(utf8);

    if (string.isEmpty())
        return strdup("");

    int len = string.length();
    char* result = (char*)calloc(len + 1, 1);
    if (!result)
        return 0;

    for (int i = 0 ; i < len ; i++) {
        uint32 c = string[i];
        if (c < 128)
            result[i] = c;
        else {
            result[i] = '?';

            if (unicode_map)
                for (int j = 128 ; j < 256 ; j++)
                    if (unicode_map[j] == c) {
                        result[i] = j;
                        break;
                    }
        }
    }

    return result;
}

int main (int argc, char* argv[])
{
    if (!initTimer())
       return RETURN_ERROR;

    locale = ILocale->OpenLocale(NULL);
    if (locale)
        unicode_map = (uint32*)IDiskfont->ObtainCharsetInfo(DFCS_NUMBER, locale->loc_CodeSet, DFCS_MAPTABLE);

    initAmigaConfig();
    initARexx();

    sharedPort = (MsgPort*)IExec->AllocSysObjectTags(ASOT_PORT, TAG_DONE);
    if (sharedPort) {
        appIconPort = (MsgPort*)IExec->AllocSysObjectTags(ASOT_PORT, TAG_DONE);
        if (appIconPort) {
            WebView* webView = WebView::createInstance();
            BalWidget* owbwindow = createAmigaWindow(webView);

            if (owbwindow) {
                BalRectangle clientRect = {0, 0, owbwindow->webViewWidth, owbwindow->webViewHeight};
                webView->initWithFrame(clientRect, "", "");

                webView->setViewWindow(owbwindow);

                webView->mainFrame()->loadURL(argc > 1 ? argv[1] : startPage ?: amigaConfig.homeURL);

                waitEvent();
            }
            else
                delete webView;

            IExec->FreeSysObject(ASOT_PORT, appIconPort);
        }
        IExec->FreeSysObject(ASOT_PORT, sharedPort);
    }

    cleanupARexx();
    cleanupTimer();

    if (locale)
        ILocale->CloseLocale(locale);

    if (icon)
        IIcon->FreeDiskObject(icon);

    return 0;
}

