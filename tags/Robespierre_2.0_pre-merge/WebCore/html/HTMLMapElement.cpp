/**
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2004, 2005, 2006 Apple Computer, Inc.
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
 */
#include "config.h"
#include "HTMLMapElement.h"

#include "Document.h"
#include "HTMLAreaElement.h"
#include "HTMLCollection.h"
#include "HTMLNames.h"
#include "IntSize.h"
#include "HitTestResult.h"

using namespace std;

namespace WebCore {

using namespace HTMLNames;

HTMLMapElement::HTMLMapElement(Document *doc)
    : HTMLElement(mapTag, doc)
{
}

HTMLMapElement::~HTMLMapElement()
{
    document()->removeImageMap(this);
}

bool HTMLMapElement::checkDTD(const Node* newChild)
{
    // FIXME: This seems really odd, allowing only blocks inside map elements.
    return newChild->hasTagName(areaTag) || newChild->hasTagName(scriptTag) || inBlockTagList(newChild)
        || (!document()->inStrictMode() && newChild->hasTagName(imgTag));
}

bool HTMLMapElement::mapMouseEvent(int x, int y, const IntSize& size, HitTestResult& result)
{
    HTMLAreaElement* defaultArea = 0;
    Node *node = this;
    while ((node = node->traverseNextNode(this))) {
        if (node->hasTagName(areaTag)) {
            HTMLAreaElement* areaElt = static_cast<HTMLAreaElement*>(node);
            if (areaElt->isDefault()) {
                if (!defaultArea)
                    defaultArea = areaElt;
            } else if (areaElt->mapMouseEvent(x, y, size, result))
                return true;
        }
    }
    
    if (defaultArea) {
        result.setInnerNode(defaultArea);
        result.setURLElement(defaultArea);
    }
    return defaultArea;
}

void HTMLMapElement::parseMappedAttribute(MappedAttribute* attr)
{
    const QualifiedName& attrName = attr->name();
    if (attrName == idAttr || attrName == nameAttr) {
        Document* doc = document();
        if (attrName == idAttr) {
            // Call base class so that hasID bit gets set.
            HTMLElement::parseMappedAttribute(attr);
            if (doc->htmlMode() != Document::XHtml)
                return;
        }
        doc->removeImageMap(this);
        String mapName = attr->value();
        if (mapName[0] == '#')
            mapName = mapName.substring(1);
        m_name = doc->htmlMode() == Document::XHtml ? mapName : mapName.lower();
        doc->addImageMap(this);
    } else
        HTMLElement::parseMappedAttribute(attr);
}

PassRefPtr<HTMLCollection> HTMLMapElement::areas()
{
    return new HTMLCollection(this, HTMLCollection::MapAreas);
}

String HTMLMapElement::name() const
{
    return getAttribute(nameAttr);
}

void HTMLMapElement::setName(const String& value)
{
    setAttribute(nameAttr, value);
}

}
