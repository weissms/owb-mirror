##################################################
# SVG support part.                              #
##################################################

if(ENABLE_SVG)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK BAL)

    if(USE_GRAPHICS_GTK)
        create_include_link(${BAL_DIR}/SVG/WebCore/Cairo BAL)

        aux_source_directory(${BAL_DIR}/SVG/WebCore/Cairo WEBCORE_SRC)
    endif(USE_GRAPHICS_GTK)

    if(USE_GRAPHICS_QT)
        aux_source_directory(${BAL_DIR}/SVG/WebCore/Qt WEBCORE_SRC)
    endif(USE_GRAPHICS_QT)
    
    if(USE_GRAPHICS_SDL)
        create_include_link(${BAL_DIR}/SVG/WebCore/SDL BAL)

        aux_source_directory(${BAL_DIR}/SVG/WebCore/SDL WEBCORE_SRC)
    endif(USE_GRAPHICS_SDL)

    aux_source_directory(${BAL_DIR}/SVG/WebCore/WK WEBCORE_SRC)
endif(ENABLE_SVG)

