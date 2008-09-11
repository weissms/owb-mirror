# Find include and libraries for GNOMEVFS library
# GNOMEVFS_INCLUDE     Directories to include to use GNOMEVFS
# GNOMEVFS_INCLUDE-I   Directories to include to use GNOMEVFS (with -I)
# GNOMEVFS_LIBRARIES   Libraries to link against to use GNOMEVFS
# GNOMEVFS_FOUND       GNOMEVFS was found

IF (UNIX)
    INCLUDE (UsePkgConfig)
    PKGCONFIG (gnome-vfs-2.0 GNOMEVFS_include_dir GNOMEVFS_link_dir GNOMEVFS_libraries GNOMEVFS_include)
    IF (GNOMEVFS_include AND GNOMEVFS_libraries)
        SET (GNOMEVFS_FOUND TRUE)
        EXEC_PROGRAM ("echo"
            ARGS "${GNOMEVFS_include} | sed 's/[[:blank:]]*-I/;/g'"
            OUTPUT_VARIABLE GNOMEVFS_INCLUDE
        )
        SET (GNOMEVFS_INCLUDE-I ${GNOMEVFS_include})
        SET (GNOMEVFS_LIBRARIES ${GNOMEVFS_libraries})
    ELSE (GNOMEVFS_include AND GNOMEVFS_libraries)
        SET (GNOMEVFS_FOUND FALSE)
    ENDIF (GNOMEVFS_include AND GNOMEVFS_libraries)
ENDIF (UNIX)
