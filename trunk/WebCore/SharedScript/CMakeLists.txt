if(ENABLE_SHARED_SCRIPT)
    list(APPEND WEBCORE_SRC
        SharedScript/SharedScriptContext.cpp
        SharedScript/WebKitSharedScript.cpp
        SharedScript/WebKitSharedScriptRepository.cpp
    )
    list(APPEND IDL_SRC
        SharedScript/SharedScriptContext.idl
        SharedScript/WebKitSharedScript.idl
    )
endif(ENABLE_SHARED_SCRIPT)
