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


#ifndef BINDINGS_BAL_OBJECT_H_
#define BINDINGS_BAL_OBJECT_H_

#include "Vector.h"


namespace KJS {

class ExecState;
class List;
class JSValue;

namespace Bindings {

class BalClass;
class BalMethod;

class BalObject
{
public:
    BalObject();
    virtual ~BalObject();
    void invalidate();
    bool hasMethod(const char *name);
    virtual JSValue *invoke( const char *name, ExecState* exec, const List& args);
    JSValue *invokeDefault( ExecState* exec, const List& args);
    bool hasProperty( const char *name);
    virtual JSValue *getProperty( const char *name, ExecState* exec);
    virtual void setProperty( const char *name, ExecState* exec, const JSValue *value);
    void addMethod(const char *);
    void removeMethod(const char *);
    void addProperty(const char *);
    void removeProperty(const char *);
private:
    Vector<const char *> m_balMethodList;
    Vector<const char *> m_balPropertyList;
};

}}

#endif
