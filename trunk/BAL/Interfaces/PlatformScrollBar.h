/*
 * Copyright (C) 2004, 2006 Apple Computer, Inc.  All rights reserved.
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

#ifndef PlatformScrollBar_h
#define PlatformScrollBar_h

#include "Widget.h"
#include "ScrollBar.h"

namespace WebCore {

class PlatformScrollbar : public Widget, public Scrollbar {
public:
    /**
    * constructor
    */
    PlatformScrollbar(ScrollbarClient*, ScrollbarOrientation, ScrollbarControlSize);
    /**
    * destructor
    */
    virtual ~PlatformScrollbar();

    /**
    * test if the platformScrollbar is a widget
    */
    virtual bool isWidget() const { return true; }

    /**
    * return width
    */
    virtual int width() const;
    /**
    * return height
    */
    virtual int height() const;
    /**
    * set rect
    */
    virtual void setRect(const IntRect&);
    /**
    * set if the platformScrollbar is enabled
    */
    virtual void setEnabled(bool);
    /**
    * paint the platformScrollbar
    */
    virtual void paint(GraphicsContext*, const IntRect& damageRect);

    /**
    * return the platformScrollbar height
    */
    static int horizontalScrollbarHeight() { return 15; }
    /**
    * return the platformScrollbar width
    */
    static int verticalScrollbarWidth() { return 15; }

protected:
    /**
    * update thumb position
    */
    virtual void updateThumbPosition();
    /**
    * update thumb proportion
    */
    virtual void updateThumbProportion();
    /**
    * set geometry changed
    */
    virtual void geometryChanged();

private:
};

}

#endif // PlatformScrollBar_h
