##################################################
# Facilities support part.                       #
##################################################

create_include_link(${BAL_DIR}/Facilities/WebCore/Common BAL)
#FIXME add implementation for GTK
create_include_link(${BAL_DIR}/Facilities/WebCore/Linux BAL)
create_include_link(${BAL_DIR}/Facilities/WebCore/WK BAL)

aux_source_directory(${BAL_DIR}/Facilities/WebCore/Common WEBCORE_SRC)
aux_source_directory(${BAL_DIR}/Facilities/WebCore/Linux WEBCORE_SRC)
aux_source_directory(${BAL_DIR}/Facilities/WebCore/WK WEBCORE_SRC)

if(USE_GRAPHICS_GTK)
    create_include_link(${BAL_DIR}/Facilities/WebCore/Gtk BAL)
    
    list(APPEND WEBCORE_SRC
        ${BAL_DIR}/Facilities/WebCore/Gtk/BCFileChooserGtk.cpp
        ${BAL_DIR}/Facilities/WebCore/Gtk/BCLanguageGtk.cpp
        ${BAL_DIR}/Facilities/WebCore/Gtk/BCLoggingGtk.cpp
        ${BAL_DIR}/Facilities/WebCore/Gtk/BCMIMETypeRegistryGtk.cpp
    )
endif(USE_GRAPHICS_GTK)


if(USE_GRAPHICS_QT)
    aux_source_directory(${BAL_DIR}/Facilities/WebCore/Qt WEBCORE_SRC)   
endif(USE_GRAPHICS_QT)

if(USE_GRAPHICS_SDL)
    create_include_link(${BAL_DIR}/Facilities/WebCore/SDL BAL)

    aux_source_directory(${BAL_DIR}/Facilities/WebCore/SDL WEBCORE_SRC)   
endif(USE_GRAPHICS_SDL)
