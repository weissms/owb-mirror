# Find include and libraries for Fontconfig library
# FONTCONFIG_LIBRARIES   Libraries to link against to use Fontconfig
# FONTCONFIG_FOUND       Fontconfig was found

IF (UNIX)
    INCLUDE (UsePkgConfig)
    PKGCONFIG (fontconfig fc_include_dir fc_link_dir fc_libraries fc_include)
    IF (fc_libraries)
        SET (FONTCONFIG_FOUND TRUE)
        SET (FONTCONFIG_LIBRARIES ${fc_libraries})
    ELSE (fc_libraries)
        SET (FONTCONFIG_FOUND FALSE)
    ENDIF (fc_libraries)
ENDIF (UNIX)
