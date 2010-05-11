create_include_link(${BAL_DIR}/Types/WTF/URL BAL/wtf)
aux_source_directory(${BAL_DIR}/Types/WTF/URL WTFURL_SRC)

set_source_files_properties(${WTFURL_SRC}
    PROPERTIES COMPILE_FLAGS "-fno-rtti -fno-exceptions"
)
add_library(wtfurl STATIC ${WTFURL_SRC})
