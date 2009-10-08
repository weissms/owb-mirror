if(USE_GRAPHICS STREQUAL "AMIGAOS4")
    set(USE_GRAPHICS_AMIGAOS4 TRUE)
    mark_as_advanced(USE_GRAPHICS_AMIGAOS4)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/amiga/ppc-amigaos/SDK/local/common/include/cairo")
endif(USE_GRAPHICS STREQUAL "AMIGAOS4")

if(USE_GRAPHICS STREQUAL "GTK")
    pkg_check_modules(CAIRO REQUIRED cairo>=1.4)
    pkg_check_modules(GTK REQUIRED gtk+-2.0>=2.8)
    pkg_check_modules(GAIL REQUIRED gail)
    set(GRAPHICS_INCLUDE_DIRS ${GTK_INCLUDE_DIRS} ${GAIL_INCLUDE_DIRS})
    set(GRAPHICS_LIBRARIES ${GTK_LDFLAGS} ${GAIL_LDFLAGS})

    set(USE_GRAPHICS_GTK TRUE)
    mark_as_advanced(USE_GRAPHICS_GTK)

    set(REQUIREMENT "gtk+-2.0 >= 2.8")
endif(USE_GRAPHICS STREQUAL "GTK")

if(USE_GRAPHICS STREQUAL "QT")
    FIND_PACKAGE(Qt4)
    pkg_check_modules(QTGRAPHIC REQUIRED QtGui)
    set(GRAPHICS_INCLUDE_DIRS ${QTGRAPHIC_INCLUDE_DIRS})
    set(GRAPHICS_LIBRARIES ${QTGRAPHIC_LDFLAGS})

    set(USE_GRAPHICS_QT TRUE)
    mark_as_advanced(USE_GRAPHICS_QT)
endif(USE_GRAPHICS STREQUAL "QT")

if(USE_GRAPHICS STREQUAL "SDL")

    IF(NOT WIN32)
        pkg_check_modules(SDL REQUIRED sdl>=1.2.10)
    ELSE(NOT WIN32)
        ## We haven't got a good pkg-config under Windows so we let cmake search libs
        find_path(SDL_INCLUDE_DIRS SDL.h ${WINLIB_INC_PATH} ${WINLIB_INC_PATH}/SDL)
        find_file(SDL_LIB SDL.lib ${WINLIB_LIB_PATH} ${WINLIB_LIB_PATH}/SDL)
        find_file(SDL_MAIN_LIB SDLmain.lib ${WINLIB_LIB_PATH} ${WINLIB_LIB_PATH}/SDL)
    ENDIF(NOT WIN32)
    
    include(FindSDL_gfx)
    set(GRAPHICS_INCLUDE_DIRS ${SDL_INCLUDE_DIRS} ${SDLGFX_INCLUDE_DIR})
    set(GRAPHICS_LIBRARIES ${SDL_LDFLAGS} ${SDLGFX_LIBRARY})

    set(USE_GRAPHICS_SDL TRUE)
    mark_as_advanced(USE_GRAPHICS_SDL)
    
    set(REQUIREMENT "sdl >= 1.2.10")
endif(USE_GRAPHICS STREQUAL "SDL")
