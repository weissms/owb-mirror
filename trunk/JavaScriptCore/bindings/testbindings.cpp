// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */
#include "config.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "value.h"
#include "object.h"
#include "types.h"
#include "interpreter.h"

#include "npruntime.h"
#include "npruntime_impl.h"
#include "npruntime_priv.h"

#include "runtime.h"
#include "runtime_object.h"
#include "runtime_root.h"

// ------------------ NP Interface definition --------------------
typedef struct
{
    NPObject object;
    double doubleValue;
    int intValue;
    NPVariant stringValue;
    bool boolValue;
} MyObject;


static bool identifiersInitialized = false;

#define ID_DOUBLE_VALUE                         0
#define ID_INT_VALUE                            1
#define ID_STRING_VALUE                         2
#define ID_BOOLEAN_VALUE                        3
#define ID_NULL_VALUE                           4
#define ID_UNDEFINED_VALUE                      5
#define NUM_PROPERTY_IDENTIFIERS                6

static NPIdentifier myPropertyIdentifiers[NUM_PROPERTY_IDENTIFIERS];
static const NPUTF8 *myPropertyIdentifierNames[NUM_PROPERTY_IDENTIFIERS] = {
    "doubleValue",
    "intValue",
    "stringValue",
    "booleanValue",
    "nullValue",
    "undefinedValue"
};

#define ID_LOG_MESSAGE                          0
#define ID_SET_DOUBLE_VALUE                     1
#define ID_SET_INT_VALUE                        2
#define ID_SET_STRING_VALUE                     3
#define ID_SET_BOOLEAN_VALUE                    4
#define ID_GET_DOUBLE_VALUE                     5
#define ID_GET_INT_VALUE                        6
#define ID_GET_STRING_VALUE                     7
#define ID_GET_BOOLEAN_VALUE                    8
#define NUM_METHOD_IDENTIFIERS                  9

static NPIdentifier myMethodIdentifiers[NUM_METHOD_IDENTIFIERS];
static const NPUTF8 *myMethodIdentifierNames[NUM_METHOD_IDENTIFIERS] = {
    "logMessage",
    "setDoubleValue",
    "setIntValue",
    "setStringValue",
    "setBooleanValue",
    "getDoubleValue",
    "getIntValue",
    "getStringValue",
    "getBooleanValue"
};

static void initializeIdentifiers()
{
    _NPN_GetStringIdentifiers (myPropertyIdentifierNames, NUM_PROPERTY_IDENTIFIERS, myPropertyIdentifiers);
    _NPN_GetStringIdentifiers (myMethodIdentifierNames, NUM_METHOD_IDENTIFIERS, myMethodIdentifiers);
};

bool myHasProperty (NPClass *theClass, NPIdentifier name)
{
    int i;
    for (i = 0; i < NUM_PROPERTY_IDENTIFIERS; i++) {
        if (name == myPropertyIdentifiers[i]){
            return true;
        }
    }
    return false;
}

bool myHasMethod (NPClass *theClass, NPIdentifier name)
{
    int i;
    for (i = 0; i < NUM_METHOD_IDENTIFIERS; i++) {
        if (name == myMethodIdentifiers[i]){
            return true;
        }
    }
    return false;
}


void logMessage (const NPVariant *message)
{
    if (NPVARIANT_IS_STRING(*message)) {
        char msgBuf[1024];
        strncpy (msgBuf, message->value.stringValue.UTF8Characters, message->value.stringValue.UTF8Length);
        msgBuf[message->value.stringValue.UTF8Length] = 0;
        printf ("%s\n", msgBuf);
    }
    else if (NPVARIANT_IS_DOUBLE(*message))
        printf ("%f\n", (float)message->value.doubleValue);
    else if (NPVARIANT_IS_INT32(*message))
        printf ("%d\n", message->value.intValue);
    else if (NPVARIANT_IS_OBJECT(*message))
        printf ("%p\n", message->value.objectValue);
}

void setDoubleValue (MyObject *obj, const NPVariant *variant)
{
    if (!NPVARIANT_IS_DOUBLE(*variant)) {
        NPUTF8 *msg = "Attempt to set double value with invalid type.";
        NPString aString;
        aString.UTF8Characters = msg;
        aString.UTF8Length = strlen (msg);
        _NPN_SetException ((NPObject *)obj, aString.UTF8Characters);
    }
    else
        obj->doubleValue = NPVARIANT_TO_DOUBLE(*variant);
}

void setIntValue (MyObject *obj, const NPVariant *variant)
{
    if (NPVARIANT_IS_INT32(*variant))
        obj->intValue = NPVARIANT_TO_INT32(*variant);
    else if (NPVARIANT_IS_DOUBLE(*variant))
        obj->intValue = static_cast<int>(NPVARIANT_TO_DOUBLE(*variant));
    else {
        NPUTF8 *msg = "Attempt to set int value with invalid type.";
        NPString aString;
        aString.UTF8Characters = msg;
        aString.UTF8Length = strlen (msg);
        _NPN_SetException ((NPObject *)obj, aString.UTF8Characters);
    }
}

void setStringValue (MyObject *obj, const NPVariant *variant)
{
    if (!NPVARIANT_IS_STRING(*variant)) {
        _NPN_SetException((NPObject *)obj, "setstringvalue exception");
    }
    else {
        _NPN_ReleaseVariantValue (&obj->stringValue);
        NPN_InitializeVariantWithStringCopy(&obj->stringValue, &NPVARIANT_TO_STRING(*variant));
    }
}

void setBooleanValue (MyObject *obj, const NPVariant *variant)
{
    if (!NPVARIANT_IS_BOOLEAN(*variant)) {
        NPUTF8 *msg = "Attempt to set bool value with invalid type.";
        NPString aString;
        aString.UTF8Characters = msg;
        aString.UTF8Length = strlen (msg);
        _NPN_SetException ((NPObject *)obj, aString.UTF8Characters);
    }
    else
        obj->boolValue = NPVARIANT_TO_BOOLEAN(*variant);
}

void getDoubleValue (MyObject *obj, NPVariant *variant)
{
    DOUBLE_TO_NPVARIANT(obj->doubleValue, *variant);
}

void getIntValue (MyObject *obj, NPVariant *variant)
{
    INT32_TO_NPVARIANT(obj->intValue, *variant);
}

void getStringValue (MyObject *obj, NPVariant *variant)
{
//    STRINGZ_TO_NPVARIANT(obj->stringValue, *variant);
    *variant = obj->stringValue;
}

void getBooleanValue (MyObject *obj, NPVariant *variant)
{
    BOOLEAN_TO_NPVARIANT(obj->boolValue, *variant);
}

void myGetProperty (MyObject *obj, NPIdentifier name, NPVariant *variant)
{
    if (name == myPropertyIdentifiers[ID_DOUBLE_VALUE]){
        getDoubleValue (obj, variant);
    }
    else if (name == myPropertyIdentifiers[ID_INT_VALUE]){
        getIntValue (obj, variant);
    }
    else if (name == myPropertyIdentifiers[ID_STRING_VALUE]){
        getStringValue (obj, variant);
    }
    else if (name == myPropertyIdentifiers[ID_BOOLEAN_VALUE]){
        getBooleanValue (obj, variant);
    }
    else if (name == myPropertyIdentifiers[ID_NULL_VALUE]){
        NULL_TO_NPVARIANT(*variant);
    }
    else if (name == myPropertyIdentifiers[ID_UNDEFINED_VALUE]){
        VOID_TO_NPVARIANT(*variant);
    }
    else
        VOID_TO_NPVARIANT(*variant);
}

void mySetProperty (MyObject *obj, NPIdentifier name, const NPVariant *variant)
{
    if (name == myPropertyIdentifiers[ID_DOUBLE_VALUE]) {
        setDoubleValue (obj, variant);
    }
    else if (name == myPropertyIdentifiers[ID_INT_VALUE]) {
        setIntValue (obj, variant);
    }
    else if (name == myPropertyIdentifiers[ID_STRING_VALUE]) {
        setStringValue (obj, variant);
    }
    else if (name == myPropertyIdentifiers[ID_BOOLEAN_VALUE]) {
        setBooleanValue (obj, variant);
    }
    else if (name == myPropertyIdentifiers[ID_NULL_VALUE]) {
        // Do nothing!
    }
    else if (name == myPropertyIdentifiers[ID_UNDEFINED_VALUE]) {
        // Do nothing!
    }
}

void myInvoke (MyObject *obj, NPIdentifier name, NPVariant *args, uint32_t argCount, NPVariant *result)
{
    if (name == myMethodIdentifiers[ID_LOG_MESSAGE]) {
        if (argCount == 1 && NPVARIANT_IS_STRING (args[0]))
            logMessage (&args[0]);
        VOID_TO_NPVARIANT(*result);
    }
    else if (name == myMethodIdentifiers[ID_SET_DOUBLE_VALUE]) {
        if (argCount == 1 && NPVARIANT_IS_DOUBLE (args[0]))
            setDoubleValue (obj, &args[0]);
        *result = args[0];
    }
    else if (name == myMethodIdentifiers[ID_SET_INT_VALUE]) {
        if (argCount == 1 && (NPVARIANT_IS_DOUBLE (args[0]) || NPVARIANT_IS_INT32 (args[0])))
            setIntValue (obj, &args[0]);
        *result = args[0];
    }
    else if (name == myMethodIdentifiers[ID_SET_STRING_VALUE]) {
        if (argCount == 1 && NPVARIANT_IS_STRING (args[0]))
            setStringValue (obj, &args[0]);
        VOID_TO_NPVARIANT(*result); // crash if *result = args[0];
    }
    else if (name == myMethodIdentifiers[ID_SET_BOOLEAN_VALUE]) {
        if (argCount == 1 && NPVARIANT_IS_BOOLEAN (args[0]))
            setBooleanValue (obj, &args[0]);
        *result = args[0];
    }
    else if (name == myMethodIdentifiers[ID_GET_DOUBLE_VALUE]) {
        getDoubleValue (obj, result);
    }
    else if (name == myMethodIdentifiers[ID_GET_INT_VALUE]) {
        getIntValue (obj, result);
    }
    else if (name == myMethodIdentifiers[ID_GET_STRING_VALUE]) {
        getStringValue (obj, result);
    }
    else if (name == myMethodIdentifiers[ID_GET_BOOLEAN_VALUE]) {
        getBooleanValue (obj, result);
    }
    else 
        VOID_TO_NPVARIANT(*result);
}

NPObject *myAllocate ()
{
    MyObject *newInstance = (MyObject *)malloc (sizeof(MyObject));
    
    if (!identifiersInitialized) {
        identifiersInitialized = true;
        initializeIdentifiers();
    }
    
    
    newInstance->doubleValue = 666.666;
    newInstance->intValue = 1234;
    newInstance->boolValue = true;
    newInstance->stringValue.type = NPVariantType_String;
    newInstance->stringValue.value.stringValue.UTF8Length = strlen ("Hello world");
    newInstance->stringValue.value.stringValue.UTF8Characters = strdup ("Hello world");
    
    return (NPObject *)newInstance;
}

void myInvalidate ()
{
    // Make sure we've released any remaining references to JavaScript objects.
}

void myDeallocate (MyObject *obj) 
{
    free ((void *)obj);
}

static NPClass _myFunctionPtrs = { 
    NP_CLASS_STRUCT_VERSION,
    (NPAllocateFunctionPtr) myAllocate, 
    (NPDeallocateFunctionPtr) myDeallocate, 
    (NPInvalidateFunctionPtr) myInvalidate,
    (NPHasMethodFunctionPtr) myHasMethod,
    (NPInvokeFunctionPtr) myInvoke,
    (NPInvokeDefaultFunctionPtr) NULL,
    (NPHasPropertyFunctionPtr) myHasProperty,
    (NPGetPropertyFunctionPtr) myGetProperty,
    (NPSetPropertyFunctionPtr) mySetProperty,
    (NPRemovePropertyFunctionPtr) NULL
};
static NPClass *myFunctionPtrs = &_myFunctionPtrs;

// --------------------------------------------------------

using namespace KJS;
using namespace KJS::Bindings;

class Global : public JSObject {
public:
  virtual UString className() const { return "global"; }
};

#define BufferSize 200000
static char code[BufferSize];

const char *readJavaScriptFromFile (const char *file)
{
    FILE *f = fopen(file, "r");
    if (!f) {
        fprintf(stderr, "Error opening %s.\n", file);
        return 0;
    }
    
    int num = fread(code, 1, BufferSize, f);
    code[num] = '\0';
    if(num >= BufferSize)
        fprintf(stderr, "Warning: File may have been too long.\n");

    fclose(f);
    
    return code;
}

int main(int argc, char **argv)
{
    // expecting a filename
    if (argc < 2) {
        fprintf(stderr, "You have to specify at least one filename\n");
        return -1;
    }
    
    bool ret = true;
    {
        JSLock lock;
        
        // create interpreter w/ global object
        Global* global = new Global();
        RefPtr<Interpreter> interp = new Interpreter(global);
        ExecState *exec = interp->globalExec();
        
        MyObject *myObject = (MyObject *)_NPN_CreateObject (NPP(0), myFunctionPtrs);
        
// FIXME
//        global->put(exec, Identifier("myInterface"), Instance::createRuntimeObject(Instance::CLanguage, (void *)myObject));
        
        for (int i = 1; i < argc; i++) {
            const char *code = readJavaScriptFromFile(argv[i]);
            
            if (code) {
                // run
                Completion comp(interp->evaluate("", 0, code));
                
                if (comp.complType() == Throw) {
                    JSValue *exVal = comp.value();
                    char *msg = exVal->toString(exec).ascii();
                    int lineno = -1;
                    if (exVal->type() == ObjectType) {
                        JSValue *lineVal = exVal->getObject()->get(exec, Identifier("line"));
                        if (lineVal->type() == NumberType)
                            lineno = int(lineVal->toNumber(exec));
                    }
                    if (lineno != -1)
                        fprintf(stderr,"Exception, line %d: %s\n",lineno,msg);
                    else
                        fprintf(stderr,"Exception: %s\n",msg);
                    ret = false;
                }
                else if (comp.complType() == ReturnValue) {
                    char *msg = comp.value()->toString(interp->globalExec()).ascii();
                    fprintf(stderr,"Return value: %s\n",msg);
                }
            }
        }
                
        _NPN_ReleaseObject ((NPObject *)myObject);
        
    } // end block, so that Interpreter and global get deleted
    
    return ret ? 0 : 3;
}
