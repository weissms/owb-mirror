if(ENABLE_WEB_SOCKETS)
    list(APPEND WEBCORE_SRC
        websockets/WebSocket.cpp
        websockets/WebSocketChannel.cpp
        websockets/WebSocketHandshake.cpp
        websockets/ThreadableWebSocketChannel.cpp
    )
    list(APPEND IDL_SRC
        websockets/WebSocket.idl
    )
    if(ENABLE_WORKERS)
        list(APPEND WEBCORE_SRC
            websockets/WorkerThreadableWebSocketChannel.cpp
        )
    endif(ENABLE_WORKERS)
endif(ENABLE_WEB_SOCKETS)
