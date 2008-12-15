# Find include and libraries for CAIRO library
# CAIRO_INCLUDE     Directories to include to use CAIRO
# CAIRO_INCLUDE-I   Directories to include to use CAIRO (with -I)
# CAIRO_LIBRARIES   Libraries to link against to use CAIRO
# CAIRO_FOUND       CAIRO was found

IF (UNIX)
    INCLUDE (UsePkgConfig)
    PKGCONFIG (cairo CAIRO_include_dir CAIRO_link_dir CAIRO_libraries CAIRO_include)
    IF (CAIRO_include AND CAIRO_libraries)
        SET (CAIRO_FOUND TRUE)
        EXEC_PROGRAM ("echo"
            ARGS "${CAIRO_include} | sed 's/[[:blank:]]*-I/;/g'"
            OUTPUT_VARIABLE CAIRO_INCLUDE
        )
        SET (CAIRO_INCLUDE-I ${CAIRO_include})
        SET (CAIRO_LIBRARIES ${CAIRO_libraries})
    ELSE (CAIRO_include AND CAIRO_libraries)
        SET (CAIRO_FOUND FALSE)
    ENDIF (CAIRO_include AND CAIRO_libraries)
ENDIF (UNIX)
