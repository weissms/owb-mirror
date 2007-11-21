/*
 * Copyright (C) 2003, 2006 Apple Computer, Inc.  All rights reserved.
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

#ifndef BCGraphicsContext_h
#define BCGraphicsContext_h

#include "BIGraphicsContext.h"
#include "BTFont.h"
#include "FloatRect.h"
#include "IntRect.h"
#include "Pen.h"
#include "TextDirection.h"
#include "TextStyle.h"
#include <wtf/Noncopyable.h>
//#include "BIInternationalization.h"

typedef struct _cairo PlatformGraphicsContext;

class DeprecatedString;

namespace WebCore {

    class IntPoint;
    class IntPointArray;
    class Path;

#if SVG_SUPPORT
    class KRenderingDeviceContext;
#endif
}

namespace BAL {

    class GraphicsContextPrivate;
    class GraphicsContextPlatformPrivate;

    class BCGraphicsContext : Noncopyable, public BIGraphicsContext {
    public:
        BCGraphicsContext(PlatformGraphicsContext*);
        virtual ~BCGraphicsContext();

        PlatformGraphicsContext* platformContext() const;
        IMPLEMENT_BIGRAPHICSCONTEXT;
#if SVG_SUPPORT
#ifndef __OWB__
        KRenderingDeviceContext* createRenderingDeviceContext();
#endif
#endif

    private:
        void realDraw(BINativeImage*, const FloatRect& dstRect, const FloatRect& srcRect, CompositeOperator);
        void realDrawTiled(BINativeImage*, const FloatRect& dstRect, const FloatPoint& srcPoint, const FloatSize& tileSize, CompositeOperator);
        void realDrawTiled(BINativeImage*, const FloatRect& dstRect, const FloatRect& srcRect, WebCore::TileRule hRule, WebCore::TileRule vRule, CompositeOperator);
    
    private:
        void savePlatformState();
        void restorePlatformState();

        int focusRingWidth() const;
        int focusRingOffset() const;
        const Vector<IntRect>& focusRingRects() const;

        static GraphicsContextPrivate* createGraphicsContextPrivate();
        static void destroyGraphicsContextPrivate(GraphicsContextPrivate*);

        GraphicsContextPrivate* m_common;
        GraphicsContextPlatformPrivate* m_data;
    };

}
#endif
