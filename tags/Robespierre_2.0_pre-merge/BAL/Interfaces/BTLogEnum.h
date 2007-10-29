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
 * @file  BTLogEnum.h
 *
 * @brief Header file for BTLogEnum.h
 *
 * File containing enumerator for BTLogLevel and BTLogModule,
 * thus these integers are typed.
 * File is needed to avoid having to include the entire BTLogManager.h which may
 * induce recursive inclusion and strange errors.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 *
 */

#ifndef BTLOGENUM_H
#define BTLOGENUM_H

namespace BALFacilities {

enum BTLogLevel {
  LEVEL_INFO = 0,
  LEVEL_WARNING,
  LEVEL_ERROR,
  LEVEL_CRITICAL,
  LEVEL_EMERGENCY,
  LEVEL_UNDEFINED
};

enum BTLogModule {
  MODULE_BRIDGE = 0,
  MODULE_EVENTS,
  MODULE_FACILITIES,
  MODULE_FONTS,
  MODULE_GLUE,
  MODULE_GRAPHICS,
  MODULE_IMAGEDECODERS,
  MODULE_NETWORK,
  MODULE_TYPES,
  MODULE_WIDGETS,
  MODULE_UNDEFINED
};

// When changing enums be sure to change also LogManager.cpp !
extern const char* getNameForLevel(BTLogLevel level);
extern const char* getNameForModule(BTLogModule module);

} // namespace BALFacilities

using BALFacilities::LEVEL_INFO;
using BALFacilities::LEVEL_WARNING;
using BALFacilities::LEVEL_ERROR;
using BALFacilities::LEVEL_CRITICAL;
using BALFacilities::LEVEL_EMERGENCY;
using BALFacilities::LEVEL_UNDEFINED;

using BALFacilities::MODULE_BRIDGE;
using BALFacilities::MODULE_EVENTS;
using BALFacilities::MODULE_FACILITIES;
using BALFacilities::MODULE_FONTS;
using BALFacilities::MODULE_GLUE;
using BALFacilities::MODULE_GRAPHICS;
using BALFacilities::MODULE_IMAGEDECODERS;
using BALFacilities::MODULE_NETWORK;
using BALFacilities::MODULE_TYPES;
using BALFacilities::MODULE_WIDGETS;
using BALFacilities::MODULE_UNDEFINED;


#endif // BTLOGENUM_H

