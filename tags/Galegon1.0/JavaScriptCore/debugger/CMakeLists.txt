list(APPEND JSC_SRC
    debugger/DebuggerActivation.cpp
    debugger/DebuggerCallFrame.cpp
)

if(ENABLE_DEBUG)
    list(APPEND JSC_SRC
        debugger/Debugger.cpp
    )
endif(ENABLE_DEBUG)
