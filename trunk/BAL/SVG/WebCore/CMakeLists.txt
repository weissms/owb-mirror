##################################################
# SVG support part.                              #
##################################################

if(ENABLE_SVG)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK/BCFEBlendWK.h BAL/FEBlend.h)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK/BCFEColorMatrixWK.h BAL/FEColorMatrix.h)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK/BCFEComponentTransferWK.h BAL/FEComponentTransfer.h)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK/BCFECompositeWK.h BAL/FEComposite.h)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK/BCSVGImageEmptyClientsWK.h BAL/SVGImageEmptyClients.h)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK/BCSVGImageWK.h BAL/SVGImage.h)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK/BCSVGPaintServerGradientWK.h BAL/SVGPaintServerGradient.h)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK/BCSVGPaintServerLinearGradientWK.h BAL/SVGPaintServerLinearGradient.h)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK/BCSVGPaintServerPatternWK.h BAL/SVGPaintServerPattern.h)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK/BCSVGPaintServerRadialGradientWK.h BAL/SVGPaintServerRadialGradient.h)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK/BCSVGPaintServerSolidWK.h BAL/SVGPaintServerSolid.h)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK/BCSVGPaintServerWK.h BAL/SVGPaintServer.h)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK/BCSVGResourceClipperWK.h BAL/SVGResourceClipper.h)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK/BCSVGResourceFilterWK.h BAL/SVGResourceFilter.h)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK/BCSVGResourceListenerWK.h BAL/SVGResourceListener.h)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK/BCSVGResourceMarkerWK.h BAL/SVGResourceMarker.h)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK/BCSVGResourceMaskerWK.h BAL/SVGResourceMasker.h)
    create_include_link(${BAL_DIR}/SVG/WebCore/WK/BCSVGResourceWK.h BAL/SVGResource.h)

    if(USE_GRAPHICS_GTK)
        create_include_link(${BAL_DIR}/SVG/WebCore/Cairo/BCRenderPathCairo.h BAL/BCRenderPath.h)

        aux_source_directory(${BAL_DIR}/SVG/WebCore/Cairo WEBCORE_SRC)
    endif(USE_GRAPHICS_GTK)

    if(USE_GRAPHICS_QT)
        aux_source_directory(${BAL_DIR}/SVG/WebCore/Qt WEBCORE_SRC)
    endif(USE_GRAPHICS_QT)
    
    if(USE_GRAPHICS_SDL)
        create_include_link(${BAL_DIR}/SVG/WebCore/SDL/BCSVGPaintServerSolidSDL.h BAL/SVGPaintServerSolid.h)

        aux_source_directory(${BAL_DIR}/SVG/WebCore/SDL WEBCORE_SRC)
    endif(USE_GRAPHICS_SDL)

    aux_source_directory(${BAL_DIR}/SVG/WebCore/WK WEBCORE_SRC)
endif(ENABLE_SVG)

