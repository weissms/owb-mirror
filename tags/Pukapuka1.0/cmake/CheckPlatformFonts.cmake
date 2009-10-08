if(USE_FONTS STREQUAL "AMIGAOS4")
    set(FONTS_INCLUDE_DIRS /usr/local/amiga/ppc-amigaos/SDK/local/newlib/include/fontconfig /usr/local/amiga/ppc-amigaos/SDK/local/newlib/include/freetype)
    set(FONTS_LIBRARIES /usr/local/amiga/ppc-amigaos/SDK/local/newlib/lib/libfontconfig.so /usr/local/amiga/ppc-amigaos/SDK/local/newlib/lib/libfreetype.so)

    set(USE_FONTS_AMIGAOS4 TRUE)
    mark_as_advanced(USE_FONTS_AMIGAOS4)
endif(USE_FONTS STREQUAL "AMIGAOS4")

if(USE_FONTS STREQUAL "EMBEDDED")
    set(USE_FONTS_EMBEDDED TRUE)
    mark_as_advanced(USE_FONTS_EMBEDDED)
endif(USE_FONTS STREQUAL "EMBEDDED")

if(USE_FONTS STREQUAL "FREETYPE")
    IF(NOT WIN32)
        pkg_check_modules(FONTCONFIG REQUIRED fontconfig>=2.4)
        pkg_check_modules(FREETYPE REQUIRED freetype2>=9.0)
    ELSE(NOT WIN32)
        ## We haven't got a good pkg-config under Windows so we let cmake search libs
        find_path(FONTCONFIG_INCLUDE_DIRS fontconfig.h ${WINLIB_INC_PATH} ${WINLIB_INC_PATH}/fontconfig)
        find_path(FREETYPE_INCLUDE_DIRS freetype.h ${WINLIB_INC_PATH} ${WINLIB_INC_PATH}/freetype ${WINLIB_INC_PATH}/freetype2 ${WINLIB_INC_PATH}/freetype2/freetype)    
        find_file(FONTCONFIG_LIBRARIES fontconfig.lib ${WINLIB_LIB_PATH} ${WINLIB_LIB_PATH}/fontconfig)
        find_file(FREETYPE_LIBRARIES freetype.lib ${WINLIB_LIB_PATH} ${WINLIB_LIB_PATH}/freetype)
    ENDIF(NOT WIN32)
    
    set(FONTS_INCLUDE_DIRS ${FONTCONFIG_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})
    set(FONTS_LIBRARIES ${FONTCONFIG_LIBRARIES} ${FREETYPE_LIBRARIES})

    set(USE_FONTS_FREETYPE TRUE)
    mark_as_advanced(USE_FONTS_FREETYPE)
endif(USE_FONTS STREQUAL "FREETYPE")

if(USE_FONTS STREQUAL "GTK")
    pkg_check_modules(CAIRO REQUIRED cairo>=1.4)
    pkg_check_modules(GTK REQUIRED gtk+-2.0>=2.8)
    pkg_check_modules(FONTCONFIG REQUIRED fontconfig>=2.4)
    pkg_check_modules(FREETYPE REQUIRED freetype2>=9.0)
    set(FONTS_INCLUDE_DIRS ${GTK_INCLUDE_DIRS} ${FONTCONFIG_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})
    set(FONTS_LIBRARIES ${GTK_LIBRARIES} ${FONTCONFIG_LIBRARIES} ${FREETYPE_LIBRARIES})

    set(USE_FONTS_GTK TRUE)
    mark_as_advanced(USE_FONTS_GTK)
endif(USE_FONTS STREQUAL "GTK")

if(USE_FONTS STREQUAL "QT")
    pkg_check_modules(QTFONT REQUIRED QtCore)
    set(FONTS_INCLUDE_DIRS ${QTFONT_INCLUDE_DIRS})
    set(FONTS_LIBRARIES ${QTFONT_LIBRARIES})

    set(USE_FONTS_QT TRUE)
    mark_as_advanced(USE_FONTS_QT)
endif(USE_FONTS STREQUAL "QT")
