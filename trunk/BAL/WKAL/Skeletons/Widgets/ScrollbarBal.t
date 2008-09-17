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
#ifndef ScrollbarGtk_h
#define ScrollbarGtk_h
/**
 *  @file  ScrollbarBal.t
 *  ScrollbarGtk description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include "Scrollbar.h"
#include <wtf/PassRefPtr.h>

typedef struct _BalAdjustment BalAdjustment;

namespace WKAL {

class ScrollbarGtk : public WKALBase, public Scrollbar {
public:
    friend class Scrollbar;

    /**
     * Destroy the ~ScrollbarGtk.
     * @code
     * @endcode
     */
    virtual ~ScrollbarGtk();

    /**
     * Set the frame geometry.
     * @param[in] : a rectangle.
     * @code
     * @endcode
     */
    virtual void setFrameGeometry(const IntRect&);
    
    /**
     * Check wether it should handle mouse move event.
     * @param[in] : a mouse event.
     * @param[out] : a boolean.
     * @code
     * @endcode
     */
    virtual bool handleMouseMoveEvent(const PlatformMouseEvent&) ;
    /**
     * Check wether it should handle mouse out event.
     * @param[in] : a mouse event.
     * @param[out] : a boolean.
     * @code
     * @endcode
     */
    virtual bool handleMouseOutEvent(const PlatformMouseEvent&) ;
    /**
     * Check wether it should handle mouse press event.
     * @param[in] : a mouse event.
     * @param[out] : a boolean.
     * @code
     * @endcode
     */
    virtual bool handleMousePressEvent(const PlatformMouseEvent&) ;
    /**
     * Check wether it should  handle mouse release event.
     * @param[in] : a mouse event.
     * @param[out] : a boolean.
     * @code
     * @endcode
     */
    virtual bool handleMouseReleaseEvent(const PlatformMouseEvent&) ;

protected:
    /**
     * contructor for ScrollbarGtk.
     * @param[in] : a pointer to a ScrollbarClient.
     * @param[in] : a ScrollbarOrientation.
     * @param[in] : a ScrollbarControlSize.
     * @code
     * @endcode
     */
    ScrollbarGtk(ScrollbarClient*, ScrollbarOrientation, ScrollbarControlSize);

    /**
     * Update the thumb position.
     * @code
     * @endcode
     */
    virtual void updateThumbPosition();
    /**
     * Update the thumb proportion.
     * @code
     * @endcode
     */
    virtual void updateThumbProportion();
    /**
     * geometryChanged.
     * @code
     * @endcode
     */
    virtual void geometryChanged();
    
private:
    /**
     * balValueChanged.
     * @param[in] : a pointer to a BalAdjustment.
     * @param[in] : a pointer to a ScrollbarGtk.
     * @code
     * @endcode
     */
    static void balValueChanged(BalAdjustment*, ScrollbarGtk*);
    BalAdjustment* m_adjustment;
};

}

#endif // ScrollbarGtk_h




