execute_process(COMMAND /usr/bin/lsb_release -sc
                OUTPUT_VARIABLE CODENAME
                ERROR_VARIABLE ERROR)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Webkit OWB library is designed for embedded systems")
set(CPACK_PACKAGE_CONTACT "Olivier DOLE <odole@pleyo.com>")
set(CPACK_PACKAGE_VENDOR "Pleyo")
set(CPACK_PACKAGE_NAME "libwebkit-owb")
set(CPACK_PACKAGE_VERSION 2.0.1)
set(CPACK_PACKAGE_VERSION_MAJOR 2)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 1)
set(CPACK_STRIP_FILES "lib/libwebkit-owb.so.2.0")
set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION})

include(karmicPackage)
include(CPack)
