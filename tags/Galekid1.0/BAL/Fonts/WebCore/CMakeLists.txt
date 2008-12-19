##################################################
# Fonts support part.                            #
##################################################

if(USE_FONTS_GTK)
    create_include_link(${BAL_DIR}/Fonts/WebCore/Gtk/BCFontCustomPlatformDataGtk.h BAL/FontCustomPlatformData.h)
    create_include_link(${BAL_DIR}/Fonts/WebCore/Gtk/BCFontPlatformDataGtk.h BAL/FontPlatformData.h)
    create_include_link(${BAL_DIR}/Fonts/WebCore/WK/BCSimpleFontDataWK.h BAL/SimpleFontData.h)
    create_include_link(${BAL_DIR}/Fonts/WebCore/WK/BCGlyphBufferWK.h BAL/GlyphBuffer.h)
    

    aux_source_directory(${BAL_DIR}/Fonts/WebCore/Gtk WEBCORE_SRC)
    aux_source_directory(${BAL_DIR}/Fonts/WebCore/Cairo WEBCORE_SRC)
endif(USE_FONTS_GTK)

if(USE_FONTS_FREETYPE)
    create_include_link(${BAL_DIR}/Fonts/WebCore/Freetype/BCSimpleFontDataFreetype.h BAL/SimpleFontData.h)
    create_include_link(${BAL_DIR}/Fonts/WebCore/Freetype/BCFontCustomPlatformDataFreetype.h BAL/FontCustomPlatformData.h)
    create_include_link(${BAL_DIR}/Fonts/WebCore/Freetype/BCFontPlatformDataFreetype.h BAL/FontPlatformData.h)
    create_include_link(${BAL_DIR}/Fonts/WebCore/Freetype/BCGlyphBufferFreetype.h BAL/GlyphBuffer.h)

    aux_source_directory(${BAL_DIR}/Fonts/WebCore/Freetype WEBCORE_SRC)
endif(USE_FONTS_FREETYPE)

if(USE_FONTS_EMBEDDED)
    create_include_link(${BAL_DIR}/Fonts/WebCore/Embedded/BCSimpleFontDataEmbedded.h BAL/SimpleFontData.h)
    create_include_link(${BAL_DIR}/Fonts/WebCore/Embedded/BCFontCustomPlatformDataEmbedded.h BAL/FontCustomPlatformData.h)
    create_include_link(${BAL_DIR}/Fonts/WebCore/Embedded/BCFontPlatformDataEmbedded.h BAL/FontPlatformData.h)
    create_include_link(${BAL_DIR}/Fonts/WebCore/Embedded/BCGlyphBufferEmbedded.h BAL/GlyphBuffer.h)

    aux_source_directory(${BAL_DIR}/Fonts/WebCore/Embedded WEBCORE_SRC)
endif(USE_FONTS_EMBEDDED)

create_include_link(${BAL_DIR}/Fonts/WebCore/WK/BCFontWK.h BAL/Font.h)
create_include_link(${BAL_DIR}/Fonts/WebCore/WK/BCFontCacheWK.h BAL/FontCache.h)
create_include_link(${BAL_DIR}/Fonts/WebCore/WK/BCFontDataWK.h BAL/FontData.h)
create_include_link(${BAL_DIR}/Fonts/WebCore/WK/BCFontDescriptionWK.h BAL/FontDescription.h)
create_include_link(${BAL_DIR}/Fonts/WebCore/WK/BCFontFallbackListWK.h BAL/FontFallbackList.h)
create_include_link(${BAL_DIR}/Fonts/WebCore/WK/BCFontFamilyWK.h BAL/FontFamily.h)
create_include_link(${BAL_DIR}/Fonts/WebCore/WK/BCFontSelectorWK.h BAL/FontSelector.h)
create_include_link(${BAL_DIR}/Fonts/WebCore/WK/BCGlyphPageTreeNodeWK.h BAL/GlyphPageTreeNode.h)
create_include_link(${BAL_DIR}/Fonts/WebCore/WK/BCFontRenderingModeWK.h BAL/FontRenderingMode.h)
create_include_link(${BAL_DIR}/Fonts/WebCore/WK/BCGlyphWidthMapWK.h BAL/GlyphWidthMap.h)
create_include_link(${BAL_DIR}/Fonts/WebCore/WK/BCSegmentedFontDataWK.h BAL/SegmentedFontData.h)
create_include_link(${BAL_DIR}/Fonts/WebCore/WK/BCStringTruncatorWK.h BAL/StringTruncator.h)
create_include_link(${BAL_DIR}/Fonts/WebCore/WK/BCFontTraitsMaskWK.h BAL/FontTraitsMask.h)
create_include_link(${BAL_DIR}/Fonts/WebCore/WK/BCTextRunWK.h BAL/TextRun.h)
create_include_link(${BAL_DIR}/Fonts/WebCore/WK/BCWidthIteratorWK.h BAL/WidthIterator.h)

aux_source_directory(${BAL_DIR}/Fonts/WebCore/WK WEBCORE_SRC)
