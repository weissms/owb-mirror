LIST (APPEND JSCORE_SRC
    API/JSBase.cpp
    API/JSCallbackConstructor.cpp
    API/JSCallbackFunction.cpp
    API/JSCallbackObject.cpp
    API/JSClassRef.cpp
    API/JSContextRef.cpp
    API/JSObjectRef.cpp
    API/OpaqueJSString.cpp
    API/JSStringRef.cpp
    API/JSValueRef.cpp
)

LIST (APPEND MINIDOM_SRC
    API/tests/JSNode.c
    API/tests/JSNodeList.c
    API/tests/Node.c
    API/tests/NodeList.c
    API/tests/minidom.c
)

ADD_EXECUTABLE (minidom
    ${MINIDOM_SRC}
)

TARGET_LINK_LIBRARIES (minidom
    generated-sources
    webkit
    webcore-owb
    bal-events
    bal-facilities
    bal-fonts
    bal-graphics
    bal-imagedecoder
    bal-internationalization
    bal-media
    bal-memory
    bal-network
    bal-types
    bal-widgets
    ${XML2_LIBRARIES}
    ${XSLT_LIBRARIES}
    ${DEEPSEE_LINK}
    ${EXTRA_LDFLAGS}
)

IF (WEBKIT_USE_DATABASE)
    TARGET_LINK_LIBRARIES (minidom
        bal-database
    )
ENDIF (WEBKIT_USE_DATABASE)

TARGET_LINK_LIBRARIES (minidom
    jscore
)

IF (WEBKIT_USE_HTML_EXTENSION)
    TARGET_LINK_LIBRARIES (minidom
        htmlext
    )
ENDIF (WEBKIT_USE_HTML_EXTENSION)

