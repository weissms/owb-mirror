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
#include "ArgList.h"

#include "bal_object.h"
#include "balValuePrivate.h"
#include "wtf/HashMap.h"
#include "ObjectPrototype.h"
#include "JSDOMBinding.h"
#include "JSValue.h"
#include "JSLock.h"
#include <cstdio>
#include <map>

using namespace std;

namespace JSC {
namespace Bindings {

typedef HashMap<BalObject*, BalInstance*> BalObjectInstanceMap;
static BalObjectInstanceMap cachedInstances;

typedef HashMap<BalInstance*, JSObject*> InstanceJSObjectMap;
static InstanceJSObjectMap cachedObjects;

class BalRuntimeObjectImp : public RuntimeObjectImp {
    public:
        BalRuntimeObjectImp(ExecState*, PassRefPtr<Instance>);
        ~BalRuntimeObjectImp();
        virtual void invalidate();

        virtual void mark() {
            /*BalInstance* instance = static_cast<BalInstance*>(getInternalInstance());
            if (instance)
                instance->mark();*/
            RuntimeObjectImp::mark();
        }

    protected:
        void removeFromCache();
};

BalRuntimeObjectImp::BalRuntimeObjectImp(ExecState* exec, PassRefPtr<Instance> instance)
    : RuntimeObjectImp(exec, instance)
{
}

BalRuntimeObjectImp::~BalRuntimeObjectImp()
{
    removeFromCache();
}

void BalRuntimeObjectImp::invalidate()
{
    removeFromCache();
    RuntimeObjectImp::invalidate();
}

void BalRuntimeObjectImp::removeFromCache()
{
    /*JSLock lock(false);
    BalInstance* key = cachedObjects.get(this);
    if (key)
        cachedObjects.remove(key);*/
}



PassRefPtr<BalInstance> BalInstance::create(BalObject* object, PassRefPtr<RootObject> rootObject)
{
    return adoptRef(new BalInstance(object, rootObject));
}

BalInstance::BalInstance(BalObject* o, PassRefPtr<RootObject> rootObject)
    : Instance(rootObject),
      m_class(0),
      m_object(o)
{
}

BalInstance::~BalInstance() 
{
    m_object = 0;
    m_class = 0;
    cachedObjects.remove(this);
    //cachedInstances.remove(m_hashkey);
}

/*BalInstance::BalInstance(const BalInstance& other)
    : Instance(other.rootObject()), m_class(0), m_object(other.m_object)
{
}*/

Class* BalInstance::getClass() const
{
    if (!m_class)
        m_class = BalClass::classForObject(m_object);
    return m_class;
}

bool BalInstance::supportsInvokeDefaultMethod() const
{
    return false;//m_object->_class->invokeDefault;
}

JSValue* BalInstance::invokeMethod(ExecState* exec, const MethodList& methodList, const ArgList& args)
{
    ASSERT(methodList.size() == 1);

    BalMethod* method = static_cast<BalMethod*>(methodList[0]);

    const char *ident = method->name();
    if (!m_object->hasMethod( ident ))
        return jsUndefined();

    unsigned count = args.size();
    Vector<BalValue *, 128> cArgs(count);

    unsigned i;
    for (i = 0; i < count; i++) {
        BalValuePrivate *priv = new BalValuePrivate(exec, args.at(exec, i));
        cArgs[i] = new BalValue(priv);
    }

    BalValue *val;
    {
        JSLock::DropAllLocks dropAllLocks(false);
        val = m_object->invoke(ident, cArgs);
    }

    cArgs.clear();

    return val->d->getValue();
}


JSValue* BalInstance::invokeDefaultMethod(ExecState* exec, const ArgList& args)
{
    return jsUndefined();
}

JSValue* BalInstance::defaultValue(ExecState* exec, PreferredPrimitiveType hint) const
{
    if (hint == JSValue::PreferString)
        return stringValue(exec);
    if (hint == JSValue::PreferNumber)
        return numberValue(exec);
    return valueOf(exec);
}

JSValue* BalInstance::stringValue(ExecState* exec) const
{
    char buf[1024];
#if COMPILER(MSVC)
    _snprintf(buf, sizeof(buf), "BalObject %p BalClass (%s)", m_object, m_class->name());
#else
    snprintf(buf, sizeof(buf), "BalObject %p BalClass (%s)", m_object, m_class->name());
#endif
    return jsString(exec, buf);
}

JSValue* BalInstance::numberValue(ExecState* exec) const
{
    return jsNumber(exec, 0);
}

JSValue* BalInstance::booleanValue() const
{
    // FIXME: Implement something sensible.
    return jsBoolean(false);
}

JSValue* BalInstance::valueOf(ExecState* exec) const 
{
    return stringValue(exec);
}

void BalInstance::getPropertyNames(ExecState* exec, PropertyNameArray& nameArray)
{
    /*uint32_t count;
    {
        JSLock::DropAllLocks dropAllLocks(false);
#if PLATFORM(AMIGAOS4)
        if (!_object->_class->enumerate(_object, &identifiers, (uint32_t *)&count))
#else
        if (!_object->_class->enumerate(_object, &identifiers, &count))
#endif
            return;
    }

    for (uint32_t i = 0; i < count; i++) {
        PrivateIdentifier* identifier = static_cast<PrivateIdentifier*>(identifiers[i]);

        if (identifier->isString)
            nameArray.add(identifierFromNPIdentifier(identifier->value.string));
        else
            nameArray.add(Identifier::from(exec, identifier->value.number));
    }

    // FIXME: This should really call NPN_MemFree but that's in WebKit
    free(identifiers);*/
}

PassRefPtr<BalInstance> BalInstance::getBalInstance(BalObject* o, PassRefPtr<RootObject> rootObject)
{
    JSLock lock(false);

    /*BalInstance *instance = cachedInstances.get(o);
    if (instance->rootObject() == rootObject)
        return instance;*/

    RefPtr<BalInstance> ret = BalInstance::create(o, rootObject);
    //cachedInstances.add(o, ret.get());

    return ret.release();
}

RuntimeObjectImp* BalInstance::getRuntimeObject(ExecState* exec, PassRefPtr<BalInstance> instance)
{
    JSLock lock(false);
    BalInstance* balInstance = instance.get();
    RuntimeObjectImp* ret = static_cast<RuntimeObjectImp*>(cachedObjects.get(balInstance));
    if (!ret) {
        ret = new (exec) BalRuntimeObjectImp(exec, instance);
        cachedObjects.add(balInstance, ret);
        ret = static_cast<RuntimeObjectImp*>(cachedObjects.get(balInstance));
    }
    return ret;
}



}
}

