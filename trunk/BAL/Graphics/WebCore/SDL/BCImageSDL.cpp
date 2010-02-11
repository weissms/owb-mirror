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
#include "config.h"
#include "owb-config.h"
#include "BitmapImage.h"

#include "AffineTransform.h"
#include "ApplyTransparencySDL.h"
#include "FloatRect.h"
#include "GraphicsContext.h"
#include "ImageObserver.h"
#include "PlatformString.h"
#include "SharedBuffer.h"

#include <math.h>
#include "SDL.h"
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_rotozoom.h>

namespace WebCore {

bool FrameData::clear(bool clearMetadata)
{
    if (clearMetadata)
        m_haveMetadata = false;

    if (m_frame) {
        SDL_FreeSurface(m_frame);
        m_frame = 0;
        return true;
    }
    return false;
}

BitmapImage::BitmapImage(BalSurface* surface, ImageObserver* observer)
    : Image(observer)
    , m_currentFrame(0)
    , m_frames(0)
    , m_frameTimer(0)
    , m_repetitionCount(cAnimationNone)
    , m_repetitionCountStatus(Unknown)
    , m_repetitionsComplete(0)
    , m_isSolidColor(false)
    , m_checkedForSolidColor(false)
    , m_animationFinished(true)
    , m_allDataReceived(true)
    , m_haveSize(true)
    , m_sizeAvailable(true)
    , m_decodedSize(0)
    , m_haveFrameCount(true)
    , m_frameCount(1)
{
    initPlatformData();

    // TODO: check to be sure this is an image surface

    surface->refcount++;
    int width = surface->w;
    int height = surface->h;
    m_decodedSize = width * height * 4;
    m_size = IntSize(width, height);

    m_frames.grow(1);
    m_frames[0].m_frame = surface;
    checkForSolidColor();
}


void BitmapImage::draw(GraphicsContext* context, const FloatRect& dst, const FloatRect& src, ColorSpace styleColorSpace, CompositeOperator op)
{
    SDL_Surface* image = frameAtIndex(m_currentFrame);
    if (!image) // If it's too early we won't have an image yet.
        return;

    FloatRect sourceRect(src);
    FloatRect destRect(dst);

    if (mayFillWithSolidColor()) {
        fillWithSolidColor(context, dst, solidColor(), styleColorSpace, op);
        return;
    }

    // Set the compositing operation.
    if (op == CompositeSourceOver && !frameHasAlphaAtIndex(m_currentFrame))
        context->setCompositeOperation(CompositeCopy);
    else
        context->setCompositeOperation(op);

    SDL_Surface* cr = context->platformContext();
    
    float scaleX = destRect.width() / sourceRect.width();
    float scaleY = destRect.height() / sourceRect.height();

    // Draw the image.
    SDL_Rect srcRect, dstRect;

    srcRect.x = static_cast<Sint16>(src.x());
    srcRect.y = static_cast<Sint16>(src.y());
    if (0 == sourceRect.width())
        srcRect.w = image->w;
    else
        srcRect.w = static_cast<Uint16>(sourceRect.width());
    if (0 == sourceRect.height())
        srcRect.h = image->h;
    else
        srcRect.h = static_cast<Uint16>(sourceRect.height());
    

    dstRect.x = static_cast<Sint16>(destRect.x() + context->origin().x());
    dstRect.y = static_cast<Sint16>(destRect.y() + context->origin().y());
    dstRect.w = static_cast<Sint16>(destRect.width());
    dstRect.h = static_cast<Sint16>(destRect.height());

    if ((scaleX != 1.0)||(scaleY != 1.0)) {
        srcRect.x = static_cast<Sint16>(src.x() * scaleX);
        srcRect.y = static_cast<Sint16>(src.y() * scaleY);
        srcRect.w = dstRect.w;
        srcRect.h = dstRect.h;
    }


    if (sourceRect != destRect) {
        if (context->transparencyLayer() == 1.0) {
            if ((scaleX != 1.0) || (scaleY != 1.0)) {
               SDL_Surface *surface = zoomSurface(image, scaleX, scaleY, SMOOTHING_OFF);
               SDL_BlitSurface(surface, &srcRect, cr, &dstRect);
               SDL_FreeSurface(surface);
            } else
               SDL_BlitSurface(image, &srcRect, cr, &dstRect);
        }
        else {
            SDL_Surface *surfaceWithAlpha = applyTransparency(image, static_cast<int> (context->transparencyLayer() * 255));
            if ((scaleX != 1.0) || (scaleY != 1.0)) {
                SDL_Surface *surface = zoomSurface(surfaceWithAlpha, scaleX, scaleY, SMOOTHING_OFF);
                SDL_BlitSurface(surface, &srcRect, cr, &dstRect);
                SDL_FreeSurface(surface);
            } else
              SDL_BlitSurface(surfaceWithAlpha, &srcRect, cr, &dstRect);

            SDL_FreeSurface(surfaceWithAlpha);
        }
    } else
        if (context->transparencyLayer() == 1.0)
            SDL_BlitSurface(image, &srcRect, cr, &dstRect);
        else {
            SDL_Surface *surfaceWithAlpha = applyTransparency(image, static_cast<int> (context->transparencyLayer() * 255));
            SDL_BlitSurface(surfaceWithAlpha, &srcRect, cr, &dstRect);
            SDL_FreeSurface(surfaceWithAlpha);
        }

    startAnimation();

    if (imageObserver())
        imageObserver()->didDraw(this);
}

void Image::drawPattern(GraphicsContext* context, const FloatRect& tileRect, const AffineTransform& patternTransform,
                        const FloatPoint& phase, ColorSpace, CompositeOperator op, const FloatRect& destRect)
{
    if (destRect.isEmpty())
        return;

    SDL_Surface* image = nativeImageForCurrentFrame();
    if (!image) // If it's too early we won't have an image yet.
        return;

    SDL_Surface* cr = context->platformContext();
    context->save();
    context->setCompositeOperation(op);

    // Check and see if a single draw of the image can cover the entire area we are supposed to tile.
    // save context info
    context->clip(destRect); // don't draw outside this


    IntRect dest(IntPoint(), IntSize(image->w, image->h));
    IntRect src(static_cast<int>(phase.x()), static_cast<int>(phase.y()), static_cast<int>(tileRect.size().width()), static_cast<int>(tileRect.size().height()));

    int xMax = static_cast<int>(destRect.x() + destRect.width());
    int yMax = static_cast<int>(destRect.y() + destRect.height());


    SDL_Rect srcRect, dstRect;

    srcRect.x = 0;
    srcRect.y = 0;
    if (0 == src.width())
        srcRect.w = image->w;
    else
        srcRect.w = static_cast<Uint16>(src.width());
    if (0 == src.height())
        srcRect.h = image->h;
    else
        srcRect.h = static_cast<Uint16>(src.height());

    dstRect.x = static_cast<Sint16>(dest.x());
    dstRect.y = static_cast<Sint16>(dest.y());
    dstRect.w = static_cast<Sint16>(dest.width());
    dstRect.h = static_cast<Sint16>(dest.height());

    //compute ratio of the zoomed part:
    double  ratioW = (((double)dest.width() / (double)srcRect.w));
    double  ratioH = ((double)dest.height() / ((double)srcRect.h));

    SDL_Surface *surface = NULL;
    if ((ratioW != 1.0)||(ratioH != 1.0)) {
        surface = zoomSurface(image,
                        ratioW,
                        ratioH,
                        SMOOTHING_OFF);
        //adjust offset to the new referentiel (zoomed)
        srcRect.x = static_cast<Sint16>(src.x() * ratioW);
        srcRect.y = static_cast<Sint16>(src.y() * ratioH);
    }
    
    for (int x = static_cast<int>(phase.x()); x < xMax; x += image->w) {
        for (int y = static_cast<int>(phase.y()); y < yMax; y += image->h) {
            dest.setLocation(IntPoint(x, y) + IntSize(context->origin().x(), context->origin().y()));
            dstRect.x = static_cast<Sint16>(dest.x());
            dstRect.y = static_cast<Sint16>(dest.y());
            dstRect.w = static_cast<Sint16>(dest.width());
            dstRect.h = static_cast<Sint16>(dest.height());

            if (surface) {
                if (context->transparencyLayer() == 1.0)
                    SDL_BlitSurface(surface, &srcRect, cr, &dstRect);
                else {
		    SDL_Surface *surfaceWithAlpha = applyTransparency(surface, static_cast<int> (context->transparencyLayer() * 255));
                    SDL_BlitSurface(surfaceWithAlpha, &srcRect, cr, &dstRect);
                    SDL_FreeSurface(surfaceWithAlpha);
                }
            }
            else {
                if (context->transparencyLayer() == 1.0)
                    SDL_BlitSurface(image, &srcRect, cr, &dstRect);
                else {
                    SDL_Surface *surfaceWithAlpha = applyTransparency(image, static_cast<int> (context->transparencyLayer() * 255));
                    SDL_BlitSurface(surfaceWithAlpha, &srcRect, cr, &dstRect);
                    SDL_FreeSurface(surfaceWithAlpha);
                }
            }
        }
    }
    if(surface)
        SDL_FreeSurface(surface);

    context->restore();

    if (imageObserver())
        imageObserver()->didDraw(this);
}

void BitmapImage::checkForSolidColor()
{
    // FIXME: It's easy to implement this optimization. Just need to check the RGBA32 buffer to see if it is 1x1.
    m_isSolidColor = false;
    m_checkedForSolidColor = true;
}

}

