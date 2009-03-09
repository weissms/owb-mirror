##################################################
# Internationalization support part.             #
##################################################

create_include_link(${BAL_DIR}/Internationalization/WebCore/WK BAL)

if(USE_I18N_ICU)
    create_include_link(${BAL_DIR}/Internationalization/WebCore/ICU BAL)
    
    aux_source_directory(${BAL_DIR}/Internationalization/WebCore/ICU WEBCORE_SRC)
    
    if(USE_GRAPHICS_GTK)
        create_include_link(${BAL_DIR}/Internationalization/WebCore/Gtk BAL)

        aux_source_directory(${BAL_DIR}/Internationalization/WebCore/Gtk WEBCORE_SRC)
    endif(USE_GRAPHICS_GTK)

    if(USE_GRAPHICS_SDL)
        create_include_link(${BAL_DIR}/Internationalization/WebCore/SDL BAL)

        aux_source_directory(${BAL_DIR}/Internationalization/WebCore/SDL WEBCORE_SRC)
    endif(USE_GRAPHICS_SDL)
endif(USE_I18N_ICU)

if(USE_I18N_GENERIC)
    create_include_link(${BAL_DIR}/Internationalization/WebCore/Generic BAL)

    aux_source_directory(${BAL_DIR}/Internationalization/WebCore/Generic WEBCORE_SRC)
endif(USE_I18N_GENERIC)

if(USE_I18N_QT)
    create_include_link(${BAL_DIR}/Internationalization/WebCore/Qt BAL)
    create_include_link(${BAL_DIR}/Internationalization/WebCore/Qt BAL/qt)

    aux_source_directory(${BAL_DIR}/Internationalization/WebCore/Qt WEBCORE_SRC)
endif(USE_I18N_QT)

aux_source_directory(${BAL_DIR}/Internationalization/WebCore/WK WEBCORE_SRC)
