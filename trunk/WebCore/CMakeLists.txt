##############################
##############################
##############################
#WEBCORE
##############################
##############################
##############################

# Add CXXFLAGS here
ADD_DEFINITIONS (-include ${CMAKE_SOURCE_DIR}/BAL/Base/config.h)

INCLUDE_DIRECTORIES (
    ${CMAKE_SOURCE_DIR}/BAL/Base
    ${CMAKE_SOURCE_DIR}/BAL/Base/wtf
    ${CMAKE_BINARY_DIR}/generated_link/OWBAL
    ${CMAKE_BINARY_DIR}/generated_link/WKAL

    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/JavaScriptCore
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/debugger
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/interpreter
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/ForwardingHeaders
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/parser
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/runtime
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/bytecode
    bindings/js
    bridge
    bridge/c
    bridge/bal
    css
    dom
    editing
    history
    html
    inspector
    loader
    loader/icon
    loader/archive
    loader/appcache
    page
    page/animation
    plugins
    rendering
    rendering/style
    storage
    svg
    svg/graphics
    svg/graphics/filters
    svg/animation/
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/bindings
    ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/
    ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api
    ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/WebCoreSupport
    xml
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/xml
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/svg
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/wml

    ${XML2_INCLUDE_DIRS}
    ${HTML_EXT_CFLAGS}
)
IF (WEBKIT_ENABLE_XSLT)
    INCLUDE_DIRECTORIES (
        ${XSLT_INCLUDE_DIRS}
    )
ENDIF (WEBKIT_ENABLE_XSLT)

IF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")
    LINK_DIRECTORIES (${GTK2_LIBRARY_DIRS})
    INCLUDE_DIRECTORIES (${GTK2_INCLUDE_DIRS})
ENDIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")

IF (WEBKIT_USE_I18N STREQUAL "GENERIC")
    INCLUDE_DIRECTORIES (
        ${CMAKE_SOURCE_DIR}/JavaScriptCore/icu
    )
ENDIF (WEBKIT_USE_I18N STREQUAL "GENERIC")

IF (WEBKIT_USE_HTML_EXTENSION)
    INCLUDE_DIRECTORIES (
        ${CMAKE_SOURCE_DIR}/HtmlExt/Factory
        ${CMAKE_SOURCE_DIR}/HtmlExt/HtmlElement
        ${CMAKE_SOURCE_DIR}/HtmlExt/Renderer
    )
ENDIF (WEBKIT_USE_HTML_EXTENSION)


INCLUDE (bridge/CMakeLists.txt)
INCLUDE (css/CMakeLists.txt)
INCLUDE (dom/CMakeLists.txt)
INCLUDE (editing/CMakeLists.txt)
INCLUDE (history/CMakeLists.txt)
INCLUDE (html/CMakeLists.txt)
INCLUDE (loader/CMakeLists.txt)
INCLUDE (loader/icon/CMakeLists.txt)
INCLUDE (page/CMakeLists.txt)
INCLUDE (platform/CMakeLists.txt)
INCLUDE (plugins/CMakeLists.txt)
INCLUDE (rendering/CMakeLists.txt)
INCLUDE (storage/CMakeLists.txt)
INCLUDE (svg/CMakeLists.txt)
IF (WEBKIT_USE_WML)
    INCLUDE (wml/CMakeLists.txt)
    INCLUDE_DIRECTORIES (
        wml
    )
ENDIF (WEBKIT_USE_WML)
INCLUDE (xml/CMakeLists.txt)
IF (WEBKIT_ENABLE_INSPECTOR_CONTROLLER)
    INCLUDE (inspector/CMakeLists.txt)
ENDIF (WEBKIT_ENABLE_INSPECTOR_CONTROLLER)
LIST (APPEND WEBCORE_SRC
    ${BRIDGE_SRC}
    ${CSS_SRC}
    ${DOM_SRC}
    ${EDITING_SRC}
    ${HISTORY_SRC}
    ${HTML_SRC}
    ${ICON_SRC}
    ${LOADER_SRC}
    ${PAGE_SRC}
    ${PLUGINS_SRC}
    ${RENDERING_SRC}
    ${STORAGE_SRC}
    ${SVG_SRC}
    ${XML_SRC}
)

IF (NOT ${OWBAL_PLATFORM_GRAPHICS} STREQUAL "SDL")
    LIST (APPEND WEBCORE_SRC
        ${PLATFORM_SRC}
    )
ENDIF (NOT ${OWBAL_PLATFORM_GRAPHICS} STREQUAL "SDL")

IF (WEBKIT_USE_WML)
    LIST (APPEND WEBCORE_SRC
        ${WML_SRC}
    )
ENDIF (WEBKIT_USE_WML)

INCLUDE (bindings/js/CMakeLists.txt)
LIST (APPEND WEBCORE_SRC 
    ${BINDINGS_JS_SRC}
)

IF (WEBKIT_ENABLE_INSPECTOR_CONTROLLER)
LIST (APPEND WEBCORE_SRC 
    ${INSPECTOR_SRC}
)
ENDIF (WEBKIT_ENABLE_INSPECTOR_CONTROLLER)

IF (BUILD_SHARED_LIBS)
    ADD_LIBRARY (webcore-owb ${WEBCORE_SRC})
ELSE (BUILD_SHARED_LIBS)
    ADD_LIBRARY (webcore-owb STATIC ${WEBCORE_SRC})
ENDIF (BUILD_SHARED_LIBS)

TARGET_LINK_LIBRARIES (webcore-owb ${XML2_LIBRARIES})
IF (WEBKIT_ENABLE_XSLT)
    TARGET_LINK_LIBRARIES (webcore-owb ${XSLT_LIBRARIES})
ENDIF (WEBKIT_ENABLE_XSLT)

IF (WEBKIT_USE_NPAPI AND ${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")
    TARGET_LINK_LIBRARIES (webcore-owb ${XT_LIBRARIES})
ENDIF (WEBKIT_USE_NPAPI AND ${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")

IF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")
    TARGET_LINK_LIBRARIES (webcore-owb ${GTK2_LIBRARIES})
ENDIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")

INSTALL (TARGETS webcore-owb
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

ADD_DEPENDENCIES (webcore-owb generated-sources)
