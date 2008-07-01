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


#ifndef PlatformScrollBar_h
#define PlatformScrollBar_h

#include "Widget.h"
#include "ScrollBar.h"
#include "BALBase.h"
#include "Timer.h"
#include <wtf/PassRefPtr.h>

namespace WKAL {

enum ScrollbarPart { NoPart, BackButtonPart, BackTrackPart, ThumbPart, ForwardTrackPart, ForwardButtonPart };

class PlatformScrollbar : public Widget, public Scrollbar {
public:
    static PassRefPtr<PlatformScrollbar> create(ScrollbarClient* client, ScrollbarOrientation orientation, ScrollbarControlSize size)
    {
        return adoptRef(new PlatformScrollbar(client, orientation, size));
    }
    virtual ~PlatformScrollbar();

    virtual bool isWidget() const { return true; }

    virtual int width() const;
    virtual int height() const;
    virtual void setRect(const IntRect&);
    virtual void setEnabled(bool);
    virtual void paint(GraphicsContext*, const IntRect& damageRect);

    static int horizontalScrollbarHeight();
    static int verticalScrollbarWidth();

    virtual bool handleMouseMoveEvent(const PlatformMouseEvent&);
    virtual bool handleMouseOutEvent(const PlatformMouseEvent&);
    virtual bool handleMousePressEvent(const PlatformMouseEvent&);
    virtual bool handleMouseReleaseEvent(const PlatformMouseEvent&);

    void autoscrollTimerFired(Timer<PlatformScrollbar>*);

protected:
    PlatformScrollbar(ScrollbarClient*, ScrollbarOrientation, ScrollbarControlSize);
    virtual void updateThumbPosition();
    virtual void updateThumbProportion();
    virtual void geometryChanged();

private:
    IntRect trackRect() const;
    bool hasButtons() const;
    void paintTrack(GraphicsContext* context, const IntRect& rect, bool start, const IntRect& damageRect) const;
    void paintButton(GraphicsContext* context, const IntRect& rect, bool start, const IntRect& damageRect) const;
    void paintThumb(GraphicsContext* context, const IntRect& rect, const IntRect& damageRect) const;
    IntRect forwardButtonRect() const;
    IntRect backButtonRect() const;
    bool hasThumb() const;
    void splitTrack(const IntRect& trackRect, IntRect& beforeThumbRect, IntRect& thumbRect, IntRect& afterThumbRect) const;
    int thumbLength() const;
    int thumbPosition() const;
    int trackLength() const;
    ScrollbarPart hitTest(const PlatformMouseEvent&);
    void invalidatePart(ScrollbarPart);
    void autoscrollPressedPart(double delay);
    ScrollDirection pressedPartScrollDirection();
    ScrollGranularity pressedPartScrollGranularity();
    bool thumbUnderMouse();
    IntRect thumbRect() const;
    void startTimerIfNeeded(double delay);
    void stopTimerIfNeeded();
    static void balValueChanged(BalAdjustment*, PlatformScrollbar*);
    
    BalAdjustment* m_adjustment;
    ScrollbarPart m_hoveredPart;
    ScrollbarPart m_pressedPart;
    int m_pressedPos;
    Timer<PlatformScrollbar> m_scrollTimer;
};

}

#endif // PlatformScrollBar_h
