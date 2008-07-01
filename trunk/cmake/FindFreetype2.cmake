# Find include and libraries for Freetype2 library
# FREETYPE2_INCLUDE     Directories to include to use Freetype2
# FREETYPE2_INCLUDE-I   Directories to include to use Freetype2 (with -I)
# FREETYPE2_LIBRARIES   Libraries to link against to use Freetype2
# FREETYPE2_FOUND       Freetype2 was found

IF (UNIX)
    INCLUDE (UsePkgConfig)
    PKGCONFIG (freetype2 ft2_include_dir ft2_link_dir ft2_libraries ft2_include)
    IF (ft2_libraries AND ft2_include)
        SET (FREETYPE2_FOUND TRUE)
        EXEC_PROGRAM ("echo"
            ARGS "${ft2_include} | sed 's/[[:blank:]]*-I/;/g'"
            OUTPUT_VARIABLE FREETYPE2_INCLUDE
        )
        SET (FREETYPE2_INCLUDE-I ${ft2_include})
        SET (FREETYPE2_LIBRARIES ${ft2_libraries})
    ELSE (ft2_libraries AND ft2_include)
        SET (FREETYPE2_FOUND FALSE)
    ENDIF (ft2_libraries AND ft2_include)
ENDIF (UNIX)
