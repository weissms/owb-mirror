# Find include and libraries for GSTREAMER library
# GSTREAMER_INCLUDE     Directories to include to use GSTREAMER
# GSTREAMER_INCLUDE-I   Directories to include to use GSTREAMER (with -I)
# GSTREAMER_LIBRARIES   Libraries to link against to use GSTREAMER
# GSTREAMER_FOUND       GSTREAMER was found

IF (UNIX)
    INCLUDE (UsePkgConfig)
    PKGCONFIG (gstreamer-video-0.10 GSTREAMER_include_dir GSTREAMER_link_dir GSTREAMER_libraries GSTREAMER_include)
    IF (GSTREAMER_include AND GSTREAMER_libraries)
        SET (GSTREAMER_FOUND TRUE)
        EXEC_PROGRAM ("echo"
            ARGS "${GSTREAMER_include} | sed 's/[[:blank:]]*-I/;/g'"
            OUTPUT_VARIABLE GSTREAMER_INCLUDE
        )
        SET (GSTREAMER_INCLUDE-I ${GSTREAMER_include})
        SET (GSTREAMER_LIBRARIES ${GSTREAMER_libraries})
    ELSE (GSTREAMER_include AND GSTREAMER_libraries)
        SET (GSTREAMER_FOUND FALSE)
    ENDIF (GSTREAMER_include AND GSTREAMER_libraries)
ENDIF (UNIX)
