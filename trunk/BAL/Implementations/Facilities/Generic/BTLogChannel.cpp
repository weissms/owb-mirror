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

/**
 * @file  BTLogChannel.cpp
 *
 * @brief Implementation file for BTLogChannel.cpp
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 *
 */

#include "BTLogChannel.h"
#include "BTLogFormatter.h"

namespace BALFacilities {

BTLogChannel::BTLogChannel()
	: m_logFormatter(0)
	, m_isActive(true)
	, m_levelThreshold(0)
	, m_isSynchronized(true)
{
}

BTLogChannel::~BTLogChannel()
{
}

bool BTLogChannel::getIsActive()
{
    return m_isActive;
}

void BTLogChannel::setIsActive(bool value)
{
    m_isActive = value;
}

int BTLogChannel::getLevelThreshold()
{
    return m_levelThreshold;
}

void BTLogChannel::setLevelThreshold(int value)
{
    m_levelThreshold = value;
}

BTLogFormatter* BTLogChannel::getFormatter()
{
    return m_logFormatter.get();
}

void BTLogChannel::setFormatter(BTLogFormatter* value)
{
    if (value)
        m_logFormatter = value;
}

bool BTLogChannel::getIsSynchronized()
{
    return m_isSynchronized;
}

void BTLogChannel::setIsSynchronized(bool value)
{
    m_isSynchronized = value;
}

} // namespace BALFacilities
