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
/**
 *  @file  PlatformScrollBar.t
 *  PlatformScrollBar description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include "ScrollBar.h"
#include "Widget.h"
#include <wtf/PassRefPtr.h>

typedef struct _BalAdjustment BalAdjustment;

namespace WKAL {

class PlatformScrollbar : public WKALBase, public Widget, public Scrollbar {
public:
    /**
     * create platform scrollbar
     * @param[in] : scrollbar client
     * @param[in] : scrollbar orientation
     * @param[in] : scrollbar control size
     * @param[out] : platform scrollbar
     * @code
     * RefPtr<PlatformScrollbar> p = PlatformScrollbar::create(c, o, s);
     * @endcode
     */
    static PassRefPtr<PlatformScrollbar> create(ScrollbarClient* client, ScrollbarOrientation orientation, ScrollbarControlSize size)



    /**
     *  PlatformScrollbar description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PlatformScrollbar(ScrollbarClient*, ScrollbarOrientation, ScrollbarControlSize)

protect


    /**
     *  PlatformScrollbar description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PlatformScrollbar(ScrollbarClient*, ScrollbarOrientation, ScrollbarControlSize)

namespace WKAL {

class PlatformScrollbar : public WKALBase, public Widget, public Scrollbar {
public:
    /**
     * create platform scrollbar
     * @param[in] : scrollbar client
     * @param[in] : scrollbar orientation
     * @param[in] : scrollbar control size
     * @param[out] : platform scrollbar
     * @code
     * RefPtr<PlatformScrollbar> p = PlatformScrollbar::create(c, o, s);
     * @endcode
     */
    static PassRefPtr<PlatformScrollbar> create(ScrollbarClient* client, ScrollbarOrientation orientation, ScrollbarControlSize size);

    /**
     * PlatformScrollbar destructor
     * @code
     * delete p;
     * @endcode
     */
    virtual ~PlatformScrollbar();
    /**
     *  PlatformScrollbar description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PlatformScrollbar(ScrollbarClient*, ScrollbarOrientation, ScrollbarControlSize);
    /**
     *  updateThumbPosition description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual void updateThumbPosition();
    /**
     *  updateThumbProportion description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual void updateThumbProportion();
    /**
     *  geometryChanged description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    virtual void geometryChanged();
    /**
     *  balValueChanged description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    static void balValueChanged(BalAdjustment*, PlatformScrollbar*);
    
    
    
    
    



    /**
     *  PlatformScrollbar description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    PlatformScrollbar(ScrollbarClient*, ScrollbarOrientation, ScrollbarControlSize)

protecte

    /**
     * update Thumb Position
     */
    virtual void updateThumbPosition(

private:
    /**
     * balValueChanged
     */
    static void balValueChanged(BalAdjustment*, PlatformScrollbar*);
    BalAdjustment* m_adjustment;
};

}

#endif // P