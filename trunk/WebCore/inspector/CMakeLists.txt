if(ENABLE_INSPECTOR)
    list(APPEND WEBCORE_SRC
        inspector/ConsoleMessage.cpp
        inspector/InjectedScript.cpp
        inspector/InjectedScriptHost.cpp
        inspector/InspectorBackend.cpp
        inspector/InspectorController.cpp
        inspector/InspectorDatabaseResource.cpp
        inspector/InspectorDOMAgent.cpp
        inspector/InspectorDOMStorageResource.cpp
        inspector/InspectorFrontend.cpp
        inspector/InspectorFrontendHost.cpp
        inspector/InspectorResource.cpp
        inspector/InspectorTimelineAgent.cpp
        inspector/JavaScriptCallFrame.cpp
        inspector/JavaScriptDebugServer.cpp
        inspector/JavaScriptProfile.cpp
        inspector/JavaScriptProfileNode.cpp
        inspector/TimelineRecordFactory.cpp
    )
    list(APPEND IDL_SRC
        inspector/InjectedScriptHost.idl
        inspector/InspectorBackend.idl
        inspector/InspectorFrontendHost.idl
        inspector/JavaScriptCallFrame.idl
    )
endif(ENABLE_INSPECTOR)
