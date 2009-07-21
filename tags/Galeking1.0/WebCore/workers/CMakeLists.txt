if(ENABLE_WORKERS)
    list(APPEND WEBCORE_SRC
        workers/AbstractWorker.cpp
        workers/SharedWorker.cpp
        workers/Worker.cpp
        workers/WorkerContext.cpp
        workers/WorkerLocation.cpp
        workers/WorkerMessagingProxy.cpp
        workers/WorkerRunLoop.cpp
        workers/WorkerScriptLoader.cpp
        workers/WorkerThread.cpp
    )
    list(APPEND IDL_SRC
        workers/AbstractWorker.idl
        workers/SharedWorker.idl
        workers/Worker.idl
        workers/WorkerContext.idl
        workers/WorkerLocation.idl
    )
endif(ENABLE_WORKERS)
