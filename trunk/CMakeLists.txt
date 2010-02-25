##################################################
#                                                #
# Main CMakeLists for Origyn Web Project.        #
#                                                #
##################################################

project(OWB CXX C)

cmake_minimum_required(VERSION 2.6.2)

if(NOT WIN32)
    execute_process(COMMAND ./FindCMakeModulesPath.py
        WORKING_DIRECTORY ${OWB_SOURCE_DIR}
        OUTPUT_VARIABLE EXTRA_MODULE_PATH
    )
ELSE (NOT WIN32)
    ## We need a pur windows path for CMAKE under windows 
    execute_process(COMMAND python ./FindCMakeModulesPathwin32.py ${CYGWIN_INSTALL_PATH}
        WORKING_DIRECTORY ${OWB_SOURCE_DIR}
        OUTPUT_VARIABLE EXTRA_MODULE_PATH
    )
ENDIF(NOT WIN32)

set(CMAKE_MODULE_PATH ${EXTRA_MODULE_PATH})
set(EXECUTABLE_OUTPUT_PATH ${OWB_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${OWB_BINARY_DIR}/lib)

set(WEBKIT_VERSION 2.0)

##################################################
# Set options for compiler below.                #
##################################################

include(CMakeDependentOption)
include(CEHTMLOptions OPTIONAL RESULT_VARIABLE HAS_CEHTML)
include(DAEOptions OPTIONAL RESULT_VARIABLE HAS_DAE)

option(ENABLE_3D_CANVAS "Enable 3D canvas support")
option(ENABLE_3D_RENDERING "Enable 3d rendering support")
option(ENABLE_ACCESSIBILITY "Enable accessibility support")
option(ENABLE_DATABASE "Enable HTML5 client-side database storage support")
cmake_dependent_option(ENABLE_DOM_STORAGE "Enable HTML5 client-side session and persistent storage support" OFF ENABLE_DATABASE OFF)
cmake_dependent_option(ENABLE_ICONDATABASE "Enable icon database support" OFF ENABLE_DATABASE OFF)
cmake_dependent_option(ENABLE_INDEXED_DATABASE "Enable indexed database support" OFF ENABLE_DATABASE OFF)
cmake_dependent_option(ENABLE_OFFLINE_WEB_APPLICATIONS "Enable HTML5 offline web applications support" OFF ENABLE_DATABASE OFF)
cmake_dependent_option(ENABLE_OFFLINE_DYNAMIC_ENTRIES "Enable HTML5 offline dynamic entries support" OFF ENABLE_OFFLINE_WEB_APPLICATIONS OFF)
option(ENABLE_DATAGRID "Enable datagrid support")
option(ENABLE_DEBUG "Enable debug support")
option(ENABLE_EVENTSOURCE "Enable HTML5 server-sent events support")
cmake_dependent_option(ENABLE_FAST_MALLOC "Enable optimized memory allocator" ON "NOT ENABLE_DEBUG" OFF)
option(ENABLE_FILTERS "Enable support for filters")
option(ENABLE_GEOLOCATION "Enable geoposition support")
option(ENABLE_INSPECTOR "Enable web inspector support")
option(ENABLE_JIT_JSC "Enable JavascriptCore JIT compilation (for x86 only)")
option(ENABLE_OWB_TRACES "Enable OWB-specific traces" ON)
option(ENABLE_MATHML "Enable MathML support")
option(ENABLE_MULTIPLE_THREADS "Enable multiple threads")
cmake_dependent_option(ENABLE_WORKERS "Enable workers support" OFF ENABLE_MULTIPLE_THREADS OFF)
cmake_dependent_option(ENABLE_SHARED_WORKERS "Enable shared workers support" OFF ENABLE_WORKERS OFF)
option(ENABLE_NOTIFICATIONS "Enable notification support")
option(ENABLE_NPAPI "Enable Netscape Plugin API support")
option(ENABLE_RUBY "Enable HTML5 ruby support")
option(ENABLE_SANDBOX "Enable HTML5 sandbox attribute for iframes")
option(ENABLE_SVG "Enable SVG support")
cmake_dependent_option(ENABLE_SVG_ANIMATION "Enable support for SVG animation (EXPERIMENTAL)" OFF ENABLE_SVG OFF)
cmake_dependent_option(ENABLE_SVG_AS_IMAGE "Enable SVG as Image support (EXPERIMENTAL)" OFF ENABLE_SVG OFF)
cmake_dependent_option(ENABLE_SVG_FONTS "Enable support for SVG fonts (EXPERIMENTAL)" OFF ENABLE_SVG OFF)
cmake_dependent_option(ENABLE_SVG_FOREIGN_OBJECT "Enable support for SVG foreign objects (EXPERIMENTAL)" OFF ENABLE_SVG OFF)
cmake_dependent_option(ENABLE_SVG_USE_ELEMENT "Enable support for SVG use element (EXPERIMENTAL)" OFF ENABLE_SVG OFF)
option(ENABLE_TESTS "Enable tests" ON)
cmake_dependent_option(ENABLE_TESTS_CPPUNIT "Enable unit tests based on cppunit framework" ON ENABLE_TESTS OFF)
option(ENABLE_TOUCH_EVENTS "Enable touch events support" OFF)
option(ENABLE_VIDEO "Enable HTML5 video support")
option(ENABLE_WEB_SOCKETS "Enable Web Sockets support")
option(ENABLE_WCSS "Enable WCSS support")
option(ENABLE_WML "Enable WML support")
option(ENABLE_XHTMLMP "Enable XHTML Mobile profile.")
option(ENABLE_XPATH "Enable XPath support")
option(ENABLE_XSLT "Enable XSLT support")
option(ENABLE_YARR "Enable RegExp compilation (for x86 and x86_64 only)")

set(USE_FILESYSTEM_ACCESS "POSIX" CACHE STRING "Choose the filesystem access method, options are: GLIB POSIX QT")
set(USE_FONTS "CAIRO" CACHE STRING "Choose the font engine, options are: GTK CAIRO FREETYPE EMBEDDED QT")
set(USE_GRAPHICS "SDLCAIRO" CACHE STRING "Choose the graphic backend, options are: GTK SDL SDLCAIRO QT")
set(USE_I18N "ICU" CACHE STRING "Choose the internationalization library, options are: ICU EMBEDDED GLIB QT")
set(USE_NETWORK "CURL" CACHE STRING "Choose the network backend, options are: CURL QT SOUP")
set(USE_THREADS "PTHREADS" CACHE STRING "Choose the thread backend, options are: PTHREADS NONE QT")
set(USE_TIMER "LINUX" CACHE STRING "Choose the timer backend, options are: GLIB LINUX QT")
if(ENABLE_CEHTML_MEDIA_OBJECTS OR ENABLE_DAE_TUNER OR ENABLE_VIDEO)
    set(USE_VIDEO "NONE" CACHE STRING "Choose the video backend, options are: AMIGAOS4 GSTREAMER NONE PHONON QT")
endif(ENABLE_CEHTML_MEDIA_OBJECTS OR ENABLE_DAE_TUNER OR ENABLE_VIDEO)


set(WITH_OWB_CONFIG_DIR "$ENV{HOME}/.owb/conf/" CACHE STRING "Set configuration directory for owb")

IF(WIN32)
    ## It seems interessant to let custombers put here the directory of his win32 library or force it to use  ${OWB_SOURCE_DIR}/winlib, I don't know
    set(WINLIB_PATH "${OWB_SOURCE_DIR}/winlib" CACHE STRING "Choose the path to you Win32 include/lib/runtime")
    SET(WINLIB_LIB_PATH ${WINLIB_PATH}/lib)
    SET(WINLIB_INC_PATH ${WINLIB_PATH}/include)
ENDIF(WIN32)

include(AddOnsOptions OPTIONAL RESULT_VARIABLE HAS_ADDONS)
if(NOT IGNORE_CUSTOMER)
    include(Customer/CustomerOptions OPTIONAL RESULT_VARIABLE HAS_CUSTOMER)
endif(NOT IGNORE_CUSTOMER)
include(OrigynSuiteOptions OPTIONAL RESULT_VARIABLE HAS_ORIGYNSUITE)
include(WidgetEngineOptions OPTIONAL RESULT_VARIABLE HAS_WIDGETENGINE)

configure_file(
    ${OWB_SOURCE_DIR}/Base/owb-config.h.in
    ${OWB_SOURCE_DIR}/Base/owb-config.h
)


##################################################
# Check required dependencies for compilation.   #
##################################################

include(CheckBaseDependencies)
include(CheckCppUnit)
include(CheckPlatformSystem)
include(CheckPlatformAccessibility)
include(CheckPlatformEvents)
include(CheckPlatformFacilities)
include(CheckPlatformFileSystem)
include(CheckPlatformFonts)
include(CheckPlatformGeolocation)
include(CheckPlatformGraphics)
include(CheckPlatformImageDecoder)
include(CheckPlatformInternationalization)
include(CheckPlatformNetwork)
include(CheckPlatformThreads)
include(CheckPlatformTimer)
include(CheckPlatformTypes)
include(CheckPlatformVideoSupport)
include(CheckPlatformWidgets)
include(CheckNPAPI)
include(CheckSqlite3)
include(CheckSVG)
include(CheckXSLT)

if(NOT IGNORE_CUSTOMER)
    include(Customer/CheckPlatformCustomer OPTIONAL)
endif(NOT IGNORE_CUSTOMER)

##################################################
# Set options for compiler below.                #
##################################################

include(AddGlobalCompilerFlags)


##################################################
# Add subdirectories for project                 #
# and set some useful vars.                      #
##################################################

include(DefineOwbMacros)
include(SetCMakeVars)

include(Base/CMakeLists.txt)
if(HAS_ADDONS)
    add_subdirectory(AddOns)
endif(HAS_ADDONS)
if(HAS_ORIGYNSUITE)
    add_subdirectory(OrigynSuite)
endif(HAS_ORIGYNSUITE)
if(HAS_WIDGETENGINE)
    add_subdirectory(WidgetEngine)
endif(HAS_WIDGETENGINE)
add_subdirectory(JavaScriptCore/wtf)
add_subdirectory(JavaScriptCore)
add_subdirectory(SunSpider)
add_subdirectory(WebCore)
add_subdirectory(WebKit/OrigynWebBrowser)
if(ENABLE_TESTS)
    add_subdirectory(WebKitTools/DumpRenderTree)
endif(ENABLE_TESTS)
add_subdirectory(WebKitTools/OWBLauncher)

include(Package)

