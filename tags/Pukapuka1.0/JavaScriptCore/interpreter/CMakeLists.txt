list(APPEND JSC_SRC
    interpreter/CallFrame.cpp
    interpreter/Interpreter.cpp
)

if(ENABLE_DEBUG)
    list(APPEND JSC_SRC
        interpreter/RegisterFile.cpp
    )
endif(ENABLE_DEBUG)
