/*
 * This file is part of the internal font implementation.  It should not be included by anyone other than
 * FontMac.cpp, FontWin.cpp and Font.cpp.
 *
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com 
 * Copyright (C) 2007 Holger Hans Peter Freyther
 * All rights reserved.
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
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#ifndef BIFontPlatformDataPrivate_h
#define BIFontPlatformDataPrivate_h

namespace BAL {

class BTFontPlatformData;

class BIFontPlatformDataPrivate {
public:

    virtual ~BIFontPlatformDataPrivate() {};

    virtual uint8_t init() = 0;

    virtual bool isFixedPitch() = 0;

    virtual unsigned hash() const = 0;

    virtual bool isEqual(const BIFontPlatformDataPrivate*) const = 0;

};

}
#endif //BIFontPlatformDataPrivate_h
