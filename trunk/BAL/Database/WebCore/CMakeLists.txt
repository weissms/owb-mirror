##################################################
# Database support part.                         #
##################################################

if(ENABLE_DATABASE)
    create_include_link(${BAL_DIR}/Database/WebCore/SQLite BAL)

    aux_source_directory(${BAL_DIR}/Database/WebCore/SQLite WEBCORE_SRC)
endif(ENABLE_DATABASE)
