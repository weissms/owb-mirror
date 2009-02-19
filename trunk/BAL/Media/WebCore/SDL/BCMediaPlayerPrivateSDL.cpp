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

namespace WebCore {

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

}

MediaPlayerPrivate::~MediaPlayerPrivate()
{
}

void MediaPlayerPrivate::load(String url)
{
}

void MediaPlayerPrivate::play()
{
}

void MediaPlayerPrivate::pause()
{
}

float MediaPlayerPrivate::duration()
{
    return 0.0;
}

float MediaPlayerPrivate::currentTime() const
{
    return 0.0;
}

void MediaPlayerPrivate::seek(float time)
{
}

void MediaPlayerPrivate::setEndTime(float time)
{
}

void MediaPlayerPrivate::startEndPointTimerIfNeeded()
{
    notImplemented();
}

void MediaPlayerPrivate::cancelSeek()
{
    notImplemented();
}

void MediaPlayerPrivate::endPointTimerFired(Timer<MediaPlayerPrivate>*)
{
    notImplemented();
}

bool MediaPlayerPrivate::paused() const
{
    return !m_startedPlaying;
}

bool MediaPlayerPrivate::seeking() const
{
    return false;
}

// Returns the size of the video
IntSize MediaPlayerPrivate::naturalSize()
{
    return IntSize();
}

bool MediaPlayerPrivate::hasVideo()
{
    return false;
}

void MediaPlayerPrivate::setVolume(float volume)
{
}

void MediaPlayerPrivate::setMuted(bool b)
{

}

void MediaPlayerPrivate::setRate(float rate)
{

}

int MediaPlayerPrivate::dataRate() const
{
    notImplemented();
    return 1;
}

MediaPlayer::NetworkState MediaPlayerPrivate::networkState()
{
    return m_networkState;
}

MediaPlayer::ReadyState MediaPlayerPrivate::readyState()
{
    return m_readyState;
}

float MediaPlayerPrivate::maxTimeBuffered()
{
    notImplemented();
    // rtsp streams are not buffered
    return m_isStreaming ? 0 : maxTimeLoaded();
}

float MediaPlayerPrivate::maxTimeSeekable()
{
    // TODO
    if (m_isStreaming)
        return numeric_limits<float>::infinity();
    // infinite duration means live stream
    return maxTimeLoaded();
}

float MediaPlayerPrivate::maxTimeLoaded()
{
    // TODO
    notImplemented();
    return duration();
}

unsigned MediaPlayerPrivate::bytesLoaded()
{
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
    notImplemented();
    return totalBytes() > 0;
}

unsigned MediaPlayerPrivate::totalBytes()
{
    notImplemented();

    // Do something with m_source to get the total bytes of the media

    return 100;
}

void MediaPlayerPrivate::cancelLoad()
{
    notImplemented();
}

void MediaPlayerPrivate::updateStates()
{
}

void MediaPlayerPrivate::loadStateChanged()
{
    updateStates();
}

void MediaPlayerPrivate::rateChanged()
{
    updateStates();
}

void MediaPlayerPrivate::sizeChanged()
{
    notImplemented();
}

void MediaPlayerPrivate::timeChanged()
{
    updateStates();
    m_player->timeChanged();
}

void MediaPlayerPrivate::volumeChanged()
{
    m_player->volumeChanged();
}

void MediaPlayerPrivate::didEnd()
{
    m_isEndReached = true;
    pause();
    timeChanged();
}

void MediaPlayerPrivate::loadingFailed()
{
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
    m_rect = rect;
}

void MediaPlayerPrivate::setVisible(bool visible)
{
    m_visible = visible;
}

void MediaPlayerPrivate::repaint()
{
    m_player->repaint();
}

void MediaPlayerPrivate::paint(GraphicsContext* context, const IntRect& rect)
{
}

void MediaPlayerPrivate::getSupportedTypes(HashSet<String>& types)
{
    // FIXME: do the real thing
    notImplemented();
    types.add(String("video/x-theora+ogg"));
}

void MediaPlayerPrivate::createGSTPlayBin(String url)
{
}

}

#endif
