##################################################
# Add definitions.                               #
##################################################

if(ENABLE_3D_CANVAS)
    add_definitions(-DENABLE_3D_CANVAS)
endif(ENABLE_3D_CANVAS)

if(ENABLE_3D_RENDERING)
    add_definitions(-DENABLE_3D_RENDERING)
    add_definitions(-DWTF_USE_ACCELERATED_COMPOSITING=1)
endif(ENABLE_3D_RENDERING)

if(ENABLE_DASHBOARD_SUPPORT)
    add_definitions(-DENABLE_DASHBOARD_SUPPORT=1)
endif(ENABLE_DASHBOARD_SUPPORT)

if(ENABLE_DATABASE)
    add_definitions(-DENABLE_DATABASE=1)
else(ENABLE_DATABASE)
    add_definitions(-DENABLE_DATABASE=0)
endif(ENABLE_DATABASE)

if(ENABLE_DATAGRID)
    add_definitions(-DENABLE_DATAGRID=1)
endif(ENABLE_DATAGRID)

if(ENABLE_ICONDATABASE)
    add_definitions(-DENABLE_ICONDATABASE=1)
else(ENABLE_DATABASE)
    add_definitions(-DENABLE_ICONDATABASE=0)
endif(ENABLE_ICONDATABASE)

if(ENABLE_DOM_STORAGE)
    add_definitions(-DENABLE_DOM_STORAGE=1)
endif(ENABLE_DOM_STORAGE)

if(ENABLE_OFFLINE_WEB_APPLICATIONS)
    add_definitions(-DENABLE_OFFLINE_WEB_APPLICATIONS=1)
endif(ENABLE_OFFLINE_WEB_APPLICATIONS)

if(ENABLE_OFFLINE_DYNAMIC_ENTRIES)
    add_definitions(-DAPPLICATION_CACHE_DYNAMIC_ENTRIES=1)
endif(ENABLE_OFFLINE_DYNAMIC_ENTRIES)

if(ENABLE_INSPECTOR)
    add_definitions(-DENABLE_INSPECTOR=1)
    add_definitions(-DENABLE_JAVASCRIPT_DEBUGGER=1)
else(ENABLE_INSPECTOR)
    add_definitions(-DENABLE_INSPECTOR=0)
    add_definitions(-DENABLE_JAVASCRIPT_DEBUGGER=0)
endif(ENABLE_INSPECTOR)

if(ENABLE_GEOLOCATION)
    add_definitions(-DENABLE_GEOLOCATION=1)
endif(ENABLE_GEOLOCATION)

if(ENABLE_MATHML)
    add_definitions(-DENABLE_MATHML=1)
endif(ENABLE_MATHML)

if(ENABLE_NOTIFICATIONS)
    add_definitions(-DENABLE_NOTIFICATIONS=1)
endif(ENABLE_NOTIFICATIONS)

if(ENABLE_EVENTSOURCE)
    add_definitions(-DENABLE_EVENTSOURCE=1)
else(ENABLE_EVENTSOURCE)
    add_definitions(-DENABLE_EVENTSOURCE=0)
endif(ENABLE_EVENTSOURCE)

if(ENABLE_VIDEO)
    add_definitions(-DENABLE_VIDEO=1)
endif(ENABLE_VIDEO)

if(ENABLE_WEB_SOCKETS)
    add_definitions(-DENABLE_WEB_SOCKETS=1)
else(ENABLE_WEB_SOCKETS)
    add_definitions(-DENABLE_WEB_SOCKETS=0)
endif(ENABLE_WEB_SOCKETS)

if(ENABLE_WML)
    add_definitions(-DENABLE_WML=1)
endif(ENABLE_WML)

if(ENABLE_WORKERS)
    add_definitions(-DENABLE_WORKERS=1)
    if(ENABLE_SHARED_WORKERS)
        add_definitions(-DENABLE_SHARED_WORKERS=1)
    endif(ENABLE_SHARED_WORKERS)
endif(ENABLE_WORKERS)

if(ENABLE_XHTMLMP)
    add_definitions(-DENABLE_XHTMLMP)
    if(ENABLE_WCSS)
        add_definitions(-DENABLE_WCSS)
    endif(ENABLE_WCSS)
endif(ENABLE_XHTMLMP)

if(ENABLE_XPATH)
    add_definitions(-DENABLE_XPATH=1)
endif(ENABLE_XPATH)

if(ENABLE_XSLT)
    add_definitions(-DENABLE_XSLT=1)
endif(ENABLE_XSLT)


##################################################
# Add Compiler flags.                            #
##################################################

link_directories(${LIBRARY_OUTPUT_PATH})

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
    if(NOT ENABLE_DEBUG)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    endif(NOT ENABLE_DEBUG)

    if(APPLE) 
        set(WEBKKIT_LD_FLAGS webcore;jsc;wtf)
    else(APPLE)
        set(WEBKKIT_LD_FLAGS "-Wl,-whole-archive -lwtf -ljsc -lwebcore -Wl,-no-whole-archive")
    endif(APPLE)
else(CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "Flag for Windows compiler is not implemented")
endif(CMAKE_COMPILER_IS_GNUCXX)

include(AddJSAddOnsCompilerFlags OPTIONAL)
include(AddOrigynSuiteCompilerFlags OPTIONAL)
