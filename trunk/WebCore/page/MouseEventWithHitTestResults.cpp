/*
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "config.h"
#include "MouseEventWithHitTestResults.h"

#include "Element.h"
#include "Node.h"

// Would TargetedMouseEvent be a better name?

namespace WebCore {

static inline Element* targetElement(Node* node)
{
    if (!node)
        return 0;
    Node* parent = node->parent();
    if (!parent || !parent->isElementNode())
        return 0;
    return static_cast<Element*>(parent);
}

#ifdef __OWB__
/**
 * It is necessary to have operator= for us to get things like :
 * mev = prepareMouseEvent(true, true, false, mouseEvent);
 * not segfault.
 */
MouseEventWithHitTestResults& MouseEventWithHitTestResults::operator=(const MouseEventWithHitTestResults& mev)
{
    m_event = static_cast<BAL::BIMouseEvent*>(mev.event().clone());
    return *this;
}

MouseEventWithHitTestResults::MouseEventWithHitTestResults(const BAL::BIMouseEvent& event, const HitTestResult& hitTestResult)
    : m_event( static_cast<BAL::BIMouseEvent*>( event.clone() ) )
#else
MouseEventWithHitTestResults::MouseEventWithHitTestResults(const PlatformMouseEvent& event, const HitTestResult& hitTestResult)
    : m_event(event)
#endif //__OWB__
    , m_hitTestResult(hitTestResult)
{
}
        
#ifdef __OWB__
MouseEventWithHitTestResults::~MouseEventWithHitTestResults()
{
	if( m_event )
    	delete m_event;
}
#endif

Node* MouseEventWithHitTestResults::targetNode() const
{
    Node* node = m_hitTestResult.innerNode();
    if (node && node->inDocument())
        return node;

    Element* element = targetElement(node);
    if (element && element->inDocument())
        return element;

    return node;
}

const IntPoint MouseEventWithHitTestResults::localPoint() const
{
    return m_hitTestResult.localPoint();
}

PlatformScrollbar* MouseEventWithHitTestResults::scrollbar() const
{
    return m_hitTestResult.scrollbar();
}

bool MouseEventWithHitTestResults::isOverLink() const
{
    return m_hitTestResult.URLElement() && m_hitTestResult.URLElement()->isLink();
}

}
