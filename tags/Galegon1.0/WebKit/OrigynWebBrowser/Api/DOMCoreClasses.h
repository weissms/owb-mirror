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

#ifndef DOMCoreClasses_H
#define DOMCoreClasses_H

#include "WebScriptObject.h"
#include "BALBase.h"
#include "PlatformString.h"
#include "IntRect.h"
#include "DOMImplementation.h"

namespace WebCore {
    class Element;
    class Document;
    class Node;
    class NodeList;
    class WebCore::DOMImplementation;
}

struct WebFontDescription {
    WebCore::String family;
    unsigned familyLength;
    float size;
    bool weight;
    bool italic;
};

class DOMNodeList;
class DOMDocument;
class DOMEventListener;
class DOMEvent;
class DOMElement;
class DOMCSSStyleDeclaration;

class DOMObject : public WebScriptObject
{
public:
    /**
     * throw exception
     * @discussion Throws an exception in the current script execution context.
        @result Either NO if an exception could not be raised, YES otherwise.
     */
    virtual bool throwException(WebCore::String exceptionMessage)
    { 
        return WebScriptObject::throwException(exceptionMessage); 
    }

    /**
     * remove web script key 
     * @param name The name of the method to call in the script environment.
        @param args The arguments to pass to the script environment.
        @discussion Calls the specified method in the script environment using the
        specified arguments.
        @result Returns the result of calling the script method.
     */
    virtual void removeWebScriptKey(WebCore::String name)
    {
        WebScriptObject::removeWebScriptKey(name);
    }

    /**
     *  stringRepresentation 
     * @discussion Converts the target object to a string representation.  The coercion
        of non string objects type is dependent on the script environment.
        @result Returns the string representation of the object.
     */
    virtual WebCore::String stringRepresentation()
    {
        return WebScriptObject::stringRepresentation();
    }

    /**
     *  setException 
     * @param description The description of the exception.
        @discussion Raises an exception in the script environment in the context of the
        current object.
     */
    virtual void setException(WebCore::String description)
    {
        WebScriptObject::setException(description);
    }
};

class DOMNode : public DOMObject
{
protected:
    DOMNode(WebCore::Node* n);

public:
    virtual ~DOMNode();
    static DOMNode* createInstance(WebCore::Node* n);

public:
    
    // IWebScriptObject
    /**
     * throw exception
     * @discussion Throws an exception in the current script execution context.
        @result Either NO if an exception could not be raised, YES otherwise.
     */
    virtual bool throwException(WebCore::String exceptionMessage)
    { 
        return WebScriptObject::throwException(exceptionMessage); 
    }

    /**
     * remove web script key 
     * @param name The name of the method to call in the script environment.
        @param args The arguments to pass to the script environment.
        @discussion Calls the specified method in the script environment using the
        specified arguments.
        @result Returns the result of calling the script method.
     */
    virtual void removeWebScriptKey(WebCore::String name)
    {
        WebScriptObject::removeWebScriptKey(name);
    }

    /**
     *  stringRepresentation 
     * @discussion Converts the target object to a string representation.  The coercion
        of non string objects type is dependent on the script environment.
        @result Returns the string representation of the object.
     */
    virtual WebCore::String stringRepresentation()
    {
        return WebScriptObject::stringRepresentation();
    }

    /**
     *  setException 
     * @param description The description of the exception.
        @discussion Raises an exception in the script environment in the context of the
        current object.
     */
    virtual void setException(WebCore::String description)
    {
        WebScriptObject::setException(description);
    }

    // IDOMNode
    virtual WebCore::String nodeName();
    
    virtual WebCore::String nodeValue();
    
    virtual void setNodeValue(WebCore::String);
    
    virtual unsigned short nodeType();
    
    virtual DOMNode* parentNode();
    
    virtual DOMNodeList* childNodes();
    
    virtual DOMNode* firstChild();
    
    virtual DOMNode* lastChild();
    
    virtual DOMNode* previousSibling();
    
    virtual DOMNode* nextSibling();
    
    //virtual DOMNamedNodeMap* attributes();
    
    virtual DOMDocument* ownerDocument();
    
    virtual DOMNode* insertBefore(DOMNode *newChild, DOMNode *refChild);
    
    virtual DOMNode* replaceChild(DOMNode *newChild, DOMNode *oldChild);
    
    virtual DOMNode* removeChild(DOMNode *oldChild);
    
    virtual DOMNode* appendChild(DOMNode *oldChild);
    
    virtual bool hasChildNodes();
    
    virtual DOMNode* cloneNode(bool deep);
    
    virtual void normalize();
    
    virtual bool isSupported(WebCore::String feature, WebCore::String version);
    
    virtual WebCore::String namespaceURI();
    
    virtual WebCore::String prefix();
    
    virtual void setPrefix(WebCore::String);
    
    virtual WebCore::String localName();
    
    virtual bool hasAttributes();

    virtual bool isSameNode(DOMNode* other);
    
    virtual bool isEqualNode(DOMNode* other);
    
    virtual WebCore::String textContent();
    
    virtual void setTextContent(WebCore::String text);

    // IDOMEventTarget
    virtual void addEventListener(WebCore::String type, DOMEventListener *listener, bool useCapture);
    
    virtual void removeEventListener( WebCore::String type, DOMEventListener *listener, bool useCapture);
    
    virtual bool dispatchEvent(DOMEvent *evt);

    // DOMNode
    WebCore::Node* node() const { return m_node; }

protected:
    WebCore::Node* m_node;
};

class DOMNodeList : public DOMObject
{
protected:
    DOMNodeList(WebCore::NodeList* l);

public:
    virtual ~DOMNodeList();
    static DOMNodeList* createInstance(WebCore::NodeList* l);

public:
    // IWebScriptObject
    /**
     * throw exception
     * @discussion Throws an exception in the current script execution context.
        @result Either NO if an exception could not be raised, YES otherwise.
     */
    virtual bool throwException(WebCore::String exceptionMessage)
    { 
        return WebScriptObject::throwException(exceptionMessage); 
    }

    /**
     * remove web script key 
     * @param name The name of the method to call in the script environment.
        @param args The arguments to pass to the script environment.
        @discussion Calls the specified method in the script environment using the
        specified arguments.
        @result Returns the result of calling the script method.
     */
    virtual void removeWebScriptKey(WebCore::String name)
    {
        WebScriptObject::removeWebScriptKey(name);
    }

    /**
     *  stringRepresentation 
     * @discussion Converts the target object to a string representation.  The coercion
        of non string objects type is dependent on the script environment.
        @result Returns the string representation of the object.
     */
    virtual WebCore::String stringRepresentation()
    {
        return WebScriptObject::stringRepresentation();
    }

    /**
     *  setException 
     * @param description The description of the exception.
        @discussion Raises an exception in the script environment in the context of the
        current object.
     */
    virtual void setException(WebCore::String description)
    {
        WebScriptObject::setException(description);
    }

    // IDOMNodeList
    virtual DOMNode* item(unsigned index);
    
    virtual unsigned length();

protected:
    WebCore::NodeList* m_nodeList;
};

class DOMDocument : public DOMNode
{
protected:
    DOMDocument(WebCore::Document* d);

public:
    static DOMDocument* createInstance(WebCore::Document* d);
    virtual ~DOMDocument();

    // WebScriptObject
    /**
     * throw exception
     * @discussion Throws an exception in the current script execution context.
        @result Either NO if an exception could not be raised, YES otherwise.
     */
    virtual bool throwException(WebCore::String exceptionMessage)
    { 
        return WebScriptObject::throwException(exceptionMessage);
    }

    /**
     * remove web script key 
     * @param name The name of the method to call in the script environment.
        @param args The arguments to pass to the script environment.
        @discussion Calls the specified method in the script environment using the
        specified arguments.
        @result Returns the result of calling the script method.
     */
    virtual void removeWebScriptKey(WebCore::String name)
    {
        WebScriptObject::removeWebScriptKey(name);
    }

    /**
     *  stringRepresentation 
     * @discussion Converts the target object to a string representation.  The coercion
        of non string objects type is dependent on the script environment.
        @result Returns the string representation of the object.
     */
    virtual WebCore::String stringRepresentation()
    {
        return WebScriptObject::stringRepresentation();
    }

    /**
     *  setException 
     * @param description The description of the exception.
        @discussion Raises an exception in the script environment in the context of the
        current object.
     */
    virtual void setException(WebCore::String description)
    {
        WebScriptObject::setException(description);
    }
    
    // DOMNode
    virtual WebCore::String nodeName() { return DOMNode::nodeName(); }
    
    virtual WebCore::String nodeValue() { return DOMNode::nodeValue(); }
    
    virtual void setNodeValue(WebCore::String value) { DOMNode::setNodeValue(value); }
    
    virtual unsigned short nodeType() { return DOMNode::nodeType(); }
    
    virtual DOMNode* parentNode() { return DOMNode::parentNode(); }
    
    virtual DOMNodeList* childNodes() { return DOMNode::childNodes(); }
    
    virtual DOMNode* firstChild() { return DOMNode::firstChild(); }
    
    virtual DOMNode* lastChild() { return DOMNode::lastChild(); }
    
    virtual DOMNode* previousSibling() { return DOMNode::previousSibling(); }
    
    virtual DOMNode* nextSibling() { return DOMNode::nextSibling(); }
    
    //virtual DOMNamedNodeMap* attributes() { return DOMNode::attributes(); }
    
    virtual DOMDocument* ownerDocument() { return DOMNode::ownerDocument(); }
    
    virtual DOMNode* insertBefore(DOMNode *newChild, DOMNode *refChild) { return DOMNode::insertBefore(newChild, refChild); }
    
    virtual DOMNode* replaceChild(DOMNode *newChild, DOMNode *oldChild) { return DOMNode::replaceChild(newChild, oldChild); }
    
    virtual DOMNode* removeChild(DOMNode *oldChild) { return DOMNode::removeChild(oldChild); }
    
    virtual DOMNode* appendChild(DOMNode *oldChild) { return DOMNode::appendChild(oldChild); }
    
    virtual bool hasChildNodes() { return DOMNode::hasChildNodes(); }
    
    virtual DOMNode* cloneNode(bool deep) { return DOMNode::cloneNode(deep); }
    
    virtual void normalize() { DOMNode::normalize(); }
    
    virtual bool isSupported(WebCore::String feature, WebCore::String version) { return DOMNode::isSupported(feature, version); }
    
    virtual WebCore::String namespaceURI() { return DOMNode::namespaceURI(); }
    
    virtual WebCore::String prefix() { return DOMNode::prefix(); }
    
    virtual void setPrefix(WebCore::String prefix) { DOMNode::setPrefix(prefix); }
    
    virtual WebCore::String localName() { return DOMNode::localName(); }
    
    virtual bool hasAttributes() { return DOMNode::hasAttributes(); }

    virtual bool isSameNode(DOMNode* other) { return DOMNode::isSameNode(other); }
    
    virtual bool isEqualNode(DOMNode* other) { return DOMNode::isEqualNode(other); }
    
    virtual WebCore::String textContent() { return DOMNode::textContent(); }
    
    virtual void setTextContent(WebCore::String text) { DOMNode::setTextContent(text); }
    
    // DOMDocument
//    virtual DOMDocumentType* doctype();
    
    virtual WebCore::DOMImplementation* implementation();
    
    virtual DOMElement* documentElement();
    
    virtual DOMElement* createElement(WebCore::String tagName);
    
    //virtual DOMDocumentFragment* createDocumentFragment();
    
    /*virtual DOMText* createTextNode(WebCore::String data);
    
    virtual DOMComment* createComment(WebCore::String data);
    
    virtual DOMCDATASection* createCDATASection(WebCore::String data);
    
    virtual DOMProcessingInstruction* createProcessingInstruction(WebCore::String target, WebCore::String data);
    
    virtual DOMAttr* createAttribute(WebCore::String name);
    
    virtual DOMEntityReference* createEntityReference(WebCore::String name);*/
    
    virtual DOMNodeList* getElementsByTagName(WebCore::String tagName);
    
    virtual DOMNode* importNode(DOMNode *importedNode, bool deep);
    
    virtual DOMElement* createElementNS(WebCore::String namespaceURI, WebCore::String qualifiedName);
    
    //virtual DOMAttr* createAttributeNS(WebCore::String namespaceURI, WebCore::String qualifiedName);
    
    virtual DOMNodeList* getElementsByTagNameNS(WebCore::String namespaceURI, WebCore::String localName);
    
    virtual DOMElement* getElementById(WebCore::String elementId);

    // DOMViewCSS
    virtual DOMCSSStyleDeclaration* getComputedStyle(DOMElement *elt, WebCore::String pseudoElt);

    // DOMDocumentEvent
    virtual DOMEvent* createEvent(WebCore::String eventType);

    // DOMDocument
    WebCore::Document* document() { return m_document; }

protected:
    WebCore::Document* m_document;
};

class DOMElement : public DOMNode
{
protected:
    DOMElement(WebCore::Element* e);
    ~DOMElement();

public:
    static DOMElement* createInstance(WebCore::Element* e);

    // IWebScriptObject
    /**
     * throw exception
     * @discussion Throws an exception in the current script execution context.
        @result Either NO if an exception could not be raised, YES otherwise.
     */
    virtual bool throwException(WebCore::String exceptionMessage)
    { 
        return WebScriptObject::throwException(exceptionMessage); 
    }

    /**
     * remove web script key 
     * @param name The name of the method to call in the script environment.
        @param args The arguments to pass to the script environment.
        @discussion Calls the specified method in the script environment using the
        specified arguments.
        @result Returns the result of calling the script method.
     */
    virtual void removeWebScriptKey(WebCore::String name)
    {
        WebScriptObject::removeWebScriptKey(name);
    }

    /**
     *  stringRepresentation 
     * @discussion Converts the target object to a string representation.  The coercion
        of non string objects type is dependent on the script environment.
        @result Returns the string representation of the object.
     */
    virtual WebCore::String stringRepresentation()
    {
        return WebScriptObject::stringRepresentation();
    }

    /**
     *  setException 
     * @param description The description of the exception.
        @discussion Raises an exception in the script environment in the context of the
        current object.
     */
    virtual void setException(WebCore::String description)
    {
        WebScriptObject::setException(description);
    }
    
    // DOMNode
    virtual WebCore::String nodeName() { return DOMNode::nodeName(); }
    
    virtual WebCore::String nodeValue() { return DOMNode::nodeValue(); }
    
    virtual void setNodeValue(WebCore::String value) { DOMNode::setNodeValue(value); }
    
    virtual unsigned short nodeType() { return DOMNode::nodeType(); }
    
    virtual DOMNode* parentNode() { return DOMNode::parentNode(); }
    
    virtual DOMNodeList* childNodes() { return DOMNode::childNodes(); }
    
    virtual DOMNode* firstChild() { return DOMNode::firstChild(); }
    
    virtual DOMNode* lastChild() { return DOMNode::lastChild(); }
    
    virtual DOMNode* previousSibling() { return DOMNode::previousSibling(); }
    
    virtual DOMNode* nextSibling() { return DOMNode::nextSibling(); }
    
    //virtual DOMNamedNodeMap* attributes() { return DOMNode::attributes(); }
    
    virtual DOMDocument* ownerDocument() { return DOMNode::ownerDocument(); }
    
    virtual DOMNode* insertBefore(DOMNode *newChild, DOMNode *refChild) { return DOMNode::insertBefore(newChild, refChild); }
    
    virtual DOMNode* replaceChild(DOMNode *newChild, DOMNode *oldChild) { return DOMNode::replaceChild(newChild, oldChild); }
    
    virtual DOMNode* removeChild(DOMNode *oldChild) { return DOMNode::removeChild(oldChild); }
    
    virtual DOMNode* appendChild(DOMNode *oldChild) { return DOMNode::appendChild(oldChild); }
    
    virtual bool hasChildNodes() { return DOMNode::hasChildNodes(); }
    
    virtual DOMNode* cloneNode(bool deep) { return DOMNode::cloneNode(deep); }
    
    virtual void normalize() { DOMNode::normalize(); }
    
    virtual bool isSupported(WebCore::String feature, WebCore::String version) { return DOMNode::isSupported(feature, version); }
    
    virtual WebCore::String namespaceURI() { return DOMNode::namespaceURI(); }
    
    virtual WebCore::String prefix() { return DOMNode::prefix(); }
    
    virtual void setPrefix(WebCore::String prefix) { DOMNode::setPrefix(prefix); }
    
    virtual WebCore::String localName() { return DOMNode::localName(); }
    
    virtual bool hasAttributes() { return DOMNode::hasAttributes(); }

    virtual bool isSameNode(DOMNode* other) { return DOMNode::isSameNode(other); }
    
    virtual bool isEqualNode(DOMNode* other) { return DOMNode::isEqualNode(other); }
    
    virtual WebCore::String textContent() { return DOMNode::textContent(); }
    
    virtual void setTextContent(WebCore::String text) { DOMNode::setTextContent(text); }
    
    // IDOMElement
    virtual WebCore::String tagName();
    
    virtual WebCore::String getAttribute(WebCore::String name);
    
    virtual void setAttribute(WebCore::String name, WebCore::String value);
    
    virtual void removeAttribute(WebCore::String name);
    
    //virtual DOMAttr* getAttributeNode(WebCore::String name);
    
    //virtual DOMAttr* setAttributeNode(DOMAttr *newAttr);
    
    //virtual DOMAttr* removeAttributeNode(DOMAttr *oldAttr);
    
    virtual DOMNodeList* getElementsByTagName(WebCore::String name);
    
    virtual WebCore::String getAttributeNS(WebCore::String namespaceURI, WebCore::String localName);
    
    virtual void setAttributeNS(WebCore::String namespaceURI, WebCore::String qualifiedName, WebCore::String value);
    
    virtual void removeAttributeNS(WebCore::String namespaceURI, WebCore::String localName);
    
    //virtual DOMAttr* getAttributeNodeNS(WebCore::String namespaceURI, WebCore::String localName);
    
    //virtual DOMAttr* setAttributeNodeNS(DOMAttr *newAttr);
    
    virtual DOMNodeList* getElementsByTagNameNS(WebCore::String namespaceURI, WebCore::String localName);
    
    virtual bool hasAttribute(WebCore::String name);
    
    virtual bool hasAttributeNS(WebCore::String namespaceURI, WebCore::String localName);

    virtual void focus();
    
    virtual void blur();

    // DOMNodeExtensions
    virtual WebCore::IntRect boundingBox();
    
    virtual void lineBoxRects( WebCore::IntRect *rects, int cRects);

    // DOMElementPrivate
    virtual void* coreElement();

    virtual bool isEqual( DOMElement *other);

    virtual bool isFocused();

    virtual WebCore::String innerText();

    virtual WebFontDescription* font();

    // DOMElementCSSInlineStyle
    virtual DOMCSSStyleDeclaration* style();

    // DOMElementExtensions
    virtual int offsetLeft();
    
    virtual int offsetTop();
    
    virtual int offsetWidth();
    
    virtual int offsetHeight();
    
    virtual DOMElement* offsetParent();
    
    virtual int clientWidth();
    
    virtual int clientHeight();
    
    virtual int scrollLeft();
    
    virtual void setScrollLeft(int newScrollLeft);
    
    virtual int scrollTop();
    
    virtual void setScrollTop(int newScrollTop);
    
    virtual int scrollWidth();
    
    virtual int scrollHeight();
    
    virtual void scrollIntoView(bool alignWithTop);
    
    virtual void scrollIntoViewIfNeeded(bool centerIfNeeded);

    // DOMElement
    WebCore::Element* element() { return m_element; }

protected:
    WebCore::Element* m_element;
};

#endif
