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

#include "WebObject.h"
#include "WebValuePrivate.h"
#include "JSValue.h"
#include "CString.h"
#include "Color.h"

#include <cstdio>

WebValue::WebValue()
    : d(new WebValuePrivate())
    , m_obj(0)
{
}

WebValue::WebValue(WebValuePrivate *priv)
    : m_obj(0)
{
    d = priv;
}

WebValue::~WebValue()
{
    if(d)
        delete d;
    d = 0;
}

bool WebValue::isUndefined() const
{
    return d->isUndefined();
}

bool WebValue::isNull() const
{
    return d->isNull();
}

bool WebValue::isUndefinedOrNull() const
{
    return d->isUndefinedOrNull();
}

bool WebValue::isBoolean() const
{
    return d->isBoolean();
}

bool WebValue::isNumber() const
{
    return d->isNumber();
}

bool WebValue::isString() const
{
    return d->isString();
}

bool WebValue::isGetterSetter() const
{
    return d->isGetterSetter();
}

bool WebValue::isObject() const
{
    return d->isObject();
}

unsigned int WebValue::toRGBA32() const
{
    //FIXME: remove this namespace from here! :)
    WebCore::Color   color(d->toString());
    return color.rgb();
}

bool WebValue::toBoolean() const
{
    return d->toBoolean();
}

double WebValue::toNumber() const
{
    return d->toNumber();
}

const char* WebValue::toString() const
{
    return d->toString();
}

WebObject *WebValue::toObject() const
{
    return d->toObject();
}


void WebValue::balUndefined()
{
    d->balUndefined();
}

void WebValue::balNull()
{
    d->balNull();
}

void WebValue::balNaN()
{
    d->balNaN();
}

void WebValue::balBoolean(bool b)
{
    d->balBoolean(b);
}

void WebValue::balNumber(double dd)
{
    d->balNumber(dd);
}

void WebValue::balString(const char* s)
{
    d->balString(s);
}

void WebValue::balObject(WebObject *obj)
{
     m_obj = obj;
}

WebObject::WebObject()
{
}

WebObject::~WebObject()
{
    m_balPropertyList.clear();
    m_balMethodList.clear();
}

void WebObject::invalidate()
{
}

bool WebObject::hasMethod(const char *name)
{
    string methodName = name;
    for (unsigned i=0; i<m_balMethodList.size(); i++) {
        if (methodName ==  m_balMethodList[i]) {
            return true;
            break;
        }
    }
    return false;
}

WebValue *WebObject::invoke(const char *name, vector<WebValue *> args)
{
    return NULL;
}

bool WebObject::hasProperty( const char *name)
{
    string propName = name;
    for (unsigned i = 0; i < m_balPropertyList.size(); i++) {
        if (propName == m_balPropertyList[i]) {
            return true;
            break;
        }
    }
    return false;
}

WebValue *WebObject::getProperty(const char *name)
{
    return NULL;
}

void WebObject::setProperty(const char *name, WebValue *value)
{
	printf("set property\n");
}


void WebObject::addMethod(const char *aMethod)
{
    m_balMethodList.push_back(aMethod);
}

void WebObject::removeMethod(const char *aMethod)
{
    string methodName = aMethod;
    vector<string>::iterator it;
    for (it = m_balMethodList.begin(); it != m_balMethodList.end(); ++it) {
        if (methodName ==  *it) {
            m_balMethodList.erase(it);
            break;
        }
    }
}

void WebObject::addProperty(const char *aProperty)
{
    m_balPropertyList.push_back(aProperty);
}

void WebObject::removeProperty(const char *aProperty)
{
    string propName = aProperty;
    vector<string>::iterator it;
    for (it = m_balPropertyList.begin(); it != m_balPropertyList.end(); ++it) {
        if (propName == *it) {
            m_balPropertyList.erase(it);
            break;
        }
    }
}

