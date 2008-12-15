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

#ifndef WebKitTypes_h
#define WebKitTypes_h

#include <WebKitDefines.h>

#if PLATFORM(GTK)

#include <gtk/gtk.h>

typedef GtkWidget BalWidget;
typedef struct _BalEventExpose{} BalEventExpose;
typedef struct _BalResizeEvent{} BalResizeEvent;
typedef struct _BalQuitEvent{} BalQuitEvent;
typedef struct _BalUserEvent{} BalUserEvent;
typedef struct _BalEventKey{} BalEventKey;
typedef struct _BalEventButton{} BalEventButton;
typedef struct _BalEventMotion{} BalEventMotion;
typedef struct _BalEventScroll{} BalEventScroll;

typedef GdkPoint BalPoint;
typedef GdkRectangle BalRectangle;

#elif PLATFORM(SDL)

#include <SDL.h>

typedef SDL_Surface BalWidget;

typedef struct SDL_ExposeEvent BalEventExpose;
typedef struct SDL_ResizeEvent BalResizeEvent;
typedef struct SDL_QuitEvent BalQuitEvent;
typedef struct SDL_UserEvent BalUserEvent;
typedef struct SDL_KeyboardEvent BalEventKey;
typedef struct SDL_MouseButtonEvent BalEventButton;
typedef struct SDL_MouseMotionEvent BalEventMotion;
typedef struct SDL_MouseButtonEvent BalEventScroll;

typedef struct _BalPoint{
    int x;
    int y;
} BalPoint;
typedef SDL_Rect BalRectangle;

#endif

typedef enum WebCacheModel {
    WebCacheModelDocumentViewer = 0,
    WebCacheModelDocumentBrowser = 1,
    WebCacheModelPrimaryWebBrowser = 2
} WebCacheModel;

#endif
