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

#include "AccessibilityController.h"

#include "AccessibilityUIElement.h"
#include "DumpRenderTree.h"
#include <WebKit.h>

AccessibilityController::AccessibilityController()
{
}

AccessibilityController::~AccessibilityController()
{
}

AccessibilityUIElement AccessibilityController::focusedElement()
{
    /*COMPtr<IAccessible> rootAccessible = rootElement().platformUIElement();

    VARIANT vFocus;
    if (FAILED(rootAccessible->get_accFocus(&vFocus)))
        return 0;

    if (V_VT(&vFocus) == VT_I4) {
        ASSERT(V_I4(&vFocus) == CHILDID_SELF);
        // The root accessible object is the focused object.
        return rootAccessible;
    }

    ASSERT(V_VT(&vFocus) == VT_DISPATCH);
    // We have an IDispatch; query for IAccessible.
    return COMPtr<IAccessible>(Query, V_DISPATCH(&vFocus));*/
    return 0;
}

AccessibilityUIElement AccessibilityController::rootElement()
{
    /*COMPtr<IWebView> view;
    if (FAILED(frame->webView(&view)))
        return 0;

    COMPtr<IWebViewPrivate> viewPrivate(Query, view);
    if (!viewPrivate)
        return 0;

    HWND webViewWindow;
    if (FAILED(viewPrivate->viewWindow((OLE_HANDLE*)&webViewWindow)))
        return 0;

    // Get the root accessible object by querying for the accessible object for the
    // WebView's window.
    COMPtr<IAccessible> rootAccessible;
    if (FAILED(AccessibleObjectFromWindow(webViewWindow, static_cast<DWORD>(OBJID_CLIENT), __uuidof(IAccessible), reinterpret_cast<void**>(&rootAccessible))))
        return 0;

    return rootAccessible;*/
    return 0;
}

void AccessibilityController::setLogFocusEvents(bool /*logFocusEvents*/)
{
}

void AccessibilityController::setLogScrollingStartEvents(bool /*logScrollingStartEvents*/)
{
}

