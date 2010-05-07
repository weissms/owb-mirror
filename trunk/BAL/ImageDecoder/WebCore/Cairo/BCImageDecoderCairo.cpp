/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
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

#include "config.h"
#include "ImageDecoder.h"

#include <cairo.h>
#include <directfb.h>
#include <cairo-directfb.h>

namespace WebCore {

NativeImagePtr RGBA32Buffer::asNewNativeImage() const
{
    cairo_surface_t *src = cairo_image_surface_create_for_data(reinterpret_cast<unsigned char*>(const_cast<PixelData*>(m_bytes.data())),
                                                               CAIRO_FORMAT_ARGB32,
                                                               width(),
                                                               height(),
                                                               width() * sizeof(PixelData));
                                                               
#if PLATFORM(CAIRO_DIRECTFB)
    IDirectFB *dfb = NULL;
    DFBCHECK(DirectFBCreate(&dfb));

    DFBSurfaceDescription dsc;
    dsc.flags       = (DFBSurfaceDescriptionFlags)(DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
    dsc.caps        = DSCAPS_VIDEOONLY;
    dsc.width       = width();
    dsc.height      = height();
    dsc.pixelformat = DSPF_ARGB;    // DFB equivalent for CAIRO_FORMAT_ARGB32

    IDirectFBSurface* dfbsurface = NULL;
    DFBCHECK(dfb->CreateSurface(dfb, &dsc, &dfbsurface));
    cairo_surface_t *dest = cairo_directfb_surface_create(dfb, dfbsurface);

    // Copy the image data back to the native Cairo-DirectFB backend surface
    // in order to leverage the backend hardware accelerations
    cairo_t* cr = cairo_create(dest);
    cairo_set_source_surface(cr, src, 0, 0);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    cairo_destroy(cr);
    cairo_surface_destroy(src);
    
    return dest;
#else
    return src;
#endif //PLATFORM(CAIRO_DIRECTFB)
}

} // namespace WebCore
