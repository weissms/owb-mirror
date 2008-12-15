/*
 * Copyright (C) 2008 Pleyo.  All rights reserved.
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

#include "config.h"
#include "DOMCoreClasses.h"

#include "DOMCSSClasses.h"
#include "DOMEventsClasses.h"
#include "DOMHTMLClasses.h"

#include <PlatformString.h>
#include <DOMWindow.h>
#include <Document.h>
#include <Element.h>
#include <SimpleFontData.h>
#include <HTMLFormElement.h>
#include <HTMLInputElement.h>
#include <HTMLNames.h>
#include <HTMLOptionElement.h>
#include <HTMLSelectElement.h>
#include <HTMLTextAreaElement.h>
#include <NodeList.h>
#include <RenderObject.h>


using namespace WebCore;
using namespace HTMLNames;

// DOMNode --------------------------------------------------------------------

WebCore::String DOMNode::nodeName()
{
    return WebCore::String();
}

WebCore::String DOMNode::nodeValue()
{
    if (!m_node)
        return WebCore::String();
    return m_node->nodeValue();
}

void DOMNode::setNodeValue(WebCore::String)
{
}

unsigned short DOMNode::nodeType()
{
    return 0;
}

DOMNode* DOMNode::parentNode()
{
    if (!m_node || !m_node->parentNode())
        return 0;
    return DOMNode::createInstance(m_node->parentNode());
}

DOMNodeList* DOMNode::childNodes()
{
    return 0;
}

DOMNode* DOMNode::firstChild()
{
    return 0;
}

DOMNode* DOMNode::lastChild()
{
    return 0;
}

DOMNode* DOMNode::previousSibling()
{
    return 0;
}

DOMNode* DOMNode::nextSibling()
{
    return 0;
}

/*DOMNamedNodeMap* DOMNode::attributes()
{
    return 0;
}*/

DOMDocument* DOMNode::ownerDocument()
{
    if (!m_node)
        return 0;
    return DOMDocument::createInstance(m_node->ownerDocument());
}

DOMNode* DOMNode::insertBefore(DOMNode* /*newChild*/, DOMNode* /*refChild*/)
{
    return 0;
}

DOMNode* DOMNode::replaceChild(DOMNode* /*newChild*/, DOMNode* /*oldChild*/)
{
    return 0;
}

DOMNode* DOMNode::removeChild(DOMNode* /*oldChild*/)
{
    return 0;
}

DOMNode* DOMNode::appendChild(DOMNode* /*oldChild*/)
{
    return 0;
}

bool DOMNode::hasChildNodes()
{
    return false;
}

DOMNode* DOMNode::cloneNode(bool /*deep*/)
{
    return 0;
}

void DOMNode::normalize()
{
}

bool DOMNode::isSupported(WebCore::String /*feature*/, WebCore::String /*version*/)
{
    return false;
}

WebCore::String DOMNode::namespaceURI()
{
    return WebCore::String();
}

WebCore::String DOMNode::prefix()
{
    return WebCore::String();
}

void DOMNode::setPrefix(WebCore::String /*prefix*/)
{
}

WebCore::String DOMNode::localName()
{
    return WebCore::String();
}

bool DOMNode::hasAttributes()
{
    return false;
}

bool DOMNode::isSameNode(DOMNode* other)
{
    if (!other)
        return false;

    return m_node->isSameNode(other->node()) ? true : false;
}

bool DOMNode::isEqualNode( DOMNode* /*other*/)
{
    return false;
}

WebCore::String DOMNode::textContent()
{
    return m_node->textContent();
}

void DOMNode::setTextContent(WebCore::String /*text*/)
{
}

// DOMNode - DOMEventTarget --------------------------------------------------

void DOMNode::addEventListener(WebCore::String /*type*/, DOMEventListener* /*listener*/, bool /*useCapture*/)
{
}

void DOMNode::removeEventListener(WebCore::String /*type*/, DOMEventListener* /*listener*/, bool /*useCapture*/)
{
}

bool DOMNode::dispatchEvent(DOMEvent* evt)
{
    if (!m_node || !evt)
        return false;

#if 0   // FIXME - raise dom exceptions
    if (![self _node]->isEventTargetNode())
        WebCore::raiseDOMException(DOM_NOT_SUPPORTED_ERR);
#endif

    WebCore::ExceptionCode ec = 0;
    return WebCore::EventTargetNodeCast(m_node)->dispatchEvent(evt->coreEvent(), ec) ? true : false;
#if 0   // FIXME - raise dom exceptions
    WebCore::raiseOnDOMError(ec);
#endif
}

// DOMNode - DOMNode ----------------------------------------------------------

DOMNode::DOMNode(WebCore::Node* n)
: m_node(0)
{
    if (n)
        n->ref();

    m_node = n;
}

DOMNode::~DOMNode()
{
    if (m_node)
        m_node->deref();
}

DOMNode* DOMNode::createInstance(WebCore::Node* n)
{
    if (!n)
        return 0;

    DOMNode* domNode = 0;
    WebCore::Node::NodeType nodeType = n->nodeType();

    switch (nodeType) {
    case WebCore::Node::ELEMENT_NODE: 
    {
        domNode = DOMElement::createInstance(static_cast<WebCore::Element*>(n));
    }
    break;
    case WebCore::Node::DOCUMENT_NODE:
    {
        domNode = DOMDocument::createInstance(n->document());
    }
    break;
    default:
    {
        domNode = new DOMNode(n);
    }
    break;
    }

    return domNode;
}


// DOMNodeList ---------------------------------------------------------------

DOMNode* DOMNodeList::item(unsigned index)
{
    if (!m_nodeList)
        return 0;

    WebCore::Node* itemNode = m_nodeList->item(index);
    if (!itemNode)
        return 0;

    return DOMNode::createInstance(itemNode);
}

unsigned DOMNodeList::length()
{
    if (!m_nodeList)
        return 0;
    return m_nodeList->length();
}

// DOMNodeList - DOMNodeList --------------------------------------------------

DOMNodeList::DOMNodeList(WebCore::NodeList* l)
: m_nodeList(0)
{
    if (l)
        l->ref();

    m_nodeList = l;
}

DOMNodeList::~DOMNodeList()
{
    if (m_nodeList)
        m_nodeList->deref();
}

DOMNodeList* DOMNodeList::createInstance(WebCore::NodeList* l)
{
    if (!l)
        return 0;

    return new DOMNodeList(l);
}

// DOMDocument ----------------------------------------------------------------

/*DOMDocumentType* DOMDocument::doctype()
{
    return 0;
}*/

DOMImplementation* DOMDocument::implementation()
{
    return 0;
}

DOMElement* DOMDocument::documentElement()
{
    return DOMElement::createInstance(m_document->documentElement());
}

DOMElement* DOMDocument::createElement(WebCore::String tagName)
{
    if (!m_document)
        return 0;

    ExceptionCode ec;
    return DOMElement::createInstance(m_document->createElement(tagName, ec).get());
}

/*DOMDocumentFragment* DOMDocument::createDocumentFragment()
{
    return 0;
}*/

// DOMText* DOMDocument::createTextNode(WebCore::String /*data*/)
// {
//     return 0;
// }
// 
// DOMComment* DOMDocument::createComment(WebCore::String /*data*/)
// {
//     return 0;
// }
// 
// DOMCDATASection* DOMDocument::createCDATASection(WebCore::String /*data*/)
// {
//     return 0;
// }
// 
// DOMProcessingInstruction* DOMDocument::createProcessingInstruction(WebCore::String /*target*/, WebCore::String /*data*/)
// {
//     return 0;
// }
// 
// DOMAttr* DOMDocument::createAttribute(WebCore::String /*name*/)
// {
//     return 0;
// }
// 
// DOMEntityReference* DOMDocument::createEntityReference(WebCore::String /*name*/)
// {
//     return 0;
// }

DOMNodeList* DOMDocument::getElementsByTagName(WebCore::String tagName)
{
    if (!m_document)
        return 0;

    return DOMNodeList::createInstance(m_document->getElementsByTagName(tagName).get());
}

DOMNode* DOMDocument::importNode(DOMNode* /*importedNode*/, bool /*deep*/)
{
    return 0;
}

DOMElement* DOMDocument::createElementNS(WebCore::String /*namespaceURI*/, WebCore::String /*qualifiedName*/)
{
    return 0;
}

// DOMAttr* DOMDocument::createAttributeNS(WebCore::String /*namespaceURI*/, WebCore::String /*qualifiedName*/)
// {
//     return 0;
// }

DOMNodeList* DOMDocument::getElementsByTagNameNS(WebCore::String namespaceURI, WebCore::String localName)
{
    if (!m_document)
        return 0;

    return  DOMNodeList::createInstance(m_document->getElementsByTagNameNS(namespaceURI, localName).get());
}

DOMElement* DOMDocument::getElementById(WebCore::String elementId)
{
    if (!m_document)
        return 0;

    return DOMElement::createInstance(m_document->getElementById(elementId));
}

// DOMDocument - DOMViewCSS --------------------------------------------------

DOMCSSStyleDeclaration* DOMDocument::getComputedStyle(DOMElement* elt, WebCore::String pseudoElt)
{
    if (!elt)
        return 0;

    Element* element = elt->element();

    WebCore::DOMWindow* dv = m_document->defaultView();
    
    if (!dv)
        return 0;
    
    return DOMCSSStyleDeclaration::createInstance(dv->getComputedStyle(element, pseudoElt.impl()).get());
}

// DOMDocument - DOMDocumentEvent --------------------------------------------

DOMEvent* DOMDocument::createEvent(WebCore::String eventType)
{
    WebCore::ExceptionCode ec = 0;
    return DOMEvent::createInstance(m_document->createEvent(eventType, ec));
}

// DOMDocument - DOMDocument --------------------------------------------------

DOMDocument::DOMDocument(WebCore::Document* d)
: DOMNode(d)
, m_document(d)
{
}

DOMDocument::~DOMDocument()
{
}

DOMDocument* DOMDocument::createInstance(WebCore::Document* d)
{
    if (!d)
        return 0;

    DOMDocument* domDocument = 0;

    if (d->isHTMLDocument()) {
        domDocument = new DOMHTMLDocument(d);
    } else {
        domDocument = new DOMDocument(d);
    }

    return domDocument;
}

// DOMElement - DOMNodeExtensions---------------------------------------------

IntRect DOMElement::boundingBox()
{
    if (!m_element)
        return IntRect();

    WebCore::RenderObject *renderer = m_element->renderer();
    if (renderer) {
        return renderer->absoluteBoundingBoxRect();
    }

    return IntRect();
}

void DOMElement::lineBoxRects(IntRect */*rects*/, int /*cRects*/)
{
}

// DOMElement ----------------------------------------------------------------

WebCore::String DOMElement::tagName()
{
    return WebCore::String();
}
    
WebCore::String DOMElement::getAttribute(WebCore::String name)
{
    if (!m_element)
        return WebCore::String();
    WebCore::String& attrValueString = (WebCore::String&) m_element->getAttribute(name);
    return attrValueString.characters();
}
    
void DOMElement::setAttribute(WebCore::String name, WebCore::String value)
{
    if (!m_element)
        return;

    WebCore::ExceptionCode ec = 0;
    m_element->setAttribute(name, value, ec);
}
    
void DOMElement::removeAttribute(WebCore::String /*name*/)
{
}
    
//DOMAttr* DOMElement::getAttributeNode(WebCore::String /*name*/)
//{
//    return 0;
//}
    
// DOMAttr* DOMElement::setAttributeNode(DOMAttr* /*newAttr*/)
// {
//     return 0;
// }
    
// DOMAttr* DOMElement::removeAttributeNode(DOMAttr* /*oldAttr*/)
// {
//     return 0;
// }
    
DOMNodeList* DOMElement::getElementsByTagName(WebCore::String /*name*/)
{
    return 0;
}
    
WebCore::String DOMElement::getAttributeNS(WebCore::String /*namespaceURI*/, WebCore::String /*localName*/)
{
    return WebCore::String();
}
    
void DOMElement::setAttributeNS(WebCore::String /*namespaceURI*/, WebCore::String /*qualifiedName*/, WebCore::String /*value*/)
{
}
    
void DOMElement::removeAttributeNS(WebCore::String /*namespaceURI*/, WebCore::String /*localName*/)
{
}
    
// DOMAttr* DOMElement::getAttributeNodeNS(WebCore::String /*namespaceURI*/, WebCore::String /*localName*/)
// {
//     return 0;
// }
    
// DOMAttr* DOMElement::setAttributeNodeNS(DOMAttr* /*newAttr*/)
// {
//     return 0;
// }
    
DOMNodeList* DOMElement::getElementsByTagNameNS(WebCore::String /*namespaceURI*/, WebCore::String /*localName*/)
{
    return 0;
}
    
bool DOMElement::hasAttribute(WebCore::String /*name*/)
{
    return false;
}
    
bool DOMElement::hasAttributeNS(WebCore::String /*namespaceURI*/, WebCore::String /*localName*/)
{
    return false;
}

void DOMElement::focus()
{
    if (!m_element)
        return;
    m_element->focus();
}

void DOMElement::blur()
{
    if (!m_element)
        return ;
    m_element->blur();
}

// DOMElementPrivate ---------------------------------------------------------

void *DOMElement::coreElement()
{
    if (!m_element)
        return 0;
    return (void*) m_element;
}

bool DOMElement::isEqual(DOMElement *other)
{
    if (!other)
        return false;
    
    void* otherCoreEle = other->coreElement();
    return (otherCoreEle == (void*)m_element) ? true : false;
}

bool DOMElement::isFocused()
{
    if (!m_element)
        return false;

    if (m_element->document()->focusedNode() == m_element)
        return true;
        
    return false;
}

WebCore::String DOMElement::innerText()
{
    if (!m_element) 
        return WebCore::String();

    return m_element->innerText();
}

WebFontDescription* DOMElement::font()
{
    ASSERT(m_element);

    WebCore::RenderObject* renderer = m_element->renderer();
    if (!renderer)
        return 0;

    FontDescription fontDescription = renderer->style()->font().fontDescription();
    AtomicString family = fontDescription.family().family();
    
    WebFontDescription *webFontDescription = new WebFontDescription();
    webFontDescription->family = family.characters();
    webFontDescription->familyLength = family.length();
    webFontDescription->size = fontDescription.computedSize();
    webFontDescription->weight = fontDescription.weight() >= FontWeight600;
    webFontDescription->italic = fontDescription.italic();

    return webFontDescription;
}

// DOMElementCSSInlineStyle --------------------------------------------------

DOMCSSStyleDeclaration* DOMElement::style()
{
    if (!m_element)
        return 0;

    WebCore::CSSStyleDeclaration* style = m_element->style();
    if (!style)
        return 0;

    return DOMCSSStyleDeclaration::createInstance(style);
}

// DOMElementExtensions ------------------------------------------------------

int DOMElement::offsetLeft()
{
    if (!m_element)
        return 0;

    return m_element->offsetLeft();
}

int DOMElement::offsetTop()
{
    if (!m_element)
        return 0;

    return m_element->offsetTop();
}

int DOMElement::offsetWidth()
{
    if (!m_element)
        return 0;

    return m_element->offsetWidth();
}

int DOMElement::offsetHeight()
{
    if (!m_element)
        return 0;

    return m_element->offsetHeight();
}

DOMElement* DOMElement::offsetParent()
{
    return 0;
}

int DOMElement::clientWidth()
{
    if (!m_element)
        return 0;

    return m_element->clientWidth();
}

int DOMElement::clientHeight()
{
    if (!m_element)
        return 0;

    return m_element->clientHeight();
}

int DOMElement::scrollLeft()
{
    if (!m_element)
        return 0;

    return m_element->scrollLeft();
}

void DOMElement::setScrollLeft(int /*newScrollLeft*/)
{
}

int DOMElement::scrollTop()
{
    if (!m_element)
        return 0;

    return m_element->scrollTop();
}

void DOMElement::setScrollTop(int /*newScrollTop*/)
{
}

int DOMElement::scrollWidth()
{
    if (!m_element)
        return 0;

    return m_element->scrollWidth();
}

int DOMElement::scrollHeight()
{
    if (!m_element)
        return 0;

    return m_element->scrollHeight();
}

void DOMElement::scrollIntoView(bool alignWithTop)
{
    if (!m_element)
        return ;

    m_element->scrollIntoView(!!alignWithTop);
}

void DOMElement::scrollIntoViewIfNeeded(bool centerIfNeeded)
{
    if (!m_element)
        return;

    m_element->scrollIntoViewIfNeeded(!!centerIfNeeded);
}

// DOMElement -----------------------------------------------------------------

DOMElement::DOMElement(WebCore::Element* e)
: DOMNode(e)
, m_element(e)
{
}

DOMElement::~DOMElement()
{
}

DOMElement* DOMElement::createInstance(WebCore::Element* e)
{
    if (!e)
        return 0;

    DOMElement* domElement = 0;

    if (e->hasTagName(formTag))
        domElement = new DOMHTMLFormElement(e);
    else if (e->hasTagName(selectTag))
        domElement =  new DOMHTMLSelectElement(e);
    else if (e->hasTagName(optionTag))
        domElement = new DOMHTMLOptionElement(e);
    else if (e->hasTagName(inputTag))
        domElement = new DOMHTMLInputElement(e);
    else if (e->hasTagName(textareaTag))
        domElement = new DOMHTMLTextAreaElement(e);
    else if (e->isHTMLElement())
        domElement = new DOMHTMLElement(e);
    else
        domElement = new DOMElement(e);

    return domElement;
}
