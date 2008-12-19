##################################################
# Internationalization support part.             #
##################################################

create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCAtomicStringWK.h BAL/AtomicString.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCAtomicStringHashWK.h BAL/AtomicStringHash.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCAtomicStringImplWK.h BAL/AtomicStringImpl.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCBase64WK.h BAL/Base64.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCBidiContextWK.h BAL/BidiContext.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCBidiResolverWK.h BAL/BidiResolver.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCCStringWK.h BAL/CString.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCCharacterNamesWK.h BAL/CharacterNames.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCPlatformStringWK.h BAL/PlatformString.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCRegularExpressionWK.h BAL/RegularExpression.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCSegmentedStringWK.h BAL/SegmentedString.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCStringBufferWK.h BAL/StringBuffer.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCStringHashWK.h BAL/StringHash.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCStringImplWK.h BAL/StringImpl.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCTextCodecWK.h BAL/TextCodec.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCTextCodecLatin1WK.h BAL/TextCodecLatin1.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCTextCodecUTF16WK.h BAL/TextCodecUTF16.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCTextCodecUserDefinedWK.h BAL/TextCodecUserDefined.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCTextDecoderWK.h BAL/TextDecoder.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCTextDirectionWK.h BAL/TextDirection.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCTextEncodingWK.h BAL/TextEncoding.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCTextEncodingRegistryWK.h BAL/TextEncodingRegistry.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCTextStreamWK.h BAL/TextStream.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCUnicodeRangeWK.h BAL/UnicodeRange.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCParserUtilitiesWK.h BAL/ParserUtilities.h)
create_include_link(${BAL_DIR}/Internationalization/WebCore/WK/BCStringBuilderWK.h BAL/StringBuilder.h)

if(USE_I18N_ICU)
    create_include_link(${BAL_DIR}/Internationalization/WebCore/ICU/BCTextBoundariesICU.h BAL/TextBoundaries.h)
    create_include_link(${BAL_DIR}/Internationalization/WebCore/ICU/BCTextBreakIteratorICU.h BAL/TextBreakIterator.h)
    create_include_link(${BAL_DIR}/Internationalization/WebCore/ICU/BCTextCodecICU.h BAL/TextCodecICU.h)
    
    aux_source_directory(${BAL_DIR}/Internationalization/WebCore/ICU WEBCORE_SRC)
    
    if(USE_GRAPHICS_GTK)
        create_include_link(${BAL_DIR}/Internationalization/WebCore/Gtk/BCLocalizedStringsGtk.h BAL/LocalizedStrings.h)
        create_include_link(${BAL_DIR}/Internationalization/WebCore/Gtk/BCTextBreakIteratorInternalICUGtk.h BAL/TextBreakIteratorInternalICU.h)

        aux_source_directory(${BAL_DIR}/Internationalization/WebCore/Gtk WEBCORE_SRC)
    endif(USE_GRAPHICS_GTK)

    if(USE_GRAPHICS_SDL)
        create_include_link(${BAL_DIR}/Internationalization/WebCore/SDL/BCLocalizedStringsSDL.h BAL/LocalizedStrings.h)
        create_include_link(${BAL_DIR}/Internationalization/WebCore/SDL/BCTextBreakIteratorInternalICUSDL.h BAL/TextBreakIteratorInternalICU.h)

        aux_source_directory(${BAL_DIR}/Internationalization/WebCore/SDL WEBCORE_SRC)
    endif(USE_GRAPHICS_SDL)
endif(USE_I18N_ICU)

if(USE_I18N_GENERIC)
    create_include_link(${BAL_DIR}/Internationalization/WebCore/Generic/BCTextBoundariesGeneric.h BAL/TextBoundaries.h)
    create_include_link(${BAL_DIR}/Internationalization/WebCore/Generic/BCTextBreakIteratorGeneric.h BAL/TextBreakIterator.h)
    create_include_link(${BAL_DIR}/Internationalization/WebCore/Generic/BCTextCodecICU.h BAL/TextCodecICU.h)
    create_include_link(${BAL_DIR}/Internationalization/WebCore/Generic/BCLocalizedStringsGeneric.h BAL/LocalizedStrings.h)
    create_include_link(${BAL_DIR}/Internationalization/WebCore/Generic/BCTextBreakIteratorInternalICUGeneric.h BAL/TextBreakIteratorInternalICU.h)

    aux_source_directory(${BAL_DIR}/Internationalization/WebCore/Generic WEBCORE_SRC)
    aux_source_directory(${BAL_DIR}/Internationalization/WebCore/Generic/ICU WEBCORE_SRC)
endif(USE_I18N_GENERIC)

aux_source_directory(${BAL_DIR}/Internationalization/WebCore/WK WEBCORE_SRC)
