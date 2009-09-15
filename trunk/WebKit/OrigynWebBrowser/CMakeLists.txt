if(USE_GRAPHICS_GTK)
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.h ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.cpp
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND mkdir -p generated_sources/WebKit
        COMMAND echo 'extern \"C\" {' > ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.h
        COMMAND glib-genmarshal --prefix=webkit_marshal ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/gtk/webkit-marshal.list --header >> generated_sources/WebKit/webkit-marshal.h
        COMMAND echo '}' >> ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.h
    
        COMMAND echo 'extern \"C\" {' > ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.cpp
        COMMAND glib-genmarshal --prefix=webkit_marshal ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/gtk/webkit-marshal.list --body >> generated_sources/WebKit/webkit-marshal.cpp
        COMMAND echo '}' >> ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.cpp
        DEPENDS ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/gtk/webkit-marshal.list
    )
    
    list(APPEND WEBKIT_SRC
        Api/gtk/browserWidget.cpp
        ${CMAKE_BINARY_DIR}/generated_sources/WebKit/webkit-marshal.cpp
    )
    include_directories(
        Api/gtk
        ${OWB_BINARY_DIR}/generated_sources/WebKit
        ${OWB_SOURCE_DIR}/WebCore/storage
    )
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_QT)
    list(APPEND WEBKIT_SRC
        Api/Qt/WebViewPrivate.cpp
    )
    include_directories(
        Api/Qt
    )
endif(USE_GRAPHICS_QT)

if(USE_GRAPHICS_SDL)
    aux_source_directory(Api/SDL WEBKIT_SRC)
    include_directories(
        Api/SDL
    )
endif(USE_GRAPHICS_SDL)

aux_source_directory(Api WEBKIT_SRC)
aux_source_directory(WebCoreSupport WEBKIT_SRC)

if(NOT ENABLE_INSPECTOR)
    list(REMOVE_ITEM WEBKIT_SRC
        Api/WebInspector.cpp
        Api/WebScriptCallFrame.cpp 
        Api/WebScriptDebugServer.cpp
        WebCoreSupport/WebInspectorClient.cpp
    )
endif(NOT ENABLE_INSPECTOR)

include_directories(
    ${BASE_INCLUDE_DIRS}
    ${WTF_INCLUDE_DIRS}
    ${BAL_INCLUDE_DIRS}
    ${JAVASCRIPTCORE_INCLUDE_DIRS}
    ${WEBCORE_INCLUDE_DIRS}
    ${WEBKIT_INCLUDE_DIRS}
    ${OWB_BINARY_DIR}/generated_sources/WebCore
    ${OWB_BINARY_DIR}/generated_sources/WebCore/html
    ${EXTERNAL_DEPS_INCLUDE_DIRS}
)

include(Customer/CMakeLists.txt OPTIONAL)

IF(NOT WIN32)
    include(AddWebKitCompilerFlags)
ELSE(NOT WIN32)
    ## We had an issues with link under windows so we put directly the target of it (AddWebCoreCompilerFlags)
    include(AddWebCoreCompilerFlags)
ENDIF(NOT WIN32)

set_source_files_properties(${WEBKIT_SRC}
    PROPERTIES COMPILE_FLAGS "-fno-rtti -fno-exceptions"
)
add_library(webkit-owb SHARED ${WEBKIT_SRC})
add_dependencies(webkit-owb webcore)

target_link_libraries(webkit-owb
    ${WEBKKIT_LD_FLAGS}
    ${EXTERNAL_DEPS_LIBRARIES}
)

target_link_libraries(webkit-owb LINK_INTERFACE_LIBRARIES)

##################################################
# webkit-owb installation.                       #
##################################################

set_target_properties(webkit-owb
    PROPERTIES SOVERSION 1.0
)
configure_file(
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/webkit-owb.pc.in
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/webkit-owb.pc
    @ONLY
)

IF(NOT WIN32)
    ## LIBRARY DESTINATION is useless with vcproj
    install(TARGETS webkit-owb
        LIBRARY DESTINATION lib
    )
ENDIF(NOT WIN32)

install(FILES ${API_HEADER}
    DESTINATION include/webkit-owb
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
)
install(FILES ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/webkit-owb.pc
    DESTINATION lib/pkgconfig
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
)
install(DIRECTORY ${OWB_SOURCE_DIR}/Resources/
    DESTINATION share/webkit-owb/resources
    PATTERN ".svn" EXCLUDE
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
)
