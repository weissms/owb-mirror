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
#include "DOMCSSClasses.h"

#include <PlatformString.h>

// DOMCSSStyleDeclaration - DOMCSSStyleDeclaration ----------------------------

DOMCSSStyleDeclaration::DOMCSSStyleDeclaration(WebCore::CSSStyleDeclaration* s)
: m_style(0)
{
    if (s)
        s->ref();

    m_style = s;
}

DOMCSSStyleDeclaration::~DOMCSSStyleDeclaration()
{
    if (m_style)
        m_style->deref();
}

DOMCSSStyleDeclaration* DOMCSSStyleDeclaration::createInstance(WebCore::CSSStyleDeclaration* s)
{
    if (!s)
        return 0;

    return new DOMCSSStyleDeclaration(s);
}


// DOMCSSStyleDeclaration - DOMCSSStyleDeclaration ---------------------------

WebCore::String DOMCSSStyleDeclaration::cssText()
{
    return WebCore::String();
}

void DOMCSSStyleDeclaration::setCssText(WebCore::String cssText)
{
    // FIXME: <rdar://5148045> return DOM exception info
    WebCore::ExceptionCode ec;
    m_style->setCssText(cssText, ec);
}

WebCore::String DOMCSSStyleDeclaration::getPropertyValue(WebCore::String propertyName)
{
    return m_style->getPropertyValue(propertyName);
}

//DOMCSSValue* DOMCSSStyleDeclaration::getPropertyCSSValue(WebCore::String /*propertyName*/)
//{
//    return 0;
//}

WebCore::String DOMCSSStyleDeclaration::removeProperty(WebCore::String /*propertyName*/)
{
    return WebCore::String();
}

WebCore::String DOMCSSStyleDeclaration::getPropertyPriority(WebCore::String /*propertyName*/)
{
    return WebCore::String();
}

void DOMCSSStyleDeclaration::setProperty(WebCore::String propertyName, WebCore::String value, WebCore::String priority)
{
    // FIXME: <rdar://5148045> return DOM exception info
    WebCore::ExceptionCode code;
    m_style->setProperty(propertyName, value, priority, code);
}

unsigned DOMCSSStyleDeclaration::length()
{
    return 0;
}

WebCore::String DOMCSSStyleDeclaration::item(unsigned /*index*/)
{
    return WebCore::String();
}

// DOMCSSRule* DOMCSSStyleDeclaration::parentRule()
// {
//     return 0;
// }
