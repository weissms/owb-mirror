##################################################
# Graphics support part.                         #
##################################################

set(GRAPHICS_DIR ${BAL_DIR}/Graphics/WebCore)

create_include_link(${GRAPHICS_DIR}/WK BAL)
aux_source_directory(${GRAPHICS_DIR}/WK WEBCORE_SRC)

if(USE_GRAPHICS_AMIGAOS4)
    create_include_link(${GRAPHICS_DIR}/Cairo BAL)

    aux_source_directory(${GRAPHICS_DIR}/AmigaOS4 WEBCORE_SRC)
    aux_source_directory(${GRAPHICS_DIR}/Cairo WEBCORE_SRC)
endif(USE_GRAPHICS_AMIGAOS4)

if(USE_GRAPHICS_GTK)
    create_include_link(${GRAPHICS_DIR}/Cairo BAL)
    
    aux_source_directory(${GRAPHICS_DIR}/Gtk WEBCORE_SRC)
    aux_source_directory(${GRAPHICS_DIR}/Cairo WEBCORE_SRC)
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_QT)
    create_include_link(${GRAPHICS_DIR}/Qt BAL)
    aux_source_directory(${GRAPHICS_DIR}/Qt WEBCORE_SRC)
endif(USE_GRAPHICS_QT)

if(USE_GRAPHICS_SDL)
    create_include_link(${GRAPHICS_DIR}/SDL BAL)

    aux_source_directory(${GRAPHICS_DIR}/SDL WEBCORE_SRC)
endif(USE_GRAPHICS_SDL)

if(USE_GRAPHICS_CAIRO)
    create_include_link(${GRAPHICS_DIR}/Cairo BAL)
    aux_source_directory(${GRAPHICS_DIR}/Cairo WEBCORE_SRC)
endif(USE_GRAPHICS_CAIRO)

if(USE_GRAPHICS_SDLCAIRO)
    create_include_link(${GRAPHICS_DIR}/SDL BAL)

    list(APPEND WEBCORE_SRC
        ${GRAPHICS_DIR}/SDL/BCIntPointSDL.cpp
        ${GRAPHICS_DIR}/SDL/BCIntRectSDL.cpp
    )
endif(USE_GRAPHICS_SDLCAIRO)

if(USE_GRAPHICS_GENERIC)
    create_include_link(${GRAPHICS_DIR}/Generic BAL)
    aux_source_directory(${GRAPHICS_DIR}/Generic WEBCORE_SRC)
endif(USE_GRAPHICS_GENERIC)


if(ENABLE_FILTERS)
    create_include_link(${GRAPHICS_DIR}/WK/filters BAL)
    aux_source_directory(${GRAPHICS_DIR}/WK/filters WEBCORE_SRC)
endif(ENABLE_FILTERS)

if(ENABLE_3D_CANVAS)
    aux_source_directory(${GRAPHICS_DIR}/Empty WEBCORE_SRC)
endif(ENABLE_3D_CANVAS)
