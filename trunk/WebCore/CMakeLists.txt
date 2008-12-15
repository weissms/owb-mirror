##################################################
#                                                #
# WebCore build instructions.                    #
#                                                #
##################################################


##################################################
# Generate sources.                              #
##################################################

list(APPEND FEATURE_DEFINES_JAVASCRIPT LANGUAGE_JAVASCRIPT=1)
if(ENABLE_XPATH)
    list(APPEND FEATURE_DEFINES_JAVASCRIPT
        ENABLE_XPATH=1)
endif(ENABLE_XPATH)
if(ENABLE_XSLT)
    list(APPEND FEATURE_DEFINES_JAVASCRIPT
        ENABLE_XSLT=1)
endif(ENABLE_XSLT)
if(ENABLE_OFFLINE_WEB_APPLICATIONS)
    list(APPEND FEATURE_DEFINES_JAVASCRIPT
        ENABLE_OFFLINE_WEB_APPLICATIONS=1)
endif(ENABLE_OFFLINE_WEB_APPLICATIONS)
if(ENABLE_DATABASE)
    list(APPEND FEATURE_DEFINES_JAVASCRIPT
        ENABLE_DATABASE=1)
endif(ENABLE_DATABASE)
if(ENABLE_DOM_STORAGE)
    list(APPEND FEATURE_DEFINES_JAVASCRIPT
        ENABLE_DOM_STORAGE=1)
endif(ENABLE_DOM_STORAGE)
if(ENABLE_GEOLOCATION)
    list(APPEND FEATURE_DEFINES_JAVASCRIPT
        ENABLE_GEOLOCATION=1)
endif(ENABLE_GEOLOCATION)
if(ENABLE_VIDEO)
    list(APPEND FEATURE_DEFINES_JAVASCRIPT
        ENABLE_VIDEO=1)
    list(APPEND HTML_FEATURES
        --extraDefines ENABLE_VIDEO=1)
endif(ENABLE_VIDEO)
if(ENABLE_SVG)
    list(APPEND FEATURE_DEFINES_JAVASCRIPT
        ENABLE_SVG=1)
endif(ENABLE_SVG)
if(ENABLE_SVG_ANIMATION)
    list(APPEND FEATURE_DEFINES_JAVASCRIPT
        ENABLE_SVG_ANIMATION=1
    )
    list(APPEND SVG_FEATURES
        ENABLE_SVG_ANIMATION=1
    )
endif(ENABLE_SVG_ANIMATION)
if(ENABLE_SVG_AS_IMAGE)
    list(APPEND FEATURE_DEFINES_JAVASCRIPT
        ENABLE_SVG_AS_IMAGE=1
    )
    list(APPEND SVG_FEATURES
        ENABLE_SVG_AS_IMAGE=1
    )
endif(ENABLE_SVG_AS_IMAGE)
if(ENABLE_SVG_FILTERS)
    list(APPEND FEATURE_DEFINES_JAVASCRIPT
        ENABLE_SVG_FILTERS=1
    )
    list(APPEND SVG_FEATURES
        ENABLE_SVG_FILTERS=1
    )
endif(ENABLE_SVG_FILTERS)
if(ENABLE_SVG_FONTS)
    list(APPEND FEATURE_DEFINES_JAVASCRIPT
        ENABLE_SVG_FONTS=1
    )
    list(APPEND SVG_FEATURES
        ENABLE_SVG_FONTS=1
    )
endif(ENABLE_SVG_FONTS)
if(ENABLE_SVG_FOREIGN_OBJECT)
    list(APPEND FEATURE_DEFINES_JAVASCRIPT
        ENABLE_SVG_FOREIGN_OBJECT=1
    )
    list(APPEND SVG_FEATURES
        ENABLE_SVG_FOREIGN_OBJECT=1
    )
endif(ENABLE_SVG_FOREIGN_OBJECT)
if(ENABLE_SVG_USE_ELEMENT)
    list(APPEND FEATURE_DEFINES_JAVASCRIPT
        ENABLE_SVG_USE=1
    )
    list(APPEND SVG_FEATURES
        ENABLE_SVG_USE=1
    )
endif(ENABLE_SVG_USE_ELEMENT)
if(ENABLE_WORKERS)
    list(APPEND FEATURE_DEFINES_JAVASCRIPT
        ENABLE_WORKERS=1
    )
endif(ENABLE_WORKERS)
if(ENABLE_WML)
    list(APPEND FEATURE_DEFINES_JAVASCRIPT
        ENABLE_WML=1
    )
endif(ENABLE_WML)

list(APPEND WEBCORE_CSS_PROPERTY_NAMES 
    ${OWB_SOURCE_DIR}/WebCore/css/CSSPropertyNames.in
)
if(ENABLE_SVG)
    list(APPEND WEBCORE_CSS_PROPERTY_NAMES
        ${OWB_SOURCE_DIR}/WebCore/css/SVGCSSPropertyNames.in
    )
endif(ENABLE_SVG)
list(APPEND WEBCORE_CSS_VALUE_KEYWORDS
    ${OWB_SOURCE_DIR}/WebCore/css/CSSValueKeywords.in
)
if(ENABLE_SVG)
    list(APPEND WEBCORE_CSS_VALUE_KEYWORDS
        ${OWB_SOURCE_DIR}/WebCore/css/SVGCSSValueKeywords.in
    )
endif(ENABLE_SVG)
list(APPEND USER_AGENT_STYLE_SHEETS
    ${OWB_SOURCE_DIR}/WebCore/css/html4.css
    ${OWB_SOURCE_DIR}/WebCore/css/quirks.css
    ${OWB_SOURCE_DIR}/WebCore/css/view-source.css
    ${OWB_SOURCE_DIR}/WebCore/css/svg.css
    ${OWB_SOURCE_DIR}/WebCore/css/wml.css
)

set(WEBCORE_GENERATED_DIR ${OWB_BINARY_DIR}/generated_sources/WebCore)
add_custom_command(
    OUTPUT ${WEBCORE_GENERATED_DIR}/ColorData.c
    COMMAND mkdir ARGS -p ${WEBCORE_GENERATED_DIR}
    COMMAND echo ARGS '\#include <string.h>' > ColorData.c
    COMMAND ${GPERF_EXECUTABLE} ARGS -CDEot -L ANSI-C -k \"*\" -N findColor -D -s 2 < ${OWB_SOURCE_DIR}/WebCore/platform/ColorData.gperf >> ColorData.c
    WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
    DEPENDS ${OWB_SOURCE_DIR}/WebCore/platform/ColorData.gperf)

add_custom_command(
    OUTPUT ${WEBCORE_GENERATED_DIR}/CSSGrammar.cpp ${WEBCORE_GENERATED_DIR}/CSSGrammar.h
    COMMAND mkdir -p ${WEBCORE_GENERATED_DIR}
    COMMAND rm -f ${WEBCORE_GENERATED_DIR}/CSSGrammar.cpp.h ${WEBCORE_GENERATED_DIR}/CSSGrammar.hpp
    COMMAND ${BISON_EXECUTABLE} -d -p cssyy ${OWB_SOURCE_DIR}/WebCore/css/CSSGrammar.y -o ${WEBCORE_GENERATED_DIR}/CSSGrammar.cpp
    COMMAND touch ${WEBCORE_GENERATED_DIR}/CSSGrammar.cpp.h ${WEBCORE_GENERATED_DIR}/CSSGrammar.hpp
    COMMAND echo "#ifndef CSSGrammar_h" > ${WEBCORE_GENERATED_DIR}/CSSGrammar.h
    COMMAND echo "#define CSSGrammar_h" >> ${WEBCORE_GENERATED_DIR}/CSSGrammar.h
    COMMAND cat ${WEBCORE_GENERATED_DIR}/CSSGrammar.cpp.h ${WEBCORE_GENERATED_DIR}/CSSGrammar.hpp >> ${WEBCORE_GENERATED_DIR}/CSSGrammar.h
    COMMAND echo "#endif" >> ${WEBCORE_GENERATED_DIR}/CSSGrammar.h
    COMMAND rm -f ${WEBCORE_GENERATED_DIR}/CSSGrammar.cpp.h ${WEBCORE_GENERATED_DIR}/CSSGrammar.hpp
    WORKING_DIRECTORY ${OWB_BINARY_DIR}
    DEPENDS ${OWB_SOURCE_DIR}/WebCore/css/CSSGrammar.y
    VERBATIM)

add_custom_command(
    OUTPUT ${WEBCORE_GENERATED_DIR}/CSSPropertyNames.h
    #COMMAND if sort ${WEBCORE_CSS_PROPERTY_NAMES} | uniq -d | grep -E '^[^#]'; then echo 'Duplicate value!'; exit 1; fi
    COMMAND mkdir ARGS -p ${WEBCORE_GENERATED_DIR}
    COMMAND cat ${WEBCORE_CSS_PROPERTY_NAMES} > ${WEBCORE_GENERATED_DIR}/CSSPropertyNames.in
    COMMAND ${PERL_EXECUTABLE} ${OWB_SOURCE_DIR}/WebCore/css/makeprop.pl
    WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
    DEPENDS ${WEBCORE_CSS_PROPERTY_NAMES}
    VERBATIM)

add_custom_command(
    OUTPUT ${WEBCORE_GENERATED_DIR}/CSSValueKeywords.h
    COMMAND mkdir ARGS -p ${WEBCORE_GENERATED_DIR}
    COMMAND ${PERL_EXECUTABLE} -ne 'print lc' ${WEBCORE_CSS_VALUE_KEYWORDS} > ${WEBCORE_GENERATED_DIR}/CSSValueKeywords.in
    #COMMAND if sort CSSValueKeywords.in | uniq -d | grep -E '^[^#]'; then echo 'Duplicate value!'; exit 1; fi
    COMMAND ${PERL_EXECUTABLE} ${OWB_SOURCE_DIR}/WebCore/css/makevalues.pl
    WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
    DEPENDS ${WEBCORE_CSS_VALUE_KEYWORDS})

add_custom_command(
    OUTPUT ${WEBCORE_GENERATED_DIR}/html/DocTypeStrings.cpp
    COMMAND mkdir -p ${WEBCORE_GENERATED_DIR}/html
    COMMAND echo '\#include <string.h>' > html/DocTypeStrings.cpp
    COMMAND ${GPERF_EXECUTABLE} -CEot -L ANSI-C -k \"*\" -N findDoctypeEntry -F ,PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards < ${OWB_SOURCE_DIR}/WebCore/html/DocTypeStrings.gperf >> html/DocTypeStrings.cpp
    WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
    DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/html/DocTypeStrings.gperf)

add_custom_command(
    COMMAND mkdir ARGS -p ${WEBCORE_GENERATED_DIR}/html
    OUTPUT ${WEBCORE_GENERATED_DIR}/html/HTMLEntityNames.c
    COMMAND echo '\#include <string.h>' > html/HTMLEntityNames.c
    COMMAND ${GPERF_EXECUTABLE} -a -L ANSI-C -C -G -c -o -t --key-positions="*" -N findEntity -D -s 2 < ${OWB_SOURCE_DIR}/WebCore/html/HTMLEntityNames.gperf >> html/HTMLEntityNames.c
    WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
    DEPENDS ${OWB_SOURCE_DIR}/WebCore/html/HTMLEntityNames.gperf)

add_custom_command(
    OUTPUT ${WEBCORE_GENERATED_DIR}/html/HTMLNames.cpp ${WEBCORE_GENERATED_DIR}/html/HTMLNames.h ${WEBCORE_GENERATED_DIR}/html/JSHTMLElementWrapperFactory.cpp ${WEBCORE_GENERATED_DIR}/html/JSHTMLElementWrapperFactory.h
    COMMAND mkdir ARGS -p ${WEBCORE_GENERATED_DIR}/html
    COMMAND ${PERL_EXECUTABLE} -I${OWB_SOURCE_DIR}/WebCore/bindings/scripts ${CMAKE_SOURCE_DIR}/WebCore/dom/make_names.pl --tags ${OWB_SOURCE_DIR}/WebCore/html/HTMLTagNames.in --attrs ${OWB_SOURCE_DIR}/WebCore/html/HTMLAttributeNames.in ${HTML_FEATURES} --wrapperFactory --outputDir ${WEBCORE_GENERATED_DIR}/html
    WORKING_DIRECTORY ${OWB_BINARY_DIR}
    DEPENDS ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl ${OWB_SOURCE_DIR}/WebCore/html/HTMLTagNames.in ${OWB_SOURCE_DIR}/WebCore/html/HTMLAttributeNames.in
    VERBATIM)

add_custom_command(
    OUTPUT ${WEBCORE_GENERATED_DIR}/tokenizer.cpp
    COMMAND mkdir ARGS -p ${WEBCORE_GENERATED_DIR}
    COMMAND ${FLEX_EXECUTABLE} -t ${OWB_SOURCE_DIR}/WebCore/css/tokenizer.flex | ${PERL_EXECUTABLE} ${OWB_SOURCE_DIR}/WebCore/css/maketokenizer > tokenizer.cpp
    WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
    DEPENDS ${OWB_SOURCE_DIR}/WebCore/css/tokenizer.flex ${OWB_SOURCE_DIR}/WebCore/css/maketokenizer
    VERBATIM)

if(ENABLE_SVG)
    if(SVG_FLAGS)
        add_custom_command(
            OUTPUT ${WEBCORE_GENERATED_DIR}/JSSVGElementWrapperFactory.cpp ${WEBCORE_GENERATED_DIR}/JSSVGElementWrapperFactory.h ${WEBCORE_GENERATED_DIR}/SVGElementFactory.cpp ${WEBCORE_GENERATED_DIR}/SVGElementFactory.h ${WEBCORE_GENERATED_DIR}/SVGNames.cpp ${WEBCORE_GENERATED_DIR}/SVGNames.h
            COMMAND mkdir ARGS -p ${WEBCORE_GENERATED_DIR}
            COMMAND ${PERL_EXECUTABLE} -I${OWB_SOURCE_DIR}/WebCore/bindings/scripts ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl --tags ${OWB_SOURCE_DIR}/WebCore/svg/svgtags.in --attrs ${OWB_SOURCE_DIR}/WebCore/svg/svgattrs.in --extraDefines \"${SVG_FEATURES}\" --factory --wrapperFactory --outputDir ${WEBCORE_GENERATED_DIR}
            WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
            DEPENDS ${OWB_SOURCE_DIR}/WebCore/svg/svgtags.in ${OWB_SOURCE_DIR}/WebCore/svg/svgattrs.in
        )
    else(SVG_FLAGS)
        add_custom_command(
            OUTPUT ${WEBCORE_GENERATED_DIR}/JSSVGElementWrapperFactory.cpp ${WEBCORE_GENERATED_DIR}/JSSVGElementWrapperFactory.h ${WEBCORE_GENERATED_DIR}/SVGElementFactory.cpp ${WEBCORE_GENERATED_DIR}/SVGElementFactory.h ${WEBCORE_GENERATED_DIR}/SVGNames.cpp ${WEBCORE_GENERATED_DIR}/SVGNames.h
            COMMAND mkdir ARGS -p ${WEBCORE_GENERATED_DIR}
            COMMAND ${PERL_EXECUTABLE} -I${OWB_SOURCE_DIR}/WebCore/bindings/scripts ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl --tags ${OWB_SOURCE_DIR}/WebCore/svg/svgtags.in --attrs ${OWB_SOURCE_DIR}/WebCore/svg/svgattrs.in --factory --wrapperFactory --outputDir ${WEBCORE_GENERATED_DIR}
            WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
            DEPENDS ${OWB_SOURCE_DIR}/WebCore/svg/svgtags.in ${OWB_SOURCE_DIR}/WebCore/svg/svgattrs.in
            VERBATIM
        )
    endif(SVG_FLAGS)
    add_custom_command(
        OUTPUT ${WEBCORE_GENERATED_DIR}/XLinkNames.cpp ${WEBCORE_GENERATED_DIR}/XLinkNames.h
        COMMAND mkdir ARGS -p ${WEBCORE_GENERATED_DIR}
        COMMAND ${PERL_EXECUTABLE}
        -I${OWB_SOURCE_DIR}/WebCore/bindings/scripts ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl --attrs ${OWB_SOURCE_DIR}/WebCore/svg/xlinkattrs.in --outputDir ${WEBCORE_GENERATED_DIR}
        WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
        DEPENDS ${OWB_SOURCE_DIR}/WebCore/svg/xlinkattrs.in
        VERBATIM
    )
endif(ENABLE_SVG)

add_custom_command(
    OUTPUT ${WEBCORE_GENERATED_DIR}/UserAgentStyleSheetsData.cpp ${WEBCORE_GENERATED_DIR}/UserAgentStyleSheets.h
    COMMAND mkdir ARGS -p ${WEBCORE_GENERATED_DIR}
    COMMAND ${PERL_EXECUTABLE} ${OWB_SOURCE_DIR}/WebCore/css/make-css-file-arrays.pl UserAgentStyleSheets.h UserAgentStyleSheetsData.cpp ${USER_AGENT_STYLE_SHEETS}
    WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
    DEPENDS ${OWB_SOURCE_DIR}/WebCore/css/make-css-file-arrays.pl ${USER_AGENT_STYLE_SHEETS}
    VERBATIM)

add_custom_command(OUTPUT ${WEBCORE_GENERATED_DIR}/xml/XMLNames.cpp ${WEBCORE_GENERATED_DIR}/xml/XMLNames.h
    COMMAND mkdir -p ${WEBCORE_GENERATED_DIR}/xml
    COMMAND ${PERL_EXECUTABLE} -I${OWB_SOURCE_DIR}/WebCore/bindings/scripts ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl --attrs ${OWB_SOURCE_DIR}/WebCore/xml/xmlattrs.in --output xml
    WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
    DEPENDS ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl ${OWB_SOURCE_DIR}/WebCore/xml/xmlattrs.in
    VERBATIM)

if(ENABLE_WML)
    add_custom_command(OUTPUT ${WEBCORE_GENERATED_DIR}/wml/WMLNames.cpp ${WEBCORE_GENERATED_DIR}/wml/WMLNames.h ${WEBCORE_GENERATED_DIR}/wml/WMLElementFactory.cpp
        COMMAND mkdir -p ${WEBCORE_GENERATED_DIR}/wml
        COMMAND ${PERL_EXECUTABLE} -I${OWB_SOURCE_DIR}/WebCore/bindings/scripts ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl --tags ${OWB_SOURCE_DIR}/WebCore/wml/WMLTagNames.in --attrs ${OWB_SOURCE_DIR}/WebCore/wml/WMLAttributeNames.in --factory --wrapperFactory --output wml
        WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
        DEPENDS ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl ${OWB_SOURCE_DIR}/WebCore/wml/WMLTagNames.in ${OWB_SOURCE_DIR}/WebCore/wml/WMLAttributeNames.in
        VERBATIM)
endif(ENABLE_WML)

if(ENABLE_XPATH)
    add_custom_command(OUTPUT ${WEBCORE_GENERATED_DIR}/XPathGrammar.cpp ${WEBCORE_GENERATED_DIR}/XPathGrammar.h
        COMMAND mkdir ARGS -p ${WEBCORE_GENERATED_DIR}
        COMMAND ${BISON_EXECUTABLE} -d -p xpathyy ${OWB_SOURCE_DIR}/WebCore/xml/XPathGrammar.y -o XPathGrammar.cpp
        COMMAND touch XPathGrammar.cpp.h XPathGrammar.hpp
        COMMAND echo '\#ifndef XPathGrammar_h' > XPathGrammar.h
        COMMAND echo '\#define XPathGrammar_h' >> XPathGrammar.h
        COMMAND cat XPathGrammar.cpp.h XPathGrammar.hpp >> ${CMAKE_BINARY_DIR}/generated_sources/WebCore/XPathGrammar.h
        COMMAND echo '\#endif' >> XPathGrammar.h
        COMMAND rm -f XPathGrammar.cpp.h XPathGrammar.hpp
        WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
        DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/xml/XPathGrammar.y)
endif(ENABLE_XPATH)

##################################################
# Add sources for JavaScriptore library.         #
##################################################

list(APPEND WEBCORE_SRC
    ${WEBCORE_GENERATED_DIR}/ColorData.c
    ${WEBCORE_GENERATED_DIR}/CSSGrammar.cpp
    ${WEBCORE_GENERATED_DIR}/CSSPropertyNames.h
    ${WEBCORE_GENERATED_DIR}/CSSValueKeywords.h
    ${WEBCORE_GENERATED_DIR}/html/DocTypeStrings.cpp
    ${WEBCORE_GENERATED_DIR}/html/HTMLEntityNames.c
    ${WEBCORE_GENERATED_DIR}/html/HTMLNames.cpp
    ${WEBCORE_GENERATED_DIR}/html/JSHTMLElementWrapperFactory.cpp
    ${WEBCORE_GENERATED_DIR}/tokenizer.cpp
    ${WEBCORE_GENERATED_DIR}/UserAgentStyleSheetsData.cpp
    ${WEBCORE_GENERATED_DIR}/xml/XMLNames.cpp
)

if(ENABLE_SVG)
    list(APPEND WEBCORE_SRC
        ${WEBCORE_GENERATED_DIR}/JSSVGElementWrapperFactory.cpp
        ${WEBCORE_GENERATED_DIR}/SVGElementFactory.cpp
        ${WEBCORE_GENERATED_DIR}/SVGNames.cpp
        ${WEBCORE_GENERATED_DIR}/XLinkNames.cpp
    )
endif(ENABLE_SVG)

if(ENABLE_WML)
    list(APPEND WEBCORE_SRC
        ${WEBCORE_GENERATED_DIR}/wml/WMLNames.cpp
        ${WEBCORE_GENERATED_DIR}/wml/WMLElementFactory.cpp
    )
endif(ENABLE_WML)

if(ENABLE_XPATH)
    list(APPEND WEBCORE_SRC
        ${WEBCORE_GENERATED_DIR}/XPathGrammar.cpp
    )
endif(ENABLE_XPATH)

include(${OWB_SOURCE_DIR}/BAL/CMakeLists.txt)
include(bindings/js/CMakeLists.txt)
include(bridge/CMakeLists.txt)
include(css/CMakeLists.txt)
include(dom/CMakeLists.txt)
include(editing/CMakeLists.txt)
include(history/CMakeLists.txt)
include(html/CMakeLists.txt)
include(inspector/CMakeLists.txt)
include(loader/CMakeLists.txt)
include(loader/icon/CMakeLists.txt)
include(page/CMakeLists.txt)
include(platform/CMakeLists.txt)
include(plugins/CMakeLists.txt)
include(rendering/CMakeLists.txt)
include(rendering/style/CMakeLists.txt)
include(storage/CMakeLists.txt)
include(svg/CMakeLists.txt)
include(wml/CMakeLists.txt)
include(xml/CMakeLists.txt)

generate_sources_from_idl(WEBCORE_SRC)

list(APPEND WEBCORE_SRC_NO_BUILD
    ${WEBCORE_GENERATED_DIR}/ColorData.c
    ${WEBCORE_GENERATED_DIR}/html/DocTypeStrings.cpp
    ${WEBCORE_GENERATED_DIR}/JSHTMLInputElementBaseTable.cpp
    ${WEBCORE_GENERATED_DIR}/tokenizer.cpp
)
if(ENABLE_SVG)
    list(APPEND WEBCORE_SRC_NO_BUILD
        ${WEBCORE_GENERATED_DIR}/JSElementTimeControl.cpp
        ${WEBCORE_GENERATED_DIR}/JSSVGAnimatedPathData.cpp
        ${WEBCORE_GENERATED_DIR}/JSSVGAnimatedPoints.cpp
        ${WEBCORE_GENERATED_DIR}/JSSVGExternalResourcesRequired.cpp
        ${WEBCORE_GENERATED_DIR}/JSSVGFilterPrimitiveStandardAttributes.cpp
        ${WEBCORE_GENERATED_DIR}/JSSVGFitToViewBox.cpp
        ${WEBCORE_GENERATED_DIR}/JSSVGLangSpace.cpp
        ${WEBCORE_GENERATED_DIR}/JSSVGLocatable.cpp
        ${WEBCORE_GENERATED_DIR}/JSSVGStylable.cpp
        ${WEBCORE_GENERATED_DIR}/JSSVGTests.cpp
        ${WEBCORE_GENERATED_DIR}/JSSVGTransformable.cpp
        ${WEBCORE_GENERATED_DIR}/JSSVGURIReference.cpp
        ${WEBCORE_GENERATED_DIR}/JSSVGViewSpec.cpp
        ${WEBCORE_GENERATED_DIR}/JSSVGZoomAndPan.cpp
    )
endif(ENABLE_SVG)
set_source_files_properties(${WEBCORE_SRC_NO_BUILD}
    PROPERTIES HEADER_FILE_ONLY TRUE)


##################################################
# Compilation settings.                          #
##################################################

if(ENABLE_DATABASE)
    list(APPEND WEBCORE_INCLUDE_DIRS
        ${OWB_SOURCE_DIR}/WebCore/storage)
endif(ENABLE_DATABASE)

include_directories(
    ${BALBASE_INCLUDE_DIRS}
    ${WTF_INCLUDE_DIRS}
    ${BAL_INCLUDE_DIRS}
    ${JAVASCRIPTCORE_INCLUDE_DIRS}
    ${WEBCORE_INCLUDE_DIRS}
    ${WEBCORE_GENERATED_DIR}
    ${WEBCORE_GENERATED_DIR}/html
    ${WEBCORE_GENERATED_DIR}/xml
    ${EXTERNAL_DEPS_INCLUDE_DIRS})

include(AddWebCoreCompilerFlags)

add_library(webcore STATIC ${WEBCORE_SRC})
add_dependencies(webcore kjs)
