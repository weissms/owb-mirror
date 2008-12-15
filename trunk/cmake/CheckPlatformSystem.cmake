if(UNIX)
    include(CheckLibraryExists)
    check_library_exists(pthread pthread_attr_destroy "" HAVE_PTHREAD_ATTR_DESRTOY)
    check_library_exists(pthread pthread_attr_init "" HAVE_PTHREAD_ATTR_INIT)
    check_library_exists(pthread pthread_getattr_np "" HAVE_PTHREAD_GETATTR_NP)

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        if(NOT HAVE_PTHREAD_GETATTR_NP)
            set(HAVE_UCLIBC TRUE)
            mark_as_advanced(HAVE_UCLIBC)
        endif(NOT HAVE_PTHREAD_GETATTR_NP)
    endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

    if(APPLE)
        set(WITH_MACPORT_DIR "/opt/local" CACHE STRING "Set macport directory")
        #FIXME: try to remove these lines
        include_directories(${WITH_MACPORT_DIR}/include)
        link_directories(${WITH_MACPORT_DIR}/lib)
        add_definitions (-D__OWBAL_PLATFORM_APPLE__)

    endif(APPLE)
endif(UNIX)
