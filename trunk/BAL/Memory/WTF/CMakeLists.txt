create_include_link(${BAL_DIR}/Memory/WTF BAL/wtf)

if(ENABLE_DEBUG)
    aux_source_directory(${BAL_DIR}/Memory/WTF WTF_SRC)
endif(ENABLE_DEBUG)
