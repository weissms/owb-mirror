##################################################
# Facilities support part.                       #
##################################################

create_include_link(${BAL_DIR}/Facilities/WebCore/Common/BCBookmarkletCommon.h BAL/Bookmarklet.h)
create_include_link(${BAL_DIR}/Facilities/WebCore/Common/BCObserverBookmarkletCommon.h BAL/ObserverBookmarklet.h)
create_include_link(${BAL_DIR}/Facilities/WebCore/Common/BCObserverCommon.h BAL/Observer.h)
create_include_link(${BAL_DIR}/Facilities/WebCore/Common/BCObserverDataCommon.h BAL/ObserverData.h)
create_include_link(${BAL_DIR}/Facilities/WebCore/Common/BCObserverServiceBookmarkletCommon.h BAL/ObserverServiceBookmarklet.h)
create_include_link(${BAL_DIR}/Facilities/WebCore/Common/BCObserverServiceCommon.h BAL/ObserverService.h)
create_include_link(${BAL_DIR}/Facilities/WebCore/Common/BCObserverServiceDataCommon.h BAL/ObserverServiceData.h)
create_include_link(${BAL_DIR}/Facilities/WebCore/WK/BCFileChooserWK.h BAL/FileChooser.h)
create_include_link(${BAL_DIR}/Facilities/WebCore/WK/BCFloatConversionWK.h BAL/FloatConversion.h)
create_include_link(${BAL_DIR}/Facilities/WebCore/WK/BCGeolocationServiceWK.h BAL/GeolocationService.h)
create_include_link(${BAL_DIR}/Facilities/WebCore/WK/BCHTTPParsersWK.h BAL/HTTPParsers.h)
create_include_link(${BAL_DIR}/Facilities/WebCore/WK/BCLoggingWK.h BAL/Logging.h)
create_include_link(${BAL_DIR}/Facilities/WebCore/WK/BCMIMETypeRegistryWK.h BAL/MIMETypeRegistry.h)
create_include_link(${BAL_DIR}/Facilities/WebCore/WK/BCSSLKeyGeneratorWK.h BAL/SSLKeyGenerator.h)
create_include_link(${BAL_DIR}/Facilities/WebCore/WK/BCThreadGlobalDataWK.h BAL/ThreadGlobalData.h)
create_include_link(${BAL_DIR}/Facilities/WebCore/WK/BCTimerWK.h BAL/Timer.h)
#FIXME add implementation for GTK
create_include_link(${BAL_DIR}/Facilities/WebCore/Linux/BCFileIOLinux.h BAL/FileIO.h)


if(USE_GRAPHICS_GTK)
    create_include_link(${BAL_DIR}/Facilities/WebCore/Gtk/BCLanguageGtk.h BAL/Language.h)
    create_include_link(${BAL_DIR}/Facilities/WebCore/Gtk/BCguriescapeGtk.h BAL/guriescape.h)
    
    list(APPEND WEBCORE_SRC
        ${BAL_DIR}/Facilities/WebCore/Gtk/BCFileChooserGtk.cpp
        ${BAL_DIR}/Facilities/WebCore/Gtk/BCLanguageGtk.cpp
        ${BAL_DIR}/Facilities/WebCore/Gtk/BCLoggingGtk.cpp
        ${BAL_DIR}/Facilities/WebCore/Gtk/BCMIMETypeRegistryGtk.cpp
        ${BAL_DIR}/Facilities/WebCore/Linux/BCFileIOLinux.cpp
    )
    if(ENABLE_GEOLOCATION)
        create_include_link(${BAL_DIR}/Facilities/WebCore/Gtk/BCGeolocationServiceGtk.h BAL/GeolocationServiceGtk.h)
        list(APPEND WEBCORE_SRC
            ${BAL_DIR}/Facilities/WebCore/Gtk/BCGeolocationServiceGtk.cpp
        )
    endif(ENABLE_GEOLOCATION)
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_SDL)
    create_include_link(${BAL_DIR}/Facilities/WebCore/SDL/BCLanguageSDL.h BAL/Language.h)

    list(APPEND WEBCORE_SRC
        ${BAL_DIR}/Facilities/WebCore/SDL/BCFileChooserSDL.cpp
        ${BAL_DIR}/Facilities/WebCore/SDL/BCLanguageSDL.cpp
        ${BAL_DIR}/Facilities/WebCore/SDL/BCLoggingSDL.cpp
        ${BAL_DIR}/Facilities/WebCore/SDL/BCMIMETypeRegistrySDL.cpp
        ${BAL_DIR}/Facilities/WebCore/SDL/BCSSLKeyGeneratorSDL.cpp
    )
    if(ENABLE_GEOLOCATION)
        create_include_link(${BAL_DIR}/Facilities/WebCore/SDL/BCGeolocationServiceSDL.h	BAL/GeolocationServiceSDL.h)
        list(APPEND WEBCORE_SRC
            ${BAL_DIR}/Facilities/WebCore/SDL/BCGeolocationServiceSDL.cpp
        )
    endif(ENABLE_GEOLOCATION)
endif(USE_GRAPHICS_SDL)

if(USE_FILESYSTEM_ACCESS_GLIB)
    create_include_link(${BAL_DIR}/Facilities/WebCore/Gtk/BCFileSystemGtk.h BAL/FileSystem.h)
    
    list(APPEND WEBCORE_SRC
        ${BAL_DIR}/Facilities/WebCore/Gtk/BCFileSystemGtk.cpp
    )
endif(USE_FILESYSTEM_ACCESS_GLIB)

if(USE_FILESYSTEM_ACCESS_POSIX)
    create_include_link(${BAL_DIR}/Facilities/WebCore/Posix/BCFileSystemPosix.h BAL/FileSystem.h)

    aux_source_directory(${BAL_DIR}/Facilities/WebCore/Posix WEBCORE_SRC)
endif(USE_FILESYSTEM_ACCESS_POSIX)

if(USE_TIMER_GLIB)
    create_include_link(${BAL_DIR}/Facilities/WebCore/Gtk/BCSystemTimeGtk.h BAL/SystemTime.h)
    create_include_link(${BAL_DIR}/Facilities/WebCore/Gtk/BCSharedTimerGtk.h BAL/SharedTimer.h)
    
    list(APPEND WEBCORE_SRC
        ${BAL_DIR}/Facilities/WebCore/Gtk/BCSystemTimeGtk.cpp
        ${BAL_DIR}/Facilities/WebCore/Gtk/BCSharedTimerGtk.cpp
    )
endif(USE_TIMER_GLIB)

if(USE_TIMER_LINUX)
    create_include_link(${BAL_DIR}/Facilities/WebCore/Linux/BCSystemTimeLinux.h BAL/SystemTime.h)
    create_include_link(${BAL_DIR}/Facilities/WebCore/Linux/BCSharedTimerLinux.h BAL/SharedTimer.h)

    aux_source_directory(${BAL_DIR}/Facilities/WebCore/Linux WEBCORE_SRC)
endif(USE_TIMER_LINUX)

aux_source_directory(${BAL_DIR}/Facilities/WebCore/WK WEBCORE_SRC)
aux_source_directory(${BAL_DIR}/Facilities/WebCore/Common WEBCORE_SRC)
