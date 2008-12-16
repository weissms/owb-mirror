if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_C_FLAGS "-Wall -W -Wcast-align -Wchar-subscripts -Wreturn-type -Wformat -Wformat-security -Wno-format-y2k -Wundef -Wmissing-format-attribute -Wpointer-arith -Wwrite-strings -Wno-unused-parameter -Wno-parentheses -fPIC")
    set(CMAKE_CXX_FLAGS "-Wall -W -Wcast-align -Wchar-subscripts -Wreturn-type -Wformat -Wformat-security -Wno-format-y2k -Wundef -Wmissing-format-attribute -Wpointer-arith -Wwrite-strings -Wno-unused-parameter -Wno-parentheses -fvisibility-inlines-hidden -fPIC")
    if(ENABLE_DEBUG)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    else(ENABLE_DEBUG)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-exceptions -fvisibility=hidden")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fvisibility=hidden -fno-rtti")
    endif(ENABLE_DEBUG)
else(CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "Flag for Windows compiler is not implemented")
endif(CMAKE_COMPILER_IS_GNUCXX)

add_definitions(-DOWBAL=WebCore -DWKAL=WebCore)
add_definitions(-DWTF_CHANGES)
add_definitions(-DWTF_USE_ICU_UNICODE=1)
add_definitions(-DWTF_USE_CURL=1)

#FIXME
add_definitions(-DENABLE_NETSCAPE_PLUGIN_API=1)

if(ENABLE_DEBUG)
    add_definitions(-DUSE_SYSTEM_MALLOC)
else (ENABLE_DEBUG)
    add_definitions(-DNDEBUG)
endif(ENABLE_DEBUG)

if(ENABLE_JIT)
    add_definitions(-DENABLE_JIT)
    add_definitions(-DENABLE_WREC)
    add_definitions(-DENABLE_JIT_OPTIMIZE_CALL)
    add_definitions(-DENABLE_JIT_OPTIMIZE_PROPERTY_ACCESS)
    add_definitions(-DENABLE_JIT_OPTIMIZE_ARITHMETIC)
endif(ENABLE_JIT)

if(ENABLE_MULTIPLE_THREADS)
    add_definitions(-DENABLE_JSC_MULTIPLE_THREADS=1)
    add_definitions(-DHAVE_POSIX_MEMALIGN=1)
endif(ENABLE_MULTIPLE_THREADS)

if(ENABLE_NPAPI)
    add_definitions(-DXP_UNIX)
endif(ENABLE_NPAPI)

if(ENABLE_SVG)
    add_definitions(-DENABLE_SVG=1)
endif(ENABLE_SVG)

if(ENABLE_SVG_ANIMATION)
    add_definitions(-DENABLE_SVG_ANIMATION=1)
endif(ENABLE_SVG_ANIMATION)

if(ENABLE_SVG_AS_IMAGE)
    add_definitions(-DENABLE_SVG_AS_IMAGE=1)
endif(ENABLE_SVG_AS_IMAGE)

if(ENABLE_SVG_FILTERS)
    add_definitions(-DENABLE_SVG_FILTERS=1)
endif(ENABLE_SVG_FILTERS)

if(ENABLE_SVG_FONTS)
    add_definitions(-DENABLE_SVG_FONTS=1)
endif(ENABLE_SVG_FONTS)

if(ENABLE_SVG_FOREIGN_OBJECT)
    add_definitions(-DENABLE_SVG_FOREIGN_OBJECT=1)
endif(ENABLE_SVG_FOREIGN_OBJECT)

if(ENABLE_SVG_USE_ELEMENT)
    add_definitions(-DENABLE_SVG_USE=1)
endif(ENABLE_SVG_USE_ELEMENT)

if(NOT USE_FONTS_EMBEDDED)
    add_definitions(-DUSE_FREETYPE=1)
endif(NOT USE_FONTS_EMBEDDED)

if(USE_GRAPHICS_GTK)
    add_definitions(-DBUILDING_GTK__=1)
    add_definitions(-DBUILDING_CAIRO__=1)
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_SDL)
    add_definitions(-DWTF_PLATFORM_SDL=1)
    add_definitions(-DWTF_PLATFORM_BAL=1)
endif(USE_GRAPHICS_SDL)

if(USE_I18N_GENERIC)
    add_definitions(-DWTF_USE_BALI18N=1)
endif(USE_I18N_GENERIC)

if(USE_THREADS_PTHREADS)
    add_definitions(-DWTF_USE_PTHREADS=1)
endif(USE_THREADS_PTHREADS)

include(AddCustomerCompilerFlags OPTIONAL)
