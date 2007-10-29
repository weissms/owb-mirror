
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

OPTION (BUILD_SHARED_LIBS "Build shared OWB." ON)

# add -static for vers before 2.4.6
IF (NOT BUILD_SHARED_LIBS)
	IF (CMAKE_MAJOR_VERSION EQUAL 2 AND CMAKE_MINOR_VERSION EQUAL 4 AND CMAKE_PATCH_VERSION LESS 6)
       SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
	ENDIF (CMAKE_MAJOR_VERSION EQUAL 2 AND CMAKE_MINOR_VERSION EQUAL 4 AND CMAKE_PATCH_VERSION LESS 6)
ENDIF (NOT BUILD_SHARED_LIBS)

# Global switch for specific hooks
OPTION (N800_BUILD "Build OWB with N800 specific hooks." OFF)
IF (N800_BUILD)
	ADD_DEFINITIONS (-D__OWB_N800__)
ENDIF (N800_BUILD)

# Global switch for Console input support
OPTION (OWBAL_USE_CONSOLE_INPUT "Compile OWBAL with console input support." OFF)

# Global switch for OWBAL support for Mac port
OPTION (OWBAL_PLATFORM_MACPORT "OWBAL support for Mac port" OFF)
IF (OWBAL_PLATFORM_MACPORT)
    ADD_DEFINITIONS (-D__OWBAL_PLATFORM_MACPORT__)
ENDIF (OWBAL_PLATFORM_MACPORT)

# Global switch for Web UIF support
OPTION (WEBKIT_USE_WEB_UI "Compile WebKit with Web UI support." ON)
IF (WEBKIT_USE_WEB_UI)
    ADD_DEFINITIONS (-D__OWB_WEB_UI__)
ENDIF (WEBKIT_USE_WEB_UI)

# Global switch for GIF support
OPTION (WEBKIT_USE_IMAGEDECODER_GIF_GENERIC "Compile WebKit with generic GIF support." ON)

# Global switch for JPEG support
OPTION (WEBKIT_USE_IMAGEDECODER_JPEG_GENERIC "Compile WebKit with generic JPEG support." ON)

# Global switch for PNG support
OPTION (WEBKIT_USE_IMAGEDECODER_PNG_GENERIC "Compile WebKit with generic PNG support." ON)

# Global switch for SDL input support
OPTION (WEBKIT_USE_EVENTS_SDL "Compile OWBAL with SDL input support." ON)

# Global switch for Generic input support
OPTION (WEBKIT_USE_EVENTS_GENERIC "Compile OWBAL with Generic input support." OFF)

# Global switch for Cairo GC support
#OPTION (WEBKIT_USE_GRAPHICS_CAIRO "Compile WebKit with Cairo Graphics Context support: currently broken." OFF)

# Global switch for SDL GC support
OPTION (WEBKIT_USE_GRAPHICS_SDL "Compile WebKit with SDL Graphics Context support." ON)

# Global switch for Cairo fonts support
#OPTION (WEBKIT_USE_FONTS_CAIRO "Compile WebKit with Cairo fonts support: currently broken." OFF)

# Global switch for SDL fonts support
OPTION (WEBKIT_USE_FONTS_DIRECTFB "Compile WebKit with directfb fonts support." OFF)

# Global switch for SDL fonts support
OPTION (WEBKIT_USE_FONTS_FREETYPE "Compile WebKit with freetype fonts support." ON)

# Global switch for SDL fonts support
OPTION (WEBKIT_USE_FONTS_SDL "Compile WebKit with SDL fonts support." OFF)

# Global switch for SDL_TTF fonts support
OPTION (WEBKIT_USE_FONTS_SDL_TTF "Compile WebKit with SDL_TTF fonts support." OFF)

# Global switch for XML support, disables XMLTokenizer, XSLStyleSheet and XSLTProcessor
OPTION (WEBKIT_USE_XML_SUPPORT "Compile WebKit with XML support." ON)

IF (WEBKIT_USE_XML_SUPPORT)
    ADD_DEFINITIONS (-D__OWB_XML_SUPPORT__)
ELSE (WEBKIT_USE_XML_SUPPORT)
    SET (WEBKIT_USE_XSLT_SUPPORT OFF)
    SET (WEBKIT_USE_XPATH_SUPPORT OFF)
ENDIF (WEBKIT_USE_XML_SUPPORT)

# Global switch for icon support
OPTION (WEBKIT_USE_ICON_SUPPORT "Compile WebKit with Icon support." OFF)
IF (WEBKIT_USE_ICON_SUPPORT)
	ADD_DEFINITIONS (-DOWB_ICON_SUPPORT=1)
ENDIF (WEBKIT_USE_ICON_SUPPORT)

# Global switch for SVG support
OPTION (WEBKIT_USE_SVG_SUPPORT "Compile WebKit with SVG support." OFF)

IF (WEBKIT_USE_SVG_SUPPORT)
    ADD_DEFINITIONS (-DSVG_SUPPORT=1)
ENDIF (WEBKIT_USE_SVG_SUPPORT)

# Global switch for XPath support
OPTION (WEBKIT_USE_XPATH_SUPPORT "Compile WebKit with XPath support." ON)

IF (WEBKIT_USE_XPATH_SUPPORT)
    ADD_DEFINITIONS (-DXPATH_SUPPORT)
ENDIF (WEBKIT_USE_XPATH_SUPPORT)

# Global switch for XSLT support
OPTION (WEBKIT_USE_XSLT_SUPPORT "Compile WebKit with XSLT support." ON)

IF (WEBKIT_USE_XSLT_SUPPORT)
    ADD_DEFINITIONS (-DKHTML_XSLT=1)
    ADD_DEFINITIONS (-DXSLT_SUPPORT=1)
ENDIF (WEBKIT_USE_XSLT_SUPPORT)

# Global switch for XBL support
OPTION (WEBKIT_USE_XBL_SUPPORT "Compile WebKit with XBL support." OFF)

IF (WEBKIT_USE_XBL_SUPPORT)
    ADD_DEFINITIONS (-DXBL_SUPPORT=1)
ELSE (WEBKIT_USE_XBL_SUPPORT)
    ADD_DEFINITIONS (-DKHTML_NO_XBL=1)
ENDIF (WEBKIT_USE_XBL_SUPPORT)

# Global switch for MATH support
OPTION (WEBKIT_USE_BAL_MATH_POSIX "Compile WebKit with Math posix support." OFF)

IF (WEBKIT_USE_BAL_MATH_POSIX)
    ADD_DEFINITIONS (-D__BAL_MATH_POSIX__)
ENDIF (WEBKIT_USE_BAL_MATH_POSIX)

# Global switch for MemoryManager support
OPTION (WEBKIT_USE_BAL_MEMORY_MANAGER "Compile WebKit with MemoryManager support." OFF)

IF (WEBKIT_USE_BAL_MEMORY_MANAGER)
    ADD_DEFINITIONS (-D__BAL_MEMORY_MANAGER__)
ENDIF (WEBKIT_USE_BAL_MEMORY_MANAGER)

# Global switch for THREAD support
OPTION (WEBKIT_USE_BAL_THREAD_POSIX "Compile WebKit with Thread posix support." OFF)

IF (WEBKIT_USE_BAL_THREAD_POSIX)
    ADD_DEFINITIONS (-D__BAL_THREAD_POSIX__)
    ADD_DEFINITIONS (-D__BAL_MEMORY_MANAGER__)
ENDIF (WEBKIT_USE_BAL_THREAD_POSIX)

# Global switch for Internationalization support
OPTION (WEBKIT_USE_BAL_I18N "Compile WebKit with Internationalization support." OFF)

IF (WEBKIT_USE_BAL_I18N)
    ADD_DEFINITIONS (-D__BAL_I18N__)
ENDIF (WEBKIT_USE_BAL_I18N)

# Global switch for PIC support
OPTION (WEBKIT_USE_CC_PIC "Compile WebKit with PIC support." ON)

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

IF (NOT WEBKIT_CC_USE_RTTI)
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
    #ADD_DEFINITIONS (-fno-rtti)
ENDIF (NOT WEBKIT_CC_USE_RTTI)

# Global switch fo Verbose mode
OPTION (VERBOSE "Compile Webkit with verbose mode." OFF)

IF (VERBOSE)
    SET (CMAKE_VERBOSE_MAKEFILE ON)
ENDIF (VERBOSE)

# Global switch for DEBUG support
OPTION (WEBKIT_DEBUG "Compile WebKit with debug support." ON)

IF (WEBKIT_DEBUG)
    ADD_DEFINITIONS (-W)
    ADD_DEFINITIONS (-Wall)
    ADD_DEFINITIONS (-DDEBUG_LAYOUTTEST)
    ADD_DEFINITIONS (-g)
    ADD_DEFINITIONS (-DBAL_LOG)
    ADD_DEFINITIONS (-Wno-unused-parameter)
    ADD_DEFINITIONS (-Wno-unused-variable)
ELSE (WEBKIT_DEBUG)
    ADD_DEFINITIONS (-DNDEBUG)
ENDIF (WEBKIT_DEBUG)

# Global switch for JS support
OPTION (WEBKIT_USE_JS "Compile WebKit with Java Script support." ON)

IF (WEBKIT_USE_JS)
	ADD_DEFINITIONS (-D__OWB_JS__)
ENDIF (WEBKIT_USE_JS)

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
ADD_DEFINITIONS (-O2)

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

EXEC_PROGRAM ("find" ARGS " ${CMAKE_CURRENT_SOURCE_DIR} -maxdepth 2 -name ClosedBAL -type d -print | grep -v `basename $(pwd)`" OUTPUT_VARIABLE CLOSEDBAL_DIR)
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

IF (WEBKIT_USE_XML_SUPPORT)
    INCLUDE (BAL/Implementations/XML/SourcesCMakeLists.txt)
    SET (bal_SRC ${bal_SRC} ${bal_xml_SRC})
ENDIF (WEBKIT_USE_XML_SUPPORT)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Configuration/
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Implementations/Widgets/
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Implementations/Widgets/Generic/
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Interfaces/
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Interfaces/network
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Interfaces/graphics
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Implementations/ImageDecoders/Common/
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Implementations/Events/Common/
    ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/pcre
    ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/bindings
    ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/bindings/c
    ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/bindings/bal
    ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/kjs
    ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/
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
    TARGET_LINK_LIBRARIES (jscore
    	pthread	
    )

	ADD_EXECUTABLE (testkjs JavaScriptCore/kjs/testkjs.cpp)
	TARGET_LINK_LIBRARIES (testkjs
		jscore
		bal
	)
	
	ADD_EXECUTABLE (testbindings JavaScriptCore/bindings/testbindings.cpp)
        TARGET_LINK_LIBRARIES (testbindings
                jscore
                bal
         )

	 ADD_EXECUTABLE (testbalbindings
	 JavaScriptCore/bindings/testbalbindings.cpp)
         TARGET_LINK_LIBRARIES (testbalbindings
                jscore
                bal
         )


IF (OWBAL_PLATFORM_MACPORT)
    TARGET_LINK_LIBRARIES (jscore
    	${LIBICU_LIBS}
    )
	TARGET_LINK_LIBRARIES (testkjs
    	${LIBICU_LIBS}
    )
ENDIF (OWBAL_PLATFORM_MACPORT)

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

ELSE (WEBKIT_USE_JS)
	ADD_LIBRARY (wtf ${jscore_SRC})
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

EXEC_PROGRAM ("find" ARGS " ${CMAKE_CURRENT_SOURCE_DIR} -maxdepth 1 -name AddOns -type d -print | grep -v `basename $(pwd)`" OUTPUT_VARIABLE ADDONS_DIR)
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

IF (BUILD_N800)
	ADD_EXECUTABLE (owb WebKit/OrigynWebBrowser/OrigynLauncher/main-N800.cpp)
ELSE (BUILD_N800)
	ADD_EXECUTABLE (owb WebKit/OrigynWebBrowser/OrigynLauncher/main.cpp)
ENDIF (BUILD_N800)
ADD_DEPENDENCIES (owb bal balglue webkit jscore webcore-owb)
TARGET_LINK_LIBRARIES (owb
	webkit
	webcore-owb
	balglue
	bal
	${ADDONS_LINK}
	${LIBFREETYPE_LIBS}
	${LIBRARIES_TO_LINK}
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

INCLUDE_DIRECTORIES (
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Tests/
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Implementations/Widgets/Common/
)

INCLUDE (BAL/Tests/SourcesCMakeLists.txt)
ADD_EXECUTABLE (testBAL ${bal_tests_SRC})
TARGET_LINK_LIBRARIES (testBAL
    bal
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
##############################
##############################
##############################
#DUMP_RENDER_TREE
##############################
##############################
##############################

ADD_EXECUTABLE (DumpRenderTree WebKitTools/OrigynDumpRenderTree/DumpRenderTree.cpp)
TARGET_LINK_LIBRARIES (DumpRenderTree
	webcore-owb
	webkit
	balglue
	bal
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

IF (DESTDIR)
	SET (INSTALLDIR ${DESTDIR})
	SET (BIN_INSTALLDIR "${INSTALLDIR}/bin")
	SET (LIB_INSTALLDIR "${INSTALLDIR}/lib")
ELSE (DESTDIR)
	SET (INSTALLDIR "/usr")
	SET (BIN_INSTALLDIR "${INSTALLDIR}/bin")
	SET (LIB_INSTALLDIR "${INSTALLDIR}/lib")
ENDIF (DESTDIR)

INSTALL (TARGETS owb DESTINATION ${BIN_INSTALLDIR})

IF (BUILD_SHARED_LIBS)
   	INSTALL (TARGETS webcore-owb DESTINATION ${LIB_INSTALLDIR})
	IF (WEBKIT_USE_JS)
		INSTALL (TARGETS jscore DESTINATION ${LIB_INSTALLDIR})
	ENDIF (WEBKIT_USE_JS)
    INSTALL (TARGETS bal DESTINATION ${LIB_INSTALLDIR})
ENDIF (BUILD_SHARED_LIBS)
