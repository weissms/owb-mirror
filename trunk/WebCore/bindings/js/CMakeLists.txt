create_lut_webcore(WEBCORE_SRC WebCore/bindings/js/JSDOMBinding.cpp generated_sources/WebCore/JSDOMBinding.lut.h WebCore/bindings/js/JSDOMBinding.cpp)
create_lut_webcore(WEBCORE_SRC WebCore/bindings/js/JSDOMWindowBase.cpp generated_sources/WebCore/JSDOMWindowBase.lut.h WebCore/bindings/js/JSDOMWindowBase.cpp)
create_lut_webcore(WEBCORE_SRC WebCore/bindings/js/JSEventListener.cpp generated_sources/WebCore/JSEventListener.lut.h WebCore/bindings/js/JSEventListener.cpp)
create_lut_webcore(WEBCORE_SRC WebCore/bindings/js/JSImageConstructor.cpp generated_sources/WebCore/JSImageConstructor.lut.h WebCore/bindings/js/JSImageConstructor.cpp)
create_lut_webcore(WEBCORE_SRC WebCore/bindings/js/JSPluginElementFunctions.cpp generated_sources/WebCore/JSPluginElementFunctions.lut.h WebCore/bindings/js/JSPluginElementFunctions.cpp)

if(ENABLE_WORKERS)
    create_lut_webcore(WEBCORE_SRC WebCore/bindings/js/JSWorkerContextBase.cpp generated_sources/WebCore/JSWorkerContextBase.lut.h WebCore/bindings/js/JSWorkerContextBase.cpp)
endif(ENABLE_WORKERS)

list(APPEND WEBCORE_SRC
    bindings/ScriptControllerBase.cpp
    bindings/js/DOMWrapperWorld.cpp
    bindings/js/DOMObjectHashTableMap.cpp
    bindings/js/GCController.cpp
    bindings/js/JSAttrCustom.cpp
    bindings/js/JSCDATASectionCustom.cpp
    bindings/js/JSCoordinatesCustom.cpp
    bindings/js/JSCSSRuleCustom.cpp
    bindings/js/JSCSSRuleListCustom.cpp
    bindings/js/JSCSSStyleDeclarationCustom.cpp
    bindings/js/JSCSSValueCustom.cpp
    bindings/js/JSCallbackData.cpp
    bindings/js/JSCanvasNumberArrayCustom.cpp
    bindings/js/JSCanvasRenderingContextCustom.cpp
    bindings/js/JSCanvasRenderingContext2DCustom.cpp
    bindings/js/JSClipboardCustom.cpp
    bindings/js/JSCustomPositionCallback.cpp
    bindings/js/JSCustomPositionErrorCallback.cpp
    bindings/js/JSCustomVoidCallback.cpp
    bindings/js/JSDebugWrapperSet.cpp
    bindings/js/JSDOMBinding.cpp
    bindings/js/JSDOMFormDataCustom.cpp
    bindings/js/JSDOMGlobalObject.cpp
    bindings/js/JSDOMWindowBase.cpp
    bindings/js/JSDOMWindowCustom.cpp
    bindings/js/JSDOMWindowShell.cpp
    bindings/js/JSDocumentCustom.cpp
    bindings/js/JSDOMWrapper.cpp
    bindings/js/JSDocumentFragmentCustom.cpp
    bindings/js/JSElementCustom.cpp
    bindings/js/JSEventCustom.cpp
    bindings/js/JSEventListener.cpp
    bindings/js/JSEventTarget.cpp
    bindings/js/JSExceptionBase.cpp
    bindings/js/JSDataGridColumnListCustom.cpp
    bindings/js/JSDataGridDataSource.h
    bindings/js/JSDataGridDataSource.cpp
    bindings/js/JSHTMLAllCollectionCustom.cpp
    bindings/js/JSHTMLAppletElementCustom.cpp
    bindings/js/JSHTMLCanvasElementCustom.cpp
    bindings/js/JSHTMLCollectionCustom.cpp
    bindings/js/JSHTMLDataGridElementCustom.cpp
    bindings/js/JSHTMLDocumentCustom.cpp
    bindings/js/JSHTMLElementCustom.cpp
    bindings/js/JSHTMLEmbedElementCustom.cpp
    bindings/js/JSHTMLFormElementCustom.cpp
    bindings/js/JSHTMLFrameElementCustom.cpp
    bindings/js/JSHTMLFrameSetElementCustom.cpp
    bindings/js/JSHTMLIFrameElementCustom.cpp
    bindings/js/JSHTMLInputElementCustom.cpp
    bindings/js/JSHTMLObjectElementCustom.cpp
    bindings/js/JSHTMLOptionsCollectionCustom.cpp
    bindings/js/JSHTMLSelectElementCustom.cpp
    bindings/js/JSHistoryCustom.cpp
    bindings/js/JSImageConstructor.cpp
    bindings/js/JSImageDataCustom.cpp
    bindings/js/JSInjectedScriptHostCustom.cpp
    bindings/js/JSInspectorFrontendHostCustom.cpp
    bindings/js/JSLazyEventListener.cpp
    bindings/js/JSLocationCustom.cpp
    bindings/js/JSMessageChannelConstructor.cpp
    bindings/js/JSMessageChannelCustom.cpp
    bindings/js/JSMessageEventCustom.cpp
    bindings/js/JSMessagePortCustom.cpp
    bindings/js/JSMimeTypeArrayCustom.cpp
    bindings/js/JSNamedNodeMapCustom.cpp
    bindings/js/JSNavigatorCustom.cpp
    bindings/js/JSNodeCustom.cpp
    bindings/js/JSNodeFilterCondition.cpp
    bindings/js/JSNodeFilterCustom.cpp
    bindings/js/JSNodeIteratorCustom.cpp
    bindings/js/JSNodeListCustom.cpp
    bindings/js/JSOptionConstructor.cpp
    bindings/js/JSPluginArrayCustom.cpp
    bindings/js/JSPluginCustom.cpp
    bindings/js/JSPluginElementFunctions.cpp
    bindings/js/JSPopStateEventCustom.cpp
    bindings/js/JSScriptProfileNodeCustom.cpp
    bindings/js/JSStyleSheetCustom.cpp
    bindings/js/JSStyleSheetListCustom.cpp
    bindings/js/JSTextCustom.cpp
    bindings/js/JSTreeWalkerCustom.cpp
    bindings/js/JSWebGLArrayBufferConstructor.cpp
    bindings/js/JSWebGLArrayCustom.cpp
    bindings/js/JSWebGLByteArrayConstructor.cpp
    bindings/js/JSWebGLByteArrayCustom.cpp
    bindings/js/JSWebGLFloatArrayConstructor.cpp
    bindings/js/JSWebGLFloatArrayCustom.cpp
    bindings/js/JSWebGLIntArrayConstructor.cpp
    bindings/js/JSWebGLIntArrayCustom.cpp
    bindings/js/JSWebGLShortArrayConstructor.cpp
    bindings/js/JSWebGLShortArrayCustom.cpp
    bindings/js/JSWebGLUnsignedByteArrayConstructor.cpp
    bindings/js/JSWebGLUnsignedByteArrayCustom.cpp
    bindings/js/JSWebGLUnsignedIntArrayConstructor.cpp
    bindings/js/JSWebGLUnsignedIntArrayCustom.cpp
    bindings/js/JSWebGLUnsignedShortArrayConstructor.cpp
    bindings/js/JSWebGLUnsignedShortArrayCustom.cpp
    bindings/js/JSWebKitCSSMatrixConstructor.cpp
    bindings/js/JSWebKitPointConstructor.cpp
    bindings/js/JSWorkerContextErrorHandler.cpp
    bindings/js/JSXMLHttpRequestConstructor.cpp
    bindings/js/JSXMLHttpRequestCustom.cpp
    bindings/js/JSXMLHttpRequestUploadCustom.cpp
    bindings/js/ScheduledAction.cpp
    bindings/js/ScriptArray.cpp
    bindings/js/ScriptCachedFrameData.cpp
    bindings/js/ScriptCallFrame.cpp
    bindings/js/ScriptCallStack.cpp
    bindings/js/ScriptController.cpp
    bindings/js/ScriptDebugServer.cpp
    bindings/js/ScriptEventListener.cpp
    bindings/js/ScriptFunctionCall.cpp
    bindings/js/ScriptObject.cpp
    bindings/js/ScriptProfiler.cpp
    bindings/js/ScriptState.cpp
    bindings/js/ScriptValue.cpp
    bindings/js/SerializedScriptValue.cpp
)

if(ENABLE_3D_CANVAS)
    list(APPEND WEBCORE_SRC
        bindings/js/JSWebGLRenderingContextCustom.cpp
    )
endif(ENABLE_3D_CANVAS)

if(ENABLE_DATABASE)
    list(APPEND WEBCORE_SRC
        bindings/js/JSCustomSQLStatementCallback.cpp
        bindings/js/JSCustomSQLStatementErrorCallback.cpp
        bindings/js/JSCustomSQLTransactionCallback.cpp
        bindings/js/JSCustomSQLTransactionErrorCallback.cpp
        bindings/js/JSDatabaseCallback.cpp
        bindings/js/JSDatabaseCustom.cpp
        bindings/js/JSSQLResultSetRowListCustom.cpp
        bindings/js/JSSQLTransactionCustom.cpp
    )
endif(ENABLE_DATABASE)

if(ENABLE_DOM_STORAGE)
    list(APPEND WEBCORE_SRC
        bindings/js/JSStorageCustom.cpp
    )
endif(ENABLE_DOM_STORAGE)

if(ENABLE_OFFLINE_WEB_APPLICATIONS)
    list(APPEND WEBCORE_SRC
        bindings/js/JSDOMApplicationCacheCustom.cpp
    )
endif(ENABLE_OFFLINE_WEB_APPLICATIONS)

if(ENABLE_GEOLOCATION)
    list(APPEND WEBCORE_SRC
        bindings/js/JSGeolocationCustom.cpp
    )
endif(ENABLE_GEOLOCATION)

if(ENABLE_INSPECTOR)
    list(APPEND WEBCORE_SRC
        bindings/js/JSConsoleCustom.cpp
        bindings/js/JSJavaScriptCallFrameCustom.cpp
    )
endif(ENABLE_INSPECTOR)

if(ENABLE_NOTIFICATIONS)
    list(APPEND WEBCORE_SRC
        bindings/js/JSDesktopNotificationsCustom.cpp
    )
endif(ENABLE_NOTIFICATIONS)


if(ENABLE_SVG)
    list(APPEND WEBCORE_SRC
        bindings/js/JSSVGElementInstanceCustom.cpp
        bindings/js/JSSVGLengthCustom.cpp
        bindings/js/JSSVGMatrixCustom.cpp
        bindings/js/JSSVGPathSegCustom.cpp
        bindings/js/JSSVGPathSegListCustom.cpp
    )
endif(ENABLE_SVG)

if(ENABLE_VIDEO)
    list(APPEND WEBCORE_SRC
        bindings/js/JSAudioConstructor.cpp
    )
endif(ENABLE_VIDEO)

if(ENABLE_WEB_SOCKETS)
    list(APPEND WEBCORE_SRC
        bindings/js/JSWebSocketConstructor.cpp
        bindings/js/JSWebSocketCustom.cpp
    )
endif(ENABLE_WEB_SOCKETS)

if(ENABLE_WORKERS)
    list(APPEND WEBCORE_SRC
        bindings/js/JSAbstractWorkerCustom.cpp
        bindings/js/JSDedicatedWorkerContextCustom.cpp
        bindings/js/JSWorkerConstructor.cpp
        bindings/js/JSWorkerContextBase.cpp
        bindings/js/JSWorkerContextCustom.cpp
        bindings/js/JSWorkerCustom.cpp
        bindings/js/WorkerScriptController.cpp
    )
    if(ENABLE_SHARED_WORKERS)
        list(APPEND WEBCORE_SRC
            bindings/js/JSSharedWorkerConstructor.cpp
            bindings/js/JSSharedWorkerCustom.cpp
        )
    endif(ENABLE_SHARED_WORKERS)
endif(ENABLE_WORKERS)

if(ENABLE_XPATH)
    list(APPEND WEBCORE_SRC
        bindings/js/JSCustomXPathNSResolver.cpp
    )
endif(ENABLE_XPATH)

if(ENABLE_XSLT)
    list(APPEND WEBCORE_SRC
        bindings/js/JSXSLTProcessorConstructor.cpp
        bindings/js/JSXSLTProcessorCustom.cpp
    )
endif(ENABLE_XSLT)

if(USE_GRAPHICS_GTK)
    list(APPEND WEBCORE_SRC
        bindings/js/ScriptControllerGtk.cpp
    )
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_QT)
    list(APPEND WEBCORE_SRC
        bindings/js/ScriptControllerBAL.cpp
    )
endif(USE_GRAPHICS_QT)

if(USE_GRAPHICS_SDL)
    list(APPEND WEBCORE_SRC
        bindings/js/ScriptControllerBAL.cpp
    )
endif(USE_GRAPHICS_SDL)

if(USE_GRAPHICS_SDLCAIRO)
    list(APPEND WEBCORE_SRC
        bindings/js/ScriptControllerBAL.cpp
    )
endif(USE_GRAPHICS_SDLCAIRO)

if (ENABLE_CEHTML)
    list(APPEND WEBCORE_SRC
        bindings/js/JSKeyboardEventCustom.cpp
    )
endif(ENABLE_CEHTML)

if(ENABLE_EVENTSOURCE)
    list(APPEND WEBCORE_SRC
        bindings/js/JSEventSourceConstructor.cpp
        bindings/js/JSEventSourceCustom.cpp
    )
endif(ENABLE_EVENTSOURCE)

if(NOT IGNORE_CUSTOMER)
    include(bindings/js/Customer/CMakeLists.txt OPTIONAL)
endif(NOT IGNORE_CUSTOMER)

