##################################################
# ImageDecoder support part.                     #
##################################################

create_include_link(${BAL_DIR}/ImageDecoder/WebCore/WK BAL)
create_include_link(${BAL_DIR}/ImageDecoder/WebCore/BMP/WK BAL)
create_include_link(${BAL_DIR}/ImageDecoder/WebCore/GIF/WK BAL)
create_include_link(${BAL_DIR}/ImageDecoder/WebCore/ICO/WK BAL)
create_include_link(${BAL_DIR}/ImageDecoder/WebCore/JPEG/WK BAL)
create_include_link(${BAL_DIR}/ImageDecoder/WebCore/PNG/WK BAL)
create_include_link(${BAL_DIR}/ImageDecoder/WebCore/XBM/WK BAL)

aux_source_directory(${BAL_DIR}/ImageDecoder/WebCore/BMP/WK WEBCORE_SRC)
aux_source_directory(${BAL_DIR}/ImageDecoder/WebCore/GIF/WK WEBCORE_SRC)
aux_source_directory(${BAL_DIR}/ImageDecoder/WebCore/ICO/WK WEBCORE_SRC)
aux_source_directory(${BAL_DIR}/ImageDecoder/WebCore/JPEG/WK WEBCORE_SRC)
aux_source_directory(${BAL_DIR}/ImageDecoder/WebCore/PNG/WK WEBCORE_SRC)
aux_source_directory(${BAL_DIR}/ImageDecoder/WebCore/XBM/WK WEBCORE_SRC)
