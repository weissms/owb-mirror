create_lexer(JSC_SRC JavaScriptCore/parser/Keywords.table  generated_sources/JavaScriptCore/Lexer.lut.h JavaScriptCore/parser/Lexer.cpp)

list(APPEND JSC_SRC
    parser/Lexer.cpp
    parser/Nodes.cpp
    parser/Parser.cpp
    parser/ParserArena.cpp
)
