list(APPEND API_HEADER
    ${OWB_SOURCE_DIR}/BAL/Widgets/WebKit/Generic/WebWindow.h
    ${OWB_SOURCE_DIR}/BAL/Widgets/WebKit/Generic/WebWindowAlert.h
    ${OWB_SOURCE_DIR}/BAL/Widgets/WebKit/Generic/WebWindowConfirm.h
    ${OWB_SOURCE_DIR}/BAL/Widgets/WebKit/Generic/WebWindowPrompt.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/DOMCoreClasses.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/DOMHTMLClasses.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/DOMRange.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/JSActionDelegate.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/SharedObject.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/SharedPtr.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/TransferSharedPtr.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebBackForwardList.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebDownloadDelegate.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebDownload.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebDragData.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebEditingDelegate.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebError.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebFrame.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebFrameLoadDelegate.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebFramePolicyListener.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebHistoryDelegate.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebHistoryItem.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebHitTestResults.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebInspector.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebKit.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebMutableURLRequest.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebNavigationAction.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebNavigationData.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebNotificationDelegate.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebObject.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebPolicyDelegate.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebPreferences.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebResourceLoadDelegate.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebScriptObject.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebScriptWorld.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebSecurityOrigin.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebURLAuthenticationChallenge.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebURLAuthenticationChallengeSender.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebURLCredential.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebURLResponse.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebValue.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebView.h
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebWorkersPrivate.h
    ${OWB_SOURCE_DIR}/Base/WebKitDefines.h
    ${OWB_SOURCE_DIR}/Base/WebKitTypes.h
)

if(ENABLE_DATABASE)
    list(APPEND API_HEADER
        ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebDatabaseManager.h
    )
endif(ENABLE_DATABASE)

if(ENABLE_ICONDATABASE)
    list(APPEND API_HEADER
        ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/WebIconDatabase.h
    )
endif(ENABLE_ICONDATABASE)

list(APPEND BASE_INCLUDE_DIRS
    ${OWB_SOURCE_DIR}
    ${OWB_SOURCE_DIR}/Base
    ${OWB_SOURCE_DIR}/Base/wtf
)
list(APPEND WTF_INCLUDE_DIRS
    ${OWB_BINARY_DIR}/generated_link/BAL/wtf
    ${OWB_BINARY_DIR}/generated_link/BAL/wtf/text
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
    ${OWB_SOURCE_DIR}/WebCore/bindings/js
    ${OWB_SOURCE_DIR}/WebCore/bridge
    ${OWB_SOURCE_DIR}/WebCore/bridge/bal
    ${OWB_SOURCE_DIR}/WebCore/bridge/c
    ${OWB_SOURCE_DIR}/WebCore/bridge/jsc/
    ${OWB_SOURCE_DIR}/WebCore/css
    ${OWB_SOURCE_DIR}/WebCore/dom
    ${OWB_SOURCE_DIR}/WebCore/dom/default
    ${OWB_SOURCE_DIR}/WebCore/editing
    ${OWB_SOURCE_DIR}/WebCore/history
    ${OWB_SOURCE_DIR}/WebCore/html
    ${OWB_SOURCE_DIR}/WebCore/html/canvas
    ${OWB_SOURCE_DIR}/WebCore/loader
    ${OWB_SOURCE_DIR}/WebCore/loader/appcache
    ${OWB_SOURCE_DIR}/WebCore/loader/archive
    ${OWB_SOURCE_DIR}/WebCore/loader/icon
    ${OWB_SOURCE_DIR}/WebCore/page
    ${OWB_SOURCE_DIR}/WebCore/page/animation
    ${OWB_SOURCE_DIR}/WebCore/platform
    ${OWB_SOURCE_DIR}/WebCore/plugins
    ${OWB_SOURCE_DIR}/WebCore/rendering
    ${OWB_SOURCE_DIR}/WebCore/rendering/style
    ${OWB_SOURCE_DIR}/WebCore/xml
)

if(ENABLE_ACCESSIBILITY)
    list(APPEND WEBCORE_INCLUDE_DIRS
        ${OWB_SOURCE_DIR}/WebCore/accessibility
    )
endif(ENABLE_ACCESSIBILITY)
        

list(APPEND WEBKIT_INCLUDE_DIRS
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/WebCoreSupport
    ${OWB_SOURCE_DIR}/BAL/Widgets/WebKit/Generic/
)
if(USE_GRAPHICS_SDL)
    list(APPEND WEBKIT_INCLUDE_DIRS
        ${OWB_SOURCE_DIR}/BAL/Widgets/WebKit/SDL
    )
endif(USE_GRAPHICS_SDL)

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

if(ENABLE_NOTIFICATIONS)
    list(APPEND WEBCORE_INCLUDE_DIRS
        ${OWB_SOURCE_DIR}/WebCore/notifications
    )
endif(ENABLE_NOTIFICATIONS)
    
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

if(HAS_VIDEO)
    list(APPEND EXTERNAL_DEPS_INCLUDE_DIRS
        ${VIDEO_INCLUDE_DIRS}
    )
    list(APPEND EXTERNAL_DEPS_LIBRARIES
        ${VIDEO_LIBRARIES}
    )
endif(HAS_VIDEO)

if(ENABLE_WEB_SOCKETS)
    list(APPEND WEBCORE_INCLUDE_DIRS
        ${OWB_SOURCE_DIR}/WebCore/websockets
    )
endif(ENABLE_WEB_SOCKETS)

if(ENABLE_WML)
    list(APPEND WEBCORE_INCLUDE_DIRS
        ${OWB_SOURCE_DIR}/WebCore/wml
        ${OWB_BINARY_DIR}/generated_sources/WebCore/wml
    )
endif(ENABLE_WML)

if(ENABLE_WORKERS)
    list(APPEND WEBCORE_INCLUDE_DIRS
        ${OWB_BINARY_DIR}/generated_sources/WebCore/workers
        ${OWB_SOURCE_DIR}/WebCore/workers
    )
endif(ENABLE_WORKERS)

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
include(SetCEHTMLCMakeVars OPTIONAL)
include(SetDAECMakeVars OPTIONAL)
if(NOT IGNORE_CUSTOMER)
  include(Customer/SetCustomerCMakeVars OPTIONAL)
endif(NOT IGNORE_CUSTOMER)
include(SetOrigynSuiteCMakeVars OPTIONAL)
include(SetWidgetEngineCMakeVars OPTIONAL)

