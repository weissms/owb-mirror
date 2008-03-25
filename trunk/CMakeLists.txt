
##############################
##############################
##############################
# PROJECT NAME
##############################
##############################
##############################
PROJECT (WebKit)

##############################
##############################
##############################
# option DEFINITION
##############################
##############################
##############################

# Use this to cross-compile or to use a different compiler :
#SET (CMAKE_CXX_COMPILER "distcc")
#SET (CMAKE_C_COMPILER "distcc")


OPTION (BUILD_SHARED_LIBS "Build OWB with shared libraries." ON)

INCLUDE(CMakeLists_deepseeProject.txt)

# add -static for vers before 2.4.6
IF (NOT BUILD_SHARED_LIBS)
    SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
ENDIF (NOT BUILD_SHARED_LIBS)

# Global switch for specific hooks
OPTION (N800_BUILD "Build OWB with N800 specific hooks." OFF)
IF (N800_BUILD)
    ADD_DEFINITIONS (-D__OWB_N800__)
    SET (TARGET "N800")
ELSE (N800_BUILD)
    SET (TARGET "")
ENDIF (N800_BUILD)

# Global switch for uClibc
OPTION (WEBKIT_USE_UCLIBC "Link OWB with uClibc instead of glibc." OFF)
IF (WEBKIT_USE_UCLIBC)
    ADD_DEFINITIONS (-D__OWB_UCLIBC__)
ENDIF (WEBKIT_USE_UCLIBC)

# Global switch for Console input support
OPTION (OWBAL_USE_CONSOLE_INPUT "Compile OWBAL with console input support." OFF)

# Global switch for OWBAL support for Mac port
OPTION (OWBAL_PLATFORM_MACPORT "OWBAL support for Mac port" OFF)
IF (OWBAL_PLATFORM_MACPORT)
    ADD_DEFINITIONS (-D__OWBAL_PLATFORM_MACPORT__)
ENDIF (OWBAL_PLATFORM_MACPORT)

# Global switch for Web UIF support
OPTION (WEBKIT_USE_WEB_UI "Compile WebKit with Web UI support." OFF)
IF (WEBKIT_USE_WEB_UI)
    SET (USE_WEB_UI ON)
    ADD_DEFINITIONS (-D__OWB_WEB_UI__)
ENDIF (WEBKIT_USE_WEB_UI)

# Selection for image decoder support
SET (WEBKIT_USE_IMAGEDECODER "GENERIC" CACHE STRING "Image decoder to use. (GENERIC/LIBRARY)")
IF (${WEBKIT_USE_IMAGEDECODER} STREQUAL "LIBRARY")
    SET (IMAGEDECODER_GIF "Library/libungif")
    SET (IMAGEDECODER_JPEG "Library/libjpeg")
    SET (IMAGEDECODER_PNG "Library/libpng")
ELSE (${WEBKIT_USE_IMAGEDECODER} STREQUAL "LIBRARY")
    SET (IMAGEDECODER_GIF "Generic/GIF")
    SET (IMAGEDECODER_JPEG "Generic/JPEG")
    SET (IMAGEDECODER_PNG "Generic/PNG")
    SET (IMAGEDECODER_BMP "Generic/BMP")
ENDIF (${WEBKIT_USE_IMAGEDECODER} STREQUAL "LIBRARY")

# Selection for input support
SET (WEBKIT_USE_EVENTS "SDL" CACHE STRING "Event loop to use. (SDL/GENERIC)")
IF (${WEBKIT_USE_EVENTS} STREQUAL "SDL")
    SET (EVENTS "SDL")
    SET (EVENTS_PREFIX "")
ELSE (${WEBKIT_USE_EVENTS} STREQUAL "SDL")
    SET (EVENTS "Generic")
    SET (EVENTS_PREFIX "")
ENDIF (${WEBKIT_USE_EVENTS} STREQUAL "SDL")

# Selection for GC support
SET (WEBKIT_USE_GRAPHICS "SDL" CACHE STRING "Graphics Context support. (SDL)")
IF (${WEBKIT_USE_GRAPHICS} STREQUAL "SDL")
    SET (GRAPHICS "SDL")
    SET (GRAPHICS_PREFIX "")
ENDIF (${WEBKIT_USE_GRAPHICS} STREQUAL "SDL")

# Selection for fonts support
SET (WEBKIT_USE_FONTS "FREETYPE" CACHE STRING "fonts support. (FREETYPE/EMBEDDED)")
IF (${WEBKIT_USE_FONTS} STREQUAL "FREETYPE")
    SET (FONTS "FreeType")
ELSE (${WEBKIT_USE_FONTS} STREQUAL "FREETYPE")
    SET (FONTS "Embedded")
ENDIF (${WEBKIT_USE_FONTS} STREQUAL "FREETYPE")

# Selection for Internationalization support
SET (WEBKIT_USE_I18N "ICU" CACHE STRING "Internationalization support. (ICU/GENERIC)")
IF (${WEBKIT_USE_I18N} STREQUAL "GENERIC")
    SET (I18N "Generic")
    ADD_DEFINITIONS (-D__BAL_I18N__)
ELSE (${WEBKIT_USE_I18N} STREQUAL "GENERIC")
    SET (I18N "ICU")
ENDIF (${WEBKIT_USE_I18N} STREQUAL "GENERIC")

# Global switch for XML support, disables XMLTokenizer, XSLStyleSheet and XSLTProcessor
SET (WEBKIT_USE_XML_SUPPORT "LIBXML" CACHE STRING "XML support. (LIBXML)")
ADD_DEFINITIONS (-D__OWB_XML_SUPPORT__)

# Set icon support to off
#OPTION (WEBKIT_USE_ICON_SUPPORT "Compile WebKit with Icon support." OFF)
SET (WEBKIT_USE_ICON_SUPPORT OFF)
IF (WEBKIT_USE_ICON_SUPPORT)
    ADD_DEFINITIONS (-DENABLE_ICONDATABASE=1)
    ADD_DEFINITIONS (-DENABLE_DATABASE=1)
ELSE (WEBKIT_USE_ICON_SUPPORT)
    ADD_DEFINITIONS (-DENABLE_DATABASE=0)
ENDIF (WEBKIT_USE_ICON_SUPPORT)

# Global switch for SVG support
#OPTION (WEBKIT_USE_SVG_SUPPORT "Compile WebKit with SVG support." OFF)
#IF (WEBKIT_USE_SVG_SUPPORT)
#    ADD_DEFINITIONS (-DENABLE_SVG=1)
#    ADD_DEFINITIONS (-DENABLE_SVG_EXPERIMENTAL_FEATURES=1)
#ENDIF (WEBKIT_USE_SVG_SUPPORT)

# Global switch for XPath support
#OPTION (WEBKIT_USE_XPATH_SUPPORT "Compile WebKit with XPath support." ON)
SET (WEBKIT_USE_XPATH_SUPPORT ON)
IF (WEBKIT_USE_XPATH_SUPPORT)
    ADD_DEFINITIONS (-DENABLE_XPATH=1)
ENDIF (WEBKIT_USE_XPATH_SUPPORT)

# Global switch for XSLT support
SET (WEBKIT_USE_XSLT_SUPPORT "LIBXSLT" CACHE STRING "XSLT support. (LIBXSLT)")
ADD_DEFINITIONS (-DENABLE_XSLT=1)

# Global switch for XBL support
#OPTION (WEBKIT_USE_XBL_SUPPORT "Compile WebKit with XBL support." OFF)
SET (WEBKIT_USE_XBL_SUPPORT OFF)
IF (WEBKIT_USE_XBL_SUPPORT)
    ADD_DEFINITIONS (-DENABLE_XBL=1)
ENDIF (WEBKIT_USE_XBL_SUPPORT)

# Does not compile => re-enable this when compilation fix will be done
# Global switch for MATH support
# OPTION (WEBKIT_USE_BAL_MATH_POSIX "Compile WebKit with Math posix support." OFF)
# IF (WEBKIT_USE_BAL_MATH_POSIX)
#    ADD_DEFINITIONS (-D__BAL_MATH_POSIX__)
# ENDIF (WEBKIT_USE_BAL_MATH_POSIX)

# Global switch for MemoryManager support
#OPTION (WEBKIT_USE_BAL_MEMORY_MANAGER "Compile WebKit with MemoryManager support." OFF)
#IF (WEBKIT_USE_BAL_MEMORY_MANAGER)
#    ADD_DEFINITIONS (-D__BAL_MEMORY_MANAGER__)
#ENDIF (WEBKIT_USE_BAL_MEMORY_MANAGER)

# Global switch for THREAD support
#OPTION (WEBKIT_USE_BAL_THREAD_POSIX "Compile WebKit with Thread posix support." OFF)
#IF (WEBKIT_USE_BAL_THREAD_POSIX)
#    ADD_DEFINITIONS (-D__BAL_THREAD_POSIX__)
#    ADD_DEFINITIONS (-D__BAL_MEMORY_MANAGER__)
#ENDIF (WEBKIT_USE_BAL_THREAD_POSIX)

# Global switch for PIC support
OPTION (WEBKIT_USE_CC_PIC "Compile WebKit with PIC support." ON)

# gcc on darwin uses a -mdynamic-no-pic not compatible
IF (OWBAL_PLATFORM_MACPORT)
    SET (WEBKIT_USE_CC_PIC OFF)
ENDIF (OWBAL_PLATFORM_MACPORT)
IF (WEBKIT_USE_CC_PIC)
    ADD_DEFINITIONS (-fPIC)
    ADD_DEFINITIONS (-DPIC)
ENDIF (WEBKIT_USE_CC_PIC)

# Global switch for EXCEPTIONS support
OPTION (WEBKIT_USE_CC_EXCEPTIONS "Compile WebKit with exceptions support." OFF)
IF (NOT WEBKIT_USE_CC_EXCEPTIONS)
    ADD_DEFINITIONS (-fno-exceptions)
ENDIF (NOT WEBKIT_USE_CC_EXCEPTIONS)

# Switch for no stack protection flag
OPTION (WEBKIT_USE_CC_NO_STACK_PROTECTOR "Compile WebKit with -fno-stack-protector." OFF)
IF (WEBKIT_USE_CC_NO_STACK_PROTECTOR AND WEBKIT_DEBUG)
    ADD_DEFINITIONS (-fno-stack-protector)
ENDIF (WEBKIT_USE_CC_NO_STACK_PROTECTOR AND WEBKIT_DEBUG)

# Global switch for RTTI support
OPTION (WEBKIT_USE_CC_RTTI "Compile WebKit with rtti support." OFF)
IF (NOT WEBKIT_USE_CC_RTTI)
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
ENDIF (NOT WEBKIT_USE_CC_RTTI)

# Global switch fo Verbose mode
OPTION (VERBOSE "Compile Webkit with verbose mode." OFF)
IF (VERBOSE)
    SET (CMAKE_VERBOSE_MAKEFILE ON)
ENDIF (VERBOSE)

# Global switch for DEBUG support
SET (WEBKIT_DEBUG "DEBUG" CACHE STRING "Compile WebKit with debug support. (DEBUG/DEBUG_GCC3.X/RELEASE)")

IF (${WEBKIT_DEBUG} MATCHES "^DEBUG*")
    ADD_DEFINITIONS (-DENABLE_FTPDIR=1)
    ADD_DEFINITIONS (-W)
    ADD_DEFINITIONS (-Wall)
    ADD_DEFINITIONS (-DDEBUG_LAYOUTTEST)
    ADD_DEFINITIONS (-g)
    ADD_DEFINITIONS (-DBAL_LOG)
    ADD_DEFINITIONS (-Wno-unused-parameter)
    ADD_DEFINITIONS (-Wno-unused-variable)
ENDIF (${WEBKIT_DEBUG} MATCHES "^DEBUG*")
IF (${WEBKIT_DEBUG} STREQUAL "DEBUG_GCC3.X")
	ADD_DEFINITIONS (-funit-at-a-time)
ENDIF (${WEBKIT_DEBUG} STREQUAL "DEBUG_GCC3.X")
IF (${WEBKIT_DEBUG} STREQUAL "RELEASE")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DNDEBUG")
ENDIF (${WEBKIT_DEBUG} STREQUAL "RELEASE")

# Global switch for JS support
OPTION (WEBKIT_USE_JS "Compile WebKit with Java Script support." ON)
IF (WEBKIT_USE_JS)
    ADD_DEFINITIONS (-D__OWB_JS__)
    ADD_DEFINITIONS (-DWTF_USE_JAVASCRIPTCORE_BINDINGS)
ENDIF (WEBKIT_USE_JS)

# add NPAPI support
OPTION (WEBKIT_USE_NPAPI "Compile WebKit with NPAPI support." OFF)
IF (WEBKIT_USE_NPAPI)
    ADD_DEFINITIONS (-DWTF_USE_NPOBJECT)
    ADD_DEFINITIONS (-DXP_UNIX)
    ADD_DEFINITIONS (-D__OWB_NPAPI__)
ENDIF (WEBKIT_USE_NPAPI)


# Special option for BuildBot, to disable output colorization
OPTION (COLOR "Do not colorize compilation output." ON)
IF (NOT COLOR)
    SET (CMAKE_COLOR_MAKEFILE OFF)
ENDIF (NOT COLOR)

SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Adding definitions

INCLUDE (CheckLibraryExists)

ADD_DEFINITIONS (-DAPPLE_CHANGES=1)
ADD_DEFINITIONS (-DHAVE_FUNC_ISNAN)
ADD_DEFINITIONS (-D_THREAD_SAFE)
ADD_DEFINITIONS (-D__OWB__)
ADD_DEFINITIONS (-DUSE_CONSERVATIVE_GC=0)
ADD_DEFINITIONS (-DD_MULTISCAN_FILES_SUPPORTED)
ADD_DEFINITIONS (-DLINUX)
IF (NOT OWBAL_PLATFORM_MACPORT)
    ADD_DEFINITIONS (-pthread)
ENDIF (NOT OWBAL_PLATFORM_MACPORT)

IF (OWBAL_PLATFORM_MACPORT)
    ADD_DEFINITIONS (-I/opt/local/include)
    LINK_DIRECTORIES(/opt/local/lib)
ENDIF (OWBAL_PLATFORM_MACPORT)

IF (OWBAL_USE_CONSOLE_INPUT)
    ADD_DEFINITIONS (-DOWB_CONSOLE_INPUT)
ENDIF (OWBAL_USE_CONSOLE_INPUT)

# Configuration checks
check_library_exists(pthread pthread_attr_get_np "" HAVE_PTHREAD_ATTR_GET_NP)
check_library_exists(pthread pthread_getattr_np "" HAVE_PTHREAD_GETATTR_NP)

INCLUDE (BAL/CMakeConf.txt)

##############################
##############################
##############################
# ClosedBAL
##############################
##############################
##############################

EXEC_PROGRAM ("find" ARGS " ${CMAKE_CURRENT_SOURCE_DIR} -maxdepth 1 -name ClosedBAL -type d -print" OUTPUT_VARIABLE CLOSEDBAL_DIR)
IF (EXISTS ${CLOSEDBAL_DIR})
    ADD_SUBDIRECTORY (${CLOSEDBAL_DIR})
    INCLUDE (ClosedBAL/CMakeConf.txt)
    INCLUDE (ClosedBAL/Implementations/Facilities/SourcesCMakeLists.txt)
    INCLUDE (ClosedBAL/Implementations/ImageDecoders/SourcesCMakeLists.txt)
    INCLUDE (ClosedBAL/Implementations/Widgets/SourcesCMakeLists.txt)
    INCLUDE (ClosedBAL/Implementations/Events/SourcesCMakeLists.txt)
    INCLUDE (ClosedBAL/Implementations/Network/SourcesCMakeLists.txt)
    INCLUDE (ClosedBAL/Implementations/Fonts/SourcesCMakeLists.txt)
    INCLUDE (ClosedBAL/Implementations/Graphics/SourcesCMakeLists.txt)
ENDIF (EXISTS ${CLOSEDBAL_DIR})

##############################
##############################
##############################
# BAL
##############################
##############################
##############################
INCLUDE (BAL/Implementations/Facilities/SourcesCMakeLists.txt)
SET (bal_SRC ${bal_SRC} ${facilities_SRC})

INCLUDE (BAL/Implementations/ImageDecoders/SourcesCMakeLists.txt)
SET (bal_SRC ${bal_SRC} ${decoder_jpeg_SRC})
SET (bal_SRC ${bal_SRC} ${decoder_png_SRC})
SET (bal_SRC ${bal_SRC} ${decoder_gif_SRC})
SET (bal_SRC ${bal_SRC} ${decoder_bmp_SRC})

INCLUDE (BAL/Implementations/Widgets/SourcesCMakeLists.txt)
SET (bal_SRC ${bal_SRC} ${widget_SRC})

INCLUDE (BAL/Implementations/Events/SourcesCMakeLists.txt)
SET (bal_SRC ${bal_SRC} ${events_SRC})

INCLUDE (BAL/Implementations/Network/SourcesCMakeLists.txt)
SET (bal_SRC ${bal_SRC} ${transfers_SRC})

INCLUDE (BAL/Implementations/Fonts/SourcesCMakeLists.txt)
SET (bal_SRC ${bal_SRC} ${fonts_SRC})

INCLUDE (BAL/Implementations/Graphics/SourcesCMakeLists.txt)
SET (bal_SRC ${bal_SRC} ${graphics_SRC})

INCLUDE (BAL/Implementations/Types/SourcesCMakeLists.txt)
SET (bal_SRC ${bal_SRC} ${types_SRC})

INCLUDE (BAL/Implementations/Internationalization/SourcesCMakeLists.txt)
SET (bal_SRC ${bal_SRC} ${i18n_SRC})

IF (WEBKIT_USE_BAL_MATH_POSIX)
    INCLUDE (BAL/Implementations/Posix/Linux/SourcesCMakeLists.txt)
    SET (bal_SRC ${bal_SRC} ${posix_linux_SRC})
ENDIF (WEBKIT_USE_BAL_MATH_POSIX)

ADD_LIBRARY (bal ${bal_SRC})

IF(PNG_FOUND)
    TARGET_LINK_LIBRARIES(bal ${PNG_LIBRARY})
ENDIF(PNG_FOUND)

IF(JPEG_FOUND)
    TARGET_LINK_LIBRARIES(bal ${JPEG_LIBRARY})
ENDIF(JPEG_FOUND)

IF (WEBKIT_USE_JS)
    TARGET_LINK_LIBRARIES (bal
    jscore
    ${LIBRARIES_TO_LINK}
    )
ELSE (WEBKIT_USE_JS)
    TARGET_LINK_LIBRARIES (bal
        wtf
        ${LIBRARIES_TO_LINK}
    )
ENDIF (WEBKIT_USE_JS)
##############################
##############################
##############################
# INCLUDE DIRECTORY
##############################
##############################
##############################
# FIXME all _CFLAGS should be replaced by one LIBRARIES_TO_LINK
INCLUDE_DIRECTORIES (
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Configuration/
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Implementations/Widgets/
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Implementations/Widgets/Generic/
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Interfaces/
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Interfaces/network
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Interfaces/font
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Interfaces/graphics
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Implementations/ImageDecoders/Common/
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Implementations/Events/Common/
    ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/pcre
    ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/bindings
    ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/bindings/c
    ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/bindings/bal
    ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/kjs
    ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/
    ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/ForwardingHeaders
    ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/wtf
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCore
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/page
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/editing
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/dom
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/rendering
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/loader
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/history
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/html
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/bridge
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/loader/icon
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/bindings/js
    ${CMAKE_CURRENT_SOURCE_DIR}/WebKit/OrigynWebBrowser/WebCoreSupport
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/xml
    ${CMAKE_CURRENT_SOURCE_DIR}/BALGlue
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/
    ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/kjs
    ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore
    ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/html
    ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/xml
    ${LIBRARIES_TO_INCLUDE}
)

IF (WEBKIT_USE_SVG_SUPPORT)
    INCLUDE_DIRECTORIES (
        ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/ksvg2/css/
        ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/WebCore/ksvg2/svg/
        ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/ksvg2/misc/
        ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/kcanvas/
        ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/kcanvas/device
        ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/ksvg2/
        ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/ksvg2/events
        ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/WebCore/ksvg2/svg/
        ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/ksvg2/misc/
    )
ENDIF (WEBKIT_USE_SVG_SUPPORT)

IF (WEBKIT_USE_NPAPI)
    INCLUDE_DIRECTORIES (
        ${CMAKE_CURRENT_SOURCE_DIR}/BALGlue/Plugins/Gtk/
	${CMAKE_CURRENT_SOURCE_DIR}/WebCore/plugins/
    )
ENDIF (WEBKIT_USE_NPAPI)

IF (${WEBKIT_USE_XML_SUPPORT} STREQUAL "LIBXML")
    INCLUDE_DIRECTORIES (
        ${CMAKE_CURRENT_SOURCE_DIR}/BALGlue/XML/LIBXML/
    )
ENDIF (${WEBKIT_USE_XML_SUPPORT} STREQUAL "LIBXML")

IF (${WEBKIT_USE_XSLT_SUPPORT} STREQUAL "LIBXSLT")
    INCLUDE_DIRECTORIES (
        ${CMAKE_CURRENT_SOURCE_DIR}/BALGlue/XML/LIBXSLT/
    )
ENDIF (${WEBKIT_USE_XSLT_SUPPORT} STREQUAL "LIBXSLT")

##############################
##############################
##############################
#BAL_GLUE
##############################
##############################
##############################

INCLUDE (BALGlue/SourcesCMakeLists.txt)

ADD_LIBRARY (balglue STATIC ${bal_glue_SRC})
TARGET_LINK_LIBRARIES (balglue webcore-owb)

##############################
##############################
##############################
#WebKit
##############################
##############################
##############################

INCLUDE (WebKit/OrigynWebBrowser/SourcesCMakeLists.txt)

ADD_LIBRARY (webkit STATIC ${webkit_SRC})

##############################
##############################
##############################
# JSCore
##############################
##############################
##############################

INCLUDE (CMakeJSCore.txt)

IF (WEBKIT_USE_JS)
    ADD_LIBRARY (jscore ${jscore_SRC})
    IF (BUILD_SHARED_LIBS)
        TARGET_LINK_LIBRARIES (jscore pthread)
    ELSE (BUILD_SHARED_LIBS)
        TARGET_LINK_LIBRARIES (jscore -Wl,-Bstatic -lpthread)
    ENDIF (BUILD_SHARED_LIBS)
    
    IF (OWBAL_PLATFORM_MACPORT)
        SET_TARGET_PROPERTIES(jscore
                        PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    ENDIF (OWBAL_PLATFORM_MACPORT)

    IF (NOT N800_BUILD)
        ADD_EXECUTABLE (testkjs JavaScriptCore/kjs/testkjs.cpp)
        TARGET_LINK_LIBRARIES (testkjs
            bal
            jscore
            ${DEEPSEE_LINK}
        )

        ADD_EXECUTABLE (testbalbindings
            JavaScriptCore/bindings/testbalbindings.cpp)
        TARGET_LINK_LIBRARIES (testbalbindings
            jscore
            bal
            ${DEEPSEE_LINK}
        )
    ENDIF (NOT N800_BUILD)


    IF (OWBAL_PLATFORM_MACPORT)
        TARGET_LINK_LIBRARIES (jscore
            ${LIBICU_LIBS}
        )
        TARGET_LINK_LIBRARIES (testkjs
            ${LIBICU_LIBS}
            ${DEEPSEE_LINK}
        )
    ENDIF (OWBAL_PLATFORM_MACPORT)

    IF (NOT N800_BUILD)
        ADD_CUSTOM_TARGET (kjstests
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/export_test_kjs
            COMMAND cd KjsTests/mozilla/ && ${PERL_EXECUTABLE} mklistpage.pl > menubody.html
            COMMAND cd KjsTests/mozilla/ && cat menuhead.html menubody.html menufoot.html > menu.html
            COMMAND cd KjsTests/mozilla/ && SYMROOTS=${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore ./jsDriver.pl -e kjs -s ${CMAKE_CURRENT_BINARY_DIR}/testkjs -f results.html
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        ADD_DEPENDENCIES (kjstests
            testkjs
        )
    ENDIF (NOT N800_BUILD)

ELSE (WEBKIT_USE_JS)
    ADD_LIBRARY (wtf ${jscore_SRC})
    IF (NOT BUILD_SHARED_LIBS)
        TARGET_LINK_LIBRARIES (wtf -Wl,-Bstatic)
    ENDIF (NOT BUILD_SHARED_LIBS)
ENDIF (WEBKIT_USE_JS)

##############################
##############################
##############################
# WebCore
##############################
##############################
##############################

INCLUDE (CMakeWebCore.txt)

ADD_LIBRARY (webcore-owb ${webcore_SRC})

EXEC_PROGRAM ("find" ARGS " ${CMAKE_CURRENT_SOURCE_DIR} -maxdepth 1 -name AddOns -type d -print" OUTPUT_VARIABLE ADDONS_DIR)
IF (EXISTS ${ADDONS_DIR})
    SET(LIBRARIES_TO_INCLUDE "${LIBRARIES_TO_INCLUDE};${ADDONS_DIR}")
    INCLUDE (${ADDONS_DIR}/CMakeConf.txt)
    ADD_SUBDIRECTORY (${ADDONS_DIR})
    IF (NOT BUILD_SHARED_LIBS)
        SET (BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared OWB." FORCE)
    ENDIF (NOT BUILD_SHARED_LIBS)
ENDIF (EXISTS ${ADDONS_DIR})

IF (OWBAL_PLATFORM_MACPORT)
    TARGET_LINK_LIBRARIES (webcore-owb
        jscore
        bal
        balglue
        ${ADDONS_LINK}
    )
ENDIF (OWBAL_PLATFORM_MACPORT)


##############################
##############################
##############################
#ORIGYN_LAUNCHER
##############################
##############################
##############################

ADD_EXECUTABLE (owb WebKit/OrigynWebBrowser/OrigynLauncher/main${TARGET}.cpp)
ADD_DEPENDENCIES (owb bal balglue webkit jscore webcore-owb)
TARGET_LINK_LIBRARIES (owb
    webkit
    webcore-owb
    balglue
    bal
    ${ADDONS_LINK}
    ${LIBRARIES_TO_LINK}
    ${DEEPSEE_LINK}
)

IF (WEBKIT_USE_JS)
    TARGET_LINK_LIBRARIES (owb
        jscore
    )
ELSE (WEBKIT_USE_JS)
    TARGET_LINK_LIBRARIES (owb
        wtf
    )
ENDIF (WEBKIT_USE_JS)

##############################
##############################
##############################
#BAL_TEST
##############################
##############################
##############################

IF (NOT N800_BUILD)
INCLUDE_DIRECTORIES (
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Tests/
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Implementations/Widgets/Common/
)

INCLUDE (BAL/Tests/SourcesCMakeLists.txt)
ADD_EXECUTABLE (testBAL ${bal_tests_SRC})
TARGET_LINK_LIBRARIES (testBAL
    bal
    ${DEEPSEE_LINK} 
    jscore
)

IF (WEBKIT_USE_JS)
    TARGET_LINK_LIBRARIES (testBAL
        jscore
    )
ELSE (WEBKIT_USE_JS)
    TARGET_LINK_LIBRARIES (testBAL
        wtf
    )
ENDIF (WEBKIT_USE_JS)

ADD_CUSTOM_TARGET (test
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/testBAL -a -p ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Tests/
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
ADD_DEPENDENCIES (test
    testBAL
)
ENDIF (NOT N800_BUILD)
##############################
##############################
##############################
#DUMP_RENDER_TREE
##############################
##############################
##############################

ADD_EXECUTABLE (DumpRenderTree 
	WebKitTools/DumpRenderTree/bal/DumpRenderTree.cpp
	WebKitTools/DumpRenderTree/bal/LayoutTestControllerBal.cpp
	WebKitTools/DumpRenderTree/WorkQueue.cpp
	WebKitTools/DumpRenderTree/LayoutTestController.cpp
        WebKitTools/DumpRenderTree/bal/WorkQueueItemBal.cpp
)
INCLUDE_DIRECTORIES (
    ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/ForwardingHeaders/
    ${CMAKE_CURRENT_SOURCE_DIR}/WebKitTools/DumpRenderTree/
)

TARGET_LINK_LIBRARIES (DumpRenderTree
    webcore-owb
    balglue
    webkit
    bal
    ${ADDONS_LINK}
    ${DEEPSEE_LINK}
)

IF (WEBKIT_USE_JS)
    TARGET_LINK_LIBRARIES (DumpRenderTree
        jscore
    )
ELSE (WEBKIT_USE_JS)
    TARGET_LINK_LIBRARIES (DumpRenderTree
        wtf
    )
ENDIF (WEBKIT_USE_JS)

##############################
# Embedder sample
##############################

# Use it to add a binary that embeds OWB

IF (FALSE)

INCLUDE_DIRECTORIES(/usr/include/embedder)
ADD_EXECUTABLE (embedder Embedder/embedder.cpp)
ADD_DEPENDENCIES (embedder bal balglue webkit jscore webcore-owb)
TARGET_LINK_LIBRARIES (embedder
    webcore-owb
    webkit
    jscore
    balglue
    bal
)

ENDIF (FALSE)

########################
#    Install Files     #
########################

IF (CMAKE_INSTALL_PREFIX)
    SET (INSTALLDIR ${CMAKE_INSTALL_PREFIX})
    SET (BIN_INSTALLDIR "${INSTALLDIR}/bin")
    SET (LIB_INSTALLDIR "${INSTALLDIR}/lib")
ELSE (CMAKE_INSTALL_PREFIX)
    SET (INSTALLDIR "/usr/local")
    SET (BIN_INSTALLDIR "${INSTALLDIR}/bin")
    SET (LIB_INSTALLDIR "${INSTALLDIR}/lib")
ENDIF (CMAKE_INSTALL_PREFIX)

INSTALL (TARGETS owb DESTINATION ${BIN_INSTALLDIR})

IF (BUILD_SHARED_LIBS)
    INSTALL (TARGETS webcore-owb DESTINATION ${LIB_INSTALLDIR})
    IF (WEBKIT_USE_JS)
        INSTALL (TARGETS jscore DESTINATION ${LIB_INSTALLDIR})
    ENDIF (WEBKIT_USE_JS)
    INSTALL (TARGETS bal DESTINATION ${LIB_INSTALLDIR})
ENDIF (BUILD_SHARED_LIBS)

IF (WEBKIT_USE_WEB_UI)
	INSTALL (DIRECTORY UI DESTINATION "${INSTALLDIR}/share/owb/"
		PATTERN ".svn" EXCLUDE)
ENDIF (WEBKIT_USE_WEB_UI)
