if(NOT HAS_CUSTOMER)
    pkg_check_modules(PNG12 REQUIRED libpng12)
    find_package(JPEG QUIET REQUIRED)
    set(IMAGE_INCLUDE_DIRS 
        ${PNG12_INCLUDE_DIRS}
        ${JPEG_INCLUDE_DIR}
    )
    set(IMAGE_LIBRARIES
        ${PNG12_LIBRARIES}
        ${JPEG_LIBRARY}
    )
endif(NOT HAS_CUSTOMER)

