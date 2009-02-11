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

#ifndef DOMHTMLClasses_H
#define DOMHTMLClasses_H

#include "DOMCoreClasses.h"
#include "WebScriptObject.h"
#include "IntRect.h"

#include <RefPtr.h>

namespace WebCore {
    class HTMLCollection;
}

class DOMHTMLElement;

class DOMHTMLCollection : public DOMObject
{
protected:
    DOMHTMLCollection(WebCore::HTMLCollection* c);

public:
    static DOMHTMLCollection* createInstance(WebCore::HTMLCollection*);

    // WebScriptObject
    /**
     * throw exception
     * @discussion Throws an exception in the current script execution context.
        @result Either NO if an exception could not be raised, YES otherwise.
     */
    virtual bool throwException(WebCore::String exceptionMessage)
    { 
        return DOMObject::throwException(exceptionMessage); 
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
        DOMObject::removeWebScriptKey(name);
    }

    /**
     *  stringRepresentation 
     * @discussion Converts the target object to a string representation.  The coercion
        of non string objects type is dependent on the script environment.
        @result Returns the string representation of the object.
     */
    virtual WebCore::String stringRepresentation()
    {
        return DOMObject::stringRepresentation();
    }

    /**
     *  setException 
     * @param description The description of the exception.
        @discussion Raises an exception in the script environment in the context of the
        current object.
     */
    virtual void setException(WebCore::String description)
    {
        DOMObject::setException(description);
    }

    // IDOMHTMLCollection
    virtual unsigned length();
    
    virtual DOMNode* item(unsigned index);
    
    virtual DOMNode* namedItem(WebCore::String name);

protected:
    RefPtr<WebCore::HTMLCollection> m_collection;
};

class DOMHTMLOptionsCollection : public DOMObject
{
    // IWebScriptObject
    /**
     * throw exception
     * @discussion Throws an exception in the current script execution context.
        @result Either NO if an exception could not be raised, YES otherwise.
     */
    virtual bool throwException(WebCore::String exceptionMessage)
    { 
        return DOMObject::throwException(exceptionMessage); 
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
        DOMObject::removeWebScriptKey(name);
    }

    /**
     *  stringRepresentation 
     * @discussion Converts the target object to a string representation.  The coercion
        of non string objects type is dependent on the script environment.
        @result Returns the string representation of the object.
     */
    virtual WebCore::String stringRepresentation()
    {
        return DOMObject::stringRepresentation();
    }

    /**
     *  setException 
     * @param description The description of the exception.
        @discussion Raises an exception in the script environment in the context of the
        current object.
     */
    virtual void setException(WebCore::String description)
    {
        DOMObject::setException(description);
    }

    // IDOMHTMLOptionsCollection
    virtual unsigned int length();
    
    virtual void setLength(unsigned int);
    
    virtual DOMNode* item(unsigned int index);
    
    virtual DOMNode* namedItem(WebCore::String name);
};

class DOMHTMLDocument : public DOMDocument
{
protected:
    DOMHTMLDocument();
public:
    DOMHTMLDocument(WebCore::Document* d) : DOMDocument(d) {}

    /**
     * throw exception
     * @discussion Throws an exception in the current script execution context.
        @result Either NO if an exception could not be raised, YES otherwise.
     */
    virtual bool throwException(WebCore::String exceptionMessage)
    { 
        return DOMObject::throwException(exceptionMessage); 
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
        DOMObject::removeWebScriptKey(name);
    }

    /**
     *  stringRepresentation 
     * @discussion Converts the target object to a string representation.  The coercion
        of non string objects type is dependent on the script environment.
        @result Returns the string representation of the object.
     */
    virtual WebCore::String stringRepresentation()
    {
        return DOMObject::stringRepresentation();
    }

    /**
     *  setException 
     * @param description The description of the exception.
        @discussion Raises an exception in the script environment in the context of the
        current object.
     */
    virtual void setException(WebCore::String description)
    {
        DOMObject::setException(description);
    }

    // IDOMNode
    virtual WebCore::String nodeName() { return DOMDocument::nodeName(); }
    
    virtual WebCore::String nodeValue() { return DOMDocument::nodeValue(); }
    
    virtual void setNodeValue(WebCore::String value) { DOMDocument::setNodeValue(value); }
    
    virtual unsigned short nodeType() { return DOMDocument::nodeType(); }
    
    virtual DOMNode* parentNode() { return DOMDocument::parentNode(); }
    
    virtual DOMNodeList* childNodes() { return DOMDocument::childNodes(); }
    
    virtual DOMNode* firstChild() { return DOMDocument::firstChild(); }
    
    virtual DOMNode* lastChild() { return DOMDocument::lastChild(); }
    
    virtual DOMNode* previousSibling() { return DOMDocument::previousSibling(); }
    
    virtual DOMNode* nextSibling() { return DOMDocument::nextSibling(); }
    
    //virtual DOMNamedNodeMap* attributes() { return DOMDocument::attributes(); }
    
    virtual DOMDocument* ownerDocument() { return DOMDocument::ownerDocument(); }
    
    virtual DOMNode* insertBefore(DOMNode *newChild, DOMNode *refChild) { return DOMDocument::insertBefore(newChild, refChild); }
    
    virtual DOMNode* replaceChild(DOMNode *newChild, DOMNode *oldChild) { return DOMDocument::replaceChild(newChild, oldChild); }
    
    virtual DOMNode* removeChild(DOMNode *oldChild) { return DOMDocument::removeChild(oldChild); }
    
    virtual DOMNode* appendChild(DOMNode *oldChild) { return DOMDocument::appendChild(oldChild); }
    
    virtual bool hasChildNodes() { return DOMDocument::hasChildNodes(); }
    
    virtual DOMNode* cloneNode(bool deep) { return DOMDocument::cloneNode(deep); }
    
    virtual void normalize() { DOMDocument::normalize(); }
    
    virtual bool isSupported(WebCore::String feature, WebCore::String version) { return DOMDocument::isSupported(feature, version); }
    
    virtual WebCore::String namespaceURI() { return DOMDocument::namespaceURI(); }
    
    virtual WebCore::String prefix() { return DOMDocument::prefix(); }
    
    virtual void setPrefix(WebCore::String prefix) { return DOMDocument::setPrefix(prefix); }
    
    virtual WebCore::String localName() { return DOMDocument::localName(); }
    
    virtual bool hasAttributes() { return DOMDocument::hasAttributes(); }

    virtual bool isSameNode(DOMNode* other) { return DOMDocument::isSameNode(other); }
    
    virtual bool isEqualNode(DOMNode* other) { return DOMDocument::isEqualNode(other); }
    
    virtual WebCore::String textContent() { return DOMDocument::textContent(); }
    
    virtual void setTextContent(WebCore::String text) { DOMDocument::setTextContent(text); }
    
    // IDOMDocument
    //virtual DOMDocumentType* doctype() { return DOMDocument::doctype(); }
    
    //virtual DOMImplementation* implementation() { return DOMDocument::implementation(); }
    
    virtual DOMElement* documentElement() { return DOMDocument::documentElement(); }
    
    virtual DOMElement* createElement(WebCore::String tagName) { return DOMDocument::createElement(tagName); }
    
    //virtual DOMDocumentFragment* createDocumentFragment() { return DOMDocument::createDocumentFragment(); }
    
    //virtual DOMText* createTextNode(WebCore::String data) { return DOMDocument::createTextNode(data); }
    
    //virtual DOMComment* createComment(WebCore::String data) { return DOMDocument::createComment(data); }
    
    //virtual DOMCDATASection* createCDATASection(WebCore::String data) { return DOMDocument::createCDATASection(data); }
    
    //virtual DOMProcessingInstruction* createProcessingInstruction(WebCore::String target, WebCore::String data) { return DOMDocument::createProcessingInstruction(target, data); }
    
    //virtual DOMAttr* createAttribute(WebCore::String name) { return DOMDocument::createAttribute(name); }
    
    //virtual DOMEntityReference* createEntityReference(WebCore::String name) { return DOMDocument::createEntityReference(name); }
    
    virtual DOMNodeList* getElementsByTagName(WebCore::String tagName) { return DOMDocument::getElementsByTagName(tagName); }
    
    virtual DOMNode* importNode(DOMNode *importedNode, bool deep) { return DOMDocument::importNode(importedNode, deep); }
    
    virtual DOMElement* createElementNS(WebCore::String namespaceURI, WebCore::String qualifiedName) { return DOMDocument::createElementNS(namespaceURI, qualifiedName); }
    
    //virtual DOMAttr* createAttributeNS(WebCore::String namespaceURI, WebCore::String qualifiedName) { return DOMDocument::createAttributeNS(namespaceURI, qualifiedName); }
    
    virtual DOMNodeList* getElementsByTagNameNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMDocument::getElementsByTagNameNS(namespaceURI, localName); }
    
    virtual DOMElement* getElementById(WebCore::String elementId) { return DOMDocument::getElementById(elementId); }

    // IDOMHTMLDocument
    virtual WebCore::String title();
    
    virtual void setTitle(WebCore::String);
    
    virtual WebCore::String referrer();
    
    virtual WebCore::String domain();
    
    virtual WebCore::String URL();
    
    virtual DOMHTMLElement* body();
    
    virtual void setBody(DOMHTMLElement *body);
    
    virtual DOMHTMLCollection* images();
    
    virtual DOMHTMLCollection* applets();
    
    virtual DOMHTMLCollection* links();
    
    virtual DOMHTMLCollection* forms();
    
    virtual DOMHTMLCollection* anchors();
    
    virtual WebCore::String cookie();
    
    virtual void setCookie(WebCore::String cookie);
    
    virtual void open();
    
    virtual void close();
    
    virtual void write(WebCore::String text);
    
    virtual void writeln(WebCore::String text);
    
    virtual DOMElement* getElementById_(WebCore::String elementId);
    
    virtual DOMNodeList* getElementsByName(WebCore::String elementName);
};

class DOMHTMLElement : public DOMElement
{
protected:
    DOMHTMLElement();
public:
    DOMHTMLElement(WebCore::Element* e) : DOMElement(e) {}

    /**
     * throw exception
     * @discussion Throws an exception in the current script execution context.
        @result Either NO if an exception could not be raised, YES otherwise.
     */
    virtual bool throwException(WebCore::String exceptionMessage)
    { 
        return DOMObject::throwException(exceptionMessage); 
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
        DOMObject::removeWebScriptKey(name);
    }

    /**
     *  stringRepresentation 
     * @discussion Converts the target object to a string representation.  The coercion
        of non string objects type is dependent on the script environment.
        @result Returns the string representation of the object.
     */
    virtual WebCore::String stringRepresentation()
    {
        return DOMObject::stringRepresentation();
    }

    /**
     *  setException 
     * @param description The description of the exception.
        @discussion Raises an exception in the script environment in the context of the
        current object.
     */
    virtual void setException(WebCore::String description)
    {
        DOMObject::setException(description);
    }

    // IDOMNode
    virtual WebCore::String nodeName() { return DOMElement::nodeName(); }
    
    virtual WebCore::String nodeValue() { return DOMElement::nodeValue(); }
    
    virtual void setNodeValue(WebCore::String value) { DOMElement::setNodeValue(value); }
    
    virtual unsigned short nodeType() { return DOMElement::nodeType(); }
    
    virtual DOMNode* parentNode() { return DOMElement::parentNode(); }
    
    virtual DOMNodeList* childNodes() { return DOMElement::childNodes(); }
    
    virtual DOMNode* firstChild() { return DOMElement::firstChild(); }
    
    virtual DOMNode* lastChild() { return DOMElement::lastChild(); }
    
    virtual DOMNode* previousSibling() { return DOMElement::previousSibling(); }
    
    virtual DOMNode* nextSibling() { return DOMElement::nextSibling(); }
    
//    virtual DOMNamedNodeMap* attributes() { return DOMElement::attributes(); }
    
    virtual DOMDocument* ownerDocument() { return DOMElement::ownerDocument(); }
    
    virtual DOMNode* insertBefore(DOMNode *newChild, DOMNode *refChild) { return DOMElement::insertBefore(newChild, refChild); }
    
    virtual DOMNode* replaceChild(DOMNode *newChild, DOMNode *oldChild) { return DOMElement::replaceChild(newChild, oldChild); }
    
    virtual DOMNode* removeChild(DOMNode *oldChild) { return DOMElement::removeChild(oldChild); }
    
    virtual DOMNode* appendChild(DOMNode *oldChild) { return DOMElement::appendChild(oldChild); }
    
    virtual bool hasChildNodes() { return DOMElement::hasChildNodes(); }
    
    virtual DOMNode* cloneNode(bool deep) { return DOMElement::cloneNode(deep); }
    
    virtual void normalize() { DOMElement::normalize(); }
    
    virtual bool isSupported(WebCore::String feature, WebCore::String version) { return DOMElement::isSupported(feature, version); }
    
    virtual WebCore::String namespaceURI() { return DOMElement::namespaceURI(); }
    
    virtual WebCore::String prefix() { return DOMElement::prefix(); }
    
    virtual void setPrefix(WebCore::String prefix) { return DOMElement::setPrefix(prefix); }
    
    virtual WebCore::String localName() { return DOMElement::localName(); }
    
    virtual bool hasAttributes() { return DOMElement::hasAttributes(); }

    virtual bool isSameNode(DOMNode* other) { return DOMElement::isSameNode(other); }
    
    virtual bool isEqualNode(DOMNode* other) { return DOMElement::isEqualNode(other); }
    
    virtual WebCore::String textContent() { return DOMElement::textContent(); }
    
    virtual void setTextContent(WebCore::String text) { DOMElement::setTextContent(text); }
    
    // IDOMElement
    virtual WebCore::String tagName() { return DOMElement::tagName(); }
    
    virtual WebCore::String getAttribute(WebCore::String name) { return DOMElement::getAttribute(name); }
    
    virtual void setAttribute(WebCore::String name, WebCore::String value) { DOMElement::setAttribute(name, value); }
    
    virtual void removeAttribute(WebCore::String name) { DOMElement::removeAttribute(name); }
    
    //virtual DOMAttr* getAttributeNode(WebCore::String name) { return DOMElement::getAttributeNode(name); }
    
    //virtual DOMAttr* setAttributeNode(DOMAttr *newAttr) { return DOMElement::setAttributeNode(newAttr); }
    
    //virtual DOMAttr* removeAttributeNode(DOMAttr *oldAttr) { return DOMElement::removeAttributeNode(oldAttr); }
    
    virtual DOMNodeList* getElementsByTagName(WebCore::String name) { return DOMElement::getElementsByTagName(name); }
    
    virtual WebCore::String getAttributeNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMElement::getAttributeNS(namespaceURI, localName); }
    
    virtual void setAttributeNS(WebCore::String namespaceURI, WebCore::String qualifiedName, WebCore::String value) { DOMElement::setAttributeNS(namespaceURI, qualifiedName, value); }
    
    virtual void removeAttributeNS(WebCore::String namespaceURI, WebCore::String localName) { DOMElement::removeAttributeNS(namespaceURI, localName); }
    
    //virtual DOMAttr* getAttributeNodeNS(WebCore::String namespaceURI, WebCore:String localName) { return DOMElement::getAttributeNodeNS(namespaceURI, localName); }
    
    //virtual DOMAttr* setAttributeNodeNS(DOMAttr *newAttr) { return DOMElement::setAttributeNodeNS(newAttr); }
    
    virtual DOMNodeList* getElementsByTagNameNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMElement::getElementsByTagNameNS(namespaceURI, localName); }
    
    virtual bool hasAttribute(WebCore::String name) { return DOMElement::hasAttribute(name); }
    
    virtual bool hasAttributeNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMElement::hasAttributeNS(namespaceURI, localName); }

    virtual void focus() { DOMElement::focus(); }
    
    virtual void blur() { DOMElement::blur(); }

    // IDOMHTMLElement
    virtual WebCore::String idName();
    
    virtual void setIdName(WebCore::String idName);
    
    virtual WebCore::String title();
    
    virtual void setTitle(WebCore::String title);
    
    virtual WebCore::String lang();
    
    virtual void setLang(WebCore::String lang);
    
    virtual WebCore::String dir();
    
    virtual void setDir(WebCore::String dir);
    
    virtual WebCore::String className();
    
    virtual void setClassName(WebCore::String className);

    virtual WebCore::String innerHTML();
        
    virtual void setInnerHTML(WebCore::String html);
        
    virtual WebCore::String innerText();
        
    virtual void setInnerText(WebCore::String text);        

};

class DOMHTMLFormElement : public DOMHTMLElement
{
protected:
    DOMHTMLFormElement();
public:
    DOMHTMLFormElement(WebCore::Element* e) : DOMHTMLElement(e) {}

    /**
     * throw exception
     * @discussion Throws an exception in the current script execution context.
        @result Either NO if an exception could not be raised, YES otherwise.
     */
    virtual bool throwException(WebCore::String exceptionMessage)
    { 
        return DOMObject::throwException(exceptionMessage); 
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
        DOMObject::removeWebScriptKey(name);
    }

    /**
     *  stringRepresentation 
     * @discussion Converts the target object to a string representation.  The coercion
        of non string objects type is dependent on the script environment.
        @result Returns the string representation of the object.
     */
    virtual WebCore::String stringRepresentation()
    {
        return DOMObject::stringRepresentation();
    }

    /**
     *  setException 
     * @param description The description of the exception.
        @discussion Raises an exception in the script environment in the context of the
        current object.
     */
    virtual void setException(WebCore::String description)
    {
        DOMObject::setException(description);
    }

    // IDOMNode
    virtual WebCore::String nodeName() { return DOMHTMLElement::nodeName(); }
    
    virtual WebCore::String nodeValue() { return DOMHTMLElement::nodeValue(); }
    
    virtual void setNodeValue(WebCore::String value) { DOMHTMLElement::setNodeValue(value); }
    
    virtual unsigned short nodeType() { return DOMHTMLElement::nodeType(); }
    
    virtual DOMNode* parentNode() { return DOMHTMLElement::parentNode(); }
    
    virtual DOMNodeList* childNodes() { return DOMHTMLElement::childNodes(); }
    
    virtual DOMNode* firstChild() { return DOMHTMLElement::firstChild(); }
    
    virtual DOMNode* lastChild() { return DOMHTMLElement::lastChild(); }
    
    virtual DOMNode* previousSibling() { return DOMHTMLElement::previousSibling(); }
    
    virtual DOMNode* nextSibling() { return DOMHTMLElement::nextSibling(); }
    
    //virtual DOMNamedNodeMap* attributes() { return DOMHTMLElement::attributes(); }
    
    virtual DOMDocument* ownerDocument() { return DOMHTMLElement::ownerDocument(); }
    
    virtual DOMNode* insertBefore(DOMNode *newChild, DOMNode *refChild) { return DOMHTMLElement::insertBefore(newChild, refChild); }
    
    virtual DOMNode* replaceChild(DOMNode *newChild, DOMNode *oldChild) { return DOMHTMLElement::replaceChild(newChild, oldChild); }
    
    virtual DOMNode* removeChild(DOMNode *oldChild) { return DOMHTMLElement::removeChild(oldChild); }
    
    virtual DOMNode* appendChild(DOMNode *oldChild) { return DOMHTMLElement::appendChild(oldChild); }
    
    virtual bool hasChildNodes() { return DOMHTMLElement::hasChildNodes(); }
    
    virtual DOMNode* cloneNode(bool deep) { return DOMHTMLElement::cloneNode(deep); }
    
    virtual void normalize() { DOMHTMLElement::normalize(); }
    
    virtual bool isSupported(WebCore::String feature, WebCore::String version) { return DOMHTMLElement::isSupported(feature, version); }
    
    virtual WebCore::String namespaceURI() { return DOMHTMLElement::namespaceURI(); }
    
    virtual WebCore::String prefix() { return DOMHTMLElement::prefix(); }
    
    virtual void setPrefix(WebCore::String prefix) { return DOMHTMLElement::setPrefix(prefix); }
    
    virtual WebCore::String localName() { return DOMHTMLElement::localName(); }
    
    virtual bool hasAttributes() { return DOMHTMLElement::hasAttributes(); }

    virtual bool isSameNode(DOMNode* other) { return DOMHTMLElement::isSameNode(other); }
    
    virtual bool isEqualNode(DOMNode* other) { return DOMHTMLElement::isEqualNode(other); }
    
    virtual WebCore::String textContent() { return DOMHTMLElement::textContent(); }
    
    virtual void setTextContent(WebCore::String text) { DOMHTMLElement::setTextContent(text); }
    
    // IDOMElement
    virtual WebCore::String tagName() { return DOMHTMLElement::tagName(); }
    
    virtual WebCore::String getAttribute(WebCore::String name) { return DOMHTMLElement::getAttribute(name); }
    
    virtual void setAttribute(WebCore::String name, WebCore::String value) { DOMHTMLElement::setAttribute(name, value); }
    
    virtual void removeAttribute(WebCore::String name) { DOMHTMLElement::removeAttribute(name); }
    
    //virtual DOMAttr* getAttributeNode(WebCore::String name) { return DOMHTMLElement::getAttributeNode(name); }
    
    //virtual DOMAttr* setAttributeNode(DOMAttr *newAttr) { return DOMHTMLElement::setAttributeNode(newAttr); }
    
    //virtual DOMAttr* removeAttributeNode(DOMAttr *oldAttr) { return DOMHTMLElement::removeAttributeNode(oldAttr); }
    
    virtual DOMNodeList* getElementsByTagName(WebCore::String name) { return DOMHTMLElement::getElementsByTagName(name); }
    
    virtual WebCore::String getAttributeNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMHTMLElement::getAttributeNS(namespaceURI, localName); }
    
    virtual void setAttributeNS(WebCore::String namespaceURI, WebCore::String qualifiedName, WebCore::String value) { DOMHTMLElement::setAttributeNS(namespaceURI, qualifiedName, value); }
    
    virtual void removeAttributeNS(WebCore::String namespaceURI, WebCore::String localName) { DOMHTMLElement::removeAttributeNS(namespaceURI, localName); }
    
    //virtual DOMAttr* getAttributeNodeNS(WebCore::String namespaceURI, WebCore:String localName) { return DOMHTMLElement::getAttributeNodeNS(namespaceURI, localName); }
    
    //virtual DOMAttr* setAttributeNodeNS(DOMAttr *newAttr) { return DOMHTMLElement::setAttributeNodeNS(newAttr); }
    
    virtual DOMNodeList* getElementsByTagNameNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMHTMLElement::getElementsByTagNameNS(namespaceURI, localName); }
    
    virtual bool hasAttribute(WebCore::String name) { return DOMHTMLElement::hasAttribute(name); }
    
    virtual bool hasAttributeNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMHTMLElement::hasAttributeNS(namespaceURI, localName); }

    virtual void focus() { DOMHTMLElement::focus(); }
    
    virtual void blur() { DOMHTMLElement::blur(); }


    // IDOMHTMLElement
    virtual WebCore::String idName() { return DOMHTMLElement::idName(); }
    
    virtual void setIdName(WebCore::String idName) { DOMHTMLElement::setIdName(idName); }
    
    virtual WebCore::String title() { return DOMHTMLElement::title(); }
    
    virtual void setTitle(WebCore::String title) { DOMHTMLElement::setTitle(title); }
    
    virtual WebCore::String lang() { return DOMHTMLElement::lang(); }
    
    virtual void setLang(WebCore::String lang) { DOMHTMLElement::setLang(lang); }
    
    virtual WebCore::String dir() { return DOMHTMLElement::dir(); }
    
    virtual void setDir(WebCore::String dir) { DOMHTMLElement::setDir(dir); }
    
    virtual WebCore::String className() { return DOMHTMLElement::className(); }
    
    virtual void setClassName(WebCore::String className) { DOMHTMLElement::setClassName(className); }

    virtual WebCore::String innerHTML() { return DOMHTMLElement::innerHTML(); }
        
    virtual void setInnerHTML(WebCore::String html) { DOMHTMLElement::setInnerHTML(html); }
        
    virtual WebCore::String innerText() { return DOMHTMLElement::innerText(); }
        
    virtual void setInnerText(WebCore::String text) { DOMHTMLElement::setInnerText(text); }

    // IDOMHTMLFormElement
    virtual DOMHTMLCollection* elements();
    
    virtual int length();
    
    virtual WebCore::String name();
    
    virtual void setName(WebCore::String name);
    
    virtual WebCore::String acceptCharset();
    
    virtual void setAcceptCharset(WebCore::String acceptCharset);
    
    virtual WebCore::String action();
    
    virtual void setAction(WebCore::String action);
    
    virtual WebCore::String encType();
    
    virtual WebCore::String setEnctype();
    
    virtual WebCore::String method();
    
    virtual void setMethod(WebCore::String method);
    
    virtual WebCore::String target();
    
    virtual void setTarget(WebCore::String target);
    
    virtual void submit();
    
    virtual void reset();
};

class DOMHTMLSelectElement : public DOMHTMLElement
{
protected:
    DOMHTMLSelectElement();
public:
    DOMHTMLSelectElement(WebCore::Element* e) : DOMHTMLElement(e) {}

    /**
     * throw exception
     * @discussion Throws an exception in the current script execution context.
        @result Either NO if an exception could not be raised, YES otherwise.
     */
    virtual bool throwException(WebCore::String exceptionMessage)
    { 
        return DOMObject::throwException(exceptionMessage); 
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
        DOMObject::removeWebScriptKey(name);
    }

    /**
     *  stringRepresentation 
     * @discussion Converts the target object to a string representation.  The coercion
        of non string objects type is dependent on the script environment.
        @result Returns the string representation of the object.
     */
    virtual WebCore::String stringRepresentation()
    {
        return DOMObject::stringRepresentation();
    }

    /**
     *  setException 
     * @param description The description of the exception.
        @discussion Raises an exception in the script environment in the context of the
        current object.
     */
    virtual void setException(WebCore::String description)
    {
        DOMObject::setException(description);
    }

    // IDOMNode
    virtual WebCore::String nodeName() { return DOMHTMLElement::nodeName(); }
    
    virtual WebCore::String nodeValue() { return DOMHTMLElement::nodeValue(); }
    
    virtual void setNodeValue(WebCore::String value) { DOMHTMLElement::setNodeValue(value); }
    
    virtual unsigned short nodeType() { return DOMHTMLElement::nodeType(); }
    
    virtual DOMNode* parentNode() { return DOMHTMLElement::parentNode(); }
    
    virtual DOMNodeList* childNodes() { return DOMHTMLElement::childNodes(); }
    
    virtual DOMNode* firstChild() { return DOMHTMLElement::firstChild(); }
    
    virtual DOMNode* lastChild() { return DOMHTMLElement::lastChild(); }
    
    virtual DOMNode* previousSibling() { return DOMHTMLElement::previousSibling(); }
    
    virtual DOMNode* nextSibling() { return DOMHTMLElement::nextSibling(); }
    
    //virtual DOMNamedNodeMap* attributes() { return DOMHTMLElement::attributes(); }
    
    virtual DOMDocument* ownerDocument() { return DOMHTMLElement::ownerDocument(); }
    
    virtual DOMNode* insertBefore(DOMNode *newChild, DOMNode *refChild) { return DOMHTMLElement::insertBefore(newChild, refChild); }
    
    virtual DOMNode* replaceChild(DOMNode *newChild, DOMNode *oldChild) { return DOMHTMLElement::replaceChild(newChild, oldChild); }
    
    virtual DOMNode* removeChild(DOMNode *oldChild) { return DOMHTMLElement::removeChild(oldChild); }
    
    virtual DOMNode* appendChild(DOMNode *oldChild) { return DOMHTMLElement::appendChild(oldChild); }
    
    virtual bool hasChildNodes() { return DOMHTMLElement::hasChildNodes(); }
    
    virtual DOMNode* cloneNode(bool deep) { return DOMHTMLElement::cloneNode(deep); }
    
    virtual void normalize() { DOMHTMLElement::normalize(); }
    
    virtual bool isSupported(WebCore::String feature, WebCore::String version) { return DOMHTMLElement::isSupported(feature, version); }
    
    virtual WebCore::String namespaceURI() { return DOMHTMLElement::namespaceURI(); }
    
    virtual WebCore::String prefix() { return DOMHTMLElement::prefix(); }
    
    virtual void setPrefix(WebCore::String prefix) { return DOMHTMLElement::setPrefix(prefix); }
    
    virtual WebCore::String localName() { return DOMHTMLElement::localName(); }
    
    virtual bool hasAttributes() { return DOMHTMLElement::hasAttributes(); }

    virtual bool isSameNode(DOMNode* other) { return DOMHTMLElement::isSameNode(other); }
    
    virtual bool isEqualNode(DOMNode* other) { return DOMHTMLElement::isEqualNode(other); }
    
    virtual WebCore::String textContent() { return DOMHTMLElement::textContent(); }
    
    virtual void setTextContent(WebCore::String text) { DOMHTMLElement::setTextContent(text); }
    
    // IDOMElement
    virtual WebCore::String tagName() { return DOMHTMLElement::tagName(); }
    
    virtual WebCore::String getAttribute(WebCore::String name) { return DOMHTMLElement::getAttribute(name); }
    
    virtual void setAttribute(WebCore::String name, WebCore::String value) { DOMHTMLElement::setAttribute(name, value); }
    
    virtual void removeAttribute(WebCore::String name) { DOMHTMLElement::removeAttribute(name); }
    
    //virtual DOMAttr* getAttributeNode(WebCore::String name) { return DOMHTMLElement::getAttributeNode(name); }
    
    //virtual DOMAttr* setAttributeNode(DOMAttr *newAttr) { return DOMHTMLElement::setAttributeNode(newAttr); }
    
    //virtual DOMAttr* removeAttributeNode(DOMAttr *oldAttr) { return DOMHTMLElement::removeAttributeNode(oldAttr); }
    
    virtual DOMNodeList* getElementsByTagName(WebCore::String name) { return DOMHTMLElement::getElementsByTagName(name); }
    
    virtual WebCore::String getAttributeNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMHTMLElement::getAttributeNS(namespaceURI, localName); }
    
    virtual void setAttributeNS(WebCore::String namespaceURI, WebCore::String qualifiedName, WebCore::String value) { DOMHTMLElement::setAttributeNS(namespaceURI, qualifiedName, value); }
    
    virtual void removeAttributeNS(WebCore::String namespaceURI, WebCore::String localName) { DOMHTMLElement::removeAttributeNS(namespaceURI, localName); }
    
    //virtual DOMAttr* getAttributeNodeNS(WebCore::String namespaceURI, WebCore:String localName) { return DOMHTMLElement::getAttributeNodeNS(namespaceURI, localName); }
    
    //virtual DOMAttr* setAttributeNodeNS(DOMAttr *newAttr) { return DOMHTMLElement::setAttributeNodeNS(newAttr); }
    
    virtual DOMNodeList* getElementsByTagNameNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMHTMLElement::getElementsByTagNameNS(namespaceURI, localName); }
    
    virtual bool hasAttribute(WebCore::String name) { return DOMHTMLElement::hasAttribute(name); }
    
    virtual bool hasAttributeNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMHTMLElement::hasAttributeNS(namespaceURI, localName); }

    virtual void focus() { DOMHTMLElement::focus(); }
    
    virtual void blur() { DOMHTMLElement::blur(); }


    // IDOMHTMLElement
    virtual WebCore::String idName() { return DOMHTMLElement::idName(); }
    
    virtual void setIdName(WebCore::String idName) { DOMHTMLElement::setIdName(idName); }
    
    virtual WebCore::String title() { return DOMHTMLElement::title(); }
    
    virtual void setTitle(WebCore::String title) { DOMHTMLElement::setTitle(title); }
    
    virtual WebCore::String lang() { return DOMHTMLElement::lang(); }
    
    virtual void setLang(WebCore::String lang) { DOMHTMLElement::setLang(lang); }
    
    virtual WebCore::String dir() { return DOMHTMLElement::dir(); }
    
    virtual void setDir(WebCore::String dir) { DOMHTMLElement::setDir(dir); }
    
    virtual WebCore::String className() { return DOMHTMLElement::className(); }
    
    virtual void setClassName(WebCore::String className) { DOMHTMLElement::setClassName(className); }

    virtual WebCore::String innerHTML() { return DOMHTMLElement::innerHTML(); }
        
    virtual void setInnerHTML(WebCore::String html) { DOMHTMLElement::setInnerHTML(html); }
        
    virtual WebCore::String innerText() { return DOMHTMLElement::innerText(); }
        
    virtual void setInnerText(WebCore::String text) { DOMHTMLElement::setInnerText(text); }
    
    // DOMHTMLSelectElement
    virtual WebCore::String type();
    
    virtual int selectedIndex();
    
    virtual void setSelectedIndx(int selectedIndex);
    
    virtual WebCore::String value();
    
    virtual void setValue(WebCore::String value);
    
    virtual int length();
    
    virtual DOMHTMLFormElement* form();
    
    virtual DOMHTMLOptionsCollection* options();
    
    virtual bool disabled();
    
    virtual void setDisabled(bool);
    
    virtual bool multiple();
    
    virtual void setMultiple(bool multiple);
    
    virtual WebCore::String name();
    
    virtual void setName(WebCore::String name);
    
    virtual int size();
    
    virtual void setSize(int size);
    
    virtual int tabIndex();
    
    virtual void setTabIndex(int tabIndex);
    
    virtual void add(DOMHTMLElement *element, DOMHTMLElement *before);
    
    virtual void remove(int index);
    
    // IFormsAutoFillTransitionSelect
    virtual void activateItemAtIndex(int index);
};

class DOMHTMLOptionElement : public DOMHTMLElement
{
protected:
    DOMHTMLOptionElement();
public:
    DOMHTMLOptionElement(WebCore::Element* e) : DOMHTMLElement(e) {}

    /**
     * throw exception
     * @discussion Throws an exception in the current script execution context.
        @result Either NO if an exception could not be raised, YES otherwise.
     */
    virtual bool throwException(WebCore::String exceptionMessage)
    { 
        return DOMObject::throwException(exceptionMessage); 
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
        DOMObject::removeWebScriptKey(name);
    }

    /**
     *  stringRepresentation 
     * @discussion Converts the target object to a string representation.  The coercion
        of non string objects type is dependent on the script environment.
        @result Returns the string representation of the object.
     */
    virtual WebCore::String stringRepresentation()
    {
        return DOMObject::stringRepresentation();
    }

    /**
     *  setException 
     * @param description The description of the exception.
        @discussion Raises an exception in the script environment in the context of the
        current object.
     */
    virtual void setException(WebCore::String description)
    {
        DOMObject::setException(description);
    }

    // IDOMNode
    virtual WebCore::String nodeName() { return DOMHTMLElement::nodeName(); }
    
    virtual WebCore::String nodeValue() { return DOMHTMLElement::nodeValue(); }
    
    virtual void setNodeValue(WebCore::String value) { DOMHTMLElement::setNodeValue(value); }
    
    virtual unsigned short nodeType() { return DOMHTMLElement::nodeType(); }
    
    virtual DOMNode* parentNode() { return DOMHTMLElement::parentNode(); }
    
    virtual DOMNodeList* childNodes() { return DOMHTMLElement::childNodes(); }
    
    virtual DOMNode* firstChild() { return DOMHTMLElement::firstChild(); }
    
    virtual DOMNode* lastChild() { return DOMHTMLElement::lastChild(); }
    
    virtual DOMNode* previousSibling() { return DOMHTMLElement::previousSibling(); }
    
    virtual DOMNode* nextSibling() { return DOMHTMLElement::nextSibling(); }
    
    //virtual DOMNamedNodeMap* attributes() { return DOMHTMLElement::attributes(); }
    
    virtual DOMDocument* ownerDocument() { return DOMHTMLElement::ownerDocument(); }
    
    virtual DOMNode* insertBefore(DOMNode *newChild, DOMNode *refChild) { return DOMHTMLElement::insertBefore(newChild, refChild); }
    
    virtual DOMNode* replaceChild(DOMNode *newChild, DOMNode *oldChild) { return DOMHTMLElement::replaceChild(newChild, oldChild); }
    
    virtual DOMNode* removeChild(DOMNode *oldChild) { return DOMHTMLElement::removeChild(oldChild); }
    
    virtual DOMNode* appendChild(DOMNode *oldChild) { return DOMHTMLElement::appendChild(oldChild); }
    
    virtual bool hasChildNodes() { return DOMHTMLElement::hasChildNodes(); }
    
    virtual DOMNode* cloneNode(bool deep) { return DOMHTMLElement::cloneNode(deep); }
    
    virtual void normalize() { DOMHTMLElement::normalize(); }
    
    virtual bool isSupported(WebCore::String feature, WebCore::String version) { return DOMHTMLElement::isSupported(feature, version); }
    
    virtual WebCore::String namespaceURI() { return DOMHTMLElement::namespaceURI(); }
    
    virtual WebCore::String prefix() { return DOMHTMLElement::prefix(); }
    
    virtual void setPrefix(WebCore::String prefix) { return DOMHTMLElement::setPrefix(prefix); }
    
    virtual WebCore::String localName() { return DOMHTMLElement::localName(); }
    
    virtual bool hasAttributes() { return DOMHTMLElement::hasAttributes(); }

    virtual bool isSameNode(DOMNode* other) { return DOMHTMLElement::isSameNode(other); }
    
    virtual bool isEqualNode(DOMNode* other) { return DOMHTMLElement::isEqualNode(other); }
    
    virtual WebCore::String textContent() { return DOMHTMLElement::textContent(); }
    
    virtual void setTextContent(WebCore::String text) { DOMHTMLElement::setTextContent(text); }
    
    // IDOMElement
    virtual WebCore::String tagName() { return DOMHTMLElement::tagName(); }
    
    virtual WebCore::String getAttribute(WebCore::String name) { return DOMHTMLElement::getAttribute(name); }
    
    virtual void setAttribute(WebCore::String name, WebCore::String value) { DOMHTMLElement::setAttribute(name, value); }
    
    virtual void removeAttribute(WebCore::String name) { DOMHTMLElement::removeAttribute(name); }
    
    //virtual DOMAttr* getAttributeNode(WebCore::String name) { return DOMHTMLElement::getAttributeNode(name); }
    
    //virtual DOMAttr* setAttributeNode(DOMAttr *newAttr) { return DOMHTMLElement::setAttributeNode(newAttr); }
    
    //virtual DOMAttr* removeAttributeNode(DOMAttr *oldAttr) { return DOMHTMLElement::removeAttributeNode(oldAttr); }
    
    virtual DOMNodeList* getElementsByTagName(WebCore::String name) { return DOMHTMLElement::getElementsByTagName(name); }
    
    virtual WebCore::String getAttributeNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMHTMLElement::getAttributeNS(namespaceURI, localName); }
    
    virtual void setAttributeNS(WebCore::String namespaceURI, WebCore::String qualifiedName, WebCore::String value) { DOMHTMLElement::setAttributeNS(namespaceURI, qualifiedName, value); }
    
    virtual void removeAttributeNS(WebCore::String namespaceURI, WebCore::String localName) { DOMHTMLElement::removeAttributeNS(namespaceURI, localName); }
    
    //virtual DOMAttr* getAttributeNodeNS(WebCore::String namespaceURI, WebCore:String localName) { return DOMHTMLElement::getAttributeNodeNS(namespaceURI, localName); }
    
    //virtual DOMAttr* setAttributeNodeNS(DOMAttr *newAttr) { return DOMHTMLElement::setAttributeNodeNS(newAttr); }
    
    virtual DOMNodeList* getElementsByTagNameNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMHTMLElement::getElementsByTagNameNS(namespaceURI, localName); }
    
    virtual bool hasAttribute(WebCore::String name) { return DOMHTMLElement::hasAttribute(name); }
    
    virtual bool hasAttributeNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMHTMLElement::hasAttributeNS(namespaceURI, localName); }

    virtual void focus() { DOMHTMLElement::focus(); }
    
    virtual void blur() { DOMHTMLElement::blur(); }


    // IDOMHTMLElement
    virtual WebCore::String idName() { return DOMHTMLElement::idName(); }
    
    virtual void setIdName(WebCore::String idName) { DOMHTMLElement::setIdName(idName); }
    
    virtual WebCore::String title() { return DOMHTMLElement::title(); }
    
    virtual void setTitle(WebCore::String title) { DOMHTMLElement::setTitle(title); }
    
    virtual WebCore::String lang() { return DOMHTMLElement::lang(); }
    
    virtual void setLang(WebCore::String lang) { DOMHTMLElement::setLang(lang); }
    
    virtual WebCore::String dir() { return DOMHTMLElement::dir(); }
    
    virtual void setDir(WebCore::String dir) { DOMHTMLElement::setDir(dir); }
    
    virtual WebCore::String className() { return DOMHTMLElement::className(); }
    
    virtual void setClassName(WebCore::String className) { DOMHTMLElement::setClassName(className); }

    virtual WebCore::String innerHTML() { return DOMHTMLElement::innerHTML(); }
        
    virtual void setInnerHTML(WebCore::String html) { DOMHTMLElement::setInnerHTML(html); }
        
    virtual WebCore::String innerText() { return DOMHTMLElement::innerText(); }
        
    virtual void setInnerText(WebCore::String text) { DOMHTMLElement::setInnerText(text); }

    // IDOMHTMLOptionElement
    virtual DOMHTMLFormElement* form();
    
    virtual bool defaultSelected();
    
    virtual void setDefaultSelected(bool defaultSelected);
    
    virtual WebCore::String text();
    
    virtual int index();
    
    virtual bool disabled();
    
    virtual void setDisabled(bool disabled);
    
    virtual WebCore::String label();
    
    virtual void setLabel(WebCore::String label);
    
    virtual bool selected();
    
    virtual void setSelected(bool selected);
    
    virtual WebCore::String value();
    
    virtual void setValue(WebCore::String value);
};

class DOMHTMLInputElement : public DOMHTMLElement
{
protected:
    DOMHTMLInputElement();
public:
    DOMHTMLInputElement(WebCore::Element* e) : DOMHTMLElement(e) {}

    /**
     * throw exception
     * @discussion Throws an exception in the current script execution context.
        @result Either NO if an exception could not be raised, YES otherwise.
     */
    virtual bool throwException(WebCore::String exceptionMessage)
    { 
        return DOMObject::throwException(exceptionMessage); 
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
        DOMObject::removeWebScriptKey(name);
    }

    /**
     *  stringRepresentation 
     * @discussion Converts the target object to a string representation.  The coercion
        of non string objects type is dependent on the script environment.
        @result Returns the string representation of the object.
     */
    virtual WebCore::String stringRepresentation()
    {
        return DOMObject::stringRepresentation();
    }

    /**
     *  setException 
     * @param description The description of the exception.
        @discussion Raises an exception in the script environment in the context of the
        current object.
     */
    virtual void setException(WebCore::String description)
    {
        DOMObject::setException(description);
    }
    // IDOMNode
    virtual WebCore::String nodeName() { return DOMHTMLElement::nodeName(); }
    
    virtual WebCore::String nodeValue() { return DOMHTMLElement::nodeValue(); }
    
    virtual void setNodeValue(WebCore::String value) { DOMHTMLElement::setNodeValue(value); }
    
    virtual unsigned short nodeType() { return DOMHTMLElement::nodeType(); }
    
    virtual DOMNode* parentNode() { return DOMHTMLElement::parentNode(); }
    
    virtual DOMNodeList* childNodes() { return DOMHTMLElement::childNodes(); }
    
    virtual DOMNode* firstChild() { return DOMHTMLElement::firstChild(); }
    
    virtual DOMNode* lastChild() { return DOMHTMLElement::lastChild(); }
    
    virtual DOMNode* previousSibling() { return DOMHTMLElement::previousSibling(); }
    
    virtual DOMNode* nextSibling() { return DOMHTMLElement::nextSibling(); }
    
    //virtual DOMNamedNodeMap* attributes() { return DOMHTMLElement::attributes(); }
    
    virtual DOMDocument* ownerDocument() { return DOMHTMLElement::ownerDocument(); }
    
    virtual DOMNode* insertBefore(DOMNode *newChild, DOMNode *refChild) { return DOMHTMLElement::insertBefore(newChild, refChild); }
    
    virtual DOMNode* replaceChild(DOMNode *newChild, DOMNode *oldChild) { return DOMHTMLElement::replaceChild(newChild, oldChild); }
    
    virtual DOMNode* removeChild(DOMNode *oldChild) { return DOMHTMLElement::removeChild(oldChild); }
    
    virtual DOMNode* appendChild(DOMNode *oldChild) { return DOMHTMLElement::appendChild(oldChild); }
    
    virtual bool hasChildNodes() { return DOMHTMLElement::hasChildNodes(); }
    
    virtual DOMNode* cloneNode(bool deep) { return DOMHTMLElement::cloneNode(deep); }
    
    virtual void normalize() { DOMHTMLElement::normalize(); }
    
    virtual bool isSupported(WebCore::String feature, WebCore::String version) { return DOMHTMLElement::isSupported(feature, version); }
    
    virtual WebCore::String namespaceURI() { return DOMHTMLElement::namespaceURI(); }
    
    virtual WebCore::String prefix() { return DOMHTMLElement::prefix(); }
    
    virtual void setPrefix(WebCore::String prefix) { return DOMHTMLElement::setPrefix(prefix); }
    
    virtual WebCore::String localName() { return DOMHTMLElement::localName(); }
    
    virtual bool hasAttributes() { return DOMHTMLElement::hasAttributes(); }

    virtual bool isSameNode(DOMNode* other) { return DOMHTMLElement::isSameNode(other); }
    
    virtual bool isEqualNode(DOMNode* other) { return DOMHTMLElement::isEqualNode(other); }
    
    virtual WebCore::String textContent() { return DOMHTMLElement::textContent(); }
    
    virtual void setTextContent(WebCore::String text) { DOMHTMLElement::setTextContent(text); }
    
    // IDOMElement
    virtual WebCore::String tagName() { return DOMHTMLElement::tagName(); }
    
    virtual WebCore::String getAttribute(WebCore::String name) { return DOMHTMLElement::getAttribute(name); }
    
    virtual void setAttribute(WebCore::String name, WebCore::String value) { DOMHTMLElement::setAttribute(name, value); }
    
    virtual void removeAttribute(WebCore::String name) { DOMHTMLElement::removeAttribute(name); }
    
    //virtual DOMAttr* getAttributeNode(WebCore::String name) { return DOMHTMLElement::getAttributeNode(name); }
    
    //virtual DOMAttr* setAttributeNode(DOMAttr *newAttr) { return DOMHTMLElement::setAttributeNode(newAttr); }
    
    //virtual DOMAttr* removeAttributeNode(DOMAttr *oldAttr) { return DOMHTMLElement::removeAttributeNode(oldAttr); }
    
    virtual DOMNodeList* getElementsByTagName(WebCore::String name) { return DOMHTMLElement::getElementsByTagName(name); }
    
    virtual WebCore::String getAttributeNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMHTMLElement::getAttributeNS(namespaceURI, localName); }
    
    virtual void setAttributeNS(WebCore::String namespaceURI, WebCore::String qualifiedName, WebCore::String value) { DOMHTMLElement::setAttributeNS(namespaceURI, qualifiedName, value); }
    
    virtual void removeAttributeNS(WebCore::String namespaceURI, WebCore::String localName) { DOMHTMLElement::removeAttributeNS(namespaceURI, localName); }
    
    //virtual DOMAttr* getAttributeNodeNS(WebCore::String namespaceURI, WebCore:String localName) { return DOMHTMLElement::getAttributeNodeNS(namespaceURI, localName); }
    
    //virtual DOMAttr* setAttributeNodeNS(DOMAttr *newAttr) { return DOMHTMLElement::setAttributeNodeNS(newAttr); }
    
    virtual DOMNodeList* getElementsByTagNameNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMHTMLElement::getElementsByTagNameNS(namespaceURI, localName); }
    
    virtual bool hasAttribute(WebCore::String name) { return DOMHTMLElement::hasAttribute(name); }
    
    virtual bool hasAttributeNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMHTMLElement::hasAttributeNS(namespaceURI, localName); }

    virtual void focus() { DOMHTMLElement::focus(); }
    
    virtual void blur() { DOMHTMLElement::blur(); }


    // IDOMHTMLElement
    virtual WebCore::String idName() { return DOMHTMLElement::idName(); }
    
    virtual void setIdName(WebCore::String idName) { DOMHTMLElement::setIdName(idName); }
    
    virtual WebCore::String title() { return DOMHTMLElement::title(); }
    
    virtual void setTitle(WebCore::String title) { DOMHTMLElement::setTitle(title); }
    
    virtual WebCore::String lang() { return DOMHTMLElement::lang(); }
    
    virtual void setLang(WebCore::String lang) { DOMHTMLElement::setLang(lang); }
    
    virtual WebCore::String dir() { return DOMHTMLElement::dir(); }
    
    virtual void setDir(WebCore::String dir) { DOMHTMLElement::setDir(dir); }
    
    virtual WebCore::String className() { return DOMHTMLElement::className(); }
    
    virtual void setClassName(WebCore::String className) { DOMHTMLElement::setClassName(className); }

    virtual WebCore::String innerHTML() { return DOMHTMLElement::innerHTML(); }
        
    virtual void setInnerHTML(WebCore::String html) { DOMHTMLElement::setInnerHTML(html); }
        
    virtual WebCore::String innerText() { return DOMHTMLElement::innerText(); }
        
    virtual void setInnerText(WebCore::String text) { DOMHTMLElement::setInnerText(text); }

    // IDOMHTMLInputElement
    virtual WebCore::String defaultValue();
    
    virtual void setDefaultValue(WebCore::String val);
    
    virtual bool defaultChecked();
    
    virtual void setDefaultChecked(WebCore::String checked);
    
    virtual DOMHTMLElement* form();
    
    virtual WebCore::String accept();
    
    virtual void setAccept(WebCore::String accept);
    
    virtual WebCore::String accessKey();
    
    virtual void setAccessKey(WebCore::String key);
    
    virtual WebCore::String align();
    
    virtual void setAlign(WebCore::String align);
    
    virtual WebCore::String alt();
    
    virtual void setAlt(WebCore::String alt);
    
    virtual bool checked();
    
    virtual void setChecked(bool checked);
    
    virtual bool disabled();
    
    virtual void setDisabled(bool disabled);
    
    virtual int maxLength();
    
    virtual void setMaxLength(int maxLength);
    
    virtual WebCore::String name();
    
    virtual void setName(WebCore::String name);
    
    virtual bool readOnly();
    
    virtual void setReadOnly(bool readOnly);
    
    virtual unsigned int size();
    
    virtual void setSize(unsigned int size);
    
    virtual WebCore::String src();
    
    virtual void setSrc(WebCore::String src);
    
    virtual int tabIndex();
    
    virtual void setTabIndex(int tabIndex);
    
    virtual WebCore::String type();
    
    virtual void setType(WebCore::String type);
    
    virtual WebCore::String useMap();
    
    virtual void setUseMap(WebCore::String useMap);
    
    virtual WebCore::String value();
    
    virtual void setValue(WebCore::String value);
        
    virtual void select();
    
    virtual void click();

    virtual void setSelectionStart(long start);
    
    virtual long selectionStart();
    
    virtual void setSelectionEnd(long end);
    
    virtual long selectionEnd();

    // FormsAutoFillTransition
    virtual bool isTextField();
    
    virtual WebCore::IntRect rectOnScreen();
    
    virtual void replaceCharactersInRange(int startTarget, int endTarget, WebCore::String replacementString, int index);
    
    virtual void selectedRange(int *start, int *end);
    
    virtual void setAutofilled(bool filled);

    // FormPromptAdditions
    virtual bool isUserEdited();
};

class DOMHTMLTextAreaElement : public DOMHTMLElement
{
protected:
    DOMHTMLTextAreaElement();
public:
    DOMHTMLTextAreaElement(WebCore::Element* e) : DOMHTMLElement(e) {}

    /**
     * throw exception
     * @discussion Throws an exception in the current script execution context.
        @result Either NO if an exception could not be raised, YES otherwise.
     */
    virtual bool throwException(WebCore::String exceptionMessage)
    { 
        return DOMObject::throwException(exceptionMessage); 
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
        DOMObject::removeWebScriptKey(name);
    }

    /**
     *  stringRepresentation 
     * @discussion Converts the target object to a string representation.  The coercion
        of non string objects type is dependent on the script environment.
        @result Returns the string representation of the object.
     */
    virtual WebCore::String stringRepresentation()
    {
        return DOMObject::stringRepresentation();
    }

    /**
     *  setException 
     * @param description The description of the exception.
        @discussion Raises an exception in the script environment in the context of the
        current object.
     */
    virtual void setException(WebCore::String description)
    {
        DOMObject::setException(description);
    }

    // IDOMNode
    virtual WebCore::String nodeName() { return DOMHTMLElement::nodeName(); }
    
    virtual WebCore::String nodeValue() { return DOMHTMLElement::nodeValue(); }
    
    virtual void setNodeValue(WebCore::String value) { DOMHTMLElement::setNodeValue(value); }
    
    virtual unsigned short nodeType() { return DOMHTMLElement::nodeType(); }
    
    virtual DOMNode* parentNode() { return DOMHTMLElement::parentNode(); }
    
    virtual DOMNodeList* childNodes() { return DOMHTMLElement::childNodes(); }
    
    virtual DOMNode* firstChild() { return DOMHTMLElement::firstChild(); }
    
    virtual DOMNode* lastChild() { return DOMHTMLElement::lastChild(); }
    
    virtual DOMNode* previousSibling() { return DOMHTMLElement::previousSibling(); }
    
    virtual DOMNode* nextSibling() { return DOMHTMLElement::nextSibling(); }
    
    //virtual DOMNamedNodeMap* attributes() { return DOMHTMLElement::attributes(); }
    
    virtual DOMDocument* ownerDocument() { return DOMHTMLElement::ownerDocument(); }
    
    virtual DOMNode* insertBefore(DOMNode *newChild, DOMNode *refChild) { return DOMHTMLElement::insertBefore(newChild, refChild); }
    
    virtual DOMNode* replaceChild(DOMNode *newChild, DOMNode *oldChild) { return DOMHTMLElement::replaceChild(newChild, oldChild); }
    
    virtual DOMNode* removeChild(DOMNode *oldChild) { return DOMHTMLElement::removeChild(oldChild); }
    
    virtual DOMNode* appendChild(DOMNode *oldChild) { return DOMHTMLElement::appendChild(oldChild); }
    
    virtual bool hasChildNodes() { return DOMHTMLElement::hasChildNodes(); }
    
    virtual DOMNode* cloneNode(bool deep) { return DOMHTMLElement::cloneNode(deep); }
    
    virtual void normalize() { DOMHTMLElement::normalize(); }
    
    virtual bool isSupported(WebCore::String feature, WebCore::String version) { return DOMHTMLElement::isSupported(feature, version); }
    
    virtual WebCore::String namespaceURI() { return DOMHTMLElement::namespaceURI(); }
    
    virtual WebCore::String prefix() { return DOMHTMLElement::prefix(); }
    
    virtual void setPrefix(WebCore::String prefix) { return DOMHTMLElement::setPrefix(prefix); }
    
    virtual WebCore::String localName() { return DOMHTMLElement::localName(); }
    
    virtual bool hasAttributes() { return DOMHTMLElement::hasAttributes(); }

    virtual bool isSameNode(DOMNode* other) { return DOMHTMLElement::isSameNode(other); }
    
    virtual bool isEqualNode(DOMNode* other) { return DOMHTMLElement::isEqualNode(other); }
    
    virtual WebCore::String textContent() { return DOMHTMLElement::textContent(); }
    
    virtual void setTextContent(WebCore::String text) { DOMHTMLElement::setTextContent(text); }
    
    // IDOMElement
    virtual WebCore::String tagName() { return DOMHTMLElement::tagName(); }
    
    virtual WebCore::String getAttribute(WebCore::String name) { return DOMHTMLElement::getAttribute(name); }
    
    virtual void setAttribute(WebCore::String name, WebCore::String value) { DOMHTMLElement::setAttribute(name, value); }
    
    virtual void removeAttribute(WebCore::String name) { DOMHTMLElement::removeAttribute(name); }
    
    //virtual DOMAttr* getAttributeNode(WebCore::String name) { return DOMHTMLElement::getAttributeNode(name); }
    
    //virtual DOMAttr* setAttributeNode(DOMAttr *newAttr) { return DOMHTMLElement::setAttributeNode(newAttr); }
    
    //virtual DOMAttr* removeAttributeNode(DOMAttr *oldAttr) { return DOMHTMLElement::removeAttributeNode(oldAttr); }
    
    virtual DOMNodeList* getElementsByTagName(WebCore::String name) { return DOMHTMLElement::getElementsByTagName(name); }
    
    virtual WebCore::String getAttributeNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMHTMLElement::getAttributeNS(namespaceURI, localName); }
    
    virtual void setAttributeNS(WebCore::String namespaceURI, WebCore::String qualifiedName, WebCore::String value) { DOMHTMLElement::setAttributeNS(namespaceURI, qualifiedName, value); }
    
    virtual void removeAttributeNS(WebCore::String namespaceURI, WebCore::String localName) { DOMHTMLElement::removeAttributeNS(namespaceURI, localName); }
    
    //virtual DOMAttr* getAttributeNodeNS(WebCore::String namespaceURI, WebCore:String localName) { return DOMHTMLElement::getAttributeNodeNS(namespaceURI, localName); }
    
    //virtual DOMAttr* setAttributeNodeNS(DOMAttr *newAttr) { return DOMHTMLElement::setAttributeNodeNS(newAttr); }
    
    virtual DOMNodeList* getElementsByTagNameNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMHTMLElement::getElementsByTagNameNS(namespaceURI, localName); }
    
    virtual bool hasAttribute(WebCore::String name) { return DOMHTMLElement::hasAttribute(name); }
    
    virtual bool hasAttributeNS(WebCore::String namespaceURI, WebCore::String localName) { return DOMHTMLElement::hasAttributeNS(namespaceURI, localName); }

    virtual void focus() { DOMHTMLElement::focus(); }
    
    virtual void blur() { DOMHTMLElement::blur(); }


    // IDOMHTMLElement
    virtual WebCore::String idName() { return DOMHTMLElement::idName(); }
    
    virtual void setIdName(WebCore::String idName) { DOMHTMLElement::setIdName(idName); }
    
    virtual WebCore::String title() { return DOMHTMLElement::title(); }
    
    virtual void setTitle(WebCore::String title) { DOMHTMLElement::setTitle(title); }
    
    virtual WebCore::String lang() { return DOMHTMLElement::lang(); }
    
    virtual void setLang(WebCore::String lang) { DOMHTMLElement::setLang(lang); }
    
    virtual WebCore::String dir() { return DOMHTMLElement::dir(); }
    
    virtual void setDir(WebCore::String dir) { DOMHTMLElement::setDir(dir); }
    
    virtual WebCore::String className() { return DOMHTMLElement::className(); }
    
    virtual void setClassName(WebCore::String className) { DOMHTMLElement::setClassName(className); }

    virtual WebCore::String innerHTML() { return DOMHTMLElement::innerHTML(); }
        
    virtual void setInnerHTML(WebCore::String html) { DOMHTMLElement::setInnerHTML(html); }
        
    virtual WebCore::String innerText() { return DOMHTMLElement::innerText(); }
        
    virtual void setInnerText(WebCore::String text) { DOMHTMLElement::setInnerText(text); }
    
    // IDOMHTMLTextArea
    virtual WebCore::String defaultValue();
    
    virtual void setDefaultValue(WebCore::String val);
    
    virtual DOMHTMLElement* form();
    
    virtual WebCore::String accessKey();
    
    virtual void setAccessKey(WebCore::String key);
    
    virtual int cols();
    
    virtual void setCols(int cols);
    
    virtual bool disabled();
    
    virtual void setDisabled(bool disabled);
    
    virtual WebCore::String name();
    
    virtual void setName(WebCore::String name);
    
    virtual bool readOnly();
    
    virtual void setReadOnly(bool readOnly);
    
    virtual int rows();
    
    virtual void setRows(int rows);
    
    virtual int tabIndex();
    
    virtual void setTabIndex(int tabIndex);
    
    virtual WebCore::String type();
    
    virtual WebCore::String value();
    
    virtual void setValue(WebCore::String value);
        
    virtual void select();

    // FormPromptAdditions
    virtual bool isUserEdited();
};

#endif
