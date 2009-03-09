##################################################
# Geolocation support part.                      #
##################################################

create_include_link(${BAL_DIR}/Geolocation/WebCore/WK BAL)
aux_source_directory(${BAL_DIR}/Geolocation/WebCore/WK WEBCORE_SRC)

if(ENABLE_GEOLOCATION)
    if(USE_GRAPHICS_GTK)
        create_include_link(${BAL_DIR}/Geolocation/WebCore/Gtk BAL)
        aux_source_directory(${BAL_DIR}/Geolocation/WebCore/Gtk WEBCORE_SRC)
    endif(USE_GRAPHICS_GTK)

    if(USE_GRAPHICS_SDL)
        create_include_link(${BAL_DIR}/Geolocation/WebCore/SDL BAL)
        aux_source_directory(${BAL_DIR}/Geolocation/WebCore/SDL WEBCORE_SRC)
    endif(USE_GRAPHICS_SDL)
endif(ENABLE_GEOLOCATION)
