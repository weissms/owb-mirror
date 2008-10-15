IF (WEBKIT_USE_UCLIBC)
    ADD_DEFINITIONS (-DHAVE_UCLIBC)
ENDIF (WEBKIT_USE_UCLIBC)

INCLUDE (pcre/CMakeLists.txt)
INCLUDE (API/CMakeLists.txt)
INCLUDE (kjs/CMakeLists.txt)
INCLUDE (profiler/CMakeLists.txt)
INCLUDE (VM/CMakeLists.txt)
    
INCLUDE_DIRECTORIES (
    ${CMAKE_SOURCE_DIR}/BAL/Base
    ${CMAKE_SOURCE_DIR}/BAL/Base/wtf
    ${CMAKE_BINARY_DIR}/generated_link/OWBAL
    bindings
    bindings/bal
    bindings/c
    ForwardingHeaders
    kjs
    pcre
    VM
    profiler
    #wtf
    ${CMAKE_BINARY_DIR}/generated_sources/kjs
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/API
    ${CMAKE_SOURCE_DIR}
    ${ICU_INCLUDE}
    #${GTK2_INCLUDE}
    ${GTHREAD_INCLUDE}
    )

IF (WEBKIT_USE_I18N STREQUAL "GENERIC")
    INCLUDE_DIRECTORIES (
        icu
    )
ENDIF (WEBKIT_USE_I18N STREQUAL "GENERIC")

IF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")
    INCLUDE_DIRECTORIES (
        ${GTK2_INCLUDE_DIRS}
    )
ENDIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")


IF (BUILD_SHARED_LIBS)
    ADD_LIBRARY (jscore ${JSCORE_SRC})
ELSE (BUILD_SHARED_LIBS)
    ADD_LIBRARY (jscore STATIC ${JSCORE_SRC})
ENDIF (BUILD_SHARED_LIBS)
TARGET_LINK_LIBRARIES (jscore pthread)
    
IF (OWBAL_PLATFORM_MACPORT)
    SET_TARGET_PROPERTIES(jscore
        PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
ENDIF (OWBAL_PLATFORM_MACPORT)
    
INSTALL (TARGETS jscore
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
ADD_DEPENDENCIES (jscore generated-sources)

IF (COMPILE_TESTS)

# do the link_directories *before* add_executable
   IF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")
      LINK_DIRECTORIES(${GTK2_LIBRARY_DIRS})
   ELSEIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "SDL")
      LINK_DIRECTORIES(${SDL_LIBRARY_DIRS})
   ENDIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")

   ADD_EXECUTABLE (jsc kjs/Shell.cpp)
   
   IF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")
      TARGET_LINK_LIBRARIES (jsc ${GTK2_LIBRARIES})
   ELSEIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "SDL")
      TARGET_LINK_LIBRARIES (jsc ${SDL_LIBRARIES})
   ENDIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")

   TARGET_LINK_LIBRARIES (jsc
        generated-sources
        webcore-owb
	webkit
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
        jscore
        ${DEEPSEE_LINK}
        ${EXTRA_LDFLAGS}
   )

IF (WEBKIT_USE_DATABASE)
    TARGET_LINK_LIBRARIES (jsc
        bal-database
    )
ENDIF (WEBKIT_USE_DATABASE)

TARGET_LINK_LIBRARIES (jsc
        -lxslt -lz -lm -lxml2
        ${PNG12_LIBRARIES}
        -ljpeg
        ${GTHREAD_LIBRARIES}
	${XT_LIBRARIES}
	-lreadline
    )

IF (WEBKIT_USE_HTML_EXTENSION)
    TARGET_LINK_LIBRARIES (jsc
        htmlext
    )
ENDIF (WEBKIT_USE_HTML_EXTENSION)

    IF (OWBAL_PLATFORM_MACPORT)
        TARGET_LINK_LIBRARIES (jscore
            ${LIBICU_LIBS}
        )
        TARGET_LINK_LIBRARIES (jsc
            ${LIBICU_LIBS}
            ${DEEPSEE_LINK}
        )
    ENDIF (OWBAL_PLATFORM_MACPORT)

    ADD_CUSTOM_TARGET (kjstests
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/export_test_kjs
        COMMAND cd KjsTests/mozilla/ && ${PERL_EXECUTABLE} mklistpage.pl > menubody.html
        COMMAND cd KjsTests/mozilla/ && cat menuhead.html menubody.html menufoot.html > menu.html
        COMMAND cd KjsTests/mozilla/ && SYMROOTS=${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore ./jsDriver.pl -e kjs -s ${CMAKE_CURRENT_BINARY_DIR}/jsc -f results.html
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    ADD_DEPENDENCIES (kjstests
        jsc
    )
ENDIF (COMPILE_TESTS)
