/* This file is part of the KDE project
   Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2006 Apple Computer, Inc.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef MouseEventWithHitTestResults_h
#define MouseEventWithHitTestResults_h

#include "HitTestResult.h"
#ifdef __OWB__
#include "BIMouseEvent.h"
#else
#include "PlatformMouseEvent.h"
#endif //__OWB__

namespace WebCore {

class PlatformScrollbar;

// FIXME: Why doesn't this class just cache a HitTestResult instead of copying all of HitTestResult's fields over?
class MouseEventWithHitTestResults {
public:
#ifdef __OWB__
	MouseEventWithHitTestResults& operator=(const MouseEventWithHitTestResults& mev);
	/* Mouse event will be cloned by this object */
	MouseEventWithHitTestResults(const BIMouseEvent&, const HitTestResult&);
	~MouseEventWithHitTestResults();
#else
    MouseEventWithHitTestResults(const PlatformMouseEvent&, const HitTestResult&);
#endif //__OWB__

#ifdef __OWB__
    const BIMouseEvent& event() const { return *m_event; }
#else
    const PlatformMouseEvent& event() const { return m_event; }
#endif //__OWB__
    const HitTestResult& hitTestResult() const { return m_hitTestResult; }
    Node* targetNode() const;
    const IntPoint localPoint() const;
    PlatformScrollbar* scrollbar() const;
    bool isOverLink() const;

private:
#ifdef __OWB__
    BIMouseEvent* m_event;
#else
    PlatformMouseEvent m_event;
#endif //__OWB__
    HitTestResult m_hitTestResult;
};

}

#endif
