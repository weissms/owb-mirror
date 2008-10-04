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
#ifndef HostWindow_h
#define HostWindow_h
/**
 *  @file  HostWindow.t
 *  HostWindow description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include <wtf/Noncopyable.h>
#include "IntRect.h"

namespace WKAL {

class IntPoint;
class IntRect;

class HostWindow : public WKALBase, Noncopyable {
public:
    /**
     *  HostWindow constructor
     */
    HostWindow() ;

    /**
     * ~HostWindow destructor
     */
    virtual ~HostWindow() ;

    // The repaint method asks the host window to repaint a rect in the window's coordinate space.  The
    // contentChanged boolean indicates whether or not the Web page content actually changed (or if a repaint
    // of unchanged content is being requested).
    /**
     *  repaint 
     */
    virtual void repaint(const IntRect&, bool contentChanged, bool immediate = false, bool repaintContentOnly = false) = 0;

    /**
     *  scroll 
     */
    virtual void scroll(const IntSize& scrollDelta, const IntRect& rectToScroll, const IntRect& clipRect) = 0;

    // The paint method just causes a synchronous update of the window to happen for platforms that need it (Windows).
    /**
     *  paint 
     */
    void paint() ;
    
    // Methods for doing coordinate conversions to and from screen coordinates.
    /**
     *  screenToWindow 
     */
    virtual IntPoint screenToWindow(const IntPoint&) const = 0;

    /**
     *  windowToScreen 
     */
    virtual IntRect windowToScreen(const IntRect&) const = 0;
};

} // namespace WKAL

#endif // HostWindow_h




