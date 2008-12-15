LIST (APPEND JSCORE_SRC
    jit/JIT.cpp
    jit/JITArithmetic.cpp
    jit/JITCall.cpp
    jit/JITPropertyAccess.cpp
    jit/ExecutableAllocator.cpp
)

IF (WEBKIT_USE_FILESYSTEM STREQUAL "POSIX")
LIST (APPEND JSCORE_SRC
    jit/ExecutableAllocatorPosix.cpp
)
ENDIF (WEBKIT_USE_FILESYSTEM STREQUAL "POSIX")
