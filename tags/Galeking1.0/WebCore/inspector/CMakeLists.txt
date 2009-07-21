if(ENABLE_INSPECTOR)
    list(APPEND WEBCORE_SRC
        inspector/ConsoleMessage.cpp
        inspector/InspectorController.cpp
        inspector/InspectorDatabaseResource.cpp
        inspector/InspectorDOMStorageResource.cpp
        inspector/InspectorFrontend.cpp
        inspector/InspectorJSONObject.cpp
        inspector/InspectorResource.cpp
        inspector/JavaScriptCallFrame.cpp
        inspector/JavaScriptDebugServer.cpp
        inspector/JavaScriptProfile.cpp
        inspector/JavaScriptProfileNode.cpp
    )
    list(APPEND IDL_SRC
        inspector/InspectorController.idl
        inspector/JavaScriptCallFrame.idl
    )
endif(ENABLE_INSPECTOR)
