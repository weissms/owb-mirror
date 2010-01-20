##################################################
#                                                #
# WebCore build instructions.                    #
#                                                #
##################################################


##################################################
# Generate sources.                              #
##################################################

add_javascript_features()

list(APPEND WEBCORE_CSS_PROPERTY_NAMES 
    ${OWB_SOURCE_DIR}/WebCore/css/CSSPropertyNames.in
)
list(APPEND WEBCORE_CSS_VALUE_KEYWORDS
    ${OWB_SOURCE_DIR}/WebCore/css/CSSValueKeywords.in
)
list(APPEND USER_AGENT_STYLE_SHEETS
    ${OWB_SOURCE_DIR}/WebCore/css/html.css
    ${OWB_SOURCE_DIR}/WebCore/css/quirks.css
    ${OWB_SOURCE_DIR}/WebCore/css/view-source.css
)

if(ENABLE_MATHML)
    list(APPEND USER_AGENT_STYLE_SHEETS
        ${OWB_SOURCE_DIR}/WebCore/css/mathml.css
    )
endif(ENABLE_MATHML)

if(USE_GRAPHICS_GTK)
    list(APPEND USER_AGENT_STYLE_SHEETS
        ${OWB_SOURCE_DIR}/WebCore/css/mediaControlsGtk.css
    )
endif(USE_GRAPHICS_GTK)

if(ENABLE_SVG)
    add_svg_features()
    list(APPEND WEBCORE_CSS_PROPERTY_NAMES
        ${OWB_SOURCE_DIR}/WebCore/css/SVGCSSPropertyNames.in
    )
    list(APPEND WEBCORE_CSS_VALUE_KEYWORDS
        ${OWB_SOURCE_DIR}/WebCore/css/SVGCSSValueKeywords.in
    )
    list(APPEND USER_AGENT_STYLE_SHEETS
        ${OWB_SOURCE_DIR}/WebCore/css/svg.css
    )
endif(ENABLE_SVG)

if(ENABLE_WCSS)
    list(APPEND WEBCORE_CSS_PROPERTY_NAMES
        ${OWB_SOURCE_DIR}/WebCore/css/WCSSPropertyNames.in
    )
    list(APPEND WEBCORE_CSS_VALUE_KEYWORDS
        ${OWB_SOURCE_DIR}/WebCore/css/WCSSValueKeywords.in
    )
endif(ENABLE_WCSS)

if(ENABLE_CEHTML)
    list(APPEND WEBCORE_CSS_PROPERTY_NAMES
        ${OWB_SOURCE_DIR}/BAL/CEHTML/WebCore/Generic/CEHTMLCSSPropertyNames.in
    )
    list(APPEND WEBCORE_CSS_VALUE_KEYWORDS
        ${OWB_SOURCE_DIR}/BAL/CEHTML/WebCore/Generic/CEHTMLCSSValueKeywords.in
    )
endif(ENABLE_CEHTML)

if(ENABLE_DAE)
list(APPEND WEBCORE_CSS_PROPERTY_NAMES
        ${OWB_SOURCE_DIR}/BAL/DAE/WebCore/Generic/DAECSSPropertyNames.in
    )
endif(ENABLE_DAE)

if(ENABLE_VIDEO)
    list(APPEND HTML_FEATURES --extraDefines ENABLE_VIDEO=1)
    list(APPEND USER_AGENT_STYLE_SHEETS
        ${OWB_SOURCE_DIR}/WebCore/css/mediaControls.css
    )
    if(USE_GRAPHICS_QT)
        list(APPEND USER_AGENT_STYLE_SHEETS
            ${OWB_SOURCE_DIR}/WebCore/css/mediaControlsQt.css
        )
    endif(USE_GRAPHICS_QT)
endif(ENABLE_VIDEO)

if(ENABLE_WML)
    list(APPEND USER_AGENT_STYLE_SHEETS
        ${OWB_SOURCE_DIR}/WebCore/css/wml.css
    )
endif(ENABLE_WML)

set(WEBCORE_GENERATED_DIR ${OWB_BINARY_DIR}/generated_sources/WebCore)
if(NOT EXISTS ${WEBCORE_GENERATED_DIR})
	execute_process(
		COMMAND mkdir -p ${WEBCORE_GENERATED_DIR}/html
        COMMAND mkdir -p ${WEBCORE_GENERATED_DIR}/mathml
		COMMAND mkdir -p ${WEBCORE_GENERATED_DIR}/wml
		COMMAND mkdir -p ${WEBCORE_GENERATED_DIR}/xml
	)
endif(NOT EXISTS ${WEBCORE_GENERATED_DIR})

add_custom_command(
    OUTPUT ${WEBCORE_GENERATED_DIR}/ColorData.c
    COMMAND echo ARGS '\#include <string.h>' > ColorData.c
    COMMAND ${GPERF_EXECUTABLE} ARGS -CDEot -L ANSI-C -k \"*\" -N findColor -D -s 2 < ${OWB_SOURCE_DIR}/WebCore/platform/ColorData.gperf >> ColorData.c
    WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
    DEPENDS ${OWB_SOURCE_DIR}/WebCore/platform/ColorData.gperf
)

add_custom_command(
    OUTPUT ${WEBCORE_GENERATED_DIR}/CSSGrammar.cpp ${WEBCORE_GENERATED_DIR}/CSSGrammar.h
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
    VERBATIM
)

add_custom_command(
    OUTPUT ${WEBCORE_GENERATED_DIR}/CSSPropertyNames.h
    #COMMAND if sort ${WEBCORE_CSS_PROPERTY_NAMES} | uniq -d | grep -E '^[^#]'; then echo 'Duplicate value!'; exit 1; fi
    COMMAND cat ${WEBCORE_CSS_PROPERTY_NAMES} > ${WEBCORE_GENERATED_DIR}/CSSPropertyNames.in
    COMMAND ${PERL_EXECUTABLE} ${OWB_SOURCE_DIR}/WebCore/css/makeprop.pl
    WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
    DEPENDS ${WEBCORE_CSS_PROPERTY_NAMES}
    VERBATIM
)

add_custom_command(
    OUTPUT ${WEBCORE_GENERATED_DIR}/CSSValueKeywords.h
    COMMAND ${PERL_EXECUTABLE} -ne 'print lc' ${WEBCORE_CSS_VALUE_KEYWORDS} > ${WEBCORE_GENERATED_DIR}/CSSValueKeywords.in
    #COMMAND if sort CSSValueKeywords.in | uniq -d | grep -E '^[^#]'; then echo 'Duplicate value!'; exit 1; fi
    COMMAND ${PERL_EXECUTABLE} ${OWB_SOURCE_DIR}/WebCore/css/makevalues.pl
    WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
    DEPENDS ${WEBCORE_CSS_VALUE_KEYWORDS}
)

add_custom_command(
    OUTPUT ${WEBCORE_GENERATED_DIR}/html/DocTypeStrings.cpp
    COMMAND echo '\#include <string.h>' > html/DocTypeStrings.cpp
    COMMAND ${GPERF_EXECUTABLE} -CEot -L ANSI-C -k \"*\" -N findDoctypeEntry -F ,PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards < ${OWB_SOURCE_DIR}/WebCore/html/DocTypeStrings.gperf >> html/DocTypeStrings.cpp
    WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
    DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/html/DocTypeStrings.gperf
)

add_custom_command(
    OUTPUT ${WEBCORE_GENERATED_DIR}/html/HTMLEntityNames.c
    COMMAND echo '\#include <string.h>' > html/HTMLEntityNames.c
    COMMAND ${GPERF_EXECUTABLE} -a -L ANSI-C -C -G -c -o -t --key-positions="*" -N findEntity -D -s 2 < ${OWB_SOURCE_DIR}/WebCore/html/HTMLEntityNames.gperf >> html/HTMLEntityNames.c
    WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
    DEPENDS ${OWB_SOURCE_DIR}/WebCore/html/HTMLEntityNames.gperf
)

if(ENABLE_CEHTML)
    set(CEHTML_ATTRS --attrsCEHTML)
    set(CEHTML_FILE ${OWB_SOURCE_DIR}/BAL/CEHTML/WebCore/Generic/CEHTMLAttributeNames.in)
    set(CEHTML_EXTENSIONS ${CEHTML_ATTRS} ${CEHTML_FILE})
endif(ENABLE_CEHTML)

if(ENABLE_DAE)
    set(DAE_ATTRS --attrsDAE)
    set(DAE_FILE ${OWB_SOURCE_DIR}/BAL/DAE/WebCore/Generic/DAEHTMLAttributeNames.in)
    set(DAE_EXTENSIONS ${DAE_ATTRS} ${DAE_FILE})
endif(ENABLE_DAE)

add_custom_command(
    OUTPUT ${WEBCORE_GENERATED_DIR}/html/HTMLNames.cpp ${WEBCORE_GENERATED_DIR}/html/HTMLNames.h ${WEBCORE_GENERATED_DIR}/html/JSHTMLElementWrapperFactory.cpp ${WEBCORE_GENERATED_DIR}/html/JSHTMLElementWrapperFactory.h ${WEBCORE_GENERATED_DIR}/html/HTMLElementFactory.h ${WEBCORE_GENERATED_DIR}/html/HTMLElementFactory.cpp
    COMMAND ${PERL_EXECUTABLE} -I${OWB_SOURCE_DIR}/WebCore/bindings/scripts ${CMAKE_SOURCE_DIR}/WebCore/dom/make_names.pl --tags ${OWB_SOURCE_DIR}/WebCore/html/HTMLTagNames.in --attrs ${OWB_SOURCE_DIR}/WebCore/html/HTMLAttributeNames.in ${DAE_EXTENSIONS} ${CEHTML_EXTENSIONS} ${HTML_FEATURES} --factory --wrapperFactory --outputDir ${WEBCORE_GENERATED_DIR}/html
    WORKING_DIRECTORY ${OWB_BINARY_DIR}
    DEPENDS ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl ${OWB_SOURCE_DIR}/WebCore/html/HTMLTagNames.in ${OWB_SOURCE_DIR}/WebCore/html/HTMLAttributeNames.in ${DAE_FILE} ${CEHTML_FILE}
    VERBATIM
)

add_custom_command(
    OUTPUT ${WEBCORE_GENERATED_DIR}/tokenizer.cpp
    COMMAND ${FLEX_EXECUTABLE} -t ${OWB_SOURCE_DIR}/WebCore/css/tokenizer.flex | ${PERL_EXECUTABLE} ${OWB_SOURCE_DIR}/WebCore/css/maketokenizer > tokenizer.cpp
    WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
    DEPENDS ${OWB_SOURCE_DIR}/WebCore/css/tokenizer.flex ${OWB_SOURCE_DIR}/WebCore/css/maketokenizer
    VERBATIM
)

if(ENABLE_SVG)
    if(SVG_FLAGS)
        add_custom_command(
            OUTPUT ${WEBCORE_GENERATED_DIR}/JSSVGElementWrapperFactory.cpp ${WEBCORE_GENERATED_DIR}/JSSVGElementWrapperFactory.h ${WEBCORE_GENERATED_DIR}/SVGElementFactory.cpp ${WEBCORE_GENERATED_DIR}/SVGElementFactory.h ${WEBCORE_GENERATED_DIR}/SVGNames.cpp ${WEBCORE_GENERATED_DIR}/SVGNames.h
            COMMAND ${PERL_EXECUTABLE} -I${OWB_SOURCE_DIR}/WebCore/bindings/scripts ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl --tags ${OWB_SOURCE_DIR}/WebCore/svg/svgtags.in --attrs ${OWB_SOURCE_DIR}/WebCore/svg/svgattrs.in --extraDefines \"${SVG_FEATURES}\" --factory --wrapperFactory --outputDir ${WEBCORE_GENERATED_DIR}
            WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
            DEPENDS ${OWB_SOURCE_DIR}/WebCore/svg/svgtags.in ${OWB_SOURCE_DIR}/WebCore/svg/svgattrs.in
        )
    else(SVG_FLAGS)
        add_custom_command(
            OUTPUT ${WEBCORE_GENERATED_DIR}/JSSVGElementWrapperFactory.cpp ${WEBCORE_GENERATED_DIR}/JSSVGElementWrapperFactory.h ${WEBCORE_GENERATED_DIR}/SVGElementFactory.cpp ${WEBCORE_GENERATED_DIR}/SVGElementFactory.h ${WEBCORE_GENERATED_DIR}/SVGNames.cpp ${WEBCORE_GENERATED_DIR}/SVGNames.h
            COMMAND ${PERL_EXECUTABLE} -I${OWB_SOURCE_DIR}/WebCore/bindings/scripts ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl --tags ${OWB_SOURCE_DIR}/WebCore/svg/svgtags.in --attrs ${OWB_SOURCE_DIR}/WebCore/svg/svgattrs.in --factory --wrapperFactory --outputDir ${WEBCORE_GENERATED_DIR}
            WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
            DEPENDS ${OWB_SOURCE_DIR}/WebCore/svg/svgtags.in ${OWB_SOURCE_DIR}/WebCore/svg/svgattrs.in
            VERBATIM
        )
    endif(SVG_FLAGS)
    add_custom_command(
        OUTPUT ${WEBCORE_GENERATED_DIR}/XLinkNames.cpp ${WEBCORE_GENERATED_DIR}/XLinkNames.h
        COMMAND ${PERL_EXECUTABLE}
        -I${OWB_SOURCE_DIR}/WebCore/bindings/scripts ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl --attrs ${OWB_SOURCE_DIR}/WebCore/svg/xlinkattrs.in --outputDir ${WEBCORE_GENERATED_DIR}
        WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
        DEPENDS ${OWB_SOURCE_DIR}/WebCore/svg/xlinkattrs.in
        VERBATIM
    )
endif(ENABLE_SVG)

add_custom_command(
    OUTPUT ${WEBCORE_GENERATED_DIR}/UserAgentStyleSheetsData.cpp ${WEBCORE_GENERATED_DIR}/UserAgentStyleSheets.h
    COMMAND ${PERL_EXECUTABLE} ${OWB_SOURCE_DIR}/WebCore/css/make-css-file-arrays.pl UserAgentStyleSheets.h UserAgentStyleSheetsData.cpp ${USER_AGENT_STYLE_SHEETS}
    WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
    DEPENDS ${OWB_SOURCE_DIR}/WebCore/css/make-css-file-arrays.pl ${USER_AGENT_STYLE_SHEETS}
    VERBATIM
)

if(ENABLE_MATHML)
add_custom_command(OUTPUT ${WEBCORE_GENERATED_DIR}/mathml/MathMLElementFactory.cpp ${WEBCORE_GENERATED_DIR}/mathml/MathMLElementFactory.h
    ${WEBCORE_GENERATED_DIR}/mathml/MathMLNames.cpp ${WEBCORE_GENERATED_DIR}/mathml/MathMLNames.h
    COMMAND ${PERL_EXECUTABLE} -I${OWB_SOURCE_DIR}/WebCore/bindings/scripts ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl --tags  ${OWB_SOURCE_DIR}/WebCore/mathml/mathtags.in --factory --wrapperFactory --outputDir ${WEBCORE_GENERATED_DIR}/mathml
    WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
    DEPENDS ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl ${OWB_SOURCE_DIR}/WebCore/mathml/mathtags.in
    VERBATIM
)
endif(ENABLE_MATHML)

add_custom_command(OUTPUT ${WEBCORE_GENERATED_DIR}/xml/XMLNSNames.cpp ${WEBCORE_GENERATED_DIR}/xml/XMLNSNames.h
    COMMAND ${PERL_EXECUTABLE} -I${OWB_SOURCE_DIR}/WebCore/bindings/scripts ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl --attrs ${OWB_SOURCE_DIR}/WebCore/xml/xmlnsattrs.in --output xml
    WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
    DEPENDS ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl ${OWB_SOURCE_DIR}/WebCore/xml/xmlnsattrs.in
    VERBATIM
)

add_custom_command(OUTPUT ${WEBCORE_GENERATED_DIR}/xml/XMLNames.cpp ${WEBCORE_GENERATED_DIR}/xml/XMLNames.h
    COMMAND ${PERL_EXECUTABLE} -I${OWB_SOURCE_DIR}/WebCore/bindings/scripts ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl --attrs ${OWB_SOURCE_DIR}/WebCore/xml/xmlattrs.in --output xml
    WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
    DEPENDS ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl ${OWB_SOURCE_DIR}/WebCore/xml/xmlattrs.in
    VERBATIM
)

if(ENABLE_WML)
    add_custom_command(OUTPUT ${WEBCORE_GENERATED_DIR}/wml/WMLNames.cpp ${WEBCORE_GENERATED_DIR}/wml/WMLNames.h ${WEBCORE_GENERATED_DIR}/wml/WMLElementFactory.cpp
        COMMAND ${PERL_EXECUTABLE} -I${OWB_SOURCE_DIR}/WebCore/bindings/scripts ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl --tags ${OWB_SOURCE_DIR}/WebCore/wml/WMLTagNames.in --attrs ${OWB_SOURCE_DIR}/WebCore/wml/WMLAttributeNames.in --factory --wrapperFactory --output wml
        WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
        DEPENDS ${OWB_SOURCE_DIR}/WebCore/dom/make_names.pl ${OWB_SOURCE_DIR}/WebCore/wml/WMLTagNames.in ${OWB_SOURCE_DIR}/WebCore/wml/WMLAttributeNames.in
        VERBATIM
	)
endif(ENABLE_WML)

if(ENABLE_XPATH)
    add_custom_command(OUTPUT ${WEBCORE_GENERATED_DIR}/XPathGrammar.cpp ${WEBCORE_GENERATED_DIR}/XPathGrammar.h
        COMMAND ${BISON_EXECUTABLE} -d -p xpathyy ${OWB_SOURCE_DIR}/WebCore/xml/XPathGrammar.y -o XPathGrammar.cpp
        COMMAND touch XPathGrammar.cpp.h XPathGrammar.hpp
        COMMAND echo '\#ifndef XPathGrammar_h' > XPathGrammar.h
        COMMAND echo '\#define XPathGrammar_h' >> XPathGrammar.h
        COMMAND cat XPathGrammar.cpp.h XPathGrammar.hpp >> ${CMAKE_BINARY_DIR}/generated_sources/WebCore/XPathGrammar.h
        COMMAND echo '\#endif' >> XPathGrammar.h
        COMMAND rm -f XPathGrammar.cpp.h XPathGrammar.hpp
        WORKING_DIRECTORY ${WEBCORE_GENERATED_DIR}
        DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/xml/XPathGrammar.y
	)
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
    ${WEBCORE_GENERATED_DIR}/html/HTMLElementFactory.cpp
    ${WEBCORE_GENERATED_DIR}/html/HTMLEntityNames.c
    ${WEBCORE_GENERATED_DIR}/html/HTMLNames.cpp
    ${WEBCORE_GENERATED_DIR}/html/JSHTMLElementWrapperFactory.cpp
    ${WEBCORE_GENERATED_DIR}/tokenizer.cpp
    ${WEBCORE_GENERATED_DIR}/UserAgentStyleSheetsData.cpp
    ${WEBCORE_GENERATED_DIR}/xml/XMLNSNames.cpp
    ${WEBCORE_GENERATED_DIR}/xml/XMLNames.cpp
)

if(ENABLE_MATHML)
    list(APPEND WEBCORE_SRC
        ${WEBCORE_GENERATED_DIR}/mathml/MathMLElementFactory.cpp
        ${WEBCORE_GENERATED_DIR}/mathml/MathMLNames.cpp
    )
endif(ENABLE_MATHML)

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
if(NOT IGNORE_CUSTOMER)
    include(${OWB_SOURCE_DIR}/BAL/Customer/CMakeLists.txt OPTIONAL)
endif(NOT IGNORE_CUSTOMER)
include(accessibility/CMakeLists.txt)
include(bindings/js/CMakeLists.txt)
include(bridge/CMakeLists.txt)
include(css/CMakeLists.txt)
include(dom/CMakeLists.txt)
include(editing/CMakeLists.txt)
include(history/CMakeLists.txt)
include(html/CMakeLists.txt)
include(html/canvas/CMakeLists.txt)
include(inspector/CMakeLists.txt)
include(loader/CMakeLists.txt)
include(loader/icon/CMakeLists.txt)
include(mathml/CMakeLists.txt)
include(notifications/CMakeLists.txt)
include(page/CMakeLists.txt)
include(platform/CMakeLists.txt)
include(plugins/CMakeLists.txt)
include(rendering/CMakeLists.txt)
include(rendering/style/CMakeLists.txt)
include(storage/CMakeLists.txt)
include(svg/CMakeLists.txt)
include(websockets/CMakeLists.txt)
include(workers/CMakeLists.txt)
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

if(ENABLE_MATHML)
    list(APPEND WEBCORE_INCLUDE_DIRS
        ${OWB_SOURCE_DIR}/WebCore/mathml
    )
endif(ENABLE_MATHML)

include_directories(
    ${BASE_INCLUDE_DIRS}
    ${WTF_INCLUDE_DIRS}
    ${BAL_INCLUDE_DIRS}
    ${JAVASCRIPTCORE_INCLUDE_DIRS}
    ${WEBCORE_INCLUDE_DIRS}
    ${WEBCORE_GENERATED_DIR}
    ${WEBCORE_GENERATED_DIR}/html
    ${WEBCORE_GENERATED_DIR}/mathml
    ${WEBCORE_GENERATED_DIR}/wml
    ${WEBCORE_GENERATED_DIR}/xml
    ${WEBKIT_INCLUDE_DIRS}
    ${EXTERNAL_DEPS_INCLUDE_DIRS})

include(AddWebCoreCompilerFlags)

set_source_files_properties(${WEBCORE_SRC}
    PROPERTIES COMPILE_FLAGS "-fno-rtti -fno-exceptions"
)
add_library(webcore STATIC ${WEBCORE_SRC})
add_dependencies(webcore jsc)
