/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2007 Pleyo.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WidgetClient_h
#define WidgetClient_h

namespace WebCore {

    class Element;

}
using WebCore::Element;

namespace BAL {

    class Widget;

    class WidgetClient {
    public:
        /**
        * WidgetClient destructor
        */
        virtual ~WidgetClient() { }

        /**
        * scroll to visible widget
        */
        virtual void scrollToVisible(Widget*) { }

        /**
        * set focus in 
        */
        virtual void focusIn(Widget*) { }
        /**
        * set focus out
        */
        virtual void focusOut(Widget*) { }

        /**
        * set clicked on widget
        */
        virtual void clicked(Widget*) { }
        /**
        * change value
        */
        virtual void valueChanged(Widget*) { }
        /**
        * change selection
        */
        virtual void selectionChanged(Widget*) { }

        virtual void returnPressed(Widget*) { }
        virtual void performSearch(Widget*) { }

        /**
        * get element
        */
        virtual Element* element(Widget*) { return 0; }
        /**
        * send consumed mouse up
        */
        virtual void sendConsumedMouseUp(Widget*) { }
        /**
        * test if it's visible
        */
        virtual bool isVisible(Widget*) { return false; }
    };

}

using BAL::WidgetClient;

#endif // WidgetClient_h
