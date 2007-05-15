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
#SET (CMAKE_CXX_COMPILER "g++-3.3")
#SET (CMAKE_C_COMPILER "gcc-3.3")
#or
#SET (CMAKE_CXX_COMPILER "distcc")
#SET (CMAKE_C_COMPILER "distcc")

OPTION (BUILD_SHARED_LIBS "Build shared OWB." ON)

IF (NOT BUILD_SHARED_LIBS)
	SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
ENDIF (NOT BUILD_SHARED_LIBS)

# Global switch for Cairo GC support
OPTION (WEBKIT_USE_GRAPHICS_CAIRO "Compile WebKit with Cairo Graphics Context support: currently broken." OFF)

# Global switch for SDL GC support
OPTION (WEBKIT_USE_GRAPHICS_SDL "Compile WebKit with SDL Graphics Context support." ON)

# Global switch for Cairo fonts support
OPTION (WEBKIT_USE_FONTS_CAIRO "Compile WebKit with Cairo fonts support: currently broken." OFF)

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

##############################
##############################
##############################
# BINARY AND LIB REQUIERMENT
##############################
##############################
##############################

FIND_PACKAGE (JPEG REQUIRED)
FIND_PACKAGE (PNG REQUIRED)

#######################################
# find perl executable
#######################################
INCLUDE (FindPerl)

IF (PERL_FOUND)
	MESSAGE ("Perl executable found : ${PERL_EXECUTABLE}")
ELSE (PERL_FOUND)
	MESSAGE (FATAL_ERROR "perl executable not found, cannot create owb without it, on debian you must install perl-base package")
ENDIF (PERL_FOUND)

#######################################
# find pkg-config executable
#######################################
FIND_PROGRAM (PKG_CONFIG_EXECUTABLE pkg-config)

IF (NOT PKG_CONFIG_EXECUTABLE)
    MESSAGE (FATAL_ERROR "Could not find pkg-config, cannot create owb without it, on debian you must install pkg-config package")
ENDIF (NOT PKG_CONFIG_EXECUTABLE)

#######################################
# find BISON executable
#######################################
FIND_PROGRAM (BISON_EXECUTABLE bison)

IF (NOT BISON_EXECUTABLE)
   MESSAGE (FATAL_ERROR "Could not find bison, cannot create owb without it, on debian you must install bison package")
ENDIF (NOT BISON_EXECUTABLE)

#######################################
# find GPERF executable
#######################################
FIND_PROGRAM (GPERF_EXECUTABLE gperf)

IF (NOT GPERF_EXECUTABLE)
   MESSAGE (FATAL_ERROR "Could not find gperf, cannot create owb without it, on debian you must install gperf package")
ENDIF (NOT GPERF_EXECUTABLE)

#######################################
#quick search the libcairo is available
#######################################

IF (WEBKIT_USE_GRAPHICS_CAIRO OR WEBKIT_USE_FONTS_CAIRO)

  EXEC_PROGRAM ("${PKG_CONFIG_EXECUTABLE} --libs cairo | sed s/-[[:upper:]][[:graph:]]*[[:blank:]]//g | sed s/-l//"
	OUTPUT_VARIABLE LIBCAIRO_LIBS
	RETURN_VALUE ERROR_CODE)

  IF (ERROR_CODE EQUAL 0)
	MESSAGE ("cairo found with this parameter for libs : "${LIBCAIRO_LIBS})
  ELSE (ERROR_CODE EQUAL 0)
	MESSAGE (FATAL_ERROR "libcairo not found, cannot create owb without it, on debian you must install libcairo2-dev package")
  ENDIF (ERROR_CODE EQUAL 0)

  EXEC_PROGRAM ("${PKG_CONFIG_EXECUTABLE} --cflags cairo | sed s/-I// | sed s/\\ .*//g"
        OUTPUT_VARIABLE LIBCAIRO_CFLAGS
        RETURN_VALUE ERROR_CODE)

  IF (ERROR_CODE EQUAL 0)
        MESSAGE ("cairo found with this parameter for cflags : "${LIBCAIRO_CFLAGS})
  ELSE (ERROR_CODE EQUAL 0)
        MESSAGE (FATAL_ERROR "libcairo not found, cannot create owb without it, on debian you must install libcairo2-dev package")
  ENDIF (ERROR_CODE EQUAL 0)

ENDIF (WEBKIT_USE_GRAPHICS_CAIRO OR WEBKIT_USE_FONTS_CAIRO)

#######################################
#quick search the libicu is available
#######################################

FIND_PROGRAM (ICU_CONFIG_EXECUTABLE icu-config)

IF (NOT ICU_CONFIG_EXECUTABLE)
	MESSAGE (FATAL_ERROR "Could not find icu-config, cannot create owb without it, on debian you must install libicu34-dev package")
ENDIF (NOT ICU_CONFIG_EXECUTABLE)

EXEC_PROGRAM ("${ICU_CONFIG_EXECUTABLE} --ldflags | sed s/-[[:upper:]][[:graph:]]*[[:blank:]]//g | sed s/-l//"
	OUTPUT_VARIABLE LIBICU_LIBS
	RETURN_VALUE ERROR_CODE)

IF (ERROR_CODE EQUAL 0)
	MESSAGE ("icu found with this parameter for libs : "${LIBICU_LIBS})
ELSE (ERROR_CODE EQUAL 0)
	MESSAGE (FATAL_ERROR "libicu not found, cannot create owb without it, on debian you must install libicu34-dev package")
ENDIF (ERROR_CODE EQUAL 0)

#######################################
#quick search the libSDL is available
#######################################

FIND_PROGRAM (SDL_CONFIG_EXECUTABLE sdl-config)

IF (NOT SDL_CONFIG_EXECUTABLE)
        MESSAGE (FATAL_ERROR "Could not find sdl-config, cannot create owb without it, on debian you must install libsdl1.2-dev package")
ENDIF (NOT SDL_CONFIG_EXECUTABLE)

EXEC_PROGRAM ("${SDL_CONFIG_EXECUTABLE} --libs | sed s/-[[:upper:]][[:graph:]]*[[:blank:]]//g | sed s/-l//"
	OUTPUT_VARIABLE LIBSDL_LIBS
        RETURN_VALUE ERROR_CODE)

IF (ERROR_CODE EQUAL 0)
	IF (WEBKIT_USE_FONTS_SDL_TTF)
		SET (LIBSDL_LIBS ${LIBSDL_LIBS} " -lSDL_ttf -lSDL -lfreetype")
	ENDIF (WEBKIT_USE_FONTS_SDL_TTF)
        IF (WEBKIT_USE_GRAPHICS_SDL)
                SET (LIBSDL_LIBS ${LIBSDL_LIBS} " -lSDL_gfx -lSDL")
        ENDIF (WEBKIT_USE_GRAPHICS_SDL)
        MESSAGE ("SDL found with this parameter for libs : "${LIBSDL_LIBS})
ELSE (ERROR_CODE EQUAL 0)
	MESSAGE (FATAL_ERROR "libSDL not found, cannot create owb without it, on debian you must install libsdl1.2-dev package")
ENDIF (ERROR_CODE EQUAL 0)

#######################################
#search for freetype (TODO cleaner)
#######################################
IF (WEBKIT_USE_FONTS_FREETYPE)
	SET (LIBFREETYPE_LIBS ${LIBFREETYPE_LIBS} " -lfreetype -lz")
ENDIF (WEBKIT_USE_FONTS_FREETYPE)

#######################################
#quick search the libcurl is available
#######################################

FIND_PROGRAM (CURL_CONFIG_EXECUTABLE curl-config)

IF (NOT CURL_CONFIG_EXECUTABLE)
	MESSAGE (FATAL_ERROR "Could not find curl-config, cannot create owb without it, on debian you must install libcurl3-gnutls-dev package")
ENDIF (NOT CURL_CONFIG_EXECUTABLE)

EXEC_PROGRAM ("${CURL_CONFIG_EXECUTABLE} --libs | sed s/-[[:upper:]][[:graph:]]*[[:blank:]]//g | sed s/-l//"
	OUTPUT_VARIABLE LIBCURL_LIBS
        RETURN_VALUE ERROR_CODE)

IF (ERROR_CODE EQUAL 0)
	MESSAGE ("curl found with this parameter for libs : "${LIBCURL_LIBS})
ELSE (ERROR_CODE EQUAL 0)
	MESSAGE (FATAL_ERROR "libcurl not found, cannot create owb without it, on debian you must install libcurl3-gnutls-dev package")
ENDIF (ERROR_CODE EQUAL 0)

############################################
#quick search the libfreetype2 is available
############################################

EXEC_PROGRAM ("${PKG_CONFIG_EXECUTABLE} --libs freetype2 | sed s/-[[:upper:]][[:graph:]]*[[:blank:]]//g | sed s/-l//"
        OUTPUT_VARIABLE LIBFREETYPE2_LIBS
	RETURN_VALUE ERROR_CODE)

IF (ERROR_CODE EQUAL 0)
	MESSAGE ("freetype2 found with this parameter for libs : "${LIBFREETYPE2_LIBS})
ELSE (ERROR_CODE EQUAL 0)
	MESSAGE (FATAL_ERROR "libfreetype2 not found, cannot create owb without it, on debian you must install libfreetype6-dev package")
ENDIF (ERROR_CODE EQUAL 0)

EXEC_PROGRAM ("${PKG_CONFIG_EXECUTABLE} --cflags freetype2 | sed s/-I// | sed s/\\ .*//g"
	OUTPUT_VARIABLE LIBFREETYPE2_CFLAGS
	RETURN_VALUE ERROR_CODE)

IF (ERROR_CODE EQUAL 0)
	MESSAGE ("freetype2 found with this parameter for cflags : "${LIBFREETYPE2_CFLAGS})
ELSE (ERROR_CODE EQUAL 0)
	MESSAGE (FATAL_ERROR "libfreetype2 not found, cannot create owb without it, on debian you must install libfreetype6-dev package")
ENDIF (ERROR_CODE EQUAL 0)

#######################################
#quick search the libxml2 is available
#######################################
IF (WEBKIT_USE_XML_SUPPORT)

FIND_PROGRAM (XML2_CONFIG_EXECUTABLE xml2-config)

IF (NOT XML2_CONFIG_EXECUTABLE)
    MESSAGE (FATAL_ERROR "Could not find xml2-config, cannot create owb without it, on debian you must install libxml2-dev package")
ENDIF (NOT XML2_CONFIG_EXECUTABLE)

EXEC_PROGRAM ("${XML2_CONFIG_EXECUTABLE} --libs | sed s/-[[:upper:]][[:graph:]]*[[:blank:]]//g | sed s/-l//"
	OUTPUT_VARIABLE LIBXML2_LIBS
	RETURN_VALUE ERROR_CODE)

IF (ERROR_CODE EQUAL 0)
        MESSAGE ("xml2 found with this parameter for libs : "${LIBXML2_LIBS})
ELSE (ERROR_CODE EQUAL 0)
	MESSAGE (FATAL_ERROR "libxml2 not found, cannot create owb without it, on debian you must install libxml2-dev package")
ENDIF (ERROR_CODE EQUAL 0)

EXEC_PROGRAM ("${XML2_CONFIG_EXECUTABLE} --cflags | sed s/-I// | sed s/\\ .*//g"
        OUTPUT_VARIABLE LIBXML2_CFLAGS
        RETURN_VALUE ERROR_CODE)

IF (ERROR_CODE EQUAL 0)
	MESSAGE ("xml2 found with this parameter for cflags : "${LIBXML2_CFLAGS})
ELSE (ERROR_CODE EQUAL 0)
        MESSAGE (FATAL_ERROR "libxml2 not found, cannot create owb without it, on debian you must install libxml2-dev package")
ENDIF (ERROR_CODE EQUAL 0)

ENDIF (WEBKIT_USE_XML_SUPPORT)

#######################################
#quick search the libxslt is available
#######################################

IF (WEBKIT_USE_XSLT_SUPPORT)

FIND_PROGRAM (XSLT_CONFIG_EXECUTABLE xslt-config)

IF (NOT XSLT_CONFIG_EXECUTABLE)
    MESSAGE (FATAL_ERROR "Could not find xslt-config, cannot create owb without it, on debian you must install libxslt1-dev package")
ENDIF (NOT XSLT_CONFIG_EXECUTABLE)

EXEC_PROGRAM ("${XSLT_CONFIG_EXECUTABLE} --libs | sed s/-[[:upper:]][[:graph:]]*[[:blank:]]//g | sed s/-l//"
	OUTPUT_VARIABLE LIBXSLT_LIBS
	RETURN_VALUE ERROR_CODE)

IF (ERROR_CODE EQUAL 0)
	MESSAGE ("xslt found with this parameter for libs : "${LIBXSLT_LIBS})
ELSE (ERROR_CODE EQUAL 0)
	MESSAGE (FATAL_ERROR "libxslt not found, cannot create owb without it, on debian you must install libxslt1-dev package")
ENDIF (ERROR_CODE EQUAL 0)

ENDIF (WEBKIT_USE_XSLT_SUPPORT)



INCLUDE (CheckLibraryExists)

ADD_DEFINITIONS (-DAPPLE_CHANGES=1)
ADD_DEFINITIONS (-DHAVE_FUNC_ISNAN)
ADD_DEFINITIONS (-D_THREAD_SAFE)
ADD_DEFINITIONS (-D__OWB__)
ADD_DEFINITIONS (-DOWB_CONSOLE_INPUT)
ADD_DEFINITIONS (-DUSE_CONSERVATIVE_GC=0)
ADD_DEFINITIONS (-DD_MULTISCAN_FILES_SUPPORTED)
ADD_DEFINITIONS (-DLINUX)
ADD_DEFINITIONS (-pthread)
ADD_DEFINITIONS (-O2)

# Configuration checks
check_library_exists(pthread pthread_attr_get_np "" HAVE_PTHREAD_ATTR_GET_NP)
check_library_exists(pthread pthread_getattr_np "" HAVE_PTHREAD_GETATTR_NP)


##############################
##############################
##############################
# INCLUDE DIRECTORY
##############################
##############################
##############################
INCLUDE_DIRECTORIES (
    ${LIBXML2_CFLAGS}
    ${LIBCAIRO_CFLAGS}
    ${LIBFREETYPE2_CFLAGS}
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Configuration/
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Implementations/Widgets/
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Implementations/Widgets/Generic/
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Interfaces/
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Interfaces/network
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Interfaces/graphics
    ${CMAKE_CURRENT_SOURCE_DIR}/BAL/Implementations/ImageDecoders/Common/
    ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/pcre
    ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/bindings
    ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/bindings/c
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
#BAL
##############################
##############################
##############################

INCLUDE (BAL/Implementations/Facilities/Generic/SourcesCMakeLists.txt)
SET (bal_SRC ${bal_SRC} ${facilities_SRC})

INCLUDE (BAL/Implementations/ImageDecoders/Generic/JPEG/SourcesCMakeLists.txt)
SET (bal_SRC ${bal_SRC} ${decoder_jpeg_SRC})

INCLUDE (BAL/Implementations/ImageDecoders/Generic/PNG/SourcesCMakeLists.txt)
SET (bal_SRC ${bal_SRC} ${decoder_png_SRC})

INCLUDE (BAL/Implementations/ImageDecoders/Generic/GIF/SourcesCMakeLists.txt)
SET (bal_SRC ${bal_SRC} ${decoder_gif_SRC})

INCLUDE (BAL/Implementations/Widgets/SourcesCMakeLists.txt)
SET (bal_SRC ${bal_SRC} ${widget_SRC})

INCLUDE (BAL/Implementations/Events/SDL/SourcesCMakeLists.txt)
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

##############################
##############################
##############################
#BAL_GLUE
##############################
##############################
##############################

INCLUDE (BALGlue/SourcesCMakeLists.txt)

ADD_LIBRARY (balglue STATIC ${bal_glue_SRC})

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
#JSCORE
##############################
##############################
##############################

SET (CREATE_HASH_TABLE ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/kjs/create_hash_table )

MACRO (CREATE_LUT _srcs_LIST _in_FILE _out_FILE _dep_FILE)
   GET_FILENAME_COMPONENT (_baseName ${_out_FILE} PATH)
   ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_out_FILE}
	COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/${_baseName}
      COMMAND ${PERL_EXECUTABLE} ${CREATE_HASH_TABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${_in_FILE} -i > ${CMAKE_CURRENT_BINARY_DIR}/${_out_FILE}
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_in_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/${_dep_FILE} )
   SET ( ${_srcs_LIST}  ${${_srcs_LIST}} ${CMAKE_CURRENT_BINARY_DIR}/${_out_FILE})
ENDMACRO (CREATE_LUT)

ADD_EXECUTABLE (dftables JavaScriptCore/pcre/dftables.c)

GET_TARGET_PROPERTY (dftables_executable dftables LOCATION)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/kjs/chartables.c
   COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/generated_sources
   COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/kjs
   COMMAND ${dftables_executable} ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/kjs/chartables.c
   DEPENDS dftables )

IF (WEBKIT_USE_JS)
	CREATE_LUT (jscore_SRC JavaScriptCore/kjs/array_object.cpp generated_sources/kjs/array_object.lut.h JavaScriptCore/kjs/array_object.cpp)
	CREATE_LUT (jscore_SRC JavaScriptCore/kjs/date_object.cpp generated_sources/kjs/date_object.lut.h JavaScriptCore/kjs/date_object.cpp)
	CREATE_LUT (jscore_SRC JavaScriptCore/kjs/keywords.table generated_sources/kjs/lexer.lut.h JavaScriptCore/kjs/lexer.cpp)
	CREATE_LUT (jscore_SRC JavaScriptCore/kjs/math_object.cpp generated_sources/kjs/math_object.lut.h JavaScriptCore/kjs/math_object.cpp)
	CREATE_LUT (jscore_SRC JavaScriptCore/kjs/number_object.cpp generated_sources/kjs/number_object.lut.h JavaScriptCore/kjs/number_object.cpp)
	CREATE_LUT (jscore_SRC JavaScriptCore/kjs/regexp_object.cpp generated_sources/kjs/regexp_object.lut.h JavaScriptCore/kjs/regexp_object.cpp)
	CREATE_LUT (jscore_SRC JavaScriptCore/kjs/string_object.cpp generated_sources/kjs/string_object.lut.h JavaScriptCore/kjs/string_object.cpp)
ENDIF (WEBKIT_USE_JS)

ADD_CUSTOM_COMMAND (
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/kjs/grammar.cpp ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/kjs/grammar.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}

  COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/generated_sources
  COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/kjs
  COMMAND ${BISON_EXECUTABLE} -d -p kjsyy ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/kjs/grammar.y
  COMMAND mv grammar.tab.c ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/kjs/grammar.cpp
  COMMAND mv grammar.tab.h ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/kjs/grammar.h
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/kjs/grammar.y
)

INCLUDE (JavaScriptCore/JscoreCMakeLists.txt)


IF (WEBKIT_USE_JS)
	ADD_LIBRARY (jscore ${jscore_SRC})

	ADD_EXECUTABLE (testkjs JavaScriptCore/kjs/testkjs.cpp)
	TARGET_LINK_LIBRARIES (testkjs
		jscore
        ${LIBICU_LIBS}
	)

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
#WEBCORE
##############################
##############################
##############################

SET (FEATURE_DEFINES_JAVASCRIPT LANGUAGE_JAVASCRIPT)

IF (WEBKIT_USE_XPATH_SUPPORT)
	IF (WEBKIT_USE_JS)
		SET (IDL_BINDINGS_XPATH
			WebCore/xml/XPathExpression.idl
			WebCore/xml/XPathNSResolver.idl
			WebCore/xml/XPathEvaluator.idl
			WebCore/xml/XPathResult.idl
		)
	ENDIF (WEBKIT_USE_JS)
ENDIF (WEBKIT_USE_XPATH_SUPPORT)

IF (WEBKIT_USE_XSLT_SUPPORT)
	SET (FEATURE_DEFINES_JAVASCRIPT ${FEATURE_DEFINES_JAVASCRIPT} XSLT_SUPPORT)
ENDIF (WEBKIT_USE_XSLT_SUPPORT)

IF (WEBKIT_USE_XBL_SUPPORT)
	SET (FEATURE_DEFINES_JAVASCRIPT ${FEATURE_DEFINES_JAVASCRIPT} XBL_SUPPORT)
ENDIF (WEBKIT_USE_XBL_SUPPORT)

IF (WEBKIT_USE_SVG_SUPPORT)
	SET (FEATURE_DEFINES_JAVASCRIPT ${FEATURE_DEFINES_JAVASCRIPT} SVG_SUPPORT)

	SET (IDL_BINDINGS_SVG
    WebCore/ksvg2/events/SVGZoomEvent.idl

    WebCore/ksvg2/svg/SVGAElement.idl
    WebCore/ksvg2/svg/SVGAngle.idl
    WebCore/ksvg2/svg/SVGAnimateColorElement.idl
    WebCore/ksvg2/svg/SVGAnimatedAngle.idl
    WebCore/ksvg2/svg/SVGAnimatedBoolean.idl
    WebCore/ksvg2/svg/SVGAnimatedEnumeration.idl
    WebCore/ksvg2/svg/SVGAnimatedInteger.idl
    WebCore/ksvg2/svg/SVGAnimatedLength.idl
    WebCore/ksvg2/svg/SVGAnimatedLengthList.idl
    WebCore/ksvg2/svg/SVGAnimatedNumber.idl
    WebCore/ksvg2/svg/SVGAnimatedNumberList.idl
    WebCore/ksvg2/svg/SVGAnimatedPreserveAspectRatio.idl
    WebCore/ksvg2/svg/SVGAnimatedRect.idl
    WebCore/ksvg2/svg/SVGAnimatedString.idl
    WebCore/ksvg2/svg/SVGAnimatedTransformList.idl
    WebCore/ksvg2/svg/SVGAnimateElement.idl
    WebCore/ksvg2/svg/SVGAnimateTransformElement.idl
    WebCore/ksvg2/svg/SVGAnimationElement.idl
    WebCore/ksvg2/svg/SVGCircleElement.idl
    WebCore/ksvg2/svg/SVGClipPathElement.idl
    WebCore/ksvg2/svg/SVGColor.idl
    WebCore/ksvg2/svg/SVGComponentTransferFunctionElement.idl
    WebCore/ksvg2/svg/SVGCursorElement.idl
    WebCore/ksvg2/svg/SVGDefsElement.idl
    WebCore/ksvg2/svg/SVGDescElement.idl
    WebCore/ksvg2/svg/SVGDocument.idl
    WebCore/ksvg2/svg/SVGElement.idl
    WebCore/ksvg2/svg/SVGEllipseElement.idl
    WebCore/ksvg2/svg/SVGFEBlendElement.idl
    WebCore/ksvg2/svg/SVGFEColorMatrixElement.idl
    WebCore/ksvg2/svg/SVGFEComponentTransferElement.idl
    WebCore/ksvg2/svg/SVGFECompositeElement.idl
    WebCore/ksvg2/svg/SVGFEDiffuseLightingElement.idl
    WebCore/ksvg2/svg/SVGFEDisplacementMapElement.idl
    WebCore/ksvg2/svg/SVGFEDistantLightElement.idl
    WebCore/ksvg2/svg/SVGFEFloodElement.idl
    WebCore/ksvg2/svg/SVGFEFuncAElement.idl
    WebCore/ksvg2/svg/SVGFEFuncBElement.idl
    WebCore/ksvg2/svg/SVGFEFuncGElement.idl
    WebCore/ksvg2/svg/SVGFEFuncRElement.idl
    WebCore/ksvg2/svg/SVGFEGaussianBlurElement.idl
    WebCore/ksvg2/svg/SVGFEImageElement.idl
    WebCore/ksvg2/svg/SVGFEMergeElement.idl
    WebCore/ksvg2/svg/SVGFEMergeNodeElement.idl
    WebCore/ksvg2/svg/SVGFEOffsetElement.idl
    WebCore/ksvg2/svg/SVGFEPointLightElement.idl
    WebCore/ksvg2/svg/SVGFESpecularLightingElement.idl
    WebCore/ksvg2/svg/SVGFESpotLightElement.idl
    WebCore/ksvg2/svg/SVGFETileElement.idl
    WebCore/ksvg2/svg/SVGFETurbulenceElement.idl
    WebCore/ksvg2/svg/SVGFilterElement.idl
    WebCore/ksvg2/svg/SVGForeignObjectElement.idl
    WebCore/ksvg2/svg/SVGGElement.idl
    WebCore/ksvg2/svg/SVGGradientElement.idl
    WebCore/ksvg2/svg/SVGImageElement.idl
    WebCore/ksvg2/svg/SVGLength.idl
    WebCore/ksvg2/svg/SVGLengthList.idl
    WebCore/ksvg2/svg/SVGLinearGradientElement.idl
    WebCore/ksvg2/svg/SVGLineElement.idl
    WebCore/ksvg2/svg/SVGMarkerElement.idl
    WebCore/ksvg2/svg/SVGMaskElement.idl
    WebCore/ksvg2/svg/SVGMatrix.idl
    WebCore/ksvg2/svg/SVGMetadataElement.idl
    WebCore/ksvg2/svg/SVGNumberList.idl
    WebCore/ksvg2/svg/SVGPaint.idl
    WebCore/ksvg2/svg/SVGPathElement.idl
    WebCore/ksvg2/svg/SVGPathSegArcAbs.idl
    WebCore/ksvg2/svg/SVGPathSegArcRel.idl
    WebCore/ksvg2/svg/SVGPathSegClosePath.idl
    WebCore/ksvg2/svg/SVGPathSegCurvetoCubicAbs.idl
    WebCore/ksvg2/svg/SVGPathSegCurvetoCubicRel.idl
    WebCore/ksvg2/svg/SVGPathSegCurvetoCubicSmoothAbs.idl
    WebCore/ksvg2/svg/SVGPathSegCurvetoCubicSmoothRel.idl
    WebCore/ksvg2/svg/SVGPathSegCurvetoQuadraticAbs.idl
    WebCore/ksvg2/svg/SVGPathSegCurvetoQuadraticRel.idl
    WebCore/ksvg2/svg/SVGPathSegCurvetoQuadraticSmoothAbs.idl
    WebCore/ksvg2/svg/SVGPathSegCurvetoQuadraticSmoothRel.idl
    WebCore/ksvg2/svg/SVGPathSeg.idl
    WebCore/ksvg2/svg/SVGPathSegLinetoAbs.idl
    WebCore/ksvg2/svg/SVGPathSegLinetoHorizontalAbs.idl
    WebCore/ksvg2/svg/SVGPathSegLinetoHorizontalRel.idl
    WebCore/ksvg2/svg/SVGPathSegLinetoRel.idl
    WebCore/ksvg2/svg/SVGPathSegLinetoVerticalAbs.idl
    WebCore/ksvg2/svg/SVGPathSegLinetoVerticalRel.idl
    WebCore/ksvg2/svg/SVGPathSegList.idl
    WebCore/ksvg2/svg/SVGPathSegMovetoAbs.idl
    WebCore/ksvg2/svg/SVGPathSegMovetoRel.idl
    WebCore/ksvg2/svg/SVGPatternElement.idl
    WebCore/ksvg2/svg/SVGPointList.idl
    WebCore/ksvg2/svg/SVGPolygonElement.idl
    WebCore/ksvg2/svg/SVGPolylineElement.idl
    WebCore/ksvg2/svg/SVGPreserveAspectRatio.idl
    WebCore/ksvg2/svg/SVGRadialGradientElement.idl
    WebCore/ksvg2/svg/SVGRectElement.idl
    WebCore/ksvg2/svg/SVGRenderingIntent.idl
    WebCore/ksvg2/svg/SVGScriptElement.idl
    WebCore/ksvg2/svg/SVGSetElement.idl
    WebCore/ksvg2/svg/SVGStopElement.idl
    WebCore/ksvg2/svg/SVGStringList.idl
    WebCore/ksvg2/svg/SVGStyleElement.idl
    WebCore/ksvg2/svg/SVGSVGElement.idl
    WebCore/ksvg2/svg/SVGSwitchElement.idl
    WebCore/ksvg2/svg/SVGSymbolElement.idl
    WebCore/ksvg2/svg/SVGTextContentElement.idl
    WebCore/ksvg2/svg/SVGTextElement.idl
    WebCore/ksvg2/svg/SVGTextPositioningElement.idl
    WebCore/ksvg2/svg/SVGTitleElement.idl
    WebCore/ksvg2/svg/SVGTransform.idl
    WebCore/ksvg2/svg/SVGTransformList.idl
    WebCore/ksvg2/svg/SVGTRefElement.idl
    WebCore/ksvg2/svg/SVGTSpanElement.idl
    WebCore/ksvg2/svg/SVGUnitTypes.idl
    WebCore/ksvg2/svg/SVGUseElement.idl
    WebCore/ksvg2/svg/SVGViewElement.idl
)

ENDIF (WEBKIT_USE_SVG_SUPPORT)

IF (WEBKIT_USE_JS)

	SET (IDL_BINDINGS
        ${IDL_BINDINGS}

    WebCore/css/Counter.idl
    WebCore/css/CSSPrimitiveValue.idl
    WebCore/css/CSSRule.idl
    WebCore/css/CSSRuleList.idl
    WebCore/css/CSSStyleDeclaration.idl
    WebCore/css/CSSValue.idl
    WebCore/css/CSSValueList.idl
    WebCore/css/MediaList.idl

    WebCore/dom/Attr.idl
    WebCore/dom/CharacterData.idl
    WebCore/dom/DocumentFragment.idl
    WebCore/dom/Document.idl
    WebCore/dom/DocumentType.idl
    WebCore/dom/DOMImplementation.idl
    WebCore/dom/Element.idl
    WebCore/dom/Entity.idl
    WebCore/dom/Event.idl
    WebCore/dom/KeyboardEvent.idl
    WebCore/dom/MouseEvent.idl
    WebCore/dom/MutationEvent.idl
    WebCore/dom/NodeFilter.idl
    WebCore/dom/Node.idl
    WebCore/dom/NodeIterator.idl
    WebCore/dom/Notation.idl
    WebCore/dom/OverflowEvent.idl
    WebCore/dom/ProcessingInstruction.idl
    WebCore/dom/RangeException.idl
    WebCore/dom/Range.idl
    WebCore/dom/Text.idl
    WebCore/dom/TextEvent.idl
    WebCore/dom/TreeWalker.idl
    WebCore/dom/UIEvent.idl
    WebCore/dom/WheelEvent.idl

    WebCore/html/CanvasGradient.idl
    WebCore/html/CanvasPattern.idl
    WebCore/html/CanvasRenderingContext2D.idl
    WebCore/html/HTMLAnchorElement.idl
    WebCore/html/HTMLAppletElement.idl
    WebCore/html/HTMLAreaElement.idl
    WebCore/html/HTMLBaseElement.idl
    WebCore/html/HTMLBaseFontElement.idl
    WebCore/html/HTMLBlockquoteElement.idl
    WebCore/html/HTMLBodyElement.idl
    WebCore/html/HTMLBRElement.idl
    WebCore/html/HTMLButtonElement.idl
    WebCore/html/HTMLCanvasElement.idl
    WebCore/html/HTMLDirectoryElement.idl
    WebCore/html/HTMLDivElement.idl
    WebCore/html/HTMLDListElement.idl
    WebCore/html/HTMLDocument.idl
    WebCore/html/HTMLElement.idl
    WebCore/html/HTMLFieldSetElement.idl
    WebCore/html/HTMLFontElement.idl
    WebCore/html/HTMLFormElement.idl
    WebCore/html/HTMLHeadElement.idl
    WebCore/html/HTMLHeadingElement.idl
    WebCore/html/HTMLHRElement.idl
    WebCore/html/HTMLHtmlElement.idl
    WebCore/html/HTMLImageElement.idl
    WebCore/html/HTMLInputElement.idl
    WebCore/html/HTMLIsIndexElement.idl
    WebCore/html/HTMLLabelElement.idl
    WebCore/html/HTMLLegendElement.idl
    WebCore/html/HTMLLIElement.idl
    WebCore/html/HTMLLinkElement.idl
    WebCore/html/HTMLMapElement.idl
    WebCore/html/HTMLMenuElement.idl
    WebCore/html/HTMLMetaElement.idl
    WebCore/html/HTMLModElement.idl
    WebCore/html/HTMLOListElement.idl
    WebCore/html/HTMLOptGroupElement.idl
    WebCore/html/HTMLOptionElement.idl
    WebCore/html/HTMLOptionsCollection.idl
    WebCore/html/HTMLParagraphElement.idl
    WebCore/html/HTMLParamElement.idl
    WebCore/html/HTMLPreElement.idl
    WebCore/html/HTMLQuoteElement.idl
    WebCore/html/HTMLSelectElement.idl
    WebCore/html/HTMLScriptElement.idl
    WebCore/html/HTMLStyleElement.idl
    WebCore/html/HTMLTextAreaElement.idl
    WebCore/html/HTMLTitleElement.idl
    WebCore/html/HTMLUListElement.idl

    WebCore/page/DOMWindow.idl

    WebCore/xml/DOMParser.idl
    WebCore/xml/XMLSerializer.idl
)

ENDIF (WEBKIT_USE_JS)

SET (IDL_BINDINGS
	${IDL_BINDINGS}
	${IDL_BINDINGS_SVG}
	${IDL_BINDINGS_XPATH}
)

# create these files at build time on demand, NOT everytime at cmake-time, Alex
FOREACH (idl ${IDL_BINDINGS})
   GET_FILENAME_COMPONENT (_basename ${idl} NAME_WE)
   ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/JS${_basename}.cpp ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/JS${_basename}.h
      COMMAND ${PERL_EXECUTABLE} -I${CMAKE_CURRENT_SOURCE_DIR}/WebCore/bindings/scripts ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/bindings/scripts/generate-bindings.pl  --defines \"${FEATURE_DEFINES_JAVASCRIPT}\" --generator JS --include ${CMAKE_SOURCE_DIR}/WebCore/dom --include ${CMAKE_SOURCE_DIR}/WebCore/html --include ${CMAKE_SOURCE_DIR}/WebCore/xml --include ${CMAKE_SOURCE_DIR}/WebCore/WebCore/ksvg2/svg --outputdir ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore ${CMAKE_CURRENT_SOURCE_DIR}/${idl}
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${idl} ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/bindings/scripts/CodeGenerator.pm ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/bindings/scripts/CodeGeneratorJS.pm)
   SET (webcore_SRC ${webcore_SRC} ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/JS${_basename}.cpp ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/JS${_basename}.h )
ENDFOREACH (idl)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/ColorData.c
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND mkdir -p generated_sources
  COMMAND mkdir -p generated_sources/WebCore
  COMMAND echo '\#include <string.h>' > generated_sources/WebCore/ColorData.c
  COMMAND ${GPERF_EXECUTABLE} -CDEot -L ANSI-C -k \"*\" -N findColor -D -s 2 < ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/platform/ColorData.gperf >> generated_sources/WebCore/ColorData.c
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/platform/ColorData.gperf
)

MACRO (CREATE_GENERATED_CPP_HELPER  _inlName)
   GET_FILENAME_COMPONENT (_baseName ${_inlName} NAME_WE)
   IF (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/${_baseName}.cpp)
      FILE (WRITE ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/${_baseName}.cpp
"/* This file is autogenerated by CMake.                                       *
 * Do not edit, changes will be lost.                                         *
 * With the CMake buildsystem giving generated-but-not-compiled-on-their-own  *
 * files the suffix \".cpp\" is not recommended, because it breaks the          *
 * dependency handling. So the actual generated file has the suffix \".inl\"    *
 * and this file here only exists as a helper.                                */
#include \"${CMAKE_CURRENT_BINARY_DIR}/${_inlName}\"\n")
   ENDIF (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/${_baseName}.cpp)
   SET (webcore_SRC ${webcore_SRC} ${CMAKE_CURRENT_BINARY_DIR}/${_inlName})
ENDMACRO (CREATE_GENERATED_CPP_HELPER  _inlName)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/CSSGrammar.cpp ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/CSSGrammar.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND mkdir -p generated_sources
  COMMAND mkdir -p generated_sources/WebCore
  COMMAND ${BISON_EXECUTABLE} -d -p cssyy ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/CSSGrammar.y
  COMMAND mv CSSGrammar.tab.c ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/CSSGrammar.cpp
  COMMAND mv CSSGrammar.tab.h ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/CSSGrammar.h
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/CSSGrammar.y
)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/html/HTMLEntityNames.c
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND mkdir -p generated_sources/WebCore/html
  COMMAND ${tvcore_dir}
  COMMAND echo '\#include <string.h>' > generated_sources/WebCore/html/HTMLEntityNames.c
  COMMAND ${GPERF_EXECUTABLE} -a -L ANSI-C -C -G -c -o -t -k '*' -N findEntity -D -s 2 < ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/html/HTMLEntityNames.gperf >> generated_sources/WebCore/html/HTMLEntityNames.c

  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/html/HTMLEntityNames.gperf
)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/html/HTMLNames.cpp ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/html/HTMLNames.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/html
  COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/ksvg2/scripts/make_names.pl --tags ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/html/HTMLTagNames.in --attrs ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/html/HTMLAttributeNames.in --namespace HTML --namespacePrefix xhtml --cppNamespace WebCore --namespaceURI "http://www.w3.org/1999/xhtml" --attrsNullNamespace --output generated_sources/WebCore/html

  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/html/HTMLTagNames.in ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/html/HTMLAttributeNames.in
)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/xml/XMLNames.cpp ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/xml/XMLNames.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/xml
  COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/ksvg2/scripts/make_names.pl --attrs ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/xml/xmlattrs.in --namespace XML --cppNamespace WebCore --namespaceURI "http://www.w3.org/XML/1998/namespace" --output generated_sources/WebCore/xml

  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/xml/xmlattrs.in
)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/CSSPropertyNames.h ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/CSSPropertyNames.c

  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/CSSPropertyNames.in ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/CSSPropertyNames.in
  COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/makeprop

  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/CSSPropertyNames.in
)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/CSSValueKeywords.h ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/CSSValueKeywords.c

  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/CSSValueKeywords.in ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/CSSValueKeywords.in
  COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/makevalues

  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/CSSValueKeywords.in
)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/ksvgcssproperties.h
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/ksvg2/css/CSSPropertyNames.in ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/ksvgcssproperties.in
  COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/ksvg2/scripts/cssmakeprops -n SVG -f generated_sources/WebCore/ksvgcssproperties.in
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/ksvg2/scripts/cssmakeprops ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/ksvg2/css/CSSPropertyNames.in
)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/tokenizer.inl
  COMMAND flex -t < ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/tokenizer.flex | ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/maketokenizer > ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/tokenizer.inl
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/tokenizer.flex
)
CREATE_GENERATED_CPP_HELPER (generated_sources/WebCore/tokenizer.inl)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/UserAgentStyleSheets.h ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/UserAgentStyleSheetsData.cpp
  COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/make-css-file-arrays.pl ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/UserAgentStyleSheets.h ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/UserAgentStyleSheetsData.cpp ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/html4.css ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/quirks.css ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/svg.css ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/view-source.css
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/html4.css ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/quirks.css ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/css/svg.css
)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/XPathGrammar.cpp ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/XPathGrammar.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND ${BISON_EXECUTABLE} -d -p xpathyy ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/xml/XPathGrammar.y
  COMMAND mv XPathGrammar.tab.c ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/XPathGrammar.cpp
  COMMAND mv XPathGrammar.tab.h ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/XPathGrammar.h
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/xml/XPathGrammar.y
)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/html/DocTypeStrings.cpp
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND mkdir -p generated_sources/WebCore/html
  COMMAND echo '\#include <string.h>' > generated_sources/WebCore/html/DocTypeStrings.cpp
  COMMAND ${GPERF_EXECUTABLE} -CEot -L ANSI-C -k \"*\" -N findDoctypeEntry -F ,PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards < ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/html/DocTypeStrings.gperf >> generated_sources/WebCore/html/DocTypeStrings.cpp
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/html/DocTypeStrings.gperf
)

SET (CREATE_HASH_TABLE ${CMAKE_CURRENT_SOURCE_DIR}/JavaScriptCore/kjs/create_hash_table)

MACRO (CREATE_CPP_LUT _in_FILE _out_FILE _dep_FILE)
   ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_out_FILE}
      COMMAND ${PERL_EXECUTABLE} ${CREATE_HASH_TABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${_in_FILE} > ${CMAKE_CURRENT_BINARY_DIR}/${_out_FILE}
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_in_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/${_dep_FILE} )
ENDMACRO (CREATE_CPP_LUT)

IF (WEBKIT_USE_JS)
	CREATE_CPP_LUT (WebCore/bindings/js/JSHTMLInputElementBase.cpp generated_sources/WebCore/JSHTMLInputElementBaseTable.inl WebCore/bindings/js/JSHTMLInputElementBase.cpp)
	CREATE_GENERATED_CPP_HELPER (generated_sources/WebCore/JSHTMLInputElementBaseTable.inl)
ENDIF (WEBKIT_USE_JS)

IF (WEBKIT_USE_SVG_SUPPORT)
    CREATE_CPP_LUT (WebCore/bindings/js/JSSVGNumber.cpp generated_sources/WebCore/JSSVGNumberTable.inl WebCore/bindings/js/JSSVGNumber.cpp)
    CREATE_CPP_LUT (WebCore/bindings/js/JSSVGPoint.cpp generated_sources/WebCore/JSSVGPointTable.inl WebCore/bindings/js/JSSVGPoint.cpp)
    CREATE_GENERATED_CPP_HELPER (generated_sources/WebCore/JSSVGPointTable.inl)
    CREATE_CPP_LUT (WebCore/bindings/js/JSSVGRect.cpp generated_sources/WebCore/JSSVGRectTable.inl WebCore/bindings/js/JSSVGRect.cpp)
    CREATE_GENERATED_CPP_HELPER (generated_sources/WebCore/JSSVGRectTable.inl)
ENDIF (WEBKIT_USE_SVG_SUPPORT)

IF (WEBKIT_USE_JS)
	CREATE_LUT (webcore_SRC WebCore/bindings/js/JSLayoutTestController.cpp generated_sources/WebCore/JSLayoutTestController.lut.h WebCore/bindings/js/JSLayoutTestController.cpp)
	CREATE_LUT (webcore_SRC WebCore/bindings/js/JSXMLHttpRequest.cpp generated_sources/WebCore/JSXMLHttpRequest.lut.h WebCore/bindings/js/JSXMLHttpRequest.cpp)
	CREATE_LUT (webcore_SRC WebCore/bindings/js/JSXSLTProcessor.cpp generated_sources/WebCore/JSXSLTProcessor.lut.h WebCore/bindings/js/JSXSLTProcessor.cpp)
	CREATE_LUT (webcore_SRC WebCore/bindings/js/kjs_css.cpp generated_sources/WebCore/kjs_css.lut.h WebCore/bindings/js/kjs_css.cpp)
	CREATE_LUT (webcore_SRC WebCore/bindings/js/kjs_dom.cpp generated_sources/WebCore/kjs_dom.lut.h WebCore/bindings/js/kjs_dom.cpp)
	CREATE_LUT (webcore_SRC WebCore/bindings/js/kjs_events.cpp generated_sources/WebCore/kjs_events.lut.h WebCore/bindings/js/kjs_events.cpp)
	CREATE_LUT (webcore_SRC WebCore/bindings/js/kjs_html.cpp generated_sources/WebCore/kjs_html.lut.h WebCore/bindings/js/kjs_html.cpp)
	CREATE_LUT (webcore_SRC WebCore/bindings/js/kjs_navigator.cpp generated_sources/WebCore/kjs_navigator.lut.h WebCore/bindings/js/kjs_navigator.cpp)
	CREATE_LUT (webcore_SRC WebCore/bindings/js/kjs_proxy.cpp generated_sources/WebCore/kjs_proxy.lut.h WebCore/bindings/js/kjs_proxy.cpp)
	CREATE_LUT (webcore_SRC WebCore/bindings/js/kjs_traversal.cpp generated_sources/WebCore/kjs_traversal.lut.h WebCore/bindings/js/kjs_traversal.cpp)
	CREATE_LUT (webcore_SRC WebCore/bindings/js/kjs_window.cpp generated_sources/WebCore/kjs_window.lut.h WebCore/bindings/js/kjs_window.cpp)
ENDIF (WEBKIT_USE_JS)


IF (WEBKIT_USE_SVG_SUPPORT)

	ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/WebCore/ksvg2/svg/SVGNames.cpp ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/ksvg2/svg/SVGNames.h ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/ksvg2/svg/SVGElementFactory.cpp ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/ksvg2/svg/SVGElementFactory.h
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/ksvg2/
		COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/WebCore/ksvg2/svg
		COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/ksvg2/scripts/make_names.pl --tags ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/WebCore/ksvg2/svg/svgtags.in --attrs ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/ksvg2/svg/svgattrs.in --namespace SVG --cppNamespace WebCore --namespaceURI "http://www.w3.org/2000/svg" --factory --attrsNullNamespace --output generated_sources/WebCore/ksvg2/svg

		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/WebCore/ksvg2/svg/svgtags.in ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/ksvg2/svg/svgattrs.in
	)

	ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore/ksvgcssvalues.h
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated_sources/WebCore
		COMMAND ${PERL_EXECUTABLE} -ne 'print lc' ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/ksvg2/css/CSSValueKeywords.in > ksvgcssvalues.in
		COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/ksvg2/scripts/cssmakevalues -n SVG -f ksvgcssvalues.in
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/ksvg2/scripts/cssmakevalues ${CMAKE_CURRENT_SOURCE_DIR}/WebCore/ksvg2/css/CSSValueKeywords.in
	)

ENDIF (WEBKIT_USE_SVG_SUPPORT)

INCLUDE (WebCore/SourcesCMakeLists.txt)
SET (webcore_SRC ${webcore_SRC} ${generated_sources_SRC})

INCLUDE (WebCore/page/SourcesCMakeLists.txt)
SET (webcore_SRC ${webcore_SRC} ${page_SRC})

INCLUDE (WebCore/loader/SourcesCMakeLists.txt)
SET (webcore_SRC ${webcore_SRC} ${loader_SRC})

INCLUDE (WebCore/platform/SourcesCMakeLists.txt)
SET (webcore_SRC ${webcore_SRC} ${platform_SRC})

INCLUDE (WebCore/css/SourcesCMakeLists.txt)
SET (webcore_SRC ${webcore_SRC} ${css_SRC})

INCLUDE (WebCore/rendering/SourcesCMakeLists.txt)
SET (webcore_SRC ${webcore_SRC} ${rendering_SRC})

INCLUDE (WebCore/xml/SourcesCMakeLists.txt)
SET (webcore_SRC ${webcore_SRC} ${xml_SRC})

INCLUDE (WebCore/dom/SourcesCMakeLists.txt)
SET (webcore_SRC ${webcore_SRC} ${dom_SRC})

INCLUDE (WebCore/editing/SourcesCMakeLists.txt)
SET (webcore_SRC ${webcore_SRC} ${editing_SRC})

INCLUDE (WebCore/html/SourcesCMakeLists.txt)
SET (webcore_SRC ${webcore_SRC} ${html_SRC})

INCLUDE (WebCore/history/SourcesCMakeLists.txt)
SET (webcore_SRC ${webcore_SRC} ${history_SRC})

IF (WEBKIT_USE_ICON_SUPPORT)
	INCLUDE (WebCore/loader/icon/SourcesCMakeLists.txt)
	SET (webcore_SRC ${webcore_SRC} ${icon_SRC})
ENDIF (WEBKIT_USE_ICON_SUPPORT)

IF (WEBKIT_USE_SVG_SUPPORT)
	INCLUDE (WebCore/ksvg2/SourcesCMakeLists.txt)
	SET (webcore_SRC ${webcore_SRC} ${svg_generated_sources_SRC})

	INCLUDE (WebCore/ksvg2/css/SourcesCMakeLists.txt)
	SET (webcore_SRC ${webcore_SRC} ${svg_css_SRC})

	INCLUDE (WebCore/WebCore/ksvg2/svg/SourcesCMakeLists.txt)
	SET (webcore_SRC ${webcore_SRC} ${svg_svg_SRC})

	INCLUDE (WebCore/ksvg2/misc/SourcesCMakeLists.txt)
	SET (webcore_SRC ${webcore_SRC} ${svg_misc_SRC})

	INCLUDE (WebCore/ksvg2/events/SourcesCMakeLists.txt)
	SET (webcore_SRC ${webcore_SRC} ${svg_events_SRC})

	INCLUDE (WebCore/kcanvas/SourcesCMakeLists.txt)
	SET (webcore_SRC ${webcore_SRC} ${kcanvas_SRC})

	INCLUDE (WebCore/kcanvas/device/SourcesCMakeLists.txt)
	SET (webcore_SRC ${webcore_SRC} ${kcanvas_device_SRC})

ENDIF (WEBKIT_USE_SVG_SUPPORT)

IF (WEBKIT_USE_JS)

	INCLUDE (WebCore/bindings/js/SourcesCMakeLists.txt)
	SET (webcore_SRC ${webcore_SRC} ${bindings_js_SRC})

ENDIF (WEBKIT_USE_JS)

ADD_LIBRARY (webcore-owb ${webcore_SRC})
#ADD_LIBRARY (webcore-owb ${css_SRC})
TARGET_LINK_LIBRARIES (webcore-owb
    ${LIBXML2_LIBS}
    ${LIBXSLT_LIBS}
	${LIBICU_LIBS}
	${LIBFREETYPE2_LIBS}
	${LIBCURL_LIBS}
	${LIBCAIRO_LIBS}
)

EXEC_PROGRAM ("find" ARGS " ${CMAKE_CURRENT_SOURCE_DIR} -maxdepth 2 -name AddOns -type d -print | grep -v `basename $(pwd)`" OUTPUT_VARIABLE ADDONS_DIR)
IF (EXISTS ${ADDONS_DIR})
    MESSAGE (${ADDONS_DIR})
	SET (ADDONS_LINK )
	ADD_SUBDIRECTORY (${ADDONS_DIR})
ENDIF (EXISTS ${ADDONS_DIR})

##############################
##############################
##############################
#ORIGYN_LAUNCHER
##############################
##############################
##############################
ADD_EXECUTABLE (owb WebKit/OrigynWebBrowser/OrigynLauncher/main.cpp)
ADD_DEPENDENCIES (owb bal balglue webkit jscore webcore-owb)
TARGET_LINK_LIBRARIES (owb
	webkit
	webcore-owb
	balglue
	bal
   	${LIBCAIRO_LIBS}
    ${LIBSDL_LIBS}
	${ADDONS_LINK}
	${LIBFREETYPE_LIBS}
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
    webkit
    ${LIBXML2_LIBS}
    ${LIBXSLT_LIBS}
	${LIBICU_LIBS}
	${LIBFREETYPE2_LIBS}
	${LIBCURL_LIBS}
	${LIBCAIRO_LIBS}
	${LIBSDL_LIBS}
	${LIBFREETYPE_LIBS}
)

IF (WEBKIT_USE_JS)
	TARGET_LINK_LIBRARIES (testBAL
		jscore
		pthread
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
  	${LIBSDL_LIBS}
	${LIBFREETYPE_LIBS}
	${ADDONS_LINK}
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
#WRITE_FILE (DumpRenderTree "#!/bin/sh
#DISABLE_DISPLAY= LAYOUT_TEST=1 ./owb $*" )


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
