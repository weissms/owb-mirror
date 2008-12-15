INCLUDE_DIRECTORIES (
    ${CMAKE_SOURCE_DIR}/JavaScriptCore
    ${CMAKE_BINARY_DIR}/generated_link/WKAL
)

ADD_SUBDIRECTORY (Concretizations/Events)
ADD_SUBDIRECTORY (Concretizations/Fonts)
ADD_SUBDIRECTORY (Concretizations/Graphics)
ADD_SUBDIRECTORY (Concretizations/Network)
ADD_SUBDIRECTORY (Concretizations/Widgets)

IF (COMPILE_TESTS)
    ADD_SUBDIRECTORY (Tests)
ENDIF (COMPILE_TESTS)
