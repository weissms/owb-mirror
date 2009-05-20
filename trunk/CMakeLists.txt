##################################################
#                                                #
# Main CMakeLists for Origyn Web Project.        #
#                                                #
##################################################

project(OWB CXX C)

cmake_minimum_required(VERSION 2.6.2)

execute_process(COMMAND ./FindCMakeModulesPath.py
    WORKING_DIRECTORY ${OWB_SOURCE_DIR}
    OUTPUT_VARIABLE EXTRA_MODULE_PATH
)
set(CMAKE_MODULE_PATH ${EXTRA_MODULE_PATH})
set(EXECUTABLE_OUTPUT_PATH ${OWB_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${OWB_BINARY_DIR}/lib)

set(WEBKIT_VERSION 1.0)

##################################################
# Set options for compiler below.                #
##################################################

option(ENABLE_DEBUG "Enable debug support")
option(ENABLE_TESTS "Enable tests")

# Database related options
include(CMakeDependentOption)
option(ENABLE_3D_RENDERING "Enable 3d rendering support")
option(ENABLE_DATABASE "Enable HTML5 client-side database storage support")
cmake_dependent_option(ENABLE_DOM_STORAGE "Enable HTML5 client-side session and persistent storage support" OFF ENABLE_DATABASE OFF)
cmake_dependent_option(ENABLE_ICONDATABASE "Enable icon database support" OFF ENABLE_DATABASE OFF)
cmake_dependent_option(ENABLE_OFFLINE_WEB_APPLICATIONS "Enable HTML5 offline web applications support" OFF ENABLE_DATABASE OFF)
cmake_dependent_option(ENABLE_OFFLINE_DYNAMIC_ENTRIES "Enable HTML5 offline dynamic entries support" OFF ENABLE_OFFLINE_WEB_APPLICATIONS OFF)
option(ENABLE_GEOLOCATION "Enable geoposition support")
option(ENABLE_INSPECTOR "Enable web inspector support")
option(ENABLE_JIT_JSC "Enable JavascriptCore JIT compilation (for x86 only)")
option(ENABLE_YARR "Enable RegExp compilation (for x86 and x86_64 only)")
option(ENABLE_MULTIPLE_THREADS "Enable multiple threads")
cmake_dependent_option(ENABLE_WORKERS "Enable workers support" OFF ENABLE_MULTIPLE_THREADS OFF)
option(ENABLE_NPAPI "Enable Netscape Plugin API support")
option(ENABLE_SVG "Enable SVG support")
cmake_dependent_option(ENABLE_SVG_ANIMATION "Enable support for SVG animation (EXPERIMENTAL)" OFF ENABLE_SVG OFF)
cmake_dependent_option(ENABLE_SVG_AS_IMAGE "Enable SVG as Image support (EXPERIMENTAL)" OFF ENABLE_SVG OFF)
cmake_dependent_option(ENABLE_SVG_FILTERS "Enable support for SVG filters (EXPERIMENTAL)" OFF ENABLE_SVG OFF)
cmake_dependent_option(ENABLE_SVG_FONTS "Enable support for SVG fonts (EXPERIMENTAL)" OFF ENABLE_SVG OFF)
cmake_dependent_option(ENABLE_SVG_FOREIGN_OBJECT "Enable support for SVG foreign objects (EXPERIMENTAL)" OFF ENABLE_SVG OFF)
cmake_dependent_option(ENABLE_SVG_USE_ELEMENT "Enable support for SVG use element (EXPERIMENTAL)" OFF ENABLE_SVG OFF)
option(ENABLE_VIDEO "Enable HTML5 video support")
option(ENABLE_WML "Enable WML support")
option(ENABLE_XPATH "Enable XPath support")
option(ENABLE_XSLT "Enable XSLT support")

set(USE_FILESYSTEM_ACCESS "POSIX" CACHE STRING "Choose the filesystem access method, options are: GLIB POSIX QT")
set(USE_FONTS "FREETYPE" CACHE STRING "Choose the font engine, options are: GTK FREETYPE EMBEDDED QT")
set(USE_GRAPHICS "SDL" CACHE STRING "Choose the graphic backend, options are: GTK SDL QT")
set(USE_I18N "ICU" CACHE STRING "Choose the internationalization library, options are: ICU GENERIC QT")
set(USE_NETWORK "CURL" CACHE STRING "Choose the network backend, options are: CURL QT SOUP")
set(USE_THREADS "PTHREADS" CACHE STRING "Choose the thread backend, options are: GTHREADS PTHREADS NONE QT")
set(USE_TIMER "LINUX" CACHE STRING "Choose the timer backend, options are: GLIB LINUX QT")

set(WITH_OWB_CONFIG_DIR "$ENV{HOME}/.owb/conf/" CACHE STRING "Set configuration directory for owb")

include(AddOnsOptions OPTIONAL RESULT_VARIABLE HAS_ADDONS)
include(CustomerOptions OPTIONAL RESULT_VARIABLE HAS_CUSTOMER)
include(OrigynSuiteOptions OPTIONAL RESULT_VARIABLE HAS_ORIGYNSUITE)

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
include(CheckPlatformFileSystem)
include(CheckPlatformFonts)
include(CheckPlatformGeolocation)
include(CheckPlatformGraphics)
include(CheckPlatformImageDecoder)
include(CheckPlatformInternationalization)
include(CheckPlatformNetwork)
include(CheckPlatformThreads)
include(CheckPlatformTimer)
include(CheckPlatformVideoSupport)
include(CheckNPAPI)
include(CheckSqlite3)
include(CheckSVG)
include(CheckXSLT)

include(CheckPlatformCustomer OPTIONAL)

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
add_subdirectory(JavaScriptCore/wtf)
add_subdirectory(JavaScriptCore)
add_subdirectory(SunSpider)
add_subdirectory(WebCore)
if(HAS_CUSTOMER)
    add_subdirectory(BAL/Customer)
endif(HAS_CUSTOMER)
add_subdirectory(WebKit/OrigynWebBrowser)
if(ENABLE_TESTS)
    add_subdirectory(WebKitTools/DumpRenderTree)
endif(ENABLE_TESTS)
add_subdirectory(WebKitTools/OWBLauncher)

include(CPack)

