if(ENABLE_INSPECTOR)
    list(APPEND WEBCORE_SRC
        inspector/InspectorController.cpp
        inspector/JavaScriptCallFrame.cpp
        inspector/JavaScriptDebugServer.cpp
        inspector/JavaScriptProfile.cpp
        inspector/JavaScriptProfileNode.cpp
    )
    list(APPEND IDL_SRC
        inspector/JavaScriptCallFrame.idl
    )
endif(ENABLE_INSPECTOR)
