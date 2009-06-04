/*
 * Copyright (C) 2008 Joerg Strohmayer.
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
#include "PlatformScreen.h"
#include "Assertions.h"
#include "FrameView.h"
#include "HostWindow.h"
#include "Widget.h"
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/Picasso96API.h>

namespace WebCore {

int screenDepth(Widget* widget)
{
    int depth = 32;
    if (widget->isFrameView()) {
        HostWindow *hostWindow = static_cast<FrameView*>(widget)->hostWindow();
        BalWidget *balWidget = hostWindow ? hostWindow->platformWindow() : 0;
        if (balWidget) {
            Window *window = balWidget->window;
            if (window)
                depth = IP96->p96GetBitMapAttr(window->WScreen->RastPort.BitMap, P96BMA_BITSPERPIXEL);
        }
    }

    return depth;
}

int screenDepthPerComponent(Widget* widget)
{
    int depth = 8;
    if (widget->isFrameView()) {
        HostWindow *hostWindow = static_cast<FrameView*>(widget)->hostWindow();
        BalWidget *balWidget = hostWindow ? hostWindow->platformWindow() : 0;
        if (balWidget) {
            uint32 id = INVALID_ID;
            struct DisplayInfo displayInfo;

            Window *window = balWidget->window;
            if (window)
                IIntuition->GetScreenAttrs(window->WScreen, SA_DisplayID, &id, TAG_DONE);

            if (INVALID_ID != id
             && IGraphics->GetDisplayInfoData(NULL, &displayInfo, sizeof(displayInfo), DTAG_DISP, id) >= 48)
                depth = (displayInfo.RedBits + displayInfo.GreenBits + displayInfo.BlueBits) / 3;
        }
    }

    return depth;
}

bool screenIsMonochrome(Widget* widget)
{
    return screenDepth(widget) < 2;
}

FloatRect screenRect(Widget* widget)
{
    int x = 0, y = 0, width = 800, height = 600;

    if (widget->isFrameView()) {
        HostWindow *hostWindow = static_cast<FrameView*>(widget)->hostWindow();
        BalWidget *balWidget = hostWindow ? hostWindow->platformWindow() : 0;
        if (balWidget) {
            Window *window = balWidget->window;
            if (window)
                IIntuition->GetScreenAttrs(window->WScreen,
                                           SA_Left,   &x,
                                           SA_Top,    &y,
                                           SA_Width,  &width,
                                           SA_Height, &height,
                                           TAG_DONE);
        }
    }

    return FloatRect(x, y, width, height);
}

FloatRect screenAvailableRect(Widget* widget)
{
    return screenRect(widget);
}

} // namespace WebCore
