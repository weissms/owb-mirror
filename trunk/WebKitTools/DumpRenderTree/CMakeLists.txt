ADD_DEFINITIONS (${BINDING_DEFINITONS})
INCLUDE_DIRECTORIES (
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/BAL/Base
    ${CMAKE_SOURCE_DIR}/BAL/Base/wtf
    ${CMAKE_BINARY_DIR}/generated_link/OWBAL
    ${CMAKE_BINARY_DIR}/generated_link/WKAL
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore
    ${CMAKE_SOURCE_DIR}/JavaScriptCore
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/VM
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/kjs
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/ForwardingHeaders
    ${CMAKE_SOURCE_DIR}/WebCore
    ${CMAKE_SOURCE_DIR}/WebCore/bindings/js
    ${CMAKE_SOURCE_DIR}/WebCore/bridge
    ${CMAKE_SOURCE_DIR}/WebCore/css
    ${CMAKE_SOURCE_DIR}/WebCore/dom
    ${CMAKE_SOURCE_DIR}/WebCore/editing
    ${CMAKE_SOURCE_DIR}/WebCore/history
    ${CMAKE_SOURCE_DIR}/WebCore/html
    ${CMAKE_SOURCE_DIR}/WebCore/loader
    ${CMAKE_SOURCE_DIR}/WebCore/page
    ${CMAKE_SOURCE_DIR}/WebCore/rendering
    ${CMAKE_SOURCE_DIR}/WebCore/rendering/style
    ${CMAKE_SOURCE_DIR}/WebCore/svg
    ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/WebCoreSupport
    ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/
)

IF (WEBKIT_USE_I18N STREQUAL "GENERIC")
    INCLUDE_DIRECTORIES (
        ${CMAKE_SOURCE_DIR}/JavaScriptCore/icu
    )
ENDIF (WEBKIT_USE_I18N STREQUAL "GENERIC")

LIST (APPEND DUMPRENDERTREE_SRC
    LayoutTestController.cpp
    GCController.cpp
    WorkQueue.cpp
    AccessibilityController.cpp
    AccessibilityUIElement.cpp
    bal/AccessibilityControllerBal.cpp
    bal/AccessibilityUIElementBal.cpp
    bal/DumpRenderTree.cpp
    bal/LayoutTestControllerBal.cpp    
    bal/WorkQueueItemBal.cpp
    bal/GCControllerBal.cpp
    bal/EventSender.cpp
)

IF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")
    LINK_DIRECTORIES (${GTK2_LIBRARY_DIRS})
    INCLUDE_DIRECTORIES (${GTK2_INCLUDE_DIRS})
    LIST (APPEND DUMPRENDERTREE_SRC
        bal/gtk/DumpRenderTreeGtk.cpp
    )
ELSEIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "SDL")
    LIST (APPEND DUMPRENDERTREE_SRC
        bal/SDL/DumpRenderTreeSDL.cpp
    )
ENDIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")

INCLUDE (bal/Customer/CMakeLists.txt OPTIONAL)

ADD_EXECUTABLE (DumpRenderTree
    ${DUMPRENDERTREE_SRC}
)

TARGET_LINK_LIBRARIES (DumpRenderTree
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
    ${EXTRA_LDFLAGS}
)

IF (WEBKIT_USE_DATABASE)
    TARGET_LINK_LIBRARIES (DumpRenderTree
        bal-database
    )
ENDIF (WEBKIT_USE_DATABASE)

TARGET_LINK_LIBRARIES (DumpRenderTree
    jscore
)

IF (WEBKIT_USE_HTML_EXTENSION)
    TARGET_LINK_LIBRARIES (DumpRenderTree
        htmlext
    )
ENDIF (WEBKIT_USE_HTML_EXTENSION)

IF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")
    TARGET_LINK_LIBRARIES (DumpRenderTree ${GTK2_LIBRARIES})
ELSEIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "SDL")
    TARGET_LINK_LIBRARIES (DumpRenderTree ${SDL_LIBRARIES})
ENDIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")

INCLUDE (bal/Customer/CMakeLinks.txt OPTIONAL)
