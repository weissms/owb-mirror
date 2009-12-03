create_include_link(${BAL_DIR}/Internationalization/WTF BAL/wtf/unicode)

if(USE_I18N_EMBEDDED)
    create_include_link(${BAL_DIR}/Internationalization/WTF/Embedded BAL/wtf/unicode)

    aux_source_directory(${BAL_DIR}/Internationalization/WTF/Embedded WTF_SRC)
endif(USE_I18N_EMBEDDED)

if(USE_I18N_GLIB)
    create_include_link(${BAL_DIR}/Internationalization/WTF/GLib BAL/wtf/unicode)

    aux_source_directory(${BAL_DIR}/Internationalization/WTF/GLib WTF_SRC)
    aux_source_directory(${BAL_DIR}/Internationalization/WTF/ICU WTF_SRC)
endif(USE_I18N_GLIB)

if(USE_I18N_ICU)
    create_include_link(${BAL_DIR}/Internationalization//WTF/ICU BAL/wtf/unicode)

    aux_source_directory(${BAL_DIR}/Internationalization/WTF/ICU WTF_SRC)
endif(USE_I18N_ICU)

if(USE_I18N_QT)
    create_include_link(${BAL_DIR}/Internationalization/WTF/Qt BAL/wtf/unicode)
endif(USE_I18N_QT)

aux_source_directory(${BAL_DIR}/Internationalization/WTF WTF_SRC)
