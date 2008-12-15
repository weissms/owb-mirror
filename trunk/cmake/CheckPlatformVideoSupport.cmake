if(ENABLE_VIDEO)
    pkg_check_modules(GSTREAMER_VIDEO REQUIRED gstreamer-video-0.10>=0.10.17)
    pkg_check_modules(GNOMEVFS gnome-vfs-2.0>=2.0)
    list(APPEND VIDEO_INCLUDE_DIRS
        ${GSTREAMER_VIDEO_INCLUDE_DIRS}
        ${GNOMEVFS_INCLUDE_DIRS}
    )
    list(APPEND VIDEO_LIBRARIES
        ${GSTREAMER_VIDEO_LIBRARIES}
        ${GNOMEVFS_LIBRARIES}
    )
endif(ENABLE_VIDEO)
