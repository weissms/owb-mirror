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

#include "config.h"
#include "bal_instance.h"

#include "bal_class.h"
#include "bal_runtime.h"
#include "list.h"

#include "bal_object.h"

namespace KJS {
namespace Bindings {

BalInstance::BalInstance(BalObject* o)
    : Instance(),
      m_class(0),
      m_object(o)
{
    setRootObject(0);
}

BalInstance::~BalInstance() 
{
     m_object = 0;
     if( m_class )
     	delete m_class;
}

BalInstance::BalInstance(const BalInstance& other)
    : Instance(), m_class(0), m_object(other.m_object)
{
    setRootObject(other.rootObject());
}

BalInstance& BalInstance::operator=(const BalInstance& other)
{
    if (this == &other)
        return *this;

    m_object = other.m_object;
    m_class = 0;

    return *this;
}

Class* BalInstance::getClass() const
{
    if (!m_class)
        m_class = BalClass::classForObject(m_object);
    return m_class;
}

void BalInstance::begin()
{
    // Do nothing.
}

void BalInstance::end()
{
    // Do nothing.
}

bool BalInstance::implementsCall() const
{
    return true;
}

JSValue* BalInstance::invokeMethod(ExecState* exec, const MethodList& methodList, const List& args)
{
    assert(methodList.length() == 1);

    BalMethod* method = static_cast<BalMethod*>(methodList.methodAt(0));

    const char *ident = method->name();
    if (!m_object->hasMethod( ident ))
        return jsUndefined();

    JSValue* resultValue = m_object->invoke( ident, exec, args );

    return resultValue;
}


JSValue* BalInstance::invokeDefaultMethod(ExecState* exec, const List& args)
{
    return jsUndefined();
}


JSValue* BalInstance::defaultValue(JSType hint) const
{
    printf("coucou\n");
    if (hint == StringType)
    {
    	printf( "string\n" );
        return stringValue();
    }
    if (hint == NumberType)
    {
            printf( "number\n" );
        return numberValue();
    }
    if (hint == BooleanType)
    {
            printf( "bool\n" );
        return booleanValue();
    }
    return valueOf();
}

JSValue* BalInstance::stringValue() const
{
    char buf[1024];
    snprintf(buf, sizeof(buf), "BalObject %p (%s)", m_object, m_class->name());
    return jsString(buf);
}

JSValue* BalInstance::numberValue() const
{
    return jsNumber(0);
}

JSValue* BalInstance::booleanValue() const
{
    // FIXME: Implement something sensible.
    return jsBoolean(false);
}

JSValue* BalInstance::valueOf() const 
{
    return stringValue();
}

}
}

