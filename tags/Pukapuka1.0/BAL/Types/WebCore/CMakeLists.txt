##################################################
# Types support part.                            #
##################################################

set(TYPES_DIR ${BAL_DIR}/Types/WebCore)

create_include_link(${TYPES_DIR}/WK BAL)

aux_source_directory(${TYPES_DIR}/WK WEBCORE_SRC)
aux_source_directory(${TYPES_DIR}/Common WEBCORE_SRC)

if(USE_GRAPHICS_GTK)
    aux_source_directory(${TYPES_DIR}/Gtk WEBCORE_SRC)
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_QT)
    aux_source_directory(${TYPES_DIR}/Qt WEBCORE_SRC)
endif(USE_GRAPHICS_QT)

if(USE_GRAPHICS_SDL)
    create_include_link(${TYPES_DIR}/SDL BAL)
    list(APPEND WEBCORE_SRC
        ${TYPES_DIR}/SDL/BCIconSDL.cpp
        ${TYPES_DIR}/SDL/BCSharedBufferSDL.cpp
        ${TYPES_DIR}/SDL/BCKURLSDL.cpp
    )
endif(USE_GRAPHICS_SDL)
