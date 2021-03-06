/*
 * Copyright (C) 2007, 2008, 2009, 2010 Apple, Inc.  All rights reserved.
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

#if ENABLE(VIDEO)
#include "MediaPlayerPrivateQuickTimeVisualContext.h"

#include "Cookie.h"
#include "CookieJar.h"
#include "Frame.h"
#include "FrameView.h"
#include "GraphicsContext.h"
#include "KURL.h"
#include "MediaPlayerPrivateTaskTimer.h"
#include "QTCFDictionary.h"
#include "QTMovie.h"
#include "QTMovieTask.h"
#include "QTMovieVisualContext.h"
#include "ScrollView.h"
#include "SoftLinking.h"
#include "StringBuilder.h"
#include "StringHash.h"
#include "TimeRanges.h"
#include "Timer.h"
#include <Wininet.h>
#include <wtf/CurrentTime.h>
#include <wtf/HashSet.h>
#include <wtf/MainThread.h>
#include <wtf/MathExtras.h>
#include <wtf/StdLibExtras.h>

#if USE(ACCELERATED_COMPOSITING)
#include "GraphicsLayerCACF.h"
#include "WKCACFLayer.h"
#endif

using namespace std;

const CFStringRef kMinQuartzCoreVersion = CFSTR("1.0.43.0");
const CFStringRef kMinCoreVideoVersion = CFSTR("1.0.0.2");

namespace WebCore {

SOFT_LINK_LIBRARY(Wininet)
SOFT_LINK(Wininet, InternetSetCookieExW, DWORD, WINAPI, (LPCWSTR lpszUrl, LPCWSTR lpszCookieName, LPCWSTR lpszCookieData, DWORD dwFlags, DWORD_PTR dwReserved), (lpszUrl, lpszCookieName, lpszCookieData, dwFlags, dwReserved))

// Interface declaration for MediaPlayerPrivateQuickTimeVisualContext's QTMovieClient aggregate
class MediaPlayerPrivateQuickTimeVisualContext::MovieClient : public QTMovieClient {
public:
    MovieClient(MediaPlayerPrivateQuickTimeVisualContext* parent) : m_parent(parent) {}
    virtual ~MovieClient() { m_parent = 0; }
    virtual void movieEnded(QTMovie*);
    virtual void movieLoadStateChanged(QTMovie*);
    virtual void movieTimeChanged(QTMovie*);
private:
    MediaPlayerPrivateQuickTimeVisualContext* m_parent;
};

#if USE(ACCELERATED_COMPOSITING)
// Interface declaration for MediaPlayerPrivateQuickTimeVisualContext's GraphicsLayerClient aggregate
class MediaPlayerPrivateQuickTimeVisualContext::LayerClient : public GraphicsLayerClient {
public:
    LayerClient(MediaPlayerPrivateQuickTimeVisualContext* parent) : m_parent(parent) {}
    virtual ~LayerClient() { m_parent = 0; }
    virtual void paintContents(const GraphicsLayer*, GraphicsContext&, GraphicsLayerPaintingPhase, const IntRect& inClip);
    virtual void notifyAnimationStarted(const GraphicsLayer*, double time) { }
    virtual void notifySyncRequired(const GraphicsLayer*) { }
    virtual bool showDebugBorders() const { return false; }
    virtual bool showRepaintCounter() const { return false; }
private:
    MediaPlayerPrivateQuickTimeVisualContext* m_parent;
};
#endif

class MediaPlayerPrivateQuickTimeVisualContext::VisualContextClient : public QTMovieVisualContextClient {
public:
    VisualContextClient(MediaPlayerPrivateQuickTimeVisualContext* parent) : m_parent(parent) {}
    virtual ~VisualContextClient() { m_parent = 0; }
    void imageAvailableForTime(const QTCVTimeStamp*);
    static void retrieveCurrentImageProc(void*);
private:
    MediaPlayerPrivateQuickTimeVisualContext* m_parent;
};

MediaPlayerPrivateInterface* MediaPlayerPrivateQuickTimeVisualContext::create(MediaPlayer* player) 
{ 
    return new MediaPlayerPrivateQuickTimeVisualContext(player);
}

void MediaPlayerPrivateQuickTimeVisualContext::registerMediaEngine(MediaEngineRegistrar registrar)
{
    if (isAvailable())
        registrar(create, getSupportedTypes, supportsType);
}

MediaPlayerPrivateQuickTimeVisualContext::MediaPlayerPrivateQuickTimeVisualContext(MediaPlayer* player)
    : m_player(player)
    , m_seekTo(-1)
    , m_seekTimer(this, &MediaPlayerPrivateQuickTimeVisualContext::seekTimerFired)
    , m_visualContextTimer(this, &MediaPlayerPrivateQuickTimeVisualContext::visualContextTimerFired)
    , m_networkState(MediaPlayer::Empty)
    , m_readyState(MediaPlayer::HaveNothing)
    , m_enabledTrackCount(0)
    , m_totalTrackCount(0)
    , m_hasUnsupportedTracks(false)
    , m_startedPlaying(false)
    , m_isStreaming(false)
    , m_visible(false)
    , m_newFrameAvailable(false)
    , m_movieClient(new MediaPlayerPrivateQuickTimeVisualContext::MovieClient(this))
#if USE(ACCELERATED_COMPOSITING)
    , m_layerClient(new MediaPlayerPrivateQuickTimeVisualContext::LayerClient(this))
#endif
    , m_visualContextClient(new MediaPlayerPrivateQuickTimeVisualContext::VisualContextClient(this))
{
}

MediaPlayerPrivateQuickTimeVisualContext::~MediaPlayerPrivateQuickTimeVisualContext()
{
    tearDownVideoRendering();
    cancelCallOnMainThread(&VisualContextClient::retrieveCurrentImageProc, this);
}

bool MediaPlayerPrivateQuickTimeVisualContext::supportsFullscreen() const
{
    return true;
}

PlatformMedia MediaPlayerPrivateQuickTimeVisualContext::platformMedia() const
{
    PlatformMedia p;
    p.type = PlatformMedia::QTMovieVisualContextType;
    p.media.qtMovieVisualContext = m_visualContext.get();
    return p;
}

#if USE(ACCELERATED_COMPOSITING)
PlatformLayer* MediaPlayerPrivateQuickTimeVisualContext::platformLayer() const
{
    return m_qtVideoLayer ? m_qtVideoLayer->platformLayer() : 0;
}
#endif

String MediaPlayerPrivateQuickTimeVisualContext::rfc2616DateStringFromTime(CFAbsoluteTime time)
{
    static const char* const dayStrings[] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
    static const char* const monthStrings[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    static const CFStringRef dateFormatString = CFSTR("%s, %02d %s %04d %02d:%02d:%02d GMT");
    static CFTimeZoneRef gmtTimeZone;
    if (!gmtTimeZone)
        gmtTimeZone = CFTimeZoneCopyDefault();

    CFGregorianDate dateValue = CFAbsoluteTimeGetGregorianDate(time, gmtTimeZone); 
    if (!CFGregorianDateIsValid(dateValue, kCFGregorianAllUnits))
        return String();

    time = CFGregorianDateGetAbsoluteTime(dateValue, gmtTimeZone);
    SInt32 day = CFAbsoluteTimeGetDayOfWeek(time, 0);

    RetainPtr<CFStringRef> dateCFString(AdoptCF, CFStringCreateWithFormat(0, 0, dateFormatString, dayStrings[day - 1], dateValue.day, 
        monthStrings[dateValue.month - 1], dateValue.year, dateValue.hour, dateValue.minute, (int)dateValue.second));
    return dateCFString.get();
}

static void addCookieParam(StringBuilder& cookieBuilder, const String& name, const String& value)
{
    if (name.isEmpty())
        return;

    // If this isn't the first parameter added, terminate the previous one.
    if (cookieBuilder.length())
        cookieBuilder.append("; ");

    // Add parameter name, and value if there is one.
    cookieBuilder.append(name);
    if (!value.isEmpty()) {
        cookieBuilder.append("=");
        cookieBuilder.append(value);
    }
}

void MediaPlayerPrivateQuickTimeVisualContext::setUpCookiesForQuickTime(const String& url)
{
    // WebCore loaded the page with the movie URL with CFNetwork but QuickTime will 
    // use WinINet to download the movie, so we need to copy any cookies needed to
    // download the movie into WinInet before asking QuickTime to open it.
    Frame* frame = m_player->frameView() ? m_player->frameView()->frame() : 0;
    if (!frame || !frame->page() || !frame->page()->cookieEnabled())
        return;

    KURL movieURL = KURL(KURL(), url);
    Vector<Cookie> documentCookies;
    if (!getRawCookies(frame->document(), movieURL, documentCookies))
        return;

    for (size_t ndx = 0; ndx < documentCookies.size(); ndx++) {
        const Cookie& cookie = documentCookies[ndx];

        if (cookie.name.isEmpty())
            continue;

        // Build up the cookie string with as much information as we can get so WinINet
        // knows what to do with it.
        StringBuilder cookieBuilder;
        addCookieParam(cookieBuilder, cookie.name, cookie.value);
        addCookieParam(cookieBuilder, "path", cookie.path);
        if (cookie.expires) 
            addCookieParam(cookieBuilder, "expires", rfc2616DateStringFromTime(cookie.expires));
        if (cookie.httpOnly) 
            addCookieParam(cookieBuilder, "httpOnly", String());
        cookieBuilder.append(";");

        String cookieURL;
        if (!cookie.domain.isEmpty()) {
            StringBuilder urlBuilder;

            urlBuilder.append(movieURL.protocol());
            urlBuilder.append("://");
            if (cookie.domain[0] == '.')
                urlBuilder.append(cookie.domain.substring(1));
            else
                urlBuilder.append(cookie.domain);
            if (cookie.path.length() > 1)
                urlBuilder.append(cookie.path);

            cookieURL = urlBuilder.toString();
        } else
            cookieURL = movieURL;

        InternetSetCookieExW(cookieURL.charactersWithNullTermination(), 0, cookieBuilder.toString().charactersWithNullTermination(), 0, 0);
    }
}

void MediaPlayerPrivateQuickTimeVisualContext::load(const String& url)
{
    if (!QTMovie::initializeQuickTime()) {
        // FIXME: is this the right error to return?
        m_networkState = MediaPlayer::DecodeError; 
        m_player->networkStateChanged();
        return;
    }

    // Initialize the task timer.
    MediaPlayerPrivateTaskTimer::initialize();

    if (m_networkState != MediaPlayer::Loading) {
        m_networkState = MediaPlayer::Loading;
        m_player->networkStateChanged();
    }
    if (m_readyState != MediaPlayer::HaveNothing) {
        m_readyState = MediaPlayer::HaveNothing;
        m_player->readyStateChanged();
    }
    cancelSeek();

    setUpCookiesForQuickTime(url);

    m_movie = adoptRef(new QTMovie(m_movieClient.get()));
    m_movie->load(url.characters(), url.length(), m_player->preservesPitch());
    m_movie->setVolume(m_player->volume());

    CFDictionaryRef options = QTMovieVisualContext::getCGImageOptions();
    m_visualContext = adoptRef(new QTMovieVisualContext(m_visualContextClient.get(), options));
}

void MediaPlayerPrivateQuickTimeVisualContext::play()
{
    if (!m_movie)
        return;
    m_startedPlaying = true;

    m_movie->play();
    m_visualContextTimer.startRepeating(1.0 / 30);
}

void MediaPlayerPrivateQuickTimeVisualContext::pause()
{
    if (!m_movie)
        return;
    m_startedPlaying = false;

    m_movie->pause();
    m_visualContextTimer.stop();
}

float MediaPlayerPrivateQuickTimeVisualContext::duration() const
{
    if (!m_movie)
        return 0;
    return m_movie->duration();
}

float MediaPlayerPrivateQuickTimeVisualContext::currentTime() const
{
    if (!m_movie)
        return 0;
    return m_movie->currentTime();
}

void MediaPlayerPrivateQuickTimeVisualContext::seek(float time)
{
    cancelSeek();
    
    if (!m_movie)
        return;
    
    if (time > duration())
        time = duration();
    
    m_seekTo = time;
    if (maxTimeLoaded() >= m_seekTo)
        doSeek();
    else 
        m_seekTimer.start(0, 0.5f);
}
    
void MediaPlayerPrivateQuickTimeVisualContext::doSeek() 
{
    float oldRate = m_movie->rate();
    if (oldRate)
        m_movie->setRate(0);
    m_movie->setCurrentTime(m_seekTo);
    float timeAfterSeek = currentTime();
    // restore playback only if not at end, othewise QTMovie will loop
    if (oldRate && timeAfterSeek < duration())
        m_movie->setRate(oldRate);
    cancelSeek();
}

void MediaPlayerPrivateQuickTimeVisualContext::cancelSeek()
{
    m_seekTo = -1;
    m_seekTimer.stop();
}

void MediaPlayerPrivateQuickTimeVisualContext::seekTimerFired(Timer<MediaPlayerPrivateQuickTimeVisualContext>*)
{        
    if (!m_movie || !seeking() || currentTime() == m_seekTo) {
        cancelSeek();
        updateStates();
        m_player->timeChanged(); 
        return;
    } 
    
    if (maxTimeLoaded() >= m_seekTo)
        doSeek();
    else {
        MediaPlayer::NetworkState state = networkState();
        if (state == MediaPlayer::Empty || state == MediaPlayer::Loaded) {
            cancelSeek();
            updateStates();
            m_player->timeChanged();
        }
    }
}

bool MediaPlayerPrivateQuickTimeVisualContext::paused() const
{
    if (!m_movie)
        return true;
    return (!m_movie->rate());
}

bool MediaPlayerPrivateQuickTimeVisualContext::seeking() const
{
    if (!m_movie)
        return false;
    return m_seekTo >= 0;
}

IntSize MediaPlayerPrivateQuickTimeVisualContext::naturalSize() const
{
    if (!m_movie)
        return IntSize();
    int width;
    int height;
    m_movie->getNaturalSize(width, height);
    return IntSize(width, height);
}

bool MediaPlayerPrivateQuickTimeVisualContext::hasVideo() const
{
    if (!m_movie)
        return false;
    return m_movie->hasVideo();
}

bool MediaPlayerPrivateQuickTimeVisualContext::hasAudio() const
{
    if (!m_movie)
        return false;
    return m_movie->hasAudio();
}

void MediaPlayerPrivateQuickTimeVisualContext::setVolume(float volume)
{
    if (!m_movie)
        return;
    m_movie->setVolume(volume);
}

void MediaPlayerPrivateQuickTimeVisualContext::setRate(float rate)
{
    if (!m_movie)
        return;

    // Do not call setRate(...) unless we have started playing; otherwise
    // QuickTime's VisualContext can get wedged waiting for a rate change
    // call which will never come.
    if (m_startedPlaying)
        m_movie->setRate(rate);
}

void MediaPlayerPrivateQuickTimeVisualContext::setPreservesPitch(bool preservesPitch)
{
    if (!m_movie)
        return;
    m_movie->setPreservesPitch(preservesPitch);
}

bool MediaPlayerPrivateQuickTimeVisualContext::hasClosedCaptions() const
{
    if (!m_movie)
        return false;
    return m_movie->hasClosedCaptions();
}

void MediaPlayerPrivateQuickTimeVisualContext::setClosedCaptionsVisible(bool visible)
{
    if (!m_movie)
        return;
    m_movie->setClosedCaptionsVisible(visible);
}

PassRefPtr<TimeRanges> MediaPlayerPrivateQuickTimeVisualContext::buffered() const
{
    RefPtr<TimeRanges> timeRanges = TimeRanges::create();
    float loaded = maxTimeLoaded();
    // rtsp streams are not buffered
    if (!m_isStreaming && loaded > 0)
        timeRanges->add(0, loaded);
    return timeRanges.release();
}

float MediaPlayerPrivateQuickTimeVisualContext::maxTimeSeekable() const
{
    // infinite duration means live stream
    return !isfinite(duration()) ? 0 : maxTimeLoaded();
}

float MediaPlayerPrivateQuickTimeVisualContext::maxTimeLoaded() const
{
    if (!m_movie)
        return 0;
    return m_movie->maxTimeLoaded(); 
}

unsigned MediaPlayerPrivateQuickTimeVisualContext::bytesLoaded() const
{
    if (!m_movie)
        return 0;
    float dur = duration();
    float maxTime = maxTimeLoaded();
    if (!dur)
        return 0;
    return totalBytes() * maxTime / dur;
}

unsigned MediaPlayerPrivateQuickTimeVisualContext::totalBytes() const
{
    if (!m_movie)
        return 0;
    return m_movie->dataSize();
}

void MediaPlayerPrivateQuickTimeVisualContext::cancelLoad()
{
    if (m_networkState < MediaPlayer::Loading || m_networkState == MediaPlayer::Loaded)
        return;
    
    tearDownVideoRendering();

    // Cancel the load by destroying the movie.
    m_movie.clear();

    updateStates();
}

void MediaPlayerPrivateQuickTimeVisualContext::updateStates()
{
    MediaPlayer::NetworkState oldNetworkState = m_networkState;
    MediaPlayer::ReadyState oldReadyState = m_readyState;
  
    long loadState = m_movie ? m_movie->loadState() : QTMovieLoadStateError;

    if (loadState >= QTMovieLoadStateLoaded && m_readyState < MediaPlayer::HaveMetadata) {
        m_movie->disableUnsupportedTracks(m_enabledTrackCount, m_totalTrackCount);
        if (m_player->inMediaDocument()) {
            if (!m_enabledTrackCount || m_enabledTrackCount != m_totalTrackCount) {
                // This is a type of media that we do not handle directly with a <video> 
                // element, eg. QuickTime VR, a movie with a sprite track, etc. Tell the 
                // MediaPlayerClient that we won't support it.
                sawUnsupportedTracks();
                return;
            }
        } else if (!m_enabledTrackCount)
            loadState = QTMovieLoadStateError;
    }

    // "Loaded" is reserved for fully buffered movies, never the case when streaming
    if (loadState >= QTMovieLoadStateComplete && !m_isStreaming) {
        m_networkState = MediaPlayer::Loaded;
        m_readyState = MediaPlayer::HaveEnoughData;
    } else if (loadState >= QTMovieLoadStatePlaythroughOK) {
        m_readyState = MediaPlayer::HaveEnoughData;
    } else if (loadState >= QTMovieLoadStatePlayable) {
        // FIXME: This might not work correctly in streaming case, <rdar://problem/5693967>
        m_readyState = currentTime() < maxTimeLoaded() ? MediaPlayer::HaveFutureData : MediaPlayer::HaveCurrentData;
    } else if (loadState >= QTMovieLoadStateLoaded) {
        m_readyState = MediaPlayer::HaveMetadata;
    } else if (loadState > QTMovieLoadStateError) {
        m_networkState = MediaPlayer::Loading;
        m_readyState = MediaPlayer::HaveNothing;        
    } else {
        if (m_player->inMediaDocument()) {
            // Something went wrong in the loading of media within a standalone file. 
            // This can occur with chained ref movies that eventually resolve to a
            // file we don't support.
            sawUnsupportedTracks();
            return;
        }

        float loaded = maxTimeLoaded();
        if (!loaded)
            m_readyState = MediaPlayer::HaveNothing;

        if (!m_enabledTrackCount)
            m_networkState = MediaPlayer::FormatError;
        else {
            // FIXME: We should differentiate between load/network errors and decode errors <rdar://problem/5605692>
            if (loaded > 0)
                m_networkState = MediaPlayer::DecodeError;
            else
                m_readyState = MediaPlayer::HaveNothing;
        }
    }

    if (isReadyForRendering() && !hasSetUpVideoRendering())
        setUpVideoRendering();

    if (seeking())
        m_readyState = MediaPlayer::HaveNothing;
    
    if (m_networkState != oldNetworkState)
        m_player->networkStateChanged();
    if (m_readyState != oldReadyState)
        m_player->readyStateChanged();
}

bool MediaPlayerPrivateQuickTimeVisualContext::isReadyForRendering() const
{
    return m_readyState >= MediaPlayer::HaveMetadata && m_player->visible();
}

void MediaPlayerPrivateQuickTimeVisualContext::sawUnsupportedTracks()
{
    m_movie->setDisabled(true);
    m_hasUnsupportedTracks = true;
    m_player->mediaPlayerClient()->mediaPlayerSawUnsupportedTracks(m_player);
}

void MediaPlayerPrivateQuickTimeVisualContext::didEnd()
{
    if (m_hasUnsupportedTracks)
        return;

    m_startedPlaying = false;

    updateStates();
    m_player->timeChanged();
}

void MediaPlayerPrivateQuickTimeVisualContext::setSize(const IntSize& size) 
{ 
    if (m_hasUnsupportedTracks || !m_movie || m_size == size)
        return;
    m_size = size;
}

void MediaPlayerPrivateQuickTimeVisualContext::setVisible(bool visible)
{
    if (m_hasUnsupportedTracks || !m_movie || m_visible == visible)
        return;

    m_visible = visible;
    if (m_visible) {
        if (isReadyForRendering())
            setUpVideoRendering();
    } else
        tearDownVideoRendering();
}

void MediaPlayerPrivateQuickTimeVisualContext::paint(GraphicsContext* p, const IntRect& r)
{
#if USE(ACCELERATED_COMPOSITING)
    if (m_qtVideoLayer)
        return;
#endif

    paintCompleted(*p, r);
}

void MediaPlayerPrivateQuickTimeVisualContext::paintCompleted(GraphicsContext& context, const IntRect& rect)
{
    m_newFrameAvailable = false;
}

void MediaPlayerPrivateQuickTimeVisualContext::VisualContextClient::retrieveCurrentImageProc(void* refcon)
{
    static_cast<MediaPlayerPrivateQuickTimeVisualContext*>(refcon)->retrieveCurrentImage();
}

void MediaPlayerPrivateQuickTimeVisualContext::VisualContextClient::imageAvailableForTime(const QTCVTimeStamp* timeStamp)
{
    // This call may come in on another thread, so marshall to the main thread first:
    callOnMainThread(&retrieveCurrentImageProc, m_parent);

    // callOnMainThread must be paired with cancelCallOnMainThread in the destructor,
    // in case this object is deleted before the main thread request is handled.
}

void MediaPlayerPrivateQuickTimeVisualContext::visualContextTimerFired(Timer<MediaPlayerPrivateQuickTimeVisualContext>*)
{
    retrieveCurrentImage();
}

static CFDictionaryRef QTCFDictionaryCreateWithDataCallback(CFAllocatorRef allocator, const UInt8* bytes, CFIndex length)
{
    CFDataRef data = CFDataCreateWithBytesNoCopy(allocator, bytes, length, 0);
    if (!data)
        return 0;

    return reinterpret_cast<CFDictionaryRef>(CFPropertyListCreateFromXMLData(allocator, data, kCFPropertyListImmutable, 0));
}

static CGImageRef CreateCGImageFromPixelBuffer(QTPixelBuffer buffer)
{
#if USE(ACCELERATED_COMPOSITING)
    CGDataProviderRef provider = 0;
    CGColorSpaceRef colorSpace = 0;
    CGImageRef image = 0;

    size_t bitsPerComponent = 0;
    size_t bitsPerPixel = 0;
    CGImageAlphaInfo alphaInfo = kCGImageAlphaNone;
        
    if (buffer.pixelFormatIs32BGRA()) {
        bitsPerComponent = 8;
        bitsPerPixel = 32;
        alphaInfo = (CGImageAlphaInfo)(kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little);
    } else if (buffer.pixelFormatIs32ARGB()) {
        bitsPerComponent = 8;
        bitsPerPixel = 32;
        alphaInfo = (CGImageAlphaInfo)(kCGImageAlphaNoneSkipLast | kCGBitmapByteOrder32Big);
    } else {
        // All other pixel formats are currently unsupported:
        ASSERT_NOT_REACHED();
    }

    CGDataProviderDirectAccessCallbacks callbacks = {
        &QTPixelBuffer::dataProviderGetBytePointerCallback,
        &QTPixelBuffer::dataProviderReleaseBytePointerCallback,
        &QTPixelBuffer::dataProviderGetBytesAtPositionCallback,
        &QTPixelBuffer::dataProviderReleaseInfoCallback,
    };
    
    // Colorspace should be device, so that Quartz does not have to do an extra render.
    colorSpace = CGColorSpaceCreateDeviceRGB();
    require(colorSpace, Bail);
            
    provider = CGDataProviderCreateDirectAccess(buffer.pixelBufferRef(), buffer.dataSize(), &callbacks);
    require(provider, Bail);

    // CGDataProvider does not retain the buffer, but it will release it later, so do an extra retain here:
    QTPixelBuffer::retainCallback(buffer.pixelBufferRef());
        
    image = CGImageCreate(buffer.width(), buffer.height(), bitsPerComponent, bitsPerPixel, buffer.bytesPerRow(), colorSpace, alphaInfo, provider, 0, false, kCGRenderingIntentDefault);
 
Bail:
    // Once the image is created we can release our reference to the provider and the colorspace, they are retained by the image
    if (provider)
        CGDataProviderRelease(provider);
    if (colorSpace)
        CGColorSpaceRelease(colorSpace);
 
    return image;
#else
    return 0;
#endif
}


void MediaPlayerPrivateQuickTimeVisualContext::retrieveCurrentImage()
{
    if (!m_visualContext)
        return;

#if USE(ACCELERATED_COMPOSITING)
    if (!m_qtVideoLayer)
        return;

    QTPixelBuffer buffer = m_visualContext->imageForTime(0);
    if (!buffer.pixelBufferRef())
        return;

    WKCACFLayer* layer = static_cast<WKCACFLayer*>(m_qtVideoLayer->platformLayer());

    if (!buffer.lockBaseAddress()) {
#ifndef NDEBUG
        // Debug QuickTime links against a non-Debug version of CoreFoundation, so the CFDictionary attached to the CVPixelBuffer cannot be directly passed on into the CAImageQueue without being converted to a non-Debug CFDictionary:
        CFDictionaryRef attachments = QTCFDictionaryCreateCopyWithDataCallback(kCFAllocatorDefault, buffer.attachments(), &QTCFDictionaryCreateWithDataCallback);
#else
        // However, this is unnecssesary in the non-Debug case:
        CFDictionaryRef attachments = static_cast<CFDictionaryRef>(CFRetain(buffer.attachments()));
#endif

        CFTimeInterval imageTime = QTMovieVisualContext::currentHostTime();

        CGImageRef image = CreateCGImageFromPixelBuffer(buffer);
        layer->setContents(image);
        CGImageRelease(image);

        if (attachments)
            CFRelease(attachments);

        buffer.unlockBaseAddress();
        layer->rootLayer()->setNeedsRender();
    }
    m_visualContext->task();
#endif
}

static HashSet<String> mimeTypeCache()
{
    DEFINE_STATIC_LOCAL(HashSet<String>, typeCache, ());
    static bool typeListInitialized = false;

    if (!typeListInitialized) {
        unsigned count = QTMovie::countSupportedTypes();
        for (unsigned n = 0; n < count; n++) {
            const UChar* character;
            unsigned len;
            QTMovie::getSupportedType(n, character, len);
            if (len)
                typeCache.add(String(character, len));
        }

        typeListInitialized = true;
    }
    
    return typeCache;
}

void MediaPlayerPrivateQuickTimeVisualContext::getSupportedTypes(HashSet<String>& types)
{
    types = mimeTypeCache();
} 

bool MediaPlayerPrivateQuickTimeVisualContext::isAvailable()
{
    return QTMovie::initializeQuickTime();
}

MediaPlayer::SupportsType MediaPlayerPrivateQuickTimeVisualContext::supportsType(const String& type, const String& codecs)
{
    // only return "IsSupported" if there is no codecs parameter for now as there is no way to ask QT if it supports an
    //  extended MIME type
    return mimeTypeCache().contains(type) ? (codecs.isEmpty() ? MediaPlayer::MayBeSupported : MediaPlayer::IsSupported) : MediaPlayer::IsNotSupported;
}

void MediaPlayerPrivateQuickTimeVisualContext::MovieClient::movieEnded(QTMovie* movie)
{
    if (m_parent->m_hasUnsupportedTracks)
        return;

    m_parent->m_visualContextTimer.stop();

    ASSERT(m_parent->m_movie.get() == movie);
    m_parent->didEnd();
}

void MediaPlayerPrivateQuickTimeVisualContext::MovieClient::movieLoadStateChanged(QTMovie* movie)
{
    if (m_parent->m_hasUnsupportedTracks)
        return;

    ASSERT(m_parent->m_movie.get() == movie);
    m_parent->updateStates();
}

void MediaPlayerPrivateQuickTimeVisualContext::MovieClient::movieTimeChanged(QTMovie* movie)
{
    if (m_parent->m_hasUnsupportedTracks)
        return;

    ASSERT(m_parent->m_movie.get() == movie);
    m_parent->updateStates();
    m_parent->m_player->timeChanged();
}

bool MediaPlayerPrivateQuickTimeVisualContext::hasSingleSecurityOrigin() const
{
    // We tell quicktime to disallow resources that come from different origins
    // so we all media is single origin.
    return true;
}

MediaPlayerPrivateQuickTimeVisualContext::MediaRenderingMode MediaPlayerPrivateQuickTimeVisualContext::currentRenderingMode() const
{
    if (!m_movie)
        return MediaRenderingNone;

#if USE(ACCELERATED_COMPOSITING)
    if (m_qtVideoLayer)
        return MediaRenderingMovieLayer;
#endif

    return MediaRenderingSoftwareRenderer;
}

MediaPlayerPrivateQuickTimeVisualContext::MediaRenderingMode MediaPlayerPrivateQuickTimeVisualContext::preferredRenderingMode() const
{
    if (!m_player->frameView() || !m_movie)
        return MediaRenderingNone;

#if USE(ACCELERATED_COMPOSITING)
    if (supportsAcceleratedRendering() && m_player->mediaPlayerClient()->mediaPlayerRenderingCanBeAccelerated(m_player))
        return MediaRenderingMovieLayer;
#endif

    return MediaRenderingSoftwareRenderer;
}

void MediaPlayerPrivateQuickTimeVisualContext::setUpVideoRendering()
{
    MediaRenderingMode currentMode = currentRenderingMode();
    MediaRenderingMode preferredMode = preferredRenderingMode();

#if !USE(ACCELERATED_COMPOSITING)
    ASSERT(preferredMode != MediaRenderingMovieLayer);
#endif

    if (currentMode == preferredMode && currentMode != MediaRenderingNone)
        return;

    if (currentMode != MediaRenderingNone)  
        tearDownVideoRendering();

    if (preferredMode == MediaRenderingMovieLayer)
        createLayerForMovie();

#if USE(ACCELERATED_COMPOSITING)
    if (currentMode == MediaRenderingMovieLayer || preferredMode == MediaRenderingMovieLayer)
        m_player->mediaPlayerClient()->mediaPlayerRenderingModeChanged(m_player);
#endif

    m_visualContext->setMovie(m_movie.get());
}

void MediaPlayerPrivateQuickTimeVisualContext::tearDownVideoRendering()
{
#if USE(ACCELERATED_COMPOSITING)
    if (m_qtVideoLayer)
        destroyLayerForMovie();
#endif

    m_visualContext->setMovie(0);
}

bool MediaPlayerPrivateQuickTimeVisualContext::hasSetUpVideoRendering() const
{
#if USE(ACCELERATED_COMPOSITING)
    return m_qtVideoLayer || currentRenderingMode() != MediaRenderingMovieLayer;
#else
    return true;
#endif
}

void MediaPlayerPrivateQuickTimeVisualContext::createLayerForMovie()
{
#if USE(ACCELERATED_COMPOSITING)
    ASSERT(supportsAcceleratedRendering());

    if (!m_movie || m_qtVideoLayer)
        return;

    // Create a GraphicsLayer that won't be inserted directly into the render tree, but will used 
    // as a wrapper for a WKCACFLayer which gets inserted as the content layer of the video 
    // renderer's GraphicsLayer.
    m_qtVideoLayer.set(new GraphicsLayerCACF(m_layerClient.get()));
    if (!m_qtVideoLayer)
        return;

    // Mark the layer as drawing itself, anchored in the top left, and bottom-up.
    m_qtVideoLayer->setDrawsContent(true);
    m_qtVideoLayer->setAnchorPoint(FloatPoint3D());
    m_qtVideoLayer->setContentsOrientation(GraphicsLayer::CompositingCoordinatesBottomUp);
#ifndef NDEBUG
    m_qtVideoLayer->setName("Video layer");
#endif
    // The layer will get hooked up via RenderLayerBacking::updateGraphicsLayerConfiguration().
#endif
}

void MediaPlayerPrivateQuickTimeVisualContext::destroyLayerForMovie()
{
#if USE(ACCELERATED_COMPOSITING)
    if (!m_qtVideoLayer)
        return;
    m_qtVideoLayer = 0;
#endif
}

#if USE(ACCELERATED_COMPOSITING)
void MediaPlayerPrivateQuickTimeVisualContext::LayerClient::paintContents(const GraphicsLayer*, GraphicsContext&, GraphicsLayerPaintingPhase, const IntRect& inClip)
{
}

bool MediaPlayerPrivateQuickTimeVisualContext::supportsAcceleratedRendering() const
{
    return isReadyForRendering();
}

void MediaPlayerPrivateQuickTimeVisualContext::acceleratedRenderingStateChanged()
{
    // Set up or change the rendering path if necessary.
    setUpVideoRendering();
}

#endif


}

#endif
