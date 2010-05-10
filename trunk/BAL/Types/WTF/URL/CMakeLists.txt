list(APPEND WTFURL_SRC
    wtf/url/src/URLSegments.cpp
)

set_source_files_properties(${WTFURL_SRC}
    PROPERTIES COMPILE_FLAGS "-fno-rtti -fno-exceptions"
)
add_library(wtfurl STATIC ${WTFURL_SRC})
