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
#ifndef BitmapImage_h
#define BitmapImage_h
/**
 *  @file  BitmapImage.t
 *  BitmapImage description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include "Image.h"
#include "Color.h"
#include "IntSize.h"



namespace WKAL {
    struct FrameData;
}

// This complicated-looking declaration tells the FrameData Vector that it should copy without
// invoking our constructor or destructor. This allows us to have a vector even for a struct
// that's not copyable.
namespace WTF {
    template<> class VectorTraits<WebCore: public WKALBase, FrameData> : public SimpleClassVectorTraits {};
}

namespace WKAL {

template <typename T> class Timer;

// ================================================
// FrameData Class
// ================================================

struct FrameData : Noncopyable {
    /**
     * FrameData default constructor
     * @code
     * FrameData *fd = new FrameData();
     * @endcode
     */
    FrameData();

    /**
     * ~FrameData destructor
     * @code
     * delete fd;
     * @endcode
     */
    ~FrameData();

    /**
     * clear frame data
     * @code
     * fd->clear();
     * @endcode
     */
    void clear();

    NativeImagePtr m_frame;
    float m_duration;
    bool m_hasAlpha;
};

// =================================================
// BitmapImage Class
// =================================================

class BitmapImage : public WKALBase, public Image {
    friend class GeneratedImage;
    friend class GraphicsContext;
public:
    /**
     * BitmapImage constructor
     * @param[in] : platform surface
     * @param[in] : image Observer
     * @code
     * BitmapImage *bi = new BitmapImage(pSurf);
     * @endcode
     */
    BitmapImage(BalSurface*, ImageObserver* = 0);

    /**
     * BitmapImage constructor
     * @param[in] : image Observer
     * @code
     *  BitmapImage *bi = new BitmapImage();
     * @endcode
     */
    BitmapImage(ImageObserver* = 0);

    /**
     * ~BitmapImage destructor
     * @code
     * delete bi;
     * @endcode
     */
    ~BitmapImage();
    
    /**
     * test if it's a bitmap image
     * @param[out] : true if it's a bitmap image
     * @code
     * bi->isBitmapImage();
     * @endcode
     */
    virtual bool isBitmapImage() const ;
    
    /**
     * get bitmap size 
     * @param[out] : size
     * @code
     * IntSize s = bi->size();
     * @endcode
     */
    virtual IntSize size() const;

    /**
     * test if the data is changed 
     * @param[in] : all data received status
     * @param[out] : true if the data is changed
     * @code
     * bool dc = bi->dataChanged(true);
     * @endcode
     */
    virtual bool dataChanged(bool allDataReceived);

    // It may look unusual that there is no start animation call as public API.  This is because
    // we start and stop animating lazily.  Animation begins whenever someone draws the image.  It will
    // automatically pause once all observers no longer want to render the image anywhere.
    /**
     * stop animation 
     * @code
     * bi->stopAnimation();
     * @endcode
     */
    virtual void stopAnimation();

    /**
     * reset animation
     * @code
     * bi->resetAnimation();
     * @endcode
     */
    virtual void resetAnimation();
    
    /**
     * get decoded size
     * @param[out] : decoded size
     * @code
     * unsigned s = bi->decodedSize();
     * @endcode
     */
    virtual unsigned decodedSize() const ;

    /**
     * get nativeImage for current frame
     * @param[out] : native image
     * @code
     * NativeImagePtr n = bi->nativeImageForCurrentFrame();
     * @endcode
     */
    virtual NativeImagePtr nativeImageForCurrentFrame() ;

protected:
    /**
     * draw
     * @param[in] : graphics context
     * @param[in] : destination rectangle
     * @param[in] : source rectangle
     * @param[in] : composite operator
     * @code
     * bi->draw(gc, dr, sr, op);
     * @endcode
     */
    virtual void draw(GraphicsContext*, const FloatRect& dstRect, const FloatRect& srcRect, CompositeOperator);

    /**
     * get current frame
     * @param[out] : current frame
     * @code
     * size_t c = bi->currentFrame();
     * @endcode
     */
    size_t currentFrame() const ;

    /**
     * get frame count
     * @param[out] : frame count
     * @code
     * size_t c = bi->frameCount();
     * @endcode
     */
    size_t frameCount();

    /**
     * frame at index
     * @param[in] : index
     * @param[out] : native image
     * @code
     * NativeImagePtr i = bi->frameAtIndex(1);
     * @endcode
     */
    NativeImagePtr frameAtIndex(size_t);

    /**
     * frame duration at index
     * @param[in] : index
     * @param[out] : duration
     * @code
     * float d = bi->frameDurationAtIndex(1);
     * @endcode
     */
    float frameDurationAtIndex(size_t);

    /**
     * get if the frame has alpha at index
     * @param[in] : index
     * @param[out] : true if the frame has alpha at index
     * @code
     * bool a = bi->frameHasAlphaAtIndex(1);
     * @endcode
     */
    bool frameHasAlphaAtIndex(size_t); 

    /**
     * Decodes and caches a frame. Never accessed except internally.
     * @param[in] : index
     * @code
     * bi->cacheFrame(1);
     * @endcode
     */
    void cacheFrame(size_t index);

    /**
     * Called to invalidate all our cached data.  If an image is loading incrementally, we only  invalidate the last cached frame.
     * @param[in] : incremental status
     * @code
     * bi->destroyDecodedData();
     * @endcode
     */
    virtual void destroyDecodedData(bool incremental = false);

    /**
     * Whether or not size is available yet.
     * @param[out] : status
     * @code
     * bool s = bi->isSizeAvailable();
     * @endcode
     */
    bool isSizeAvailable();

    /**
     * should Animate
     * @param[out] : status
     * @code
     * bool s = bi->shouldAnimate();
     * @endcode
     */
    bool shouldAnimate();

    /**
     * start animation
     * @code
     * bi->startAnimation();
     * @endcode
     */
    virtual void startAnimation();

    /**
     * advance animation
     * @param[in] : timer
     * @code
     * bi->advanceAnimation(t);
     * @endcode
     */
    void advanceAnimation(Timer<BitmapImage>*);
    
    /**
     * init platform data 
     * @code
     * bi->initPlatformData();
     * @endcode
     */
    void initPlatformData();

    /**
     * invalidate platform data 
     * @code
     
* bi->invalidatePlatformData();
     * @endcode
     */
    void invalidatePlatformData();
    
    /**
     * Checks to see if the image is a 1x1 solid color.  We optimize these images and just do a fill rect instead.
     * @code
     * bi->checkForSolidColor();
     * @endcode
     */
    void checkForSolidColor();
    
    /**
     * may fill with a solid color
     * @param[out] : status
     * @code
     * bool c = bi->mayFillWithSolidColor();
     * @endcode
     */
    virtual bool mayFillWithSolidColor() const ;

    /**
     * get solid color
     * @param[out] : color
     * @code
     * Color c = bi->solidColor();
     * @endcode
     */
    virtual Color solidColor() const ;
    
    ImageSource m_source;
    mutable IntSize m_size; // The size to use for the overall image (will just be the size of the first image).
    
    size_t m_currentFrame; // The index of the current frame of animation.
    Vector<FrameData> m_frames; // An array of the cached frames of the animation. We have to ref frames to pin them in the cache.
    
    Timer<BitmapImage>* m_frameTimer;
    int m_repetitionCount; // How many total animation loops we should do.
    int m_repetitionsComplete;  // How many repetitions we've finished.


    Color m_solidColor;  // If we're a 1x1 solid color, this is the color to use to fill.
    bool m_isSolidColor;  // Whether or not we are a 1x1 solid image.

    bool m_animatingImageType;  // Whether or not we're an image type that is capable of animating (GIF).
    bool m_animationFinished;  // Whether or not we've completed the entire animation.

    bool m_allDataReceived;  // Whether or not we've received all our data.

    mutable bool m_haveSize; // Whether or not our |m_size| member variable has the final overall image size yet.
    bool m_sizeAvailable; // Whether or not we can obtain the size of the first image frame yet from ImageIO.
    unsigned m_decodedSize; // The current size of all decoded frames.

    mutable bool m_haveFrameCount;
    size_t m_frameCount;
};

}

#endif




