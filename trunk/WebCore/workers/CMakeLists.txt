if(ENABLE_WORKERS)
    list(APPEND WEBCORE_SRC
        workers/AbstractWorker.cpp
        workers/DedicatedWorkerContext.cpp
        workers/DedicatedWorkerThread.cpp
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
        workers/DedicatedWorkerContext.idl
        workers/SharedWorker.idl
        workers/Worker.idl
        workers/WorkerContext.idl
        workers/WorkerLocation.idl
    )
endif(ENABLE_WORKERS)
