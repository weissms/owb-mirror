# - Find GIF
# Find the native JPEG includes and library
# This module defines
#  GIF_INCLUDE_DIR, where to find gif_lib.h
#  GIF_LIBRARY, the libraries needed to use GIF.
#  GIF_FOUND, If false, do not try to use GIF.

FIND_PATH (GIF_INCLUDE_DIR gif_lib.h
    /usr/include
    /usr/local/include
    /opt/include
)
FIND_LIBRARY (GIF_LIBRARIES
    NAMES gif
    PATHS /usr/lib /usr/local/lib /opt/lib
)

IF (GIF_LIBRARIES AND GIF_INCLUDE_DIR)
    SET (GIF_LIBRARY ${GIF_LIBRARIES})
    SET (GIF_FOUND TRUE)
ELSE (GIF_LIBRARIES AND GIF_INCLUDE_DIR)
    SET (GIF_FOUND FALSE)
ENDIF (GIF_LIBRARIES AND GIF_INCLUDE_DIR)

