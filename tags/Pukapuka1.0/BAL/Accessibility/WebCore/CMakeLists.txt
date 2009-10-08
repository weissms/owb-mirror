##################################################
# Accessibility support part.                    #
##################################################

if(USE_GRAPHICS_GTK)
    create_include_link(${BAL_DIR}/Accessibility/WebCore/Gtk BAL)
    aux_source_directory(${BAL_DIR}/Accessibility/WebCore/Gtk WEBCORE_SRC)
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_QT)
    aux_source_directory(${BAL_DIR}/Accessibility/WebCore/Qt WEBCORE_SRC)
endif(USE_GRAPHICS_QT)

if(USE_GRAPHICS_SDL)
    aux_source_directory(${BAL_DIR}/Accessibility/WebCore/SDL WEBCORE_SRC)
endif(USE_GRAPHICS_SDL)
