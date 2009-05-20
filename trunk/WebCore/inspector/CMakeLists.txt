if(ENABLE_INSPECTOR)
    list(APPEND WEBCORE_SRC
        inspector/InspectorController.cpp
        inspector/InspectorDatabaseResource.cpp
        inspector/InspectorDOMStorageResource.cpp
        inspector/InspectorFrontend.cpp
        inspector/InspectorResource.cpp
        inspector/ConsoleMessage.cpp
        inspector/JavaScriptCallFrame.cpp
        inspector/JavaScriptDebugServer.cpp
        inspector/JavaScriptProfile.cpp
        inspector/JavaScriptProfileNode.cpp
        inspector/JSONObject.cpp
        )
    list(APPEND IDL_SRC
        inspector/InspectorController.idl
        inspector/JavaScriptCallFrame.idl
    )
endif(ENABLE_INSPECTOR)
