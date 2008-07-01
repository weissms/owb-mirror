ADD_SUBDIRECTORY (Concretizations/Database)
ADD_SUBDIRECTORY (Concretizations/Facilities)
ADD_SUBDIRECTORY (Concretizations/ImageDecoder)
ADD_SUBDIRECTORY (Concretizations/Internationalization)
ADD_SUBDIRECTORY (Concretizations/Media)
ADD_SUBDIRECTORY (Concretizations/Memory)
ADD_SUBDIRECTORY (Concretizations/Types)

IF (COMPILE_TESTS)
    ADD_SUBDIRECTORY (Tests)
ENDIF (COMPILE_TESTS)

