create_include_link(${BAL_DIR}/Internationalization/WTF/BCUTF8WTF.h BAL/wtf/unicode/UTF8.h)

if(USE_I18N_ICU)
    create_include_link(${BAL_DIR}/Internationalization/WTF/ICU/BCCollatorICU.h BAL/wtf/unicode/Collator.h)
    create_include_link(${BAL_DIR}/Internationalization//WTF/ICU/BCUnicodeICU.h BAL/wtf/unicode/Unicode.h)

    aux_source_directory(${BAL_DIR}/Internationalization/WTF/ICU WTF_SRC)
endif(USE_I18N_ICU)

if(USE_I18N_GENERIC)
    create_include_link(${BAL_DIR}/Internationalization/WTF/Generic/BCCollatorGeneric.h BAL/wtf/unicode/Collator.h)
    create_include_link(${BAL_DIR}/Internationalization/WTF/Generic/BCUnicodeGeneric.h BAL/wtf/unicode/Unicode.h)

    aux_source_directory(${BAL_DIR}/Internationalization/WTF/Generic WTF_SRC)
endif(USE_I18N_GENERIC)

aux_source_directory(${BAL_DIR}/Internationalization/WTF WTF_SRC)
