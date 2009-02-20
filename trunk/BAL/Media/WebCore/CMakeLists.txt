##################################################
# Media support part.                            #
##################################################

create_include_link(${BAL_DIR}/Media/WebCore/WK/BCSoundWK.h BAL/Sound.h)
create_include_link(${BAL_DIR}/Media/WebCore/WK/BCMediaPlayerWK.h BAL/MediaPlayer.h)
create_include_link(${BAL_DIR}/Media/WebCore/WK/BCMediaPlayerPrivateWK.h BAL/MediaPlayerPrivate.h)

if(USE_GRAPHICS_GTK)
    if(ENABLE_VIDEO)
        create_include_link(${BAL_DIR}/Media/WebCore/GStreamer/BCMediaPlayerPrivateGStreamer.h BAL/MediaPlayerPrivateGStreamer.h)
        aux_source_directory(${BAL_DIR}/Media/WebCore/WK WEBCORE_SRC)
        aux_source_directory(${BAL_DIR}/Media/WebCore/GStreamer WEBCORE_SRC)
    endif(ENABLE_VIDEO)
        aux_source_directory(${BAL_DIR}/Media/WebCore/Gtk WEBCORE_SRC)
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_QT)
    if(ENABLE_VIDEO)
        create_include_link(${BAL_DIR}/Media/WebCore/Phonon/BCMediaPlayerPrivatePhonon.h BAL/MediaPlayerPrivatePhonon.h)
        aux_source_directory(${BAL_DIR}/Media/WebCore/WK WEBCORE_SRC)
        aux_source_directory(${BAL_DIR}/Media/WebCore/Phonon WEBCORE_SRC)
        owb_generate_moc(WEBCORE_SRC ${OWB_SOURCE_DIR}/BAL/Media/WebCore/Phonon/BCMediaPlayerPrivatePhonon.h MediaPlayerPrivatePhonon.moc)
    endif(ENABLE_VIDEO)
        aux_source_directory(${BAL_DIR}/Media/WebCore/Qt WEBCORE_SRC)
endif(USE_GRAPHICS_QT)

if(USE_GRAPHICS_SDL)
    if(ENABLE_VIDEO)
        create_include_link(${BAL_DIR}/Media/WebCore/SDL/BCMediaPlayerPrivateSDL.h BAL/MediaPlayerPrivateSDL.h)
        aux_source_directory(${BAL_DIR}/Media/WebCore/WK WEBCORE_SRC)
    endif(ENABLE_VIDEO)
        aux_source_directory(${BAL_DIR}/Media/WebCore/SDL WEBCORE_SRC)
endif(USE_GRAPHICS_SDL)
