INCLUDE_DIRECTORIES (
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/ForwardingHeaders/
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/bytecode
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/interpreter
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/kjs
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/parser
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/runtime
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/WebCore/svg/
    ${CMAKE_SOURCE_DIR}/WebCore/css
    ${CMAKE_SOURCE_DIR}/WebCore/dom
    ${CMAKE_SOURCE_DIR}/WebCore/page
    ${CMAKE_SOURCE_DIR}/WebCore/page/animation
    ${CMAKE_SOURCE_DIR}/WebCore/html
    ${CMAKE_SOURCE_DIR}/WebCore/loader
    ${CMAKE_SOURCE_DIR}/WebCore/editing
    ${CMAKE_SOURCE_DIR}/WebCore/rendering
    ${CMAKE_SOURCE_DIR}/WebCore/rendering/style
    ${CMAKE_SOURCE_DIR}/WebCore/loader/icon/
    ${CMAKE_SOURCE_DIR}/WebCore/history/
    ${CMAKE_SOURCE_DIR}/WebCore/bridge
    ${CMAKE_SOURCE_DIR}/WebCore/plugins
    ${CMAKE_SOURCE_DIR}/WebCore/bindings/js
    ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api/
    ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html
)

IF (WEBKIT_USE_I18N STREQUAL "GENERIC")
    INCLUDE_DIRECTORIES (
        ${CMAKE_SOURCE_DIR}/JavaScriptCore/icu
    )
ENDIF (WEBKIT_USE_I18N STREQUAL "GENERIC")


OWBLN (WK/BCScrollbarWK.h WKAL 0 0)
OWBLN (WK/BCScrollbarClientWK.h WKAL 0 0)
OWBLN (WK/BCScrollbarThemeWK.h WKAL 0 0)
OWBLN (WK/BCScrollbarThemeCompositeWK.h WKAL 0 0)
OWBLN (WK/BCFrameWK.h WKAL 0 1)
OWBLN (WK/BCWidgetWK.h WKAL 0 0)
OWBLN (WK/BCHostWindowWK.h WKAL 0 0)
OWBLN (WK/BCPopupMenuClientWK.h WKAL 0 0)
OWBLN (WK/BCPopupMenuStyleWK.h WKAL 0 0)
OWBLN (WK/BCThemeTypesWK.h WKAL 0 0)
OWBLN (WK/BCThemeWK.h WKAL 0 0)

IF (OWBAL_PLATFORM_GRAPHICS STREQUAL "GTK")
    OWBLN (WK/BCContextMenuWK.h WKAL 0 0)

    OWBLN (Gtk/BCClipboardGtk.h WKAL 1 0)
    OWBLN (Gtk/BCContextMenuItemGtk.h WKAL 0 0)
    OWBLN (Gtk/BCCursorGtk.h WKAL 0 0)
    OWBLN (Gtk/BCDragControllerGtk.h WKAL 0 1)
    OWBLN (Gtk/BCgtkdrawingGtk.h WKAL 0 0)
    OWBLN (Gtk/BCPasteboardGtk.h WKAL 0 0)
    OWBLN (Gtk/PasteboardHelperGtk.h WKAL 1 0)
    OWBLN (Gtk/BCPasteboardHelperGtk.h WKAL 0 0)
    OWBLN (Gtk/BCPlatformMenuDescriptionGtk.h WKAL 0 0)
    OWBLN (Gtk/BCPlatformScreenGtk.h WKAL 0 0)
    OWBLN (Gtk/BCPopupMenuGtk.h WKAL 0 0)
    OWBLN (Gtk/BCRenderThemeGtk.h WKAL 1 0)
    OWBLN (Gtk/BCScrollbarGtk.h WKAL 1 0)
    OWBLN (Gtk/BCScrollbarThemeGtk.h WKAL 1 0)
    OWBLN (Gtk/BCScrollViewGtk.h WKAL 0 0)
    OWBLN (Gtk/BCSearchPopupMenuGtk.h WKAL 0 0)
    
    INCLUDE_DIRECTORIES (${GTK2_INCLUDE_DIRS})
    
    AUX_SOURCE_DIRECTORY (Gtk WIDGETS_SRC)
ELSEIF (OWBAL_PLATFORM_GRAPHICS STREQUAL "SDL")
    OWBLN (SDL/BCContextMenuSDL.h WKAL 0 0)

    OWBLN (SDL/BCClipboardSDL.h WKAL 1 0)
    OWBLN (SDL/BCContextMenuItemSDL.h WKAL 0 0)
    OWBLN (SDL/BCCursorSDL.h WKAL 0 0)
    OWBLN (SDL/BCPasteboardSDL.h WKAL 0 0)
    OWBLN (SDL/PasteboardHelperSDL.h WKAL 1 0)
    OWBLN (SDL/BCPasteboardHelperSDL.h WKAL 0 0)
    OWBLN (SDL/BCPlatformMenuDescriptionSDL.h WKAL 0 0)
    OWBLN (SDL/BCPlatformScreenSDL.h WKAL 0 0)
    OWBLN (SDL/BCPopupMenuSDL.h WKAL 0 0)
    OWBLN (SDL/BCRenderThemeSDL.h WKAL 1 0)
    OWBLN (SDL/BCScrollbarThemeSDL.h WKAL 1 0)
    OWBLN (SDL/BCScrollViewSDL.h WKAL 0 0)
    OWBLN (SDL/BCSearchPopupMenuSDL.h WKAL 0 0)

    INCLUDE_DIRECTORIES (${SDL_INCLUDE_DIRS})
    
    AUX_SOURCE_DIRECTORY (SDL WIDGETS_SRC)
ENDIF (OWBAL_PLATFORM_GRAPHICS STREQUAL "GTK")

AUX_SOURCE_DIRECTORY (WK WIDGETS_SRC)

INCLUDE (Customer/CMakeLists.txt OPTIONAL)

IF (BUILD_SHARED_LIBS)
    ADD_LIBRARY (bal-widgets ${WIDGETS_SRC})
ELSE (BUILD_SHARED_LIBS)
    ADD_LIBRARY (bal-widgets STATIC ${WIDGETS_SRC})
ENDIF (BUILD_SHARED_LIBS)

IF (OWBAL_PLATFORM_GRAPHICS STREQUAL "GTK")
    TARGET_LINK_LIBRARIES (bal-widgets ${GTK2_LIBRARIES})
ENDIF (OWBAL_PLATFORM_GRAPHICS STREQUAL "GTK")

INCLUDE (Customer/CMakeLinks.txt OPTIONAL)

INSTALL (TARGETS bal-widgets
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
ADD_DEPENDENCIES (bal-widgets generated-sources)
