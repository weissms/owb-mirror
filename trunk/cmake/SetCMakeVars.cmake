list(APPEND API_HEADER
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/JSActionDelegate.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebBackForwardList.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebFrame.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebFrameLoadDelegate.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebHistoryItem.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebKit.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebNotificationDelegate.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebDownloadDelegate.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebDownload.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebView.h
    ${OWB_SOURCE_DIR}/Base/WebKitDefines.h
    ${OWB_SOURCE_DIR}/Base/WebKitTypes.h
)

if(USE_GRAPHICS_SDL)
    list(APPEND API_HEADER
        ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/SDL/WebWindow.h
        ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/SDL/WebWindowAlert.h
        ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/SDL/WebWindowConfirm.h
        ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/SDL/WebWindowPrompt.h
    )
endif(USE_GRAPHICS_SDL)

list(APPEND BASE_INCLUDE_DIRS
    ${OWB_SOURCE_DIR}
    ${OWB_SOURCE_DIR}/Base
    ${OWB_SOURCE_DIR}/Base/wtf
)
list(APPEND WTF_INCLUDE_DIRS
    ${OWB_BINARY_DIR}/generated_link/BAL/wtf
    ${OWB_BINARY_DIR}/generated_link/BAL/wtf/unicode
)
list(APPEND BAL_INCLUDE_DIRS
    ${OWB_BINARY_DIR}/generated_link/BAL
)
list(APPEND JAVASCRIPTCORE_INCLUDE_DIRS
    ${OWB_SOURCE_DIR}/JavaScriptCore
    ${OWB_SOURCE_DIR}/JavaScriptCore/assembler
    ${OWB_SOURCE_DIR}/JavaScriptCore/API
    ${OWB_SOURCE_DIR}/JavaScriptCore/bytecode
    ${OWB_SOURCE_DIR}/JavaScriptCore/bytecompiler
    ${OWB_SOURCE_DIR}/JavaScriptCore/debugger
    ${OWB_SOURCE_DIR}/JavaScriptCore/ForwardingHeaders
    ${OWB_SOURCE_DIR}/JavaScriptCore/interpreter
    ${OWB_SOURCE_DIR}/JavaScriptCore/jit
    ${OWB_SOURCE_DIR}/JavaScriptCore/parser
    ${OWB_SOURCE_DIR}/JavaScriptCore/pcre
    ${OWB_SOURCE_DIR}/JavaScriptCore/profiler
    ${OWB_SOURCE_DIR}/JavaScriptCore/runtime
    ${OWB_SOURCE_DIR}/JavaScriptCore/wrec
    ${OWB_SOURCE_DIR}/JavaScriptCore/yarr
)
list(APPEND WEBCORE_INCLUDE_DIRS
    ${OWB_BINARY_DIR}/generated_sources/WebCore/workers
    ${OWB_SOURCE_DIR}/WebCore/accessibility
    ${OWB_SOURCE_DIR}/WebCore/bindings/js
    ${OWB_SOURCE_DIR}/WebCore/bridge
    ${OWB_SOURCE_DIR}/WebCore/bridge/bal
    ${OWB_SOURCE_DIR}/WebCore/bridge/c
    ${OWB_SOURCE_DIR}/WebCore/css
    ${OWB_SOURCE_DIR}/WebCore/dom
    ${OWB_SOURCE_DIR}/WebCore/editing
    ${OWB_SOURCE_DIR}/WebCore/history
    ${OWB_SOURCE_DIR}/WebCore/html
    ${OWB_SOURCE_DIR}/WebCore/loader
    ${OWB_SOURCE_DIR}/WebCore/loader/appcache
    ${OWB_SOURCE_DIR}/WebCore/loader/archive
    ${OWB_SOURCE_DIR}/WebCore/loader/icon
    ${OWB_SOURCE_DIR}/WebCore/page
    ${OWB_SOURCE_DIR}/WebCore/page/animation
    ${OWB_SOURCE_DIR}/WebCore/plugins
    ${OWB_SOURCE_DIR}/WebCore/rendering
    ${OWB_SOURCE_DIR}/WebCore/rendering/style
    ${OWB_SOURCE_DIR}/WebCore/workers
    ${OWB_SOURCE_DIR}/WebCore/xml
)
list(APPEND WEBKIT_INCLUDE_DIRS
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/WebCoreSupport
)
list(APPEND EXTERNAL_DEPS_INCLUDE_DIRS
    ${OWB_BASE_DEPS_INCLUDE_DIRS}
    ${FILESYSTEM_INCLUDE_DIRS}
    ${FONTS_INCLUDE_DIRS}
    ${GEOLOCATION_INCLUDE_DIRS}
    ${GRAPHICS_INCLUDE_DIRS}
    ${IMAGE_INCLUDE_DIRS}
    ${I18N_INCLUDE_DIRS}
    ${NETWORK_INCLUDE_DIRS}
    ${THREADS_INCLUDE_DIRS}
    ${TIMER_INCLUDE_DIRS}
)
list(APPEND EXTERNAL_DEPS_LIBRARIES
    ${OWB_BASE_DEPS_LIBRARIES}
    ${FILESYSTEM_LIBRARIES}
    ${FONTS_LIBRARIES}
    ${GEOLOCATION_LIBRARIES}
    ${GRAPHICS_LIBRARIES}
    ${IMAGE_LIBRARIES}
    ${I18N_LIBRARIES}
    ${NETWORK_LIBRARIES}
    ${THREADS_LIBRARIES}
    ${TIMER_LIBRARIES}
)

if(ENABLE_DATABASE)
    list(APPEND WEBCORE_INCLUDE_DIRS
        ${OWB_SOURCE_DIR}/WebCore/storage
    )
    list(APPEND EXTERNAL_DEPS_INCLUDE_DIRS
        ${DATABASE_INCLUDE_DIRS}
    )
    list(APPEND EXTERNAL_DEPS_LIBRARIES
        ${DATABASE_LIBRARIES}
    )
endif(ENABLE_DATABASE)

if(ENABLE_INSPECTOR)
    list(APPEND WEBCORE_INCLUDE_DIRS
        ${OWB_SOURCE_DIR}/WebCore/inspector
    )
endif(ENABLE_INSPECTOR)

if(ENABLE_JIT_JSC OR ENABLE_JIT_REGEXP)
    list(APPEND JAVASCRIPTCORE_INCLUDE_DIRS
        ${OWB_SOURCE_DIR}/JavaScriptCore/assembler
    )
endif(ENABLE_JIT_JSC OR ENABLE_JIT_REGEXP)
    
if(ENABLE_NPAPI)
    list(APPEND EXTERNAL_DEPS_INCLUDE_DIRS
        ${NPAPI_INCLUDE_DIRS}
    )
    list(APPEND EXTERNAL_DEPS_LIBRARIES
        ${NPAPI_LIBRARIES}
    )
endif(ENABLE_NPAPI)

if(ENABLE_SVG OR ENABLE_FILTERS)
    list(APPEND WEBCORE_INCLUDE_DIRS
        ${OWB_SOURCE_DIR}/WebCore/svg
        ${OWB_SOURCE_DIR}/WebCore/svg/animation
        ${OWB_SOURCE_DIR}/WebCore/svg/graphics
        ${OWB_SOURCE_DIR}/WebCore/svg/graphics/filters
    )
endif(ENABLE_SVG OR ENABLE_FILTERS)

if(ENABLE_VIDEO)
    list(APPEND EXTERNAL_DEPS_INCLUDE_DIRS
        ${VIDEO_INCLUDE_DIRS}
    )
    list(APPEND EXTERNAL_DEPS_LIBRARIES
        ${VIDEO_LIBRARIES}
    )
endif(ENABLE_VIDEO)

if(ENABLE_WML)
    list(APPEND WEBCORE_INCLUDE_DIRS
        ${OWB_SOURCE_DIR}/WebCore/wml
        ${OWB_BINARY_DIR}/generated_sources/WebCore/wml
    )
endif(ENABLE_WML)

if(ENABLE_XSLT)
    list(APPEND EXTERNAL_DEPS_INCLUDE_DIRS
        ${LIBXSLT_INCLUDE_DIRS}
    )
    list(APPEND EXTERNAL_DEPS_LIBRARIES
        ${LIBXSLT_LIBRARIES}
    )
endif(ENABLE_XSLT)

if(USE_GRAPHICS_QT)
    list(APPEND BAL_INCLUDE_DIRS
        ${OWB_BINARY_DIR}/generated_sources/moc
    )
    list(APPEND EXTERNAL_DEPS_INCLUDE_DIRS
        ${OWB_BINARY_DIR}/generated_sources/moc
    )
endif(USE_GRAPHICS_QT)

include(SetAddonsCMakeVars OPTIONAL)
include(SetCustomerCMakeVars OPTIONAL)
include(SetOrigynSuiteCMakeVars OPTIONAL)

