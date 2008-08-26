INCLUDE_DIRECTORIES (
    ${CMAKE_SOURCE_DIR}/BAL/Base
    ${CMAKE_SOURCE_DIR}/BAL/Base/wtf
    ${CMAKE_BINARY_DIR}/generated_link/OWBAL
    ${CMAKE_BINARY_DIR}/generated_link/WKAL
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/JavaScriptCore
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/VM
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/kjs
    ${CMAKE_SOURCE_DIR}/WebCore
    ${CMAKE_SOURCE_DIR}/WebCore/bridge
    ${CMAKE_SOURCE_DIR}/WebCore/css
    ${CMAKE_SOURCE_DIR}/WebCore/dom
    ${CMAKE_SOURCE_DIR}/WebCore/editing
    ${CMAKE_SOURCE_DIR}/WebCore/history
    ${CMAKE_SOURCE_DIR}/WebCore/html
    ${CMAKE_SOURCE_DIR}/WebCore/loader
    ${CMAKE_SOURCE_DIR}/WebCore/page
    ${CMAKE_SOURCE_DIR}/WebCore/svg
    ${CMAKE_SOURCE_DIR}/WebCore/loader/icon
    ${CMAKE_SOURCE_DIR}/WebCore/bindings/js
    ${CMAKE_SOURCE_DIR}/WebCore/rendering
    ${CMAKE_SOURCE_DIR}/WebCore/rendering/style
    ${CMAKE_SOURCE_DIR}/WebCore/storage
    ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser
    ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api
    ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/WebCoreSupport
    ${CMAKE_SOURCE_DIR}/WebKitTools/OrigynWebBrowser
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/ForwardingHeaders
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/generated_sources/WebKit
)

IF (WEBKIT_USE_I18N STREQUAL "GENERIC")
    INCLUDE_DIRECTORIES (
        ${CMAKE_SOURCE_DIR}/JavaScriptCore/icu
    )
ENDIF (WEBKIT_USE_I18N STREQUAL "GENERIC")

IF (OWBAL_PLATFORM_GRAPHICS STREQUAL "GTK")
    LINK_DIRECTORIES (${GTK2_LIBRARY_DIRS})
    INCLUDE_DIRECTORIES (${GTK2_INCLUDE_DIRS})

    ADD_EXECUTABLE (owb gtk/main${TARGET}.cpp)
ELSEIF (OWBAL_PLATFORM_GRAPHICS STREQUAL "SDL")
    INCLUDE_DIRECTORIES (
        ${SDL_INCLUDE_DIRS}
    )
    ADD_EXECUTABLE (owb SDL/main${TARGET}.cpp)
ENDIF (OWBAL_PLATFORM_GRAPHICS STREQUAL "GTK")

INCLUDE (Customer/CMakeLists.txt OPTIONAL)

TARGET_LINK_LIBRARIES (owb
    generated-sources
    webkit
    webcore-owb
    bal-events
    bal-facilities
    bal-fonts
    bal-graphics
    bal-imagedecoder
    bal-internationalization
    bal-media
    bal-memory
    bal-network
    bal-types
    bal-widgets
    ${DEEPSEE_LINK}
)

IF (WEBKIT_USE_DATABASE)
    TARGET_LINK_LIBRARIES (owb
        bal-database
    )
ENDIF (WEBKIT_USE_DATABASE)

TARGET_LINK_LIBRARIES (owb
    jscore
)

IF (WEBKIT_USE_HTML_EXTENSION)
    TARGET_LINK_LIBRARIES (owb
        htmlext
    )
ENDIF (WEBKIT_USE_HTML_EXTENSION)

IF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")
    TARGET_LINK_LIBRARIES (owb ${GTK2_LIBRARIES})
ELSEIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "SDL")
    TARGET_LINK_LIBRARIES (owb ${SDL_LIBRARIES})
ENDIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")

INSTALL (TARGETS owb 
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
INSTALL (FILES resources/missing-plugin.png
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/owb/resources
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
)
