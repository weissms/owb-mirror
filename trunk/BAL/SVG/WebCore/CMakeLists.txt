##################################################
# SVG support part.                              #
##################################################

if(ENABLE_SVG)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK BAL)

    aux_source_directory(${BAL_DIR}/SVG/WebCore/WK WEBCORE_SRC)
endif(ENABLE_SVG)

