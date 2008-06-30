IF (EXISTS ${CMAKE_BINARY_DIR}/generated_sources/kjs/chartables.c)
ELSE (EXISTS ${CMAKE_BINARY_DIR}/generated_sources/kjs/chartables.c)
    EXEC_PROGRAM (mkdir
        ARGS -p ${CMAKE_BINARY_DIR}/generated_sources/kjs
    )
    EXEC_PROGRAM (${CMAKE_SOURCE_DIR}/JavaScriptCore/pcre/dftables
        ARGS ${CMAKE_BINARY_DIR}/generated_sources/kjs/chartables.c
    )
ENDIF (EXISTS ${CMAKE_BINARY_DIR}/generated_sources/kjs/chartables.c)

LIST (APPEND JSCORE_SRC
    pcre/pcre_compile.cpp
    pcre/pcre_exec.cpp
    pcre/pcre_tables.cpp
    pcre/pcre_ucp_searchfuncs.cpp
    pcre/pcre_xclass.cpp
    #pcre/ucptable.cpp
)
