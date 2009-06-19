if(ENABLE_WORKERS)
    list(APPEND WEBCORE_SRC
        workers/Worker.cpp
        workers/WorkerContext.cpp
        workers/WorkerLocation.cpp
        workers/WorkerMessagingProxy.cpp
        workers/WorkerRunLoop.cpp
        workers/WorkerScriptLoader.cpp
        workers/WorkerThread.cpp
    )
    list(APPEND IDL_SRC
        workers/Worker.idl
        workers/WorkerContext.idl
        workers/WorkerLocation.idl
    )
endif(ENABLE_WORKERS)
