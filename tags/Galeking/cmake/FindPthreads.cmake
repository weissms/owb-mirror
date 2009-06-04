# Find include and libraries for GTK2 library
# GTK2_INCLUDE     Directories to include to use GTK2
# GTK2_INCLUDE-I   Directories to include to use GTK2 (with -I)
# GTK2_LIBRARIES   Libraries to link against to use GTK2
# GTK2_FOUND       GTK2 was found

IF (UNIX)
    INCLUDE (UsePkgConfig)
    PKGCONFIG (gtk+-2.0 Gtk2_include_dir Gtk2_link_dir Gtk2_libraries Gtk2_include)
    IF (Gtk2_include AND Gtk2_libraries)
        SET (GTK2_FOUND TRUE)
        EXEC_PROGRAM ("echo"
            ARGS "${Gtk2_include} | sed 's/[[:blank:]]*-I/;/g'"
            OUTPUT_VARIABLE GTK2_INCLUDE
        )
        SET (GTK2_INCLUDE-I ${Gtk2_include})
        SET (GTK2_LIBRARIES ${Gtk2_libraries})
    ELSE (Gtk2_include AND Gtk2_libraries)
        SET (GTK2_FOUND FALSE)
    ENDIF (Gtk2_include AND Gtk2_libraries)
ENDIF (UNIX)
