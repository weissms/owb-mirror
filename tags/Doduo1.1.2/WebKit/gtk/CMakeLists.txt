ADD_DEFINITIONS (${BINDING_DEFINITONS})
ADD_DEFINITIONS (-include ${CMAKE_SOURCE_DIR}/BAL/Base/config.h)

INCLUDE_DIRECTORIES (
    Api
    WebCoreSupport
    ${CMAKE_SOURCE_DIR}/BAL/Base
    ${CMAKE_SOURCE_DIR}/BAL/Base/wtf
    ${CMAKE_BINARY_DIR}/generated_link/OWBAL
    ${CMAKE_BINARY_DIR}/generated_link/WKAL

    ${CMAKE_SOURCE_DIR}/JavaScriptCore
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/VM
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/bindings
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/bindings/bal
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/kjs
    #${CMAKE_SOURCE_DIR}/JavaScriptCore/wtf
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
    ${CMAKE_SOURCE_DIR}/WebCore/rendering
    ${CMAKE_SOURCE_DIR}/WebCore/rendering/style
#FIXME_CMAKE
    ${CMAKE_SOURCE_DIR}/WebCore/loader/icon
    ${CMAKE_SOURCE_DIR}/WebCore/bindings/js
    ${CMAKE_SOURCE_DIR}/WebCore/storage
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/WebKit/gtk
    ${CMAKE_SOURCE_DIR}/WebKit/webkit
    ${GTK2_INCLUDE_DIRS}
    webkit
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/API
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/ForwardingHeaders/
    ${CMAKE_BINARY_DIR}/generated_sources/WebKit
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/xml
#ENDFIXME_CMAKE
    ${CMAKE_SOURCE_DIR}/WebCore/plugins
    ${CMAKE_SOURCE_DIR}/WebCore/rendering
)

IF (WEBKIT_USE_N800_HILDON)
    # be sure to add -DMAEMO_CHANGES
    ADD_DEFINITIONS (${HILDON_CFLAGS_OTHER})
    INCLUDE_DIRECTORIES (${HILDON_INCLUDE_DIRS})
ENDIF (WEBKIT_USE_N800_HILDON)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.h ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.cpp
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND mkdir -p generated_sources/WebKit
    #COMMAND echo 'extern \\"C\\" {' > ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.h && glib-genmarshal --prefix=webkit_marshal webkit-marshal.list --body >> ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.h && echo '}' >> ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.h
    COMMAND echo 'extern \"C\" {' > ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.h
    COMMAND glib-genmarshal --prefix=webkit_marshal ${CMAKE_SOURCE_DIR}/WebKit/gtk/webkit/webkit-marshal.list --header >> generated_sources/WebKit/webkit-marshal.h
    COMMAND echo '}' >> ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.h

    COMMAND echo 'extern \"C\" {' > ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.cpp
    COMMAND glib-genmarshal --prefix=webkit_marshal ${CMAKE_SOURCE_DIR}/WebKit/gtk/webkit/webkit-marshal.list --body >> generated_sources/WebKit/webkit-marshal.cpp
    COMMAND echo '}' >> ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.cpp
    DEPENDS ${CMAKE_SOURCE_DIR}/WebKit/gtk/webkit/webkit-marshal.list
)

AUX_SOURCE_DIRECTORY (Api API_SRC)
LIST (APPEND WEBKIT_SRC
    ${API_SRC}
    WebCoreSupport/ChromeClientGtk.cpp
    WebCoreSupport/ContextMenuClientGtk.cpp
    WebCoreSupport/DragClientGtk.cpp
    WebCoreSupport/EditorClientGtk.cpp
    WebCoreSupport/FrameLoaderClientGtk.cpp
    WebCoreSupport/InspectorClientGtk.cpp
#    WebCoreSupport/PasteboardHelperGtk.cpp

    webkit/webkitnetworkrequest.cpp
    webkit/webkitprivate.cpp
    webkit/webkitwebbackforwardlist.cpp
    webkit/webkitwebframe.cpp
    webkit/webkitwebhistoryitem.cpp
    webkit/webkitwebsettings.cpp
    webkit/webkitwebview.cpp
    #webkit/webkitversion.cpp

    ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.cpp
)

IF (ORIGYNSUITE_CFLAGS OR BINDINGS_JS_CFLAGS)
SET_SOURCE_FILES_PROPERTIES (
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCoreSupport/FrameLoaderClientBal.cpp
        PROPERTIES COMPILE_FLAGS "${ORIGYNSUITE_CFLAGS} ${BINDINGS_JS_CFLAGS}"
)
ENDIF (ORIGYNSUITE_CFLAGS OR BINDINGS_JS_CFLAGS)

IF (BUILD_SHARED_LIBS)
    ADD_LIBRARY (webkit ${WEBKIT_SRC})
ELSE (BUILD_SHARED_LIBS)
    ADD_LIBRARY (webkit STATIC ${WEBKIT_SRC})
ENDIF (BUILD_SHARED_LIBS)
TARGET_LINK_LIBRARIES (webkit 
    ${EXTRA_LDFLAGS}
)

INSTALL (TARGETS webkit
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

