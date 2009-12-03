/*
 * Copyright (C) 2009 Pleyo. All rights reserved.
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
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY PLEYO AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ApplicationTestController.h"

#include "DumpRenderTree.h"
#include "WebConfigLoading.h"
#include "WebStringCollection.h"
#include <JavaScriptCore/JSObjectRef.h>
#include <JavaScriptCore/JSRetainPtr.h>

ApplicationTestController::ApplicationTestController()
    : m_showChildApplications(false)
{
}

ApplicationTestController::~ApplicationTestController()
{
}

void ApplicationTestController::makeWindowObject(JSContextRef context, JSObjectRef windowObject, JSValueRef* exception)
{
    JSRetainPtr<JSStringRef> applicationControllerStr(Adopt, JSStringCreateWithUTF8CString("applicationTestController"));
    JSValueRef applicationControllerObject = JSObjectMake(context, getJSClass(), this);
    JSObjectSetProperty(context, windowObject, applicationControllerStr.get(), applicationControllerObject, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete, exception);
}

void ApplicationTestController::fillConfigurationFromFile(char* fileName, ConfigFileType type)
{
    if (type == ChannelConfigType)
        WebConfigLoading::loadChannelConfig(fileName);
}

// Application controller static callback.
static JSValueRef fillConfigurationFromFile(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception, ConfigFileType type)
{
    if (argumentCount < 1)
        return JSValueMakeUndefined(context);

    JSRetainPtr<JSStringRef> fileNameJSString(Adopt, JSValueToStringCopy(context, arguments[0], exception));
    //ASSERT(!*exception);

    ApplicationTestController* controller = static_cast<ApplicationTestController*>(JSObjectGetPrivate(thisObject));
    char* relativeURL = JSStringCopyUTF8CString(fileNameJSString.get());

    if (strncmp("http://", relativeURL, 7) != 0
     && strncmp("https://", relativeURL, 8) != 0
     && strncmp("data:", relativeURL, 5) != 0
     && strncmp("file://", relativeURL, 7) != 0) {
        WebView* webView = getWebView();
        string mainFrameURL = webView->mainFrameURL();
        size_t pos = mainFrameURL.find_last_of("/");
        string absoluteURL = mainFrameURL.substr(0, pos + 1);
        absoluteURL += relativeURL;
        size_t pos2 = absoluteURL.find_last_of("?");
        if (pos2 == absoluteURL.size() - 1)
            absoluteURL = absoluteURL.substr(0, pos2);

        controller->fillConfigurationFromFile((char*)absoluteURL.c_str(), type);
        return JSValueMakeUndefined(context);
    }

    controller->fillConfigurationFromFile(relativeURL, type);
    return JSValueMakeUndefined(context);
}

static JSValueRef fillChannelConfigurationFromFileCallBack(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    return fillConfigurationFromFile(context, function, thisObject, argumentCount, arguments, exception, ChannelConfigType);
}

static JSValueRef setPermissionsForMainApplication(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    if (argumentCount < 1)
        return JSValueMakeUndefined(context);

    JSRetainPtr<JSStringRef> permissions(Adopt, JSValueToStringCopy(context, arguments[0], exception));
    //ASSERT(!*exception);

    WebApplication* mainApplication = getWebView()->webApplication();
    // Clear permissions.
    mainApplication->permissions()->clear();
    mainApplication->setPermissions(JSStringCopyUTF8CString(permissions.get()));

    return JSValueMakeUndefined(context);
}

static JSValueRef showChildApplicationsCallback(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    if (argumentCount < 1)
        return JSValueMakeUndefined(context);

    ApplicationTestController* controller = static_cast<ApplicationTestController*>(JSObjectGetPrivate(thisObject));
    controller->setShowChildApplications(JSValueToBoolean(context, arguments[0]));

    return JSValueMakeUndefined(context);
}

JSClassRef ApplicationTestController::getJSClass()
{
    static JSClassRef applicationControllerClass;

    if (!applicationControllerClass) {
        JSStaticValue* applicationTestControllerStaticValues =  ApplicationTestController::staticValues();
        JSStaticFunction* applicationTestControllerStaticFunctions =  ApplicationTestController::staticFunctions();
        JSClassDefinition classDefinition = {
            0, kJSClassAttributeNone, "ApplicationTestController", 0, applicationTestControllerStaticValues, applicationTestControllerStaticFunctions,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

        applicationControllerClass =  JSClassCreate(&classDefinition);
    }
    return applicationControllerClass;
}

JSStaticValue* ApplicationTestController::staticValues()
{
    static JSStaticValue applicationControllerStaticValues[] = {
        { 0, 0, 0, 0 }
    };
    return applicationControllerStaticValues;
}

JSStaticFunction* ApplicationTestController::staticFunctions()
{
    static JSStaticFunction applicationControllerStaticFunctions[] = {
        { "fillChannelConfigurationFromFile", fillChannelConfigurationFromFileCallBack, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete },
        { "setPermissionsForMainApplication", setPermissionsForMainApplication, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete },
        { "showChildApplications", showChildApplicationsCallback, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete },
        { 0, 0, 0  }
    };
    return applicationControllerStaticFunctions;
}
