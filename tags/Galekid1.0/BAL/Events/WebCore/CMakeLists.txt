
##################################################
# Events support part.                           #
##################################################

if(USE_GRAPHICS_GTK)
    create_include_link(${BAL_DIR}/Events/WebCore/Gtk/BCPlatformKeyboardEventGtk.h BAL/PlatformKeyboardEvent.h)
    create_include_link(${BAL_DIR}/Events/WebCore/Gtk/BCPlatformMouseEventGtk.h BAL/PlatformMouseEvent.h)
    create_include_link(${BAL_DIR}/Events/WebCore/Gtk/BCPlatformWheelEventGtk.h BAL/PlatformWheelEvent.h)
    create_include_link(${BAL_DIR}/Events/WebCore/Gtk/BCKeyboardCodesGtk.h BAL/KeyboardCodes.h)
    create_include_link(${BAL_DIR}/Events/WebCore/Gtk/BCEventLoopGtk.h BAL/EventLoop.h)
    
    aux_source_directory(${BAL_DIR}/Events/WebCore/Gtk WEBCORE_SRC)
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_SDL)
    create_include_link(${BAL_DIR}/Events/WebCore/SDL/BCPlatformKeyboardEventSDL.h BAL/PlatformKeyboardEvent.h)
    create_include_link(${BAL_DIR}/Events/WebCore/SDL/BCPlatformMouseEventSDL.h BAL/PlatformMouseEvent.h)
    create_include_link(${BAL_DIR}/Events/WebCore/SDL/BCPlatformWheelEventSDL.h BAL/PlatformWheelEvent.h)
    create_include_link(${BAL_DIR}/Events/WebCore/SDL/BCKeyboardCodesSDL.h BAL/KeyboardCodes.h)
    create_include_link(${BAL_DIR}/Events/WebCore/SDL/BCEventLoopSDL.h BAL/EventLoop.h)

    aux_source_directory(${BAL_DIR}/Events/WebCore/SDL WEBCORE_SRC)
endif(USE_GRAPHICS_SDL)
