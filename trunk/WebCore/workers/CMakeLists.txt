if(ENABLE_WORKERS)
    list(APPEND WEBCORE_SRC
        workers/AbstractWorker.cpp
        workers/DedicatedWorkerContext.cpp
        workers/DedicatedWorkerThread.cpp
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
        workers/Worker.idl
        workers/WorkerContext.idl
        workers/WorkerLocation.idl
    )
    if(ENABLE_SHARED_WORKERS)
        list(APPEND WEBCORE_SRC
            workers/DefaultSharedWorkerRepository.cpp
            workers/SharedWorker.cpp
            workers/SharedWorkerContext.cpp
            workers/SharedWorkerThread.cpp
        )
        list(APPEND IDL_SRC
            workers/SharedWorker.idl
            workers/SharedWorkerContext.idl
        )
    endif(ENABLE_SHARED_WORKERS)
endif(ENABLE_WORKERS)
