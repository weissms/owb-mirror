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
 * @file  BTDeviceChannel.cpp
 *
 * @brief Implementation file for BTDeviceChannel.cpp
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 *
 */

#include "BTDeviceChannel.h"
#include <fcntl.h>
#include <unistd.h>

namespace BALFacilities {

/**
 * Constructors/Destructors
 */
BTDeviceChannel::BTDeviceChannel()
	: m_descriptor(STDERR_FILENO)
{
}

BTDeviceChannel::~BTDeviceChannel()
{
    if ((m_descriptor != STDIN_FILENO) &&
        (m_descriptor != STDOUT_FILENO) &&
        (m_descriptor != STDERR_FILENO))
        close(m_descriptor);
}

string BTDeviceChannel::getPathName()
{
    return m_pathName;
}

/**
 * Setting the pathname may close the old one, and opens a new descriptor
 * By default, output will go to STDERR_FILENO.
 */
void BTDeviceChannel::setPathName(string value)
{
    if ((m_descriptor != STDIN_FILENO) &&
        (m_descriptor != STDOUT_FILENO) &&
        (m_descriptor != STDERR_FILENO))
        close( m_descriptor );

    if( !value.empty() ) {
        m_descriptor = open( value.c_str(), O_WRONLY );
        if( m_descriptor == -1 )
            fprintf( stderr, "Can't set pathname to device channel !\n" );
        else
            m_pathName = value;
    }
}

/**
 * Implemented with a write()
 */
void BTDeviceChannel::send(const char* message)
{
    ssize_t received = 0;
    if (message)
        received = write(m_descriptor, message, strlen(message));
    else
        received = write(m_descriptor, "(null)", 6);
}

} // namespace BALFacilities
