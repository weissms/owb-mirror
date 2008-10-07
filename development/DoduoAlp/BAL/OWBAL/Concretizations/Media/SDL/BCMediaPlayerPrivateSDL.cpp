/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2007 Collabora Ltd.  All rights reserved.
 * Copyright (C) 2007 Alp Toker <alp@atoker.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * aint with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"

#if ENABLE(VIDEO)

#include "MediaPlayerPrivateSDL.h"

#include "CString.h"
#include "GraphicsContext.h"
#include "IntRect.h"
#include "KURL.h"
#include "MIMETypeRegistry.h"
#include "MediaPlayer.h"
#include "NotImplemented.h"
#include "ScrollView.h"
#include "Widget.h"

#include <cstdio>
#include <limits>
#include <math.h>

using namespace std;

namespace OWBAL {

MediaPlayerPrivate::MediaPlayerPrivate(MediaPlayer* player)
    : m_player(player)
    , m_rate(1.0f)
    , m_endTime(numeric_limits<float>::infinity())
    , m_isEndReached(false)
    , m_volume(0.5f)
    , m_networkState(MediaPlayer::Empty)
    , m_readyState(MediaPlayer::DataUnavailable)
    , m_startedPlaying(false)
    , m_isStreaming(false)
    , m_rect(IntRect())
    , m_visible(true)
{
    printf("MediaPlayerPrivate::MediaPlayerPrivate\n");

}

MediaPlayerPrivate::~MediaPlayerPrivate()
{
    printf("MediaPlayerPrivate::~MediaPlayerPrivate\n");
}

void MediaPlayerPrivate::load(String url)
{
    printf("MediaPlayerPrivate::load\n");
}

void MediaPlayerPrivate::play()
{
    printf("MediaPlayerPrivate::play\n");
}

void MediaPlayerPrivate::pause()
{
    printf("MediaPlayerPrivate::pause\n");
}

float MediaPlayerPrivate::duration()
{
    printf("MediaPlayerPrivate::duration\n");
    return 0.0;
}

float MediaPlayerPrivate::currentTime() const
{
    printf("MediaPlayerPrivate::currentTime\n");
    return 0.0;
}

void MediaPlayerPrivate::seek(float time)
{
    printf("MediaPlayerPrivate::seek\n");
}

void MediaPlayerPrivate::setEndTime(float time)
{
    printf("MediaPlayerPrivate::setEndTime\n");
}

void MediaPlayerPrivate::startEndPointTimerIfNeeded()
{
    printf("MediaPlayerPrivate::startEndPointTimerIfNeeded\n");
    notImplemented();
}

void MediaPlayerPrivate::cancelSeek()
{
    printf("MediaPlayerPrivate::cancelSeek\n");
    notImplemented();
}

void MediaPlayerPrivate::endPointTimerFired(Timer<MediaPlayerPrivate>*)
{
    printf("MediaPlayerPrivate::endPointTimerFired\n");
    notImplemented();
}

bool MediaPlayerPrivate::paused() const
{
    printf("MediaPlayerPrivate::paused\n");
    return !m_startedPlaying;
}

bool MediaPlayerPrivate::seeking() const
{
    printf("MediaPlayerPrivate::seeking\n");
    return false;
}

// Returns the size of the video
IntSize MediaPlayerPrivate::naturalSize()
{
    printf("MediaPlayerPrivate::naturalSize\n");
    return IntSize();
}

bool MediaPlayerPrivate::hasVideo()
{
    printf("MediaPlayerPrivate::hasVideo\n");
    return false;
}

void MediaPlayerPrivate::setVolume(float volume)
{
    printf("MediaPlayerPrivate::setVolume\n");
}

void MediaPlayerPrivate::setMuted(bool b)
{
    printf("MediaPlayerPrivate::setMuted\n");

}

void MediaPlayerPrivate::setRate(float rate)
{
    printf("MediaPlayerPrivate::setRate\n");

}

int MediaPlayerPrivate::dataRate() const
{
    printf("MediaPlayerPrivate::dataRate\n");
    notImplemented();
    return 1;
}

MediaPlayer::NetworkState MediaPlayerPrivate::networkState()
{
    printf("MediaPlayerPrivate::networkState\n");
    return m_networkState;
}

MediaPlayer::ReadyState MediaPlayerPrivate::readyState()
{
    printf("MediaPlayerPrivate::readyState\n");
    return m_readyState;
}

float MediaPlayerPrivate::maxTimeBuffered()
{
    printf("MediaPlayerPrivate::maxTimeBuffered\n");
    notImplemented();
    // rtsp streams are not buffered
    return m_isStreaming ? 0 : maxTimeLoaded();
}

float MediaPlayerPrivate::maxTimeSeekable()
{
    printf("MediaPlayerPrivate::maxTimeSeekable\n");
    // TODO
    if (m_isStreaming)
        return numeric_limits<float>::infinity();
    // infinite duration means live stream
    return maxTimeLoaded();
}

float MediaPlayerPrivate::maxTimeLoaded()
{
    printf("MediaPlayerPrivate::maxTimeLoaded\n");
    // TODO
    notImplemented();
    return duration();
}

unsigned MediaPlayerPrivate::bytesLoaded()
{
    printf("MediaPlayerPrivate::bytesLoaded\n");
    notImplemented();
    /*if (!m_playBin)
        return 0;
    float dur = duration();
    float maxTime = maxTimeLoaded();
    if (!dur)
        return 0;*/
    return 1;//totalBytes() * maxTime / dur;
}

bool MediaPlayerPrivate::totalBytesKnown()
{
    printf("MediaPlayerPrivate::totalBytesKnown\n");
    notImplemented();
    return totalBytes() > 0;
}

unsigned MediaPlayerPrivate::totalBytes()
{
    printf("MediaPlayerPrivate::totalBytes\n");
    notImplemented();

    // Do something with m_source to get the total bytes of the media

    return 100;
}

void MediaPlayerPrivate::cancelLoad()
{
    printf("MediaPlayerPrivate::cancelLoad\n");
    notImplemented();
}

void MediaPlayerPrivate::updateStates()
{
        printf("MediaPlayerPrivate::updateStates\n");
}

void MediaPlayerPrivate::loadStateChanged()
{
    printf("MediaPlayerPrivate::loadStateChanged\n");
    updateStates();
}

void MediaPlayerPrivate::rateChanged()
{
    printf("MediaPlayerPrivate::rateChanged\n");
    updateStates();
}

void MediaPlayerPrivate::sizeChanged()
{
    printf("MediaPlayerPrivate::sizeChanged\n");
    notImplemented();
}

void MediaPlayerPrivate::timeChanged()
{
    printf("MediaPlayerPrivate::timeChanged\n");
    updateStates();
    m_player->timeChanged();
}

void MediaPlayerPrivate::volumeChanged()
{
    printf("MediaPlayerPrivate::volumeChanged\n");
    m_player->volumeChanged();
}

void MediaPlayerPrivate::didEnd()
{
    printf("MediaPlayerPrivate::didEnd\n");
    m_isEndReached = true;
    pause();
    timeChanged();
}

void MediaPlayerPrivate::loadingFailed()
{
    printf("MediaPlayerPrivate::loadingFailed\n");
    if (m_networkState != MediaPlayer::LoadFailed) {
        m_networkState = MediaPlayer::LoadFailed;
        m_player->networkStateChanged();
    }
    if (m_readyState != MediaPlayer::DataUnavailable) {
        m_readyState = MediaPlayer::DataUnavailable;
        m_player->readyStateChanged();
    }
}

void MediaPlayerPrivate::setRect(const IntRect& rect)
{
    printf("MediaPlayerPrivate::setRect\n");
    m_rect = rect;
}

void MediaPlayerPrivate::setVisible(bool visible)
{
    printf("MediaPlayerPrivate::setVisible\n");
    m_visible = visible;
}

void MediaPlayerPrivate::repaint()
{
    printf("MediaPlayerPrivate::repaint\n");
    m_player->repaint();
}

void MediaPlayerPrivate::paint(GraphicsContext* context, const IntRect& rect)
{
        printf("MediaPlayerPrivate::paint\n");
}

void MediaPlayerPrivate::getSupportedTypes(HashSet<String>& types)
{
    printf("MediaPlayerPrivate::getSupportedTypes\n");
    // FIXME: do the real thing
    notImplemented();
    types.add(String("video/x-theora+ogg"));
}

void MediaPlayerPrivate::createGSTPlayBin(String url)
{
    printf("MediaPlayerPrivate::createGSTPlayBin\n");
}

}

#endif
