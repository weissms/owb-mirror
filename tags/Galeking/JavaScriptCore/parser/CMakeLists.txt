create_lexer(JSC_SRC JavaScriptCore/parser/Keywords.table  generated_sources/JavaScriptCore/Lexer.lut.h JavaScriptCore/parser/Lexer.cpp)

if(ENABLE_DEBUG)
    list(APPEND JSC_SRC
        parser/Lexer.cpp
        parser/Nodes.cpp
        parser/Parser.cpp
    )
endif(ENABLE_DEBUG)
list(APPEND JSC_SRC
    parser/ParserArena.cpp
)
