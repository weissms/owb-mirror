
##################################################
# Events support part.                           #
##################################################

create_include_link(${BAL_DIR}/Events/WebCore/WK/BCEventLoopWK.h BAL/EventLoop.h)
create_include_link(${BAL_DIR}/Events/WebCore/WK/BCPlatformKeyboardEventWK.h BAL/PlatformKeyboardEvent.h)
create_include_link(${BAL_DIR}/Events/WebCore/WK/BCPlatformMouseEventWK.h BAL/PlatformMouseEvent.h)
create_include_link(${BAL_DIR}/Events/WebCore/WK/BCPlatformWheelEventWK.h BAL/PlatformWheelEvent.h)

if(USE_GRAPHICS_GTK)
    create_include_link(${BAL_DIR}/Events/WebCore/Gtk/BCKeyboardCodesGtk.h BAL/KeyboardCodes.h)
    
    aux_source_directory(${BAL_DIR}/Events/WebCore/Gtk WEBCORE_SRC)
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_QT)
    create_include_link(${BAL_DIR}/Events/WebCore/Qt/BCKeyboardCodesQt.h BAL/KeyboardCodes.h)
    create_include_link(${BAL_DIR}/Events/WebCore/Qt/BCMenuEventProxyQt.h BAL/MenuEventProxy.h)

    aux_source_directory(${BAL_DIR}/Events/WebCore/Qt WEBCORE_SRC)
endif(USE_GRAPHICS_QT)

if(USE_GRAPHICS_SDL)
    create_include_link(${BAL_DIR}/Events/WebCore/SDL/BCPlatformKeyboardEventSDL.h BAL/PlatformKeyboardEvent.h)
    create_include_link(${BAL_DIR}/Events/WebCore/SDL/BCPlatformMouseEventSDL.h BAL/PlatformMouseEvent.h)
    create_include_link(${BAL_DIR}/Events/WebCore/SDL/BCPlatformWheelEventSDL.h BAL/PlatformWheelEvent.h)
    create_include_link(${BAL_DIR}/Events/WebCore/SDL/BCKeyboardCodesSDL.h BAL/KeyboardCodes.h)
    create_include_link(${BAL_DIR}/Events/WebCore/SDL/BCEventLoopSDL.h BAL/EventLoop.h)

    aux_source_directory(${BAL_DIR}/Events/WebCore/SDL WEBCORE_SRC)
endif(USE_GRAPHICS_SDL)
