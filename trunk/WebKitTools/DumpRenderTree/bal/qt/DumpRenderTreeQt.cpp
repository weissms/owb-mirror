/*
 * Copyright (C) 2009 Pleyo.  All rights reserved.
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


#include <WebKit.h>
#include <DumpRenderTree.h>

// Choose some default values.
const unsigned int maxViewWidth = 800;
const unsigned int maxViewHeight = 600;
unsigned int waitToDumpWatchdog = 0;

BalRectangle clientRect(bool isSVGW3CTest)
{
    return QRect(0, 0, 0, 0);
}

void init(int argc, char *argv[])
{
}

BalWidget* createWindow(WebView *webView, BalRectangle rect)
{
    return 0;
}

void startEventLoop(BalWidget *view)
{
    done = false;
}

void stopEventLoop()
{
    done = true;
}

static bool waitToDumpWatchdogFired(void*)
{
    const char* message = "FAIL: Timed out waiting for notifyDone to be called\n";
    fprintf(stderr, "%s", message);
    fprintf(stdout, "%s", message);
    waitToDumpWatchdog = 0;
    dump();
    return false;
}


void addTimetoDump(int timeoutSeconds)
{
}

void removeTimer()
{
}
