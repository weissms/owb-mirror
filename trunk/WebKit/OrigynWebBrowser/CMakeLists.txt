
IF (OWBAL_PLATFORM_GRAPHICS STREQUAL "GTK")
    LINK_DIRECTORIES (${GTK2_LIBRARY_DIRS})
    ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.h ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.cpp
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND mkdir -p generated_sources/WebKit
        #COMMAND echo 'extern \\"C\\" {' > ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.h && glib-genmarshal --prefix=webkit_marshal webkit-marshal.list --body >> ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.h && echo '}' >> ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.h
        COMMAND echo 'extern \"C\" {' > ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.h
        COMMAND glib-genmarshal --prefix=webkit_marshal ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/gtk/webkit-marshal.list --header >> generated_sources/WebKit/webkit-marshal.h
        COMMAND echo '}' >> ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.h
    
        COMMAND echo 'extern \"C\" {' > ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.cpp
        COMMAND glib-genmarshal --prefix=webkit_marshal ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/gtk/webkit-marshal.list --body >> generated_sources/WebKit/webkit-marshal.cpp
        COMMAND echo '}' >> ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.cpp
        DEPENDS ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/gtk/webkit-marshal.list
    )
ENDIF (OWBAL_PLATFORM_GRAPHICS STREQUAL "GTK")

IF (OWBAL_BENCH_LOAD_TIME)
    ADD_DEFINITIONS (-DBENCH_LOAD_TIME=1)
ENDIF (OWBAL_BENCH_LOAD_TIME)
ADD_DEFINITIONS (${BINDING_DEFINITONS})
ADD_DEFINITIONS (-include ${CMAKE_SOURCE_DIR}/BAL/Base/config.h)

INCLUDE_DIRECTORIES (
    Api
    WebCoreSupport
    ${CMAKE_SOURCE_DIR}/BAL/Base
    ${CMAKE_SOURCE_DIR}/BAL/Base/wtf
    ${CMAKE_SOURCE_DIR}/BAL/Includes
    ${CMAKE_SOURCE_DIR}/AddOns/BindingJS/Interfaces/
    ${CMAKE_BINARY_DIR}/generated_link/OWBAL
    ${CMAKE_BINARY_DIR}/generated_link/WKAL

    ${CMAKE_SOURCE_DIR}/JavaScriptCore
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/kjs
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/VM
    #${CMAKE_SOURCE_DIR}/JavaScriptCore/wtf
    ${CMAKE_SOURCE_DIR}/WebCore
    ${CMAKE_SOURCE_DIR}/WebCore/bridge
    ${CMAKE_SOURCE_DIR}/WebCore/bridge/bal
    ${CMAKE_SOURCE_DIR}/WebCore/css
    ${CMAKE_SOURCE_DIR}/WebCore/dom
    ${CMAKE_SOURCE_DIR}/WebCore/editing
    ${CMAKE_SOURCE_DIR}/WebCore/history
    ${CMAKE_SOURCE_DIR}/WebCore/html
    ${CMAKE_SOURCE_DIR}/WebCore/loader
    ${CMAKE_SOURCE_DIR}/WebCore/loader/archive
    ${CMAKE_SOURCE_DIR}/WebCore/page
    ${CMAKE_SOURCE_DIR}/WebCore/svg
    ${CMAKE_SOURCE_DIR}/WebCore/loader/icon
    ${CMAKE_SOURCE_DIR}/WebCore/bindings/js
    ${CMAKE_SOURCE_DIR}/WebCore/storage
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/API
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/ForwardingHeaders/
    ${CMAKE_BINARY_DIR}/generated_sources/WebKit
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/xml
    ${CMAKE_SOURCE_DIR}/WebCore/plugins
    ${CMAKE_SOURCE_DIR}/WebCore/rendering
    ${CMAKE_SOURCE_DIR}/WebCore/rendering/style
)
IF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")
    INCLUDE_DIRECTORIES (
        Api/gtk
        ${GTK2_INCLUDE_DIRS}
    )
ELSEIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "SDL")
    INCLUDE_DIRECTORIES (
        ${SDL_INCLUDE_DIRS}
        Api/SDL
    )
ENDIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")

IF (WEBKIT_USE_N800_HILDON)
    # be sure to add -DMAEMO_CHANGES
    ADD_DEFINITIONS (${HILDON_CFLAGS_OTHER})
    INCLUDE_DIRECTORIES (${HILDON_INCLUDE_DIRS})
ENDIF (WEBKIT_USE_N800_HILDON)

IF (WEBKIT_USE_I18N STREQUAL "GENERIC")
    INCLUDE_DIRECTORIES (
        ${CMAKE_SOURCE_DIR}/JavaScriptCore/icu
    )
ENDIF (WEBKIT_USE_I18N STREQUAL "GENERIC")

AUX_SOURCE_DIRECTORY (Api WEBKIT_SRC)
AUX_SOURCE_DIRECTORY (WebCoreSupport WEBKIT_SRC)

IF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")
    LIST (APPEND WEBKIT_SRC
#        Api/gtk/WebViewGtk.cpp
        Api/gtk/browserWidget.cpp
        ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.cpp
    )
ELSEIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "SDL")
    LIST (APPEND WEBKIT_SRC
        Api/SDL/WebViewPrivate.cpp
    )
ENDIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")

IF (BINDINGS_JS_CFLAGS)
SET_SOURCE_FILES_PROPERTIES (
    ${CMAKE_CURRENT_SOURCE_DIR}/Api/WebFrame.cpp
    PROPERTIES COMPILE_FLAGS "${BINDINGS_JS_CFLAGS}"
)
ENDIF ( BINDINGS_JS_CFLAGS)

INCLUDE (Customer/CMakeLists.txt OPTIONAL)

IF (BUILD_SHARED_LIBS)
    ADD_LIBRARY (webkit ${WEBKIT_SRC})
ELSE (BUILD_SHARED_LIBS)
    ADD_LIBRARY (webkit STATIC ${WEBKIT_SRC})
ENDIF (BUILD_SHARED_LIBS)
TARGET_LINK_LIBRARIES (webkit
    bal-graphics
    ${EXTRA_LDFLAGS}
)

IF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")
    TARGET_LINK_LIBRARIES (webkit ${GTK2_LIBRARIES})
ENDIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")

INSTALL (TARGETS webkit
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
ADD_DEPENDENCIES (webkit generated-sources)


