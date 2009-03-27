create_lut_webcore(WEBCORE_SRC WebCore/bindings/js/JSDOMBinding.cpp generated_sources/WebCore/JSDOMBinding.lut.h WebCore/bindings/js/JSDOMBinding.cpp)
create_lut_webcore(WEBCORE_SRC WebCore/bindings/js/JSDOMWindowBase.cpp generated_sources/WebCore/JSDOMWindowBase.lut.h WebCore/bindings/js/JSDOMWindowBase.cpp)
create_lut_webcore(WEBCORE_SRC WebCore/bindings/js/JSEventListener.cpp generated_sources/WebCore/JSEventListener.lut.h WebCore/bindings/js/JSEventListener.cpp)
create_lut_webcore(WEBCORE_SRC WebCore/bindings/js/JSImageConstructor.cpp generated_sources/WebCore/JSImageConstructor.lut.h WebCore/bindings/js/JSImageConstructor.cpp)
create_lut_webcore(WEBCORE_SRC WebCore/bindings/js/JSPluginElementFunctions.cpp generated_sources/WebCore/JSPluginElementFunctions.lut.h WebCore/bindings/js/JSPluginElementFunctions.cpp)
create_lut_webcore(WEBCORE_SRC WebCore/bindings/js/JSRGBColor.cpp generated_sources/WebCore/JSRGBColor.lut.h WebCore/bindings/js/JSRGBColor.cpp)

if(ENABLE_WORKERS)
    create_lut_webcore(WEBCORE_SRC WebCore/bindings/js/JSWorkerContextBase.cpp generated_sources/WebCore/JSWorkerContextBase.lut.h WebCore/bindings/js/JSWorkerContextBase.cpp)
endif(ENABLE_WORKERS)

list(APPEND WEBCORE_SRC
    bindings/js/GCController.cpp
    bindings/js/JSAttrCustom.cpp
    bindings/js/JSCDATASectionCustom.cpp
    bindings/js/JSCSSRuleCustom.cpp
    bindings/js/JSCSSStyleDeclarationCustom.cpp
    bindings/js/JSCSSValueCustom.cpp
    bindings/js/JSCanvasRenderingContext2DCustom.cpp
    bindings/js/JSClipboardCustom.cpp
    bindings/js/JSCustomPositionCallback.cpp
    bindings/js/JSCustomPositionErrorCallback.cpp
    bindings/js/JSCustomVoidCallback.cpp
    bindings/js/JSDOMBinding.cpp
    bindings/js/JSDOMGlobalObject.cpp
    bindings/js/JSDOMStringListCustom.cpp
    bindings/js/JSDOMWindowBase.cpp
    bindings/js/JSDOMWindowCustom.cpp
    bindings/js/JSDOMWindowShell.cpp
    bindings/js/JSDocumentCustom.cpp
    bindings/js/JSDocumentFragmentCustom.cpp
    bindings/js/JSElementCustom.cpp
    bindings/js/JSEventCustom.cpp
    bindings/js/JSEventListener.cpp
    bindings/js/JSEventTarget.cpp
    bindings/js/JSGeolocationCustom.cpp
    bindings/js/JSHTMLAllCollection.cpp
    bindings/js/JSHTMLAppletElementCustom.cpp
    bindings/js/JSHTMLCollectionCustom.cpp
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
    bindings/js/JSLazyEventListener.cpp
    bindings/js/JSLocationCustom.cpp
    bindings/js/JSMessageChannelConstructor.cpp
    bindings/js/JSMessageChannelCustom.cpp
    bindings/js/JSMessagePortCustom.cpp
    bindings/js/JSMimeTypeArrayCustom.cpp
    bindings/js/JSNamedNodeMapCustom.cpp
    bindings/js/JSNamedNodesCollection.cpp
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
    bindings/js/JSQuarantinedObjectWrapper.cpp
    bindings/js/JSRGBColor.cpp
    bindings/js/JSStyleSheetCustom.cpp
    bindings/js/JSStyleSheetListCustom.cpp
    bindings/js/JSTextCustom.cpp
    bindings/js/JSTreeWalkerCustom.cpp
    bindings/js/JSWebKitCSSMatrixConstructor.cpp
    bindings/js/JSWebKitPointConstructor.cpp
    bindings/js/JSXMLHttpRequestConstructor.cpp
    bindings/js/JSXMLHttpRequestCustom.cpp
    bindings/js/JSXMLHttpRequestUploadCustom.cpp
    bindings/js/ScheduledAction.cpp
    bindings/js/ScriptCachedFrameData.cpp
    bindings/js/ScriptCallFrame.cpp
    bindings/js/ScriptCallStack.cpp
    bindings/js/ScriptController.cpp
    bindings/js/ScriptFunctionCall.cpp
    bindings/js/ScriptObject.cpp
    bindings/js/ScriptValue.cpp
)

if(ENABLE_DATABASE)
    list(APPEND WEBCORE_SRC
        bindings/js/JSCustomSQLStatementCallback.cpp
        bindings/js/JSCustomSQLStatementErrorCallback.cpp
        bindings/js/JSCustomSQLTransactionCallback.cpp
        bindings/js/JSCustomSQLTransactionErrorCallback.cpp
        bindings/js/JSDatabaseCustom.cpp
        bindings/js/JSSQLResultSetRowListCustom.cpp
        bindings/js/JSSQLTransactionCustom.cpp
        bindings/js/ScriptObjectQuarantine.cpp
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

if(ENABLE_INSPECTOR)
    list(APPEND WEBCORE_SRC
        bindings/js/JSConsoleCustom.cpp
        bindings/js/JSInspectedObjectWrapper.cpp
        bindings/js/JSInspectorCallbackWrapper.cpp
        bindings/js/JSInspectorControllerCustom.cpp
        bindings/js/JSJavaScriptCallFrameCustom.cpp
    )
endif(ENABLE_INSPECTOR)

if(ENABLE_SVG)
    list(APPEND WEBCORE_SRC
        bindings/js/JSSVGElementInstanceCustom.cpp
        bindings/js/JSSVGLengthCustom.cpp
        bindings/js/JSSVGMatrixCustom.cpp
        bindings/js/JSSVGPathSegCustom.cpp
        bindings/js/JSSVGPathSegListCustom.cpp
        bindings/js/JSSVGPointListCustom.cpp
        bindings/js/JSSVGTransformListCustom.cpp
    )
endif(ENABLE_SVG)

if(ENABLE_VIDEO)
    list(APPEND WEBCORE_SRC
        bindings/js/JSAudioConstructor.cpp
    )
endif(ENABLE_VIDEO)

if(ENABLE_WORKERS)
    list(APPEND WEBCORE_SRC
        bindings/js/JSWorkerConstructor.cpp
        bindings/js/JSWorkerContextBase.cpp
        bindings/js/JSWorkerContextCustom.cpp
        bindings/js/JSWorkerCustom.cpp
        bindings/js/WorkerScriptController.cpp
    )
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

include(bindings/js/Customer/CMakeLists.txt OPTIONAL)

