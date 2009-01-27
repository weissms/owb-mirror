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
#include "DOMHTMLClasses.h"

#include <PlatformString.h>
#include <Document.h>
#include <Element.h>
#include <FrameView.h>
#include <HTMLDocument.h>
#include <HTMLFormElement.h>
#include <HTMLInputElement.h>
#include <HTMLNames.h>
#include <HTMLOptionElement.h>
#include <HTMLSelectElement.h>
#include <HTMLTextAreaElement.h>
#include <IntRect.h>
#include <RenderObject.h>
#include <RenderTextControl.h>

using namespace WebCore;
using namespace HTMLNames;

// DOMHTMLCollection

DOMHTMLCollection::DOMHTMLCollection(WebCore::HTMLCollection* c)
: m_collection(c)
{
}

DOMHTMLCollection* DOMHTMLCollection::createInstance(WebCore::HTMLCollection* c)
{
    if (!c)
        return 0;

    return new DOMHTMLCollection(c);
}

// DOMHTMLCollection ----------------------------------------------------------

unsigned DOMHTMLCollection::length()
{
    if (!m_collection)
        return 0;

    return m_collection->length();
}

DOMNode* DOMHTMLCollection::item(unsigned index)
{
    if (!m_collection)
        return 0;

    return DOMNode::createInstance(m_collection->item(index));
}

DOMNode* DOMHTMLCollection::namedItem(WebCore::String /*name*/)
{
    return 0;
}

// DOMHTMLOptionsCollection ---------------------------------------------------

unsigned int DOMHTMLOptionsCollection::length()
{
    return 0;
}
 
void DOMHTMLOptionsCollection::setLength(unsigned int /*length*/)
{
}

DOMNode* DOMHTMLOptionsCollection::item(unsigned int /*index*/)
{
    return 0;
}

DOMNode* DOMHTMLOptionsCollection::namedItem(WebCore::String /*name*/)
{
    return 0;
}

// DOMHTMLDocument ------------------------------------------------------------

WebCore::String DOMHTMLDocument::title()
{
    return WebCore::String();
}
    
void DOMHTMLDocument::setTitle(WebCore::String /*title*/)
{
}
    
WebCore::String DOMHTMLDocument::referrer()
{
    return WebCore::String();
}
    
WebCore::String DOMHTMLDocument::domain()
{
    return WebCore::String();
}
    
WebCore::String DOMHTMLDocument::URL()
{
    return static_cast<HTMLDocument*>(m_document)->url();
}
    
DOMHTMLElement* DOMHTMLDocument::body()
{
    if (!m_document || !m_document->isHTMLDocument())
        return 0;

    HTMLDocument* htmlDoc = static_cast<HTMLDocument*>(m_document);
    return new DOMHTMLElement(htmlDoc->body());
}
    
void DOMHTMLDocument::setBody(DOMHTMLElement* /*body*/)
{
}
    
DOMHTMLCollection* DOMHTMLDocument::images()
{
    return 0;
}
    
DOMHTMLCollection* DOMHTMLDocument::applets()
{
    return 0;
}
    
DOMHTMLCollection* DOMHTMLDocument::links()
{
    return 0;
}
    
DOMHTMLCollection* DOMHTMLDocument::forms()
{
    if (!m_document || !m_document->isHTMLDocument())
        return 0;

    HTMLDocument* htmlDoc = static_cast<HTMLDocument*>(m_document);
    return DOMHTMLCollection::createInstance(htmlDoc->forms().get());
}
    
DOMHTMLCollection* DOMHTMLDocument::anchors()
{
    return 0;
}
    
WebCore::String DOMHTMLDocument::cookie()
{
    return WebCore::String();
}
    
void DOMHTMLDocument::setCookie(WebCore::String /*cookie*/)
{
}
    
void DOMHTMLDocument::open()
{
}
    
void DOMHTMLDocument::close()
{
}
    
void DOMHTMLDocument::write(WebCore::String /*text*/)
{
}
    
void DOMHTMLDocument::writeln(WebCore::String /*text*/)
{
}
    
DOMElement* DOMHTMLDocument::getElementById_(WebCore::String /*elementId*/)
{
    return 0;
}
    
DOMNodeList* DOMHTMLDocument::getElementsByName(WebCore::String /*elementName*/)
{
    return 0;
}

// DOMHTMLElement -------------------------------------------------------------

WebCore::String DOMHTMLElement::idName()
{
    return WebCore::String();
}
    
void DOMHTMLElement::setIdName(WebCore::String /*idName*/)
{
}
    
WebCore::String DOMHTMLElement::title()
{
    return WebCore::String();
}
    
void DOMHTMLElement::setTitle(WebCore::String /*title*/)
{
}
    
WebCore::String DOMHTMLElement::lang()
{
    return WebCore::String();
}
    
void DOMHTMLElement::setLang(WebCore::String /*lang*/)
{
}
    
WebCore::String DOMHTMLElement::dir()
{
    return WebCore::String();
}
    
void DOMHTMLElement::setDir(WebCore::String /*dir*/)
{
}
    
WebCore::String DOMHTMLElement::className()
{
    return WebCore::String();
}
    
void DOMHTMLElement::setClassName(WebCore::String /*className*/)
{
}

WebCore::String DOMHTMLElement::innerHTML()
{
    return WebCore::String();
}
        
void DOMHTMLElement::setInnerHTML(WebCore::String /*html*/)
{
}
        
WebCore::String DOMHTMLElement::innerText()
{
    ASSERT(m_element && m_element->isHTMLElement());
    return static_cast<HTMLElement*>(m_element)->innerText();
}
        
void DOMHTMLElement::setInnerText(WebCore::String text)
{
    ASSERT(m_element && m_element->isHTMLElement());
    HTMLElement* htmlEle = static_cast<HTMLElement*>(m_element);
    WebCore::ExceptionCode ec = 0;
    htmlEle->setInnerText(text, ec);
}

// DOMHTMLFormElement ---------------------------------------------------------

DOMHTMLCollection* DOMHTMLFormElement::elements()
{
    return 0;
}
    
int DOMHTMLFormElement::length()
{
    return 0;
}
    
WebCore::String DOMHTMLFormElement::name()
{
    return WebCore::String();
}
    
void DOMHTMLFormElement::setName(WebCore::String /*name*/)
{
}
    
WebCore::String DOMHTMLFormElement::acceptCharset()
{
    return WebCore::String();
}
    
void DOMHTMLFormElement::setAcceptCharset(WebCore::String /*acceptCharset*/)
{
}
    
WebCore::String DOMHTMLFormElement::action()
{
    ASSERT(m_element && m_element->hasTagName(formTag));
    return static_cast<HTMLFormElement*>(m_element)->action();
}
    
void DOMHTMLFormElement::setAction(WebCore::String /*action*/)
{
}
    
WebCore::String DOMHTMLFormElement::encType()
{
    return WebCore::String();
}
    
WebCore::String DOMHTMLFormElement::setEnctype()
{
    return WebCore::String();
}
    
WebCore::String DOMHTMLFormElement::method()
{
    ASSERT(m_element && m_element->hasTagName(formTag));
    return static_cast<HTMLFormElement*>(m_element)->method();
}
    
void DOMHTMLFormElement::setMethod(WebCore::String /*method*/)
{
}
    
WebCore::String DOMHTMLFormElement::target()
{
    return WebCore::String();
}
    
void DOMHTMLFormElement::setTarget(WebCore::String /*target*/)
{
}
    
void DOMHTMLFormElement::submit()
{
}
    
void DOMHTMLFormElement::reset()
{
}

// DOMHTMLSelectElement -------------------------------------------------------

WebCore::String DOMHTMLSelectElement::type()
{
    return WebCore::String();
}
    
int DOMHTMLSelectElement::selectedIndex()
{
    return 0;
}
    
void DOMHTMLSelectElement::setSelectedIndx(int /*selectedIndex*/)
{
}
    
WebCore::String DOMHTMLSelectElement::value()
{
    return WebCore::String();
}
    
void DOMHTMLSelectElement::setValue(WebCore::String /*value*/)
{
}
    
int DOMHTMLSelectElement::length()
{
    return 0;
}
    
DOMHTMLFormElement* DOMHTMLSelectElement::form()
{
    return 0;
}
    
DOMHTMLOptionsCollection* DOMHTMLSelectElement::options()
{
    return 0;
}
    
bool DOMHTMLSelectElement::disabled()
{
    return false;
}
    
void DOMHTMLSelectElement::setDisabled(bool /*disabled*/)
{
}
    
bool DOMHTMLSelectElement::multiple()
{
    return false;
}
    
void DOMHTMLSelectElement::setMultiple(bool /*multiple*/)
{
}
    
WebCore::String DOMHTMLSelectElement::name()
{
    return WebCore::String();
}
    
void DOMHTMLSelectElement::setName(WebCore::String /*name*/)
{
}
    
int DOMHTMLSelectElement::size()
{
    return 0;
}
    
void DOMHTMLSelectElement::setSize(int /*size*/)
{
}
    
int DOMHTMLSelectElement::tabIndex()
{
    return 0;
}
    
void DOMHTMLSelectElement::setTabIndex(int /*tabIndex*/)
{
}
    
void DOMHTMLSelectElement::add(DOMHTMLElement* /*element*/, DOMHTMLElement* /*before*/)
{
}
    
void DOMHTMLSelectElement::remove(int /*index*/)
{
}
    
// DOMHTMLSelectElement - FormsAutoFillTransitionSelect ----------------------

void DOMHTMLSelectElement::activateItemAtIndex(int /*index*/)
{
}

// DOMHTMLOptionElement -------------------------------------------------------

DOMHTMLFormElement* DOMHTMLOptionElement::form()
{
    return 0;
}
    
bool DOMHTMLOptionElement::defaultSelected()
{
    return false;
}
    
void DOMHTMLOptionElement::setDefaultSelected(bool /*defaultSelected*/)
{
}
    
WebCore::String DOMHTMLOptionElement::text()
{
    return WebCore::String();
}
    
int DOMHTMLOptionElement::index()
{
    return 0;
}
    
bool DOMHTMLOptionElement::disabled()
{
    return false;
}
    
void DOMHTMLOptionElement::setDisabled(bool /*disabled*/)
{
}
    
WebCore::String DOMHTMLOptionElement::label()
{
    return WebCore::String();
}
    
void DOMHTMLOptionElement::setLabel(WebCore::String /*label*/)
{
}
    
bool DOMHTMLOptionElement::selected()
{
    return false;
}
    
void DOMHTMLOptionElement::setSelected(bool /*selected*/)
{
}
    
WebCore::String DOMHTMLOptionElement::value()
{
    return WebCore::String();
}
    
void DOMHTMLOptionElement::setValue(WebCore::String /*value*/)
{
}

// DOMHTMLInputElement --------------------------------------------------------

WebCore::String DOMHTMLInputElement::defaultValue()
{
    return WebCore::String();
}
    
void DOMHTMLInputElement::setDefaultValue(WebCore::String /*val*/)
{
}
    
bool DOMHTMLInputElement::defaultChecked()
{
    return false;
}
    
void DOMHTMLInputElement::setDefaultChecked(WebCore::String /*checked*/)
{
}
    
DOMHTMLElement* DOMHTMLInputElement::form()
{
    ASSERT(m_element && m_element->hasTagName(inputTag));
    return new DOMHTMLElement(static_cast<HTMLInputElement*>(m_element));
}
    
WebCore::String DOMHTMLInputElement::accept()
{
    return WebCore::String();
}
    
void DOMHTMLInputElement::setAccept(WebCore::String /*accept*/)
{
}
    
WebCore::String DOMHTMLInputElement::accessKey()
{
    return WebCore::String();
}
    
void DOMHTMLInputElement::setAccessKey(WebCore::String /*key*/)
{
}
    
WebCore::String DOMHTMLInputElement::align()
{
    return WebCore::String();
}
    
void DOMHTMLInputElement::setAlign(WebCore::String /*align*/)
{
}
    
WebCore::String DOMHTMLInputElement::alt()
{
    return WebCore::String();
}
    
void DOMHTMLInputElement::setAlt(WebCore::String /*alt*/)
{
}
    
bool DOMHTMLInputElement::checked()
{
    return false;
}
    
void DOMHTMLInputElement::setChecked(bool /*checked*/)
{
}
    
bool DOMHTMLInputElement::disabled()
{
    ASSERT(m_element && m_element->hasTagName(inputTag));
    HTMLInputElement* inputElement = static_cast<HTMLInputElement*>(m_element);
    return inputElement->disabled() ? true : false;
}
    
void DOMHTMLInputElement::setDisabled(bool /*disabled*/)
{
}
    
int DOMHTMLInputElement::maxLength()
{
    return 0;
}
    
void DOMHTMLInputElement::setMaxLength(int /*maxLength*/)
{
}
    
WebCore::String DOMHTMLInputElement::name()
{
    return WebCore::String();
}
    
void DOMHTMLInputElement::setName(WebCore::String /*name*/)
{
}
    
bool DOMHTMLInputElement::readOnly()
{
    ASSERT(m_element && m_element->hasTagName(inputTag));
    HTMLInputElement* inputElement = static_cast<HTMLInputElement*>(m_element);
    return inputElement->readOnly() ? true : false;
}
    
void DOMHTMLInputElement::setReadOnly(bool /*readOnly*/)
{
}
    
unsigned int DOMHTMLInputElement::size()
{
    return 0;
}
    
void DOMHTMLInputElement::setSize(unsigned int /*size*/)
{
}
    
WebCore::String DOMHTMLInputElement::src()
{
    return WebCore::String();
}
    
void DOMHTMLInputElement::setSrc(WebCore::String /*src*/)
{
}
    
int DOMHTMLInputElement::tabIndex()
{
    return 0;
}
    
void DOMHTMLInputElement::setTabIndex(int /*tabIndex*/)
{
}
    
WebCore::String DOMHTMLInputElement::type()
{
    return WebCore::String();
}
    
void DOMHTMLInputElement::setType(WebCore::String type)
{
    ASSERT(m_element && m_element->hasTagName(inputTag));
    HTMLInputElement* inputElement = static_cast<HTMLInputElement*>(m_element);
    inputElement->setType(type);
}
    
WebCore::String DOMHTMLInputElement::useMap()
{
    return WebCore::String();
}
    
void DOMHTMLInputElement::setUseMap(WebCore::String /*useMap*/)
{
}
    
WebCore::String DOMHTMLInputElement::value()
{
    ASSERT(m_element && m_element->hasTagName(inputTag));
    HTMLInputElement* inputElement = static_cast<HTMLInputElement*>(m_element);
    return inputElement->value();
}
    
void DOMHTMLInputElement::setValue(WebCore::String value)
{
    ASSERT(m_element && m_element->hasTagName(inputTag));
    HTMLInputElement* inputElement = static_cast<HTMLInputElement*>(m_element);
    inputElement->setValue(value);
}
    
void DOMHTMLInputElement::select()
{
    ASSERT(m_element && m_element->hasTagName(inputTag));
    HTMLInputElement* inputElement = static_cast<HTMLInputElement*>(m_element);
    inputElement->select();
}
    
void DOMHTMLInputElement::click()
{
}

void DOMHTMLInputElement::setSelectionStart(long start)
{
    ASSERT(m_element && m_element->hasTagName(inputTag));
    HTMLInputElement* inputElement = static_cast<HTMLInputElement*>(m_element);
    inputElement->setSelectionStart(start);
}

long DOMHTMLInputElement::selectionStart()
{
    ASSERT(m_element && m_element->hasTagName(inputTag));
    HTMLInputElement* inputElement = static_cast<HTMLInputElement*>(m_element);
    return inputElement->selectionStart();
}

void DOMHTMLInputElement::setSelectionEnd(long end)
{
    ASSERT(m_element && m_element->hasTagName(inputTag));
    HTMLInputElement* inputElement = static_cast<HTMLInputElement*>(m_element);
    inputElement->setSelectionEnd(end);
}

long DOMHTMLInputElement::selectionEnd()
{
    ASSERT(m_element && m_element->hasTagName(inputTag));
    HTMLInputElement* inputElement = static_cast<HTMLInputElement*>(m_element);
    return inputElement->selectionEnd();
}

// DOMHTMLInputElement -- FormsAutoFillTransition ----------------------------

bool DOMHTMLInputElement::isTextField()
{
    ASSERT(m_element && m_element->hasTagName(inputTag));
    HTMLInputElement* inputElement = static_cast<HTMLInputElement*>(m_element);
    return inputElement->isTextField() ? true : false;
}

WebCore::IntRect DOMHTMLInputElement::rectOnScreen()
{
    RenderObject* renderer = m_element->renderer();
    FrameView* view = m_element->document()->view();
    if (!renderer || !view)
        return WebCore::IntRect();

    WebCore::IntRect coreRect = renderer->absoluteBoundingBoxRect();
    coreRect.setLocation(view->contentsToWindow(coreRect.location()));
    return coreRect;
}

void DOMHTMLInputElement::replaceCharactersInRange(int /*startTarget*/, int /*endTarget*/, WebCore::String /*replacementString*/, int /*index*/)
{
}

void DOMHTMLInputElement::selectedRange(int* start, int* end)
{
    ASSERT(m_element && m_element->hasTagName(inputTag));
    HTMLInputElement* inputElement = static_cast<HTMLInputElement*>(m_element);
    *start = inputElement->selectionStart();
    *end = inputElement->selectionEnd();
}

void DOMHTMLInputElement::setAutofilled(bool filled)
{
    ASSERT(m_element && m_element->hasTagName(inputTag));
    HTMLInputElement* inputElement = static_cast<HTMLInputElement*>(m_element);
    inputElement->setAutofilled(!!filled);
}

// DOMHTMLInputElement -- FormPromptAdditions ------------------------------------

bool DOMHTMLInputElement::isUserEdited()
{
    ASSERT(m_element);
    bool textField = isTextField();
    if (!textField)
        return false;
    RenderObject* renderer = m_element->renderer();
    if (renderer && static_cast<WebCore::RenderTextControl*>(renderer)->isUserEdited())
        return true;
    return false;
}

// DOMHTMLTextAreaElement -----------------------------------------------------

WebCore::String DOMHTMLTextAreaElement::defaultValue()
{
    return WebCore::String();
}
    
void DOMHTMLTextAreaElement::setDefaultValue(WebCore::String /*val*/)
{
}
    
DOMHTMLElement* DOMHTMLTextAreaElement::form()
{
    ASSERT(m_element && m_element->hasTagName(textareaTag));
    return new DOMHTMLElement(static_cast<HTMLTextAreaElement*>(m_element));
}
    
WebCore::String DOMHTMLTextAreaElement::accessKey()
{
    return WebCore::String();
}
    
void DOMHTMLTextAreaElement::setAccessKey(WebCore::String /*key*/)
{
}
    
int DOMHTMLTextAreaElement::cols()
{
    return 0;
}
    
void DOMHTMLTextAreaElement::setCols(int /*cols*/)
{
}
    
bool DOMHTMLTextAreaElement::disabled()
{
    return false;
}
    
void DOMHTMLTextAreaElement::setDisabled(bool /*disabled*/)
{
}
    
WebCore::String DOMHTMLTextAreaElement::name()
{
    return WebCore::String();
}
    
void DOMHTMLTextAreaElement::setName(WebCore::String /*name*/)
{
}
    
bool DOMHTMLTextAreaElement::readOnly()
{
    return false;
}
    
void DOMHTMLTextAreaElement::setReadOnly(bool /*readOnly*/)
{
}
    
int DOMHTMLTextAreaElement::rows()
{
    return 0;
}
    
void DOMHTMLTextAreaElement::setRows(int /*rows*/)
{
}
    
int DOMHTMLTextAreaElement::tabIndex()
{
    return 0;
}
    
void DOMHTMLTextAreaElement::setTabIndex(int /*tabIndex*/)
{
}
    
WebCore::String DOMHTMLTextAreaElement::type()
{
    return WebCore::String();
}
    
WebCore::String DOMHTMLTextAreaElement::value()
{
    ASSERT(m_element && m_element->hasTagName(textareaTag));
    HTMLTextAreaElement* textareaElement = static_cast<HTMLTextAreaElement*>(m_element);
    return textareaElement->value();
}
    
void DOMHTMLTextAreaElement::setValue(WebCore::String value)
{
    ASSERT(m_element && m_element->hasTagName(textareaTag));
    HTMLTextAreaElement* textareaElement = static_cast<HTMLTextAreaElement*>(m_element);
    textareaElement->setValue(value);
}
    
void DOMHTMLTextAreaElement::select()
{
    ASSERT(m_element && m_element->hasTagName(textareaTag));
    HTMLTextAreaElement* textareaElement = static_cast<HTMLTextAreaElement*>(m_element);
    textareaElement->select();
}

// DOMHTMLTextAreaElement -- FormPromptAdditions ------------------------------------

bool DOMHTMLTextAreaElement::isUserEdited()
{
    ASSERT(m_element);
    RenderObject* renderer = m_element->renderer();
    if (renderer && static_cast<WebCore::RenderTextControl*>(renderer)->isUserEdited())
        return true;
    return false;
}
