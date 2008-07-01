# Find include and libraries for PNG12 library
# PNG12_INCLUDE     Directories to include to use PNG12
# PNG12_INCLUDE-I   Directories to include to use PNG12 (with -I)
# PNG12_LIBRARIES   Libraries to link against to use PNG12
# PNG12_FOUND       PNG12 was found

IF (UNIX)
    INCLUDE (UsePkgConfig)
    PKGCONFIG (libpng12 Png12_include_dir Png12_link_dir Png12_libraries Png12_include)
    IF (Png12_include AND Png12_libraries)
        SET (PNG12_FOUND TRUE)
        EXEC_PROGRAM ("echo"
            ARGS "${Png12_include} | sed 's/[[:blank:]]*-I/;/g'"
            OUTPUT_VARIABLE PNG12_INCLUDE
        )
        SET (PNG12_INCLUDE-I ${Png12_include})
        SET (PNG12_LIBRARIES ${Png12_libraries})
    ELSE (Png12_include AND Png12_libraries)
        SET (PNG12_FOUND FALSE)
    ENDIF (Png12_include AND Png12_libraries)
ENDIF (UNIX)
