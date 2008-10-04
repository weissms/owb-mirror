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
#ifndef ScrollbarClient_h
#define ScrollbarClient_h
/**
 *  @file  ScrollbarClient.t
 *  ScrollbarClient description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include "IntRect.h"

namespace WKAL {

class Scrollbar;

class ScrollbarClient : public WKALBase {
public:
    /**
     * Desturctor for ScrollbarClient
     * @code
     * @endcode
     */
    virtual ~ScrollbarClient() ;
    /**
     * Warn a ScrollBarClient that a value has changed for a scrollbar.
     * @param[in] : a pointer to the scrollbar.
     * @code
     * @endcode
     */
    virtual void valueChanged(Scrollbar*) = 0;

    // Used to obtain a window clip rect.

    /**
     *  invalidateScrollbarRect
     */
    virtual void invalidateScrollbarRect(Scrollbar*, const IntRect&) = 0;

    /**
     * To check wether a scrollBarClient  is active or not.
     * @param[out] : a boolean.
     * @code
     * @endcode
     */
    virtual bool isActive() const = 0;
};

}
#endif


