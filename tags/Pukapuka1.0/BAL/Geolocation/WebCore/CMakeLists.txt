##################################################
# Geolocation support part.                      #
##################################################


if(ENABLE_GEOLOCATION)
    create_include_link(${BAL_DIR}/Geolocation/WebCore/WK BAL)
    aux_source_directory(${BAL_DIR}/Geolocation/WebCore/WK WEBCORE_SRC)

    if(USE_GRAPHICS_GTK)
        create_include_link(${BAL_DIR}/Geolocation/WebCore/Gtk BAL)
        aux_source_directory(${BAL_DIR}/Geolocation/WebCore/Gtk WEBCORE_SRC)
    endif(USE_GRAPHICS_GTK)

    if(USE_GRAPHICS_SDL)
        create_include_link(${BAL_DIR}/Geolocation/WebCore/Mock BAL)
        aux_source_directory(${BAL_DIR}/Geolocation/WebCore/Mock WEBCORE_SRC)
    endif(USE_GRAPHICS_SDL)
endif(ENABLE_GEOLOCATION)
