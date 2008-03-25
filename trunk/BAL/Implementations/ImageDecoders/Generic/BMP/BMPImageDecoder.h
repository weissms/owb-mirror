
#ifndef BMP_DECODER_H_
#define BMP_DECODER_H_

#include "BIImageDecoder.h"
#include "ImageDecoderCommonImplementation.h"
#include "ImageObserver.h"

namespace BC {

class BMPImageReader;

// This class decodes the BMP image format.
class BCBMPImageDecoder : public BAL::BIImageDecoder
{
public:
    BCBMPImageDecoder();
    ~BCBMPImageDecoder();

    // Take the data and store it.
    virtual void setData(const Vector<char>* data, bool allDataReceived);

    // Whether or not the size information has been decoded yet.
    virtual bool isSizeAvailable() const;

    // The total number of frames for the image.  Will scan the image data for the answer
    // (without necessarily decoding all of the individual frames).
    virtual int frameCount();

    // The number of repetitions to perform for an animation loop.
    virtual int repetitionCount() const;

    virtual BAL::RGBA32Buffer* frameBufferAtIndex(size_t index);

    virtual unsigned frameDurationAtIndex(size_t /*index*/) { return 0; }

    virtual bool supportsAlpha() const { return true; }

    virtual IntSize size() const { return mImageDecoderCommonImplementation.size(); }
	void setSize(int width, int height);

public:
    enum BMPQuery { BMPFullQuery, BMPSizeQuery, BMPFrameCountQuery };
    void decode(BMPQuery query, unsigned haltAtFrame) const;
	bool outputScanlines(unsigned int haltFrame);
    void bmpComplete();

private:
    mutable BMPImageReader* m_reader;
    ImageDecoderCommonImplementation mImageDecoderCommonImplementation;
};

}

#endif
