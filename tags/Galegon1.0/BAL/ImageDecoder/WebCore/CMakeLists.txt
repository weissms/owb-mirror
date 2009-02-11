##################################################
# ImageDecoder support part.                     #
##################################################

create_include_link(${BAL_DIR}/ImageDecoder/WebCore/WK/BCImageDecoderWK.h BAL/ImageDecoder.h)
create_include_link(${BAL_DIR}/ImageDecoder/WebCore/BMP/WK/BCBMPImageDecoderWK.h BAL/BMPImageDecoder.h)
create_include_link(${BAL_DIR}/ImageDecoder/WebCore/GIF/WK/BCGIFImageDecoderWK.h BAL/GIFImageDecoder.h)
create_include_link(${BAL_DIR}/ImageDecoder/WebCore/GIF/WK/BCGIFImageReaderWK.h BAL/GIFImageReader.h)
create_include_link(${BAL_DIR}/ImageDecoder/WebCore/ICO/WK/BCICOImageDecoderWK.h BAL/ICOImageDecoder.h)
create_include_link(${BAL_DIR}/ImageDecoder/WebCore/JPEG/WK/BCJPEGImageDecoderWK.h BAL/JPEGImageDecoder.h)
create_include_link(${BAL_DIR}/ImageDecoder/WebCore/PNG/WK/BCPNGImageDecoderWK.h BAL/PNGImageDecoder.h)
create_include_link(${BAL_DIR}/ImageDecoder/WebCore/XBM/WK/BCXBMImageDecoderWK.h BAL/XBMImageDecoder.h)

aux_source_directory(${BAL_DIR}/ImageDecoder/WebCore/BMP/WK WEBCORE_SRC)
aux_source_directory(${BAL_DIR}/ImageDecoder/WebCore/GIF/WK WEBCORE_SRC)
aux_source_directory(${BAL_DIR}/ImageDecoder/WebCore/ICO/WK WEBCORE_SRC)
aux_source_directory(${BAL_DIR}/ImageDecoder/WebCore/JPEG/WK WEBCORE_SRC)
aux_source_directory(${BAL_DIR}/ImageDecoder/WebCore/PNG/WK WEBCORE_SRC)
aux_source_directory(${BAL_DIR}/ImageDecoder/WebCore/XBM/WK WEBCORE_SRC)
