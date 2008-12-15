##################################################
# Media support part.                            #
##################################################

if(USE_GRAPHICS_GTK)
    create_include_link(${BAL_DIR}/Media/WebCore/Gtk/BCSoundGtk.h BAL/Sound.h)
    create_include_link(${BAL_DIR}/Media/WebCore/WK/BCMediaPlayerWK.h BAL/MediaPlayer.h)
    
    if(ENABLE_VIDEO)
        create_include_link(${BAL_DIR}/Media/WebCore/GStreamer/BCMediaPlayerPrivateGStreamer.h BAL/MediaPlayerPrivateGStreamer.h)
        aux_source_directory(${BAL_DIR}/Media/WebCore/WK WEBCORE_SRC)
        aux_source_directory(${BAL_DIR}/Media/WebCore/GStreamer WEBCORE_SRC)
    endif(ENABLE_VIDEO)
        aux_source_directory(${BAL_DIR}/Media/WebCore/Gtk WEBCORE_SRC)
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_SDL)
    create_include_link(${BAL_DIR}/Media/WebCore/SDL/BCSoundSDL.h BAL/Sound.h)
    create_include_link(${BAL_DIR}/Media/WebCore/SDL/BCMediaPlayerSDL.h BAL/MediaPlayer.h)

    if(ENABLE_VIDEO)
        create_include_link(${BAL_DIR}/Media/WebCore/SDL/BCMediaPlayerPrivateSDL.h BAL/MediaPlayerPrivateSDL.h)
        aux_source_directory(${BAL_DIR}/Media/WebCore/WK WEBCORE_SRC)
    endif(ENABLE_VIDEO)
        aux_source_directory(${BAL_DIR}/Media/WebCore/SDL WEBCORE_SRC)
endif(USE_GRAPHICS_SDL)
