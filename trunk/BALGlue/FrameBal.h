/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com
 * All rights reserved.
 * Copyright (C) 2007 Pleyo.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FrameBal_h
#define FrameBal_h

#include "BIKeyboardEvent.h"
#include "EditorClient.h"
#include "Frame.h"
#include "ResourceHandleClient.h"

namespace WebCore {

class Element;
class FrameLoaderClientBal;
class FormData;

class FrameBal : public Frame {
public:
    FrameBal(Page*, HTMLFrameOwnerElement*, FrameLoaderClientBal*);
    virtual ~FrameBal();

    /**
    * addToJSWindowObject
    *
    * example :
    * @code
    *      class MyObject : public BalObject
    *       {
    *       public:
    *           MyObject():val(5)
    *           {
    *               addMethod("log");
    *               addProperty("val");
    *           }
    *
    *           ~MyObject()
    *           {
    *           }
    *
    *           JSValue *invoke( const char *name, ExecState* exec, const List& args, uint32_t argCount)
    *           {
    *               if (!strcmp(name, "log") ) {
    *                   JSType type = args.at(0)->type();
    *                   if (argCount == 1 && type == StringType)
    *                   {
    *                       logMessage (args.at(0)->toString(exec).ascii());
    *                   }
    *               }
    *               return jsUndefined();
    *           }
    *
    *           JSValue *getProperty( const char *name, ExecState* exec )
    *           {
    *               if( !strcmp( name, "val" ) )
    *               {
    *                   return jsNumber(val);
    *               }
    *               return jsUndefined();
    *           };
    *           void setProperty( const char *name, ExecState* exec, const JSValue *value)
    *           {
    *              if( !strcmp( name, "val" ) )
    *              {
    *                  val = (int)value->toNumber(exec);
    *              }
    *           };
    *           void logMessage (const char *message)
    *           {
    *               printf ("%s\n", message);
    *           }
    *       private:
    *           int val;
    *       };
    *
    *
    *       ....
    *
    *       MyObject *myObject = new MyObject();
    *       m_balFrame->addToJSWindowObject("myInterface", (void *)myObject );
    *
    *
    *      JS Code :
    *           window.myInterface.logMessage ("myInterface.intValue = " + myInterface.intValue);
    *      or  myInterface.logMessage ("myInterface.intValue = " + myInterface.intValue);
    *
    *  @endcode
    */
    void addToJSWindowObject(const char* name, void *object);
};

}

#endif
