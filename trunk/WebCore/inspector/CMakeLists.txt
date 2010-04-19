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
        inspector/InspectorFrontendClientLocal.cpp
        inspector/InspectorFrontendHost.cpp
        inspector/InspectorResource.cpp
        inspector/InspectorTimelineAgent.cpp
        inspector/TimelineRecordFactory.cpp
    )
    list(APPEND IDL_SRC
        inspector/InjectedScriptHost.idl
        inspector/InspectorBackend.idl
        inspector/InspectorFrontendHost.idl
        inspector/JavaScriptCallFrame.idl
        inspector/ScriptProfile.idl
        inspector/ScriptProfileNode.idl
    )
endif(ENABLE_INSPECTOR)
