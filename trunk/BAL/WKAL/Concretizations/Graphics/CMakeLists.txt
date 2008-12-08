IF (EXISTS ${CMAKE_BINARY_DIR}/generated_sources/WebCore/ColorData.c)
ELSE (EXISTS ${CMAKE_BINARY_DIR}/generated_sources/WebCore/ColorData.c)
    EXEC_PROGRAM (mkdir
        ARGS -p ${CMAKE_BINARY_DIR}/generated_sources/WebCore
    )
    EXEC_PROGRAM (echo
        ARGS '\#include <string.h>' > generated_sources/WebCore/ColorData.c
    )
    EXEC_PROGRAM (${GPERF_EXECUTABLE}
        ARGS -CDEot -L ANSI-C -k \"*\" -N findColor -D -s 2 < ${CMAKE_SOURCE_DIR}/WebCore/platform/ColorData.gperf >> ${CMAKE_BINARY_DIR}/generated_sources/WebCore/ColorData.c
    )
ENDIF (EXISTS ${CMAKE_BINARY_DIR}/generated_sources/WebCore/ColorData.c)

INCLUDE_DIRECTORIES (
    ${CMAKE_SOURCE_DIR}/WebCore/svg/
    ${CMAKE_SOURCE_DIR}/WebCore/bindings/js/
    ${CMAKE_SOURCE_DIR}/WebCore/css
    ${CMAKE_SOURCE_DIR}/WebCore/dom
    ${CMAKE_SOURCE_DIR}/WebCore/history
    ${CMAKE_SOURCE_DIR}/WebCore/page
    ${CMAKE_SOURCE_DIR}/WebCore/html
    ${CMAKE_SOURCE_DIR}/WebCore/inspector
    ${CMAKE_SOURCE_DIR}/WebCore/loader
    ${CMAKE_SOURCE_DIR}/WebCore/loader/icon
    ${CMAKE_SOURCE_DIR}/WebCore/editing
    ${CMAKE_SOURCE_DIR}/WebCore/rendering
    ${CMAKE_SOURCE_DIR}/WebCore/rendering/style
    ${CMAKE_SOURCE_DIR}/WebCore/svg/graphics/filters
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/svg
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/jit
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/kjs
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/runtime
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/ForwardingHeaders
)

IF (WEBKIT_USE_I18N STREQUAL "GENERIC")
    INCLUDE_DIRECTORIES (
        ${CMAKE_SOURCE_DIR}/JavaScriptCore/icu
    )
ENDIF (WEBKIT_USE_I18N STREQUAL "GENERIC")


FILE (GLOB GRAPHICS_SYMLINK RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} WK/BC*.h)
FOREACH (FILES ${GRAPHICS_SYMLINK})
    OWBLN (${FILES} WKAL 0 0)
ENDFOREACH (FILES)

IF (OWBAL_PLATFORM_GRAPHICS STREQUAL "GTK")
    OWBLN (Cairo/BCCairoPathCairo.h WKAL 0 0)
    OWBLN (Cairo/BCGraphicsContextPlatformPrivateCairo.h WKAL 1 0)
    OWBLN (Cairo/BCImageBufferCairo.h WKAL 0 0)
    OWBLN (Cairo/BCImageBufferDataCairo.h WKAL 0 0)
    OWBLN (Cairo/BCImageSourceCairo.h WKAL 0 0)
    OWBLN (Cairo/BCRenderPathCairo.h WKAL 0 1)
    OWBLN (Gtk/BCIntPointGtk.h WKAL 0 0)
    
    INCLUDE_DIRECTORIES (${GTK2_INCLUDE_DIRS})
    
    AUX_SOURCE_DIRECTORY (Gtk GRAPHICS_SRC)
    LIST (APPEND GRAPHICS_SRC
        Cairo/BCAffineTransformCairo.cpp
        Cairo/BCGradientCairo.cpp
        Cairo/BCGraphicsContextCairo.cpp
        Cairo/BCImageBufferCairo.cpp
        Cairo/BCImageCairo.cpp
        Cairo/BCImageSourceCairo.cpp
        Cairo/BCPathCairo.cpp
        Cairo/BCPatternCairo.cpp
    )
    IF (WEBKIT_USE_SVG)
        LIST (APPEND GRAPHICS_SRC
            Cairo/BCRenderPathCairo.cpp
            Cairo/BCSVGPaintServerCairo.cpp
            Cairo/BCSVGPaintServerPatternCairo.cpp
            Cairo/BCSVGResourceFilterCairo.cpp
            Cairo/BCSVGResourceMaskerCairo.cpp
        )
    ENDIF (WEBKIT_USE_SVG)
ELSEIF (OWBAL_PLATFORM_GRAPHICS STREQUAL "SDL")
    OWBLN (SDL/BCBitmapImageSDL.h WKAL 0 0)
    OWBLN (SDL/BCGradientSDL.h WKAL 0 0)
    OWBLN (SDL/BCGraphicsContextSDL.h WKAL 0 0)
    OWBLN (SDL/BCIntPointSDL.h WKAL 0 0)
    OWBLN (SDL/BCIntRectSDL.h WKAL 0 0)
    OWBLN (SDL/BCPathSDL.h WKAL 0 0)
    OWBLN (SDL/BCGraphicsContextPlatformPrivateSDL.h WKAL 1 0)
    OWBLN (SDL/BCImageBufferSDL.h WKAL 0 0)
    OWBLN (SDL/BCImageSourceSDL.h WKAL 0 0)
    OWBLN (SDL/BCDragImageSDL.h WKAL 0 0)
    OWBLN (SDL/BCIntPointSDL.h WKAL 0 0)
    OWBLN (SDL/BCAffineTransformSDL.h WKAL 0 0)
    OWBLN (SDL/BCColorSDL.h WKAL 0 0)
    OWBLN (SDL/BCSVGPaintServerPatternSDL.h WKAL 0 0)
    OWBLN (SDL/BCSVGPaintServerSolidSDL.h WKAL 0 0)
    OWBLN (SDL/BCGraphicsContextPrivateSDL.h WKAL 0 0)
    OWBLN (SDL/BCPatternSDL.h WKAL 0 0)

    INCLUDE_DIRECTORIES (
        ${CMAKE_SOURCE_DIR}/BAL/Includes/
        ${SDL_INCLUDE_DIRS}
    )
    
    LIST (APPEND GRAPHICS_SRC
        SDL/BCAffineTransformSDL.cpp
        SDL/BCGradientSDL.cpp
        SDL/BCGraphicsContextSDL.cpp
        SDL/BCImageBufferSDL.cpp
        SDL/BCImageSDL.cpp
        SDL/BCImageSourceSDL.cpp
        SDL/BCPathSDL.cpp
        SDL/BCColorSDL.cpp
        SDL/BCDragDataSDL.cpp
        SDL/BCDragImageSDL.cpp
        SDL/BCIntPointSDL.cpp
        SDL/BCIntRectSDL.cpp
        SDL/BCPatternSDL.cpp
    )
    IF (WEBKIT_USE_SVG)
        LIST (APPEND GRAPHICS_SRC
            SDL/BCRenderPathSDL.cpp
            SDL/BCSVGPaintServerSDL.cpp
            SDL/BCSVGPaintServerPatternSDL.cpp
            SDL/BCSVGResourceFilterSDL.cpp
            SDL/BCSVGResourceMaskerSDL.cpp
        )
    ENDIF (WEBKIT_USE_SVG)
ENDIF (OWBAL_PLATFORM_GRAPHICS STREQUAL "GTK")

AUX_SOURCE_DIRECTORY (WK GRAPHICS_SRC)

INCLUDE (Customer/CMakeLists.txt OPTIONAL)

IF (BUILD_SHARED_LIBS)
    ADD_LIBRARY (bal-graphics ${GRAPHICS_SRC})
ELSE (BUILD_SHARED_LIBS)
    ADD_LIBRARY (bal-graphics STATIC ${GRAPHICS_SRC})
ENDIF (BUILD_SHARED_LIBS)

IF (OWBAL_PLATFORM_GRAPHICS STREQUAL "GTK")
    TARGET_LINK_LIBRARIES (bal-graphics ${GTK2_LIBRARIES})
ELSEIF (OWBAL_PLATFORM_GRAPHICS STREQUAL "SDL")
    TARGET_LINK_LIBRARIES (bal-graphics ${SDL_LIBRARIES} -lSDL_gfx)
ENDIF (OWBAL_PLATFORM_GRAPHICS STREQUAL "GTK")

INCLUDE (Customer/CMakeLinks.txt OPTIONAL)

INSTALL (TARGETS bal-graphics
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
ADD_DEPENDENCIES (bal-graphics generated-sources)
