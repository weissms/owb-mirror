/*
 * Copyright (C) 2007 Pleyo.  All rights reserved.
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
#include "config.h"
#include "BTLogHelper.h"
#include "RGBA32GraphicsContext.h"
#include <assert.h>

namespace BAL {

BIGraphicsContext* createRGBA32GraphicsContext(unsigned width, unsigned height) {
    cairo_surface_t* surface = cairo_image_surface_create(
            CAIRO_FORMAT_ARGB32, width, height);
    assert(CAIRO_STATUS_SUCCESS == cairo_surface_status(surface));
    cairo_t* platformContext = cairo_create(surface);
    assert(CAIRO_STATUS_SUCCESS == cairo_status(platformContext));
    return reinterpret_cast<BIGraphicsContext*>(
            static_cast<BCGraphicsContext*>(
            new RGBA32GraphicsContext(platformContext)));
}

void deleteRGBA32GraphicsContext(BIGraphicsContext* context) {
    delete context;
}

RGBA32GraphicsContext::RGBA32GraphicsContext(PlatformGraphicsContext* platformContext) :
        BCGraphicsContext(platformContext) {
}

RGBA32GraphicsContext::~RGBA32GraphicsContext() {
}

RGBA32Array* RGBA32GraphicsContext::getRGBA32Array() {
    cairo_t* cr = platformContext();
    cairo_status_t crStatus = cairo_status(cr);
    if (CAIRO_STATUS_SUCCESS != crStatus) {
        logml(MODULE_GRAPHICS, LEVEL_WARNING, make_message("crStatus : %s", cairo_status_to_string(crStatus)));
        return 0;
    }

    m_surface = cairo_get_target(cr);
    cairo_surface_reference(m_surface);
    unsigned char* data = cairo_image_surface_get_data(m_surface);
    RGBA32Array* array = new RGBA32Array();
    int size = cairo_image_surface_get_width(m_surface)*
            cairo_image_surface_get_height(m_surface);
    logm(MODULE_GRAPHICS, make_message("size=%d (%dx%d)\n", size,
        cairo_image_surface_get_width(m_surface),
        cairo_image_surface_get_height(m_surface)));
    array->resize(size);
    for(int i=0; i<size; i++) {
        // move from ARGB to RGBA
        (*array)[i]   =    (data[i*4+3]<<24) | // A
                        (data[i*4]<<16) |   // R
                        (data[i*4+1]<<8) |  // G
                        (data[i*4+2]);      // B
    }
    // use this to write the array in a PNG
    //    cairo_surface_write_to_png(m_surface, "/tmp/bitmap.png");
    cairo_surface_destroy(m_surface);

    return array;
}

}
