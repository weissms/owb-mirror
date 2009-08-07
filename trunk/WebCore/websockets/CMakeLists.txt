if(ENABLE_WEB_SOCKETS)
    list(APPEND WEBCORE_SRC
        websockets/WebSocket.cpp
    )
    list(APPEND IDL_SRC
        websockets/WebSocket.idl
    )
endif(ENABLE_WEB_SOCKETS)
