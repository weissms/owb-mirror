##############################
##############################
##############################
#GeneratedSources
##############################
##############################
##############################

ADD_DEFINITIONS (-include ${CMAKE_SOURCE_DIR}/BAL/Base/config.h)

SET (FEATURE_DEFINES_JAVASCRIPT LANGUAGE_JAVASCRIPT)

IF (WEBKIT_ENABLE_XPATH)
    SET (IDL_BINDINGS_XPATH
        xml/XPathException.idl
        xml/XPathExpression.idl
        xml/XPathNSResolver.idl
        xml/XPathEvaluator.idl
        xml/XPathResult.idl
    )
    SET (FEATURE_DEFINES_JAVASCRIPT ${FEATURE_DEFINES_JAVASCRIPT} ENABLE_XPATH)
ENDIF (WEBKIT_ENABLE_XPATH)

IF (WEBKIT_ENABLE_XSLT)
    SET (FEATURE_DEFINES_JAVASCRIPT ${FEATURE_DEFINES_JAVASCRIPT} ENABLE_XSLT)
ENDIF (WEBKIT_ENABLE_XSLT)

# IF (WEBKIT_USE_XBL_SUPPORT)
#    SET (FEATURE_DEFINES_JAVASCRIPT ${FEATURE_DEFINES_JAVASCRIPT} ENABLE_XBL)
# ENDIF (WEBKIT_USE_XBL_SUPPORT)

IF (WEBKIT_USE_CROSS_DOCUMENT_MESSAGING)
	SET (FEATURE_DEFINES_JAVASCRIPT ${FEATURE_DEFINES_JAVASCRIPT} ENABLE_CROSS_DOCUMENT_MESSAGING=1)
ENDIF (WEBKIT_USE_CROSS_DOCUMENT_MESSAGING)

IF (WEBKIT_USE_DATABASE)
    SET (FEATURE_DEFINES_JAVASCRIPT ${FEATURE_DEFINES_JAVASCRIPT} ENABLE_DATABASE=1)
ENDIF (WEBKIT_USE_DATABASE)

IF (WEBKIT_USE_HTML5_VIDEO)
    SET (FEATURE_DEFINES_JAVASCRIPT ${FEATURE_DEFINES_JAVASCRIPT} ENABLE_VIDEO=1)
    SET (HTML_FEATURES ${HTML_FEATURES} ENABLE_VIDEO=1)
ENDIF (WEBKIT_USE_HTML5_VIDEO)

SET (IDL_BINDINGS
    ${IDL_BINDINGS}

    css/Counter.idl
    css/CSSCharsetRule.idl
    css/CSSFontFaceRule.idl
    css/CSSImportRule.idl
    css/CSSMediaRule.idl
    css/CSSPageRule.idl
    css/CSSPrimitiveValue.idl
    css/CSSRule.idl
    css/CSSRuleList.idl
    css/CSSStyleDeclaration.idl
    css/CSSStyleRule.idl
    css/CSSStyleSheet.idl
#   css/CSSUnknownRule.idl
    css/CSSValue.idl
    css/CSSValueList.idl
    css/CSSVariablesDeclaration.idl
    css/CSSVariablesRule.idl
    css/MediaList.idl
#   css/RGBColor.idl
    css/Rect.idl
    css/StyleSheet.idl
    css/StyleSheetList.idl
    css/WebKitCSSKeyframeRule.idl
    css/WebKitCSSKeyframesRule.idl
    css/WebKitCSSTransformValue.idl

    dom/Attr.idl
    dom/CDATASection.idl
    dom/CharacterData.idl
    dom/Clipboard.idl
    dom/Comment.idl
    dom/DOMCoreException.idl
    dom/DOMImplementation.idl
    dom/Document.idl
    dom/DocumentFragment.idl
    dom/DocumentType.idl
    dom/Element.idl
    dom/Entity.idl
    dom/EntityReference.idl
    dom/Event.idl
    dom/EventException.idl
#   dom/EventListener.idl
#   dom/EventTarget.idl
    dom/KeyboardEvent.idl
    dom/MessageEvent.idl
    dom/MouseEvent.idl
    dom/MutationEvent.idl
    dom/NamedNodeMap.idl
    dom/Node.idl
    dom/NodeFilter.idl
    dom/NodeIterator.idl
    dom/NodeList.idl
    dom/Notation.idl
    dom/OverflowEvent.idl
    dom/ProcessingInstruction.idl
    dom/ProgressEvent.idl
    dom/Range.idl
    dom/RangeException.idl
    dom/Text.idl
    dom/TextEvent.idl
    dom/TreeWalker.idl
    dom/UIEvent.idl
    dom/WebKitAnimationEvent.idl
    dom/WebKitTransitionEvent.idl
    dom/WheelEvent.idl

    html/CanvasGradient.idl
    html/CanvasPattern.idl
    html/CanvasPixelArray.idl
    html/CanvasRenderingContext2D.idl
    html/File.idl
    html/FileList.idl
    html/HTMLAnchorElement.idl
    html/HTMLAppletElement.idl
    html/HTMLAreaElement.idl
    html/HTMLAudioElement.idl
    html/HTMLBRElement.idl
    html/HTMLBaseElement.idl
    html/HTMLBaseFontElement.idl
    html/HTMLBlockquoteElement.idl
    html/HTMLBodyElement.idl
    html/HTMLButtonElement.idl
    html/HTMLCanvasElement.idl
    html/HTMLCollection.idl
    html/HTMLDListElement.idl
    html/HTMLDirectoryElement.idl
    html/HTMLDivElement.idl
    html/HTMLDocument.idl
    html/HTMLElement.idl
    html/HTMLEmbedElement.idl
    html/HTMLFieldSetElement.idl
    html/HTMLFontElement.idl
    html/HTMLFormElement.idl
    html/HTMLFrameElement.idl
    html/HTMLFrameSetElement.idl
    html/HTMLHRElement.idl
    html/HTMLHeadElement.idl
    html/HTMLHeadingElement.idl
    html/HTMLHtmlElement.idl
    html/HTMLIFrameElement.idl
    html/HTMLImageElement.idl
    html/HTMLInputElement.idl
    html/HTMLIsIndexElement.idl
    html/HTMLLIElement.idl
    html/HTMLLabelElement.idl
    html/HTMLLegendElement.idl
    html/HTMLLinkElement.idl
    html/HTMLMapElement.idl
    html/HTMLMarqueeElement.idl
    html/HTMLMediaElement.idl
    html/HTMLMenuElement.idl
    html/HTMLMetaElement.idl
    html/HTMLModElement.idl
    html/HTMLOListElement.idl
    html/HTMLObjectElement.idl
    html/HTMLOptGroupElement.idl
    html/HTMLOptionElement.idl
    html/HTMLOptionsCollection.idl
    html/HTMLParagraphElement.idl
    html/HTMLParamElement.idl
    html/HTMLPreElement.idl
    html/HTMLQuoteElement.idl
    html/HTMLScriptElement.idl
    html/HTMLSelectElement.idl
    html/HTMLSourceElement.idl
    html/HTMLStyleElement.idl
    html/HTMLTableCaptionElement.idl
    html/HTMLTableCellElement.idl
    html/HTMLTableColElement.idl
    html/HTMLTableElement.idl
    html/HTMLTableRowElement.idl
    html/HTMLTableSectionElement.idl
    html/HTMLTextAreaElement.idl
    html/HTMLTitleElement.idl
    html/HTMLUListElement.idl
    html/HTMLVideoElement.idl
    html/ImageData.idl
    html/MediaError.idl
    html/TextMetrics.idl
    html/TimeRanges.idl
    html/VoidCallback.idl

    page/BarInfo.idl
    page/Console.idl
    page/DOMSelection.idl
    page/DOMWindow.idl
    page/History.idl
    page/Location.idl
    page/Navigator.idl
    page/Screen.idl

    plugins/MimeType.idl
    plugins/PluginArray.idl
    plugins/Plugin.idl
    plugins/MimeTypeArray.idl

    xml/DOMParser.idl
	xml/XMLHttpRequest.idl
    xml/XMLHttpRequestException.idl
	xml/XMLHttpRequestProgressEvent.idl
    xml/XMLHttpRequestUpload.idl
    xml/XMLSerializer.idl
)

IF (WEBKIT_ENABLE_INSPECTOR_CONTROLLER)
    SET (IDL_BINDINGS
        ${IDL_BINDINGS}
        page/JavaScriptCallFrame.idl
    )
ENDIF (WEBKIT_ENABLE_INSPECTOR_CONTROLLER)

IF (WEBKIT_ENABLE_XSLT)
    SET (IDL_BINDINGS
        ${IDL_BINDINGS}
    	xml/XSLTProcessor.idl
    )
ENDIF (WEBKIT_ENABLE_XSLT)

IF (WEBKIT_USE_DATABASE)
    SET (IDL_BINDINGS
        ${IDL_BINDINGS}
        storage/Database.idl
        storage/SQLError.idl
        storage/SQLResultSet.idl
        storage/SQLResultSetRowList.idl
        storage/SQLTransaction.idl
    )
ENDIF (WEBKIT_USE_DATABASE)

IF (WEBKIT_USE_DOM_STORAGE)
    SET (IDL_BINDINGS
        ${IDL_BINDINGS}
        storage/Storage.idl
        storage/StorageEvent.idl
    )
ENDIF (WEBKIT_USE_DOM_STORAGE)

IF (WEBKIT_OFFLINE_WEB_APPLICATIONS)
    SET (IDL_BINDINGS
        ${IDL_BINDINGS}
	loader/appcache/DOMApplicationCache.idl
    )
ENDIF (WEBKIT_OFFLINE_WEB_APPLICATIONS)

IF (WEBKIT_USE_SVG)
    SET (IDL_BINDINGS_SVG
        svg/SVGAElement.idl
        svg/SVGAltGlyphElement.idl
        svg/SVGAngle.idl
        svg/SVGAnimateColorElement.idl
        svg/SVGAnimateElement.idl
        svg/SVGAnimateTransformElement.idl
        svg/SVGAnimatedAngle.idl
        svg/SVGAnimatedBoolean.idl
        svg/SVGAnimatedEnumeration.idl
        svg/SVGAnimatedInteger.idl
        svg/SVGAnimatedLength.idl
        svg/SVGAnimatedLengthList.idl
        svg/SVGAnimatedNumber.idl
        svg/SVGAnimatedNumberList.idl
        svg/SVGAnimatedPreserveAspectRatio.idl
        svg/SVGAnimatedRect.idl
        svg/SVGAnimatedString.idl
        svg/SVGAnimatedTransformList.idl
        svg/SVGAnimationElement.idl
        svg/SVGCircleElement.idl
        svg/SVGClipPathElement.idl
        svg/SVGColor.idl
        svg/SVGComponentTransferFunctionElement.idl
        svg/SVGCursorElement.idl
        svg/SVGDefinitionSrcElement.idl
        svg/SVGDefsElement.idl
        svg/SVGDescElement.idl
        svg/SVGDocument.idl
        svg/SVGElement.idl
        svg/SVGElementInstance.idl
        svg/SVGElementInstanceList.idl
        svg/SVGEllipseElement.idl
        svg/SVGException.idl
        svg/SVGFEBlendElement.idl
        svg/SVGFEColorMatrixElement.idl
        svg/SVGFEComponentTransferElement.idl
        svg/SVGFECompositeElement.idl
        svg/SVGFEDiffuseLightingElement.idl
        svg/SVGFEDisplacementMapElement.idl
        svg/SVGFEDistantLightElement.idl
        svg/SVGFEFloodElement.idl
        svg/SVGFEFuncAElement.idl
        svg/SVGFEFuncBElement.idl
        svg/SVGFEFuncGElement.idl
        svg/SVGFEFuncRElement.idl
        svg/SVGFEGaussianBlurElement.idl
        svg/SVGFEImageElement.idl
        svg/SVGFEMergeElement.idl
        svg/SVGFEMergeNodeElement.idl
        svg/SVGFEOffsetElement.idl
        svg/SVGFEPointLightElement.idl
        svg/SVGFESpecularLightingElement.idl
        svg/SVGFESpotLightElement.idl
        svg/SVGFETileElement.idl
        svg/SVGFETurbulenceElement.idl
        svg/SVGFilterElement.idl
        svg/SVGFontElement.idl
        svg/SVGFontFaceElement.idl
        svg/SVGFontFaceFormatElement.idl
        svg/SVGFontFaceNameElement.idl
        svg/SVGFontFaceSrcElement.idl
        svg/SVGFontFaceUriElement.idl
        svg/SVGForeignObjectElement.idl
        svg/SVGGElement.idl
        svg/SVGGlyphElement.idl
        svg/SVGGradientElement.idl
        svg/SVGImageElement.idl
        svg/SVGHKernElement.idl
        svg/SVGLength.idl
        svg/SVGLengthList.idl
        svg/SVGLineElement.idl
        svg/SVGLinearGradientElement.idl
        svg/SVGMarkerElement.idl
        svg/SVGMaskElement.idl
        svg/SVGMatrix.idl
        svg/SVGMetadataElement.idl
        svg/SVGMissingGlyphElement.idl
        svg/SVGNumber.idl
        svg/SVGNumberList.idl
        svg/SVGPaint.idl
        svg/SVGPathElement.idl
        svg/SVGPathSeg.idl
        svg/SVGPathSegArcAbs.idl
        svg/SVGPathSegArcRel.idl
        svg/SVGPathSegClosePath.idl
        svg/SVGPathSegCurvetoCubicAbs.idl
        svg/SVGPathSegCurvetoCubicRel.idl
        svg/SVGPathSegCurvetoCubicSmoothAbs.idl
        svg/SVGPathSegCurvetoCubicSmoothRel.idl
        svg/SVGPathSegCurvetoQuadraticAbs.idl
        svg/SVGPathSegCurvetoQuadraticRel.idl
        svg/SVGPathSegCurvetoQuadraticSmoothAbs.idl
        svg/SVGPathSegCurvetoQuadraticSmoothRel.idl
        svg/SVGPathSegLinetoAbs.idl
        svg/SVGPathSegLinetoHorizontalAbs.idl
        svg/SVGPathSegLinetoHorizontalRel.idl
        svg/SVGPathSegLinetoRel.idl
        svg/SVGPathSegLinetoVerticalAbs.idl
        svg/SVGPathSegLinetoVerticalRel.idl
        svg/SVGPathSegList.idl
        svg/SVGPathSegMovetoAbs.idl
        svg/SVGPathSegMovetoRel.idl
        svg/SVGPatternElement.idl
        svg/SVGPoint.idl
        svg/SVGPointList.idl
        svg/SVGPolygonElement.idl
        svg/SVGPolylineElement.idl
        svg/SVGPreserveAspectRatio.idl
        svg/SVGRadialGradientElement.idl
        svg/SVGRect.idl
        svg/SVGRectElement.idl
        svg/SVGRenderingIntent.idl
        svg/SVGSVGElement.idl
        svg/SVGScriptElement.idl
        svg/SVGSetElement.idl
        svg/SVGStopElement.idl
        svg/SVGStringList.idl
        svg/SVGStyleElement.idl
        svg/SVGSwitchElement.idl
        svg/SVGSymbolElement.idl
        svg/SVGTRefElement.idl
        svg/SVGTSpanElement.idl
        svg/SVGTextContentElement.idl
        svg/SVGTextElement.idl
        svg/SVGTextPathElement.idl
        svg/SVGTextPositioningElement.idl
        svg/SVGTitleElement.idl
        svg/SVGTransform.idl
        svg/SVGTransformList.idl
        svg/SVGUnitTypes.idl
        svg/SVGUseElement.idl
        svg/SVGViewElement.idl
        svg/SVGZoomEvent.idl
    )
    
    SET (FEATURE_DEFINES_JAVASCRIPT ${FEATURE_DEFINES_JAVASCRIPT} ENABLE_SVG=1)
    SET (FEATURE_DEFINES_JAVASCRIPT ${FEATURE_DEFINES_JAVASCRIPT} ENABLE_SVG_USE=1)
    SET (FEATURE_DEFINES_JAVASCRIPT ${FEATURE_DEFINES_JAVASCRIPT} ENABLE_SVG_FOREIGN_OBJECT=1)
    SET (FEATURE_DEFINES_JAVASCRIPT ${FEATURE_DEFINES_JAVASCRIPT} ENABLE_SVG_FONTS=1)
    SET (FEATURE_DEFINES_JAVASCRIPT ${FEATURE_DEFINES_JAVASCRIPT} ENABLE_SVG_FILTERS=1)
    SET (FEATURE_DEFINES_JAVASCRIPT ${FEATURE_DEFINES_JAVASCRIPT} ENABLE_SVG_AS_IMAGE=1)
    SET (FEATURE_DEFINES_JAVASCRIPT ${FEATURE_DEFINES_JAVASCRIPT} ENABLE_SVG_ANIMATION=1)
ENDIF (WEBKIT_USE_SVG)

SET (IDL_BINDINGS
    ${IDL_BINDINGS}
    ${IDL_BINDINGS_SVG}
    ${IDL_BINDINGS_XPATH}
)

#FIXME : pass this variable from HtmlExt
IF (WEBKIT_USE_HTML_EXTENSION)
    SET (IDL_BINDINGS_EXT
        ../HtmlExt/HtmlElement/HTMLPleyoElement.idl
    )
    SET (IDL_BINDINGS
        ${IDL_BINDINGS}
	${IDL_BINDINGS_EXT}
    )
ENDIF (WEBKIT_USE_HTML_EXTENSION)

# create these files at build time on demand, NOT everytime at cmake-time, Alex
FOREACH (idl ${IDL_BINDINGS})
   GET_FILENAME_COMPONENT (_basename ${idl} NAME_WE)
   ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebCore/JS${_basename}.cpp ${CMAKE_BINARY_DIR}/generated_sources/WebCore/JS${_basename}.h
      COMMAND ${PERL_EXECUTABLE} -I${CMAKE_SOURCE_DIR}/WebCore/bindings/scripts ${CMAKE_SOURCE_DIR}/WebCore/bindings/scripts/generate-bindings.pl  --defines \"${FEATURE_DEFINES_JAVASCRIPT}\" --generator JS --include ${CMAKE_SOURCE_DIR}/WebCore/dom --include ${CMAKE_SOURCE_DIR}/WebCore/html --include ${CMAKE_SOURCE_DIR}/WebCore/xml --include ${CMAKE_SOURCE_DIR}/WebCore/svg --outputdir ${CMAKE_BINARY_DIR}/generated_sources/WebCore ${CMAKE_SOURCE_DIR}/WebCore/${idl}
      DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/${idl} ${CMAKE_SOURCE_DIR}/WebCore/bindings/scripts/CodeGenerator.pm ${CMAKE_SOURCE_DIR}/WebCore/bindings/scripts/CodeGeneratorJS.pm)
   SET (GENERATED_SRC ${GENERATED_SRC} ${CMAKE_BINARY_DIR}/generated_sources/WebCore/JS${_basename}.cpp ${CMAKE_BINARY_DIR}/generated_sources/WebCore/JS${_basename}.h )
ENDFOREACH (idl)

MACRO (CREATE_GENERATED_CPP_HELPER  _inlName)
    GET_FILENAME_COMPONENT (_baseName ${_inlName} NAME_WE)
    IF (NOT EXISTS ${CMAKE_BINARY_DIR}/generated_sources/WebCore/${_baseName}.cpp)
        FILE (WRITE ${CMAKE_BINARY_DIR}/generated_sources/WebCore/${_baseName}.cpp
"/* This file is autogenerated by CMake.                                       *
 * Do not edit, changes will be lost.                                         *
 * With the CMake buildsystem giving generated-but-not-compiled-on-their-own  *
 * files the suffix \".cpp\" is not recommended, because it breaks the          *
 * dependency handling. So the actual generated file has the suffix \".inl\"    *
 * and this file here only exists as a helper.                                */
#include \"${CMAKE_BINARY_DIR}/${_inlName}\"\n")
    ENDIF (NOT EXISTS ${CMAKE_BINARY_DIR}/generated_sources/WebCore/${_baseName}.cpp)
    SET (GENERATED_SRC ${GENERATED_SRC} ${CMAKE_BINARY_DIR}/${_inlName})
ENDMACRO (CREATE_GENERATED_CPP_HELPER  _inlName)

# NOTE: older versions of bison do not inject an inclusion guard, so we do it
ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSGrammar.cpp ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSGrammar.h
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND mkdir -p generated_sources/WebCore
    COMMAND ${BISON_EXECUTABLE} -d -p cssyy ${CMAKE_SOURCE_DIR}/WebCore/css/CSSGrammar.y -o CSSGrammar.tab.c
    COMMAND echo '\#ifndef CSSGRAMMAR_H' > CSSGrammar.h
    COMMAND echo '\#define CSSGRAMMAR_H' >> CSSGrammar.h 
    COMMAND mv CSSGrammar.tab.c ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSGrammar.cpp
    COMMAND cat CSSGrammar.h CSSGrammar.tab.h > ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSGrammar.h
    COMMAND echo '\#endif' >> ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSGrammar.h 
    COMMAND rm -f CSSGrammar.h
    DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/css/CSSGrammar.y
)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html/HTMLEntityNames.c
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND mkdir -p generated_sources/WebCore/html
    COMMAND echo '\#include <string.h>' > generated_sources/WebCore/html/HTMLEntityNames.c
    COMMAND ${GPERF_EXECUTABLE} -a -L ANSI-C -C -G -c -o -t -k '*' -N findEntity -D -s 2 < ${CMAKE_SOURCE_DIR}/WebCore/html/HTMLEntityNames.gperf >> generated_sources/WebCore/html/HTMLEntityNames.c
    DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/html/HTMLEntityNames.gperf
)

IF (NOT WEBKIT_USE_HTML_EXTENSION)
    IF (HTML_FEATURES)
        ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html/HTMLNames.cpp ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html/HTMLNames.h ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html/JSHTMLElementWrapperFactory.cpp ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html/JSHTMLElementWrapperFactory.h
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMAND mkdir -p ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html
            COMMAND ${PERL_EXECUTABLE} -I${CMAKE_SOURCE_DIR}/WebCore/bindings/scripts ${CMAKE_SOURCE_DIR}/WebCore/dom/make_names.pl --tags ${CMAKE_SOURCE_DIR}/WebCore/html/HTMLTagNames.in --attrs ${CMAKE_SOURCE_DIR}/WebCore/html/HTMLAttributeNames.in --extraDefines \"${HTML_FEATURES}\" --namespace HTML --namespacePrefix xhtml --cppNamespace WebCore --namespaceURI "http://www.w3.org/1999/xhtml" --factory --wrapperFactory --attrsNullNamespace --output generated_sources/WebCore/html
            DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/html/HTMLTagNames.in ${CMAKE_SOURCE_DIR}/WebCore/html/HTMLAttributeNames.in
        )
    ELSE (HTML_FEATURES)
        ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html/HTMLNames.cpp ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html/HTMLNames.h ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html/JSHTMLElementWrapperFactory.cpp ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html/JSHTMLElementWrapperFactory.h
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMAND mkdir -p ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html
            COMMAND ${PERL_EXECUTABLE} -I${CMAKE_SOURCE_DIR}/WebCore/bindings/scripts ${CMAKE_SOURCE_DIR}/WebCore/dom/make_names.pl --tags ${CMAKE_SOURCE_DIR}/WebCore/html/HTMLTagNames.in --attrs ${CMAKE_SOURCE_DIR}/WebCore/html/HTMLAttributeNames.in --namespace HTML --namespacePrefix xhtml --cppNamespace WebCore --namespaceURI "http://www.w3.org/1999/xhtml" --factory --wrapperFactory --attrsNullNamespace --output generated_sources/WebCore/html
            DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/html/HTMLTagNames.in ${CMAKE_SOURCE_DIR}/WebCore/html/HTMLAttributeNames.in
        )
    ENDIF (HTML_FEATURES)
    
    IF (WEBKIT_USE_SVG)
        ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSValueKeywords.h ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSValueKeywords.c
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/generated_sources/WebCore
            COMMAND cp ${CMAKE_SOURCE_DIR}/WebCore/css/CSSValueKeywords.in ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSValueKeywords.in
            COMMAND perl -ne 'print lc' ${CMAKE_SOURCE_DIR}/WebCore/css/SVGCSSValueKeywords.in > ${CMAKE_BINARY_DIR}/generated_sources/WebCore/SVGCSSValueKeywords.in
            COMMAND cat ${CMAKE_BINARY_DIR}/generated_sources/WebCore/SVGCSSValueKeywords.in >> ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSValueKeywords.in
            COMMAND perl ${CMAKE_SOURCE_DIR}/WebCore/css/makevalues.pl
            DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/css/SVGCSSValueKeywords.in
        )
        
        ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSPropertyNames.cpp ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSPropertyNames.h
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/generated_sources/WebCore
            COMMAND cp ${CMAKE_SOURCE_DIR}/WebCore/css/CSSPropertyNames.in ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSPropertyNames.in
            COMMAND cat ${CMAKE_SOURCE_DIR}/WebCore/css/SVGCSSPropertyNames.in >> ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSPropertyNames.in
            COMMAND cat ${CMAKE_SOURCE_DIR}/WebCore/css/DashboardSupportCSSPropertyNames.in >> ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSPropertyNames.in
            COMMAND perl ${CMAKE_SOURCE_DIR}/WebCore/css/makeprop.pl
            DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/css/CSSPropertyNames.in
        )
    
    ELSE (WEBKIT_USE_SVG)
    
        ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSValueKeywords.h ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSValueKeywords.c
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/generated_sources/WebCore
            COMMAND cp ${CMAKE_SOURCE_DIR}/WebCore/css/CSSValueKeywords.in ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSValueKeywords.in
            COMMAND perl ${CMAKE_SOURCE_DIR}/WebCore/css/makevalues.pl
            DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/css/CSSValueKeywords.in
        )
        
        ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSPropertyNames.cpp ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSPropertyNames.h
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/generated_sources/WebCore
            COMMAND cp ${CMAKE_SOURCE_DIR}/WebCore/css/CSSPropertyNames.in ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSPropertyNames.in
            COMMAND cat ${CMAKE_SOURCE_DIR}/WebCore/css/DashboardSupportCSSPropertyNames.in >> ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSPropertyNames.in
            COMMAND perl ${CMAKE_SOURCE_DIR}/WebCore/css/makeprop.pl
            DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/css/CSSPropertyNames.in
        )
    ENDIF (WEBKIT_USE_SVG)
     
ELSE (NOT WEBKIT_USE_HTML_EXTENSION)
    INCLUDE (${CMAKE_SOURCE_DIR}/HtmlExt/Resources/CMakeLists.txt)
ENDIF (NOT WEBKIT_USE_HTML_EXTENSION)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebCore/xml/XMLNames.cpp ${CMAKE_BINARY_DIR}/generated_sources/WebCore/xml/XMLNames.h
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/generated_sources/WebCore/xml
    COMMAND ${PERL_EXECUTABLE} -I${CMAKE_SOURCE_DIR}/WebCore/bindings/scripts ${CMAKE_SOURCE_DIR}/WebCore/dom/make_names.pl --attrs ${CMAKE_SOURCE_DIR}/WebCore/xml/xmlattrs.in --namespace XML --cppNamespace WebCore --namespaceURI "http://www.w3.org/XML/1998/namespace" --output generated_sources/WebCore/xml
    DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/xml/xmlattrs.in
)



ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebCore/tokenizer.inl
    COMMAND ${FLEX_EXECUTABLE} -t ${CMAKE_SOURCE_DIR}/WebCore/css/tokenizer.flex | ${PERL_EXECUTABLE} ${CMAKE_SOURCE_DIR}/WebCore/css/maketokenizer > ${CMAKE_BINARY_DIR}/generated_sources/WebCore/tokenizer.inl
    DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/css/tokenizer.flex
)
CREATE_GENERATED_CPP_HELPER (generated_sources/WebCore/tokenizer.inl)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebCore/UserAgentStyleSheets.h ${CMAKE_BINARY_DIR}/generated_sources/WebCore/UserAgentStyleSheetsData.cpp
    COMMAND ${PERL_EXECUTABLE} ${CMAKE_SOURCE_DIR}/WebCore/css/make-css-file-arrays.pl ${CMAKE_BINARY_DIR}/generated_sources/WebCore/UserAgentStyleSheets.h ${CMAKE_BINARY_DIR}/generated_sources/WebCore/UserAgentStyleSheetsData.cpp ${CMAKE_SOURCE_DIR}/WebCore/css/html4.css ${CMAKE_SOURCE_DIR}/WebCore/css/quirks.css ${CMAKE_SOURCE_DIR}/WebCore/css/svg.css ${CMAKE_SOURCE_DIR}/WebCore/css/view-source.css
    DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/css/html4.css ${CMAKE_SOURCE_DIR}/WebCore/css/quirks.css ${CMAKE_SOURCE_DIR}/WebCore/css/svg.css
)

IF (WEBKIT_ENABLE_XPATH)
    ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebCore/XPathGrammar.cpp ${CMAKE_BINARY_DIR}/generated_sources/WebCore/XPathGrammar.h
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND ${BISON_EXECUTABLE} -d -p xpathyy ${CMAKE_SOURCE_DIR}/WebCore/xml/XPathGrammar.y -o XPathGrammar.tab.c
        COMMAND echo '\#ifndef XPATHGRAMMAR_H' > XPathGrammar.h
        COMMAND echo '\#define XPATHGRAMMAR_H' >> XPathGrammar.h
        COMMAND mv XPathGrammar.tab.c ${CMAKE_BINARY_DIR}/generated_sources/WebCore/XPathGrammar.cpp
        COMMAND cat XPathGrammar.h XPathGrammar.tab.h > ${CMAKE_BINARY_DIR}/generated_sources/WebCore/XPathGrammar.h
        COMMAND echo '\#endif' >> ${CMAKE_BINARY_DIR}/generated_sources/WebCore/XPathGrammar.h
        COMMAND rm -f XPathGrammar.h
        DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/xml/XPathGrammar.y
    )
ENDIF (WEBKIT_ENABLE_XPATH)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html/DocTypeStrings.cpp
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND mkdir -p generated_sources/WebCore/html
    COMMAND echo '\#include <string.h>' > generated_sources/WebCore/html/DocTypeStrings.cpp
    COMMAND ${GPERF_EXECUTABLE} -CEot -L ANSI-C -k \"*\" -N findDoctypeEntry -F ,PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards < ${CMAKE_SOURCE_DIR}/WebCore/html/DocTypeStrings.gperf >> generated_sources/WebCore/html/DocTypeStrings.cpp
    DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/html/DocTypeStrings.gperf
)

IF (WEBKIT_USE_SVG)
    IF (NOT EXISTS ${CMAKE_BINARY_DIR}/generated_sources/WebCore/svg/SVGNames.cpp)
        EXEC_PROGRAM (${PERL_EXECUTABLE}
            ARGS -I${CMAKE_SOURCE_DIR}/WebCore/bindings/scripts ${CMAKE_SOURCE_DIR}/WebCore/dom/make_names.pl --tags ${CMAKE_SOURCE_DIR}/WebCore/svg/svgtags.in --attrs ${CMAKE_SOURCE_DIR}/WebCore/svg/svgattrs.in --namespace SVG --cppNamespace WebCore --namespaceURI 'http://www.w3.org/2000/svg' --factory --attrsNullNamespace --wrapperFactory --extraDefines \"${SVG_FEATURES}\" --output ${CMAKE_BINARY_DIR}/generated_sources/WebCore/svg
        )
    ENDIF (NOT EXISTS ${CMAKE_BINARY_DIR}/generated_sources/WebCore/svg/SVGNames.cpp)
ENDIF (WEBKIT_USE_SVG)


#ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebCore/svg/SVGName.cpp ${CMAKE_BINARY_DIR}/generated_sources/WebCore/svg/SVGElementFactory.cpp ${CMAKE_BINARY_DIR}/generated_sources/WebCore/svg/SVGElementFactory.h
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#    COMMAND mkdir -p generated_sources/WebCore/svg
#    COMMAND ${CMAKE_SOURCE_DIR}/WebCore/dom/make_names.pl --tags ${CMAKE_SOURCE_DIR}/WebCore/svg/svgtags.in --attrs ${CMAKE_SOURCE_DIR}/WebCore/svg/svgattrs.in --namespace SVG --cppNamespace WebCore --namespaceURI 'http://www.w3.org/2000/svg' --factory --attrsNullNamespace --extraDefines "${SVG_FEATURES}" --output ${CMAKE_BINARY_DIR}/generated_sources/WebCore/svg
#    DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/svg/svgtags.in
#)

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebCore/XLinkNames.cpp
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND mkdir -p generated_sources/WebCore
    COMMAND ${PERL_EXECUTABLE} -I${CMAKE_SOURCE_DIR}/WebCore/bindings/scripts ${CMAKE_SOURCE_DIR}/WebCore/dom/make_names.pl -I${CMAKE_SOURCE_DIR}/WebCore/bindings/scripts ${CMAKE_SOURCE_DIR}/WebCore/svg/xlinkattrs.in --attrs ${CMAKE_SOURCE_DIR}/WebCore/svg/xlinkattrs.in --namespace XLink --cppNamespace WebCore --namespaceURI "http://www.w3.org/1999/xlink" --output generated_sources/WebCore
    DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/html/DocTypeStrings.gperf
)


SET (CREATE_HASH_TABLE ${CMAKE_SOURCE_DIR}/JavaScriptCore/kjs/create_hash_table)


MACRO (CREATE_LUT _srcs_LIST _in_FILE _out_FILE _dep_FILE)
    GET_FILENAME_COMPONENT (_baseName ${_out_FILE} PATH)
    ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/${_out_FILE}
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/${_baseName}
        COMMAND ${PERL_EXECUTABLE} ${CREATE_HASH_TABLE} ${CMAKE_SOURCE_DIR}/${_in_FILE} -i > ${CMAKE_BINARY_DIR}/${_out_FILE}
        DEPENDS ${CMAKE_SOURCE_DIR}/${_in_FILE} ${CMAKE_SOURCE_DIR}/${_dep_FILE}
    )
    SET (${_srcs_LIST}
        ${${_srcs_LIST}}
        ${CMAKE_BINARY_DIR}/${_out_FILE}
    )
ENDMACRO (CREATE_LUT)

ADD_CUSTOM_COMMAND (
  OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/kjs/grammar.cpp ${CMAKE_BINARY_DIR}/generated_sources/kjs/grammar.h
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMAND mkdir -p ${CMAKE_BINARY_DIR}/generated_sources/kjs
  COMMAND ${BISON_EXECUTABLE} -d -p kjsyy ${CMAKE_SOURCE_DIR}/JavaScriptCore/kjs/grammar.y -o grammar.tab.c
  COMMAND mv grammar.tab.c ${CMAKE_BINARY_DIR}/generated_sources/kjs/grammar.cpp
  COMMAND mv grammar.tab.h ${CMAKE_BINARY_DIR}/generated_sources/kjs/grammar.h
  DEPENDS ${CMAKE_SOURCE_DIR}/JavaScriptCore/kjs/grammar.y
)


MACRO (CREATE_CPP_LUT _in_FILE _out_FILE _dep_FILE)
   ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_BINARY_DIR}/${_out_FILE}
      COMMAND ${PERL_EXECUTABLE} ${CREATE_HASH_TABLE} ${CMAKE_SOURCE_DIR}/WebCore/${_in_FILE} > ${CMAKE_BINARY_DIR}/${_out_FILE}
      DEPENDS ${CMAKE_SOURCE_DIR}/WebCore/${_in_FILE} ${CMAKE_SOURCE_DIR}/WebCore/${_dep_FILE} )
ENDMACRO (CREATE_CPP_LUT)

CREATE_CPP_LUT (bindings/js/JSHTMLInputElementBase.cpp generated_sources/WebCore/JSHTMLInputElementBaseTable.inl bindings/js/JSHTMLInputElementBase.cpp)
CREATE_LUT (GENERATED_SRC JavaScriptCore/kjs/ArrayPrototype.cpp generated_sources/kjs/ArrayPrototype.lut.h JavaScriptCore/kjs/ArrayPrototype.cpp)
CREATE_LUT (GENERATED_SRC JavaScriptCore/kjs/DatePrototype.cpp generated_sources/kjs/DatePrototype.lut.h JavaScriptCore/kjs/DatePrototype.cpp)
CREATE_LUT (GENERATED_SRC JavaScriptCore/kjs/keywords.table generated_sources/kjs/lexer.lut.h JavaScriptCore/kjs/lexer.cpp)
CREATE_LUT (GENERATED_SRC JavaScriptCore/kjs/MathObject.cpp generated_sources/kjs/MathObject.lut.h JavaScriptCore/kjs/MathObject.cpp)
CREATE_LUT (GENERATED_SRC JavaScriptCore/kjs/NumberObject.cpp generated_sources/kjs/NumberObject.lut.h JavaScriptCore/kjs/NumberObject.cpp)
CREATE_LUT (GENERATED_SRC JavaScriptCore/kjs/NumberConstructor.cpp generated_sources/kjs/NumberConstructor.lut.h JavaScriptCore/kjs/NumberConstructor.cpp)
CREATE_LUT (GENERATED_SRC JavaScriptCore/kjs/RegExpObject.cpp generated_sources/kjs/RegExpObject.lut.h JavaScriptCore/kjs/RegExpObject.cpp)
CREATE_LUT (GENERATED_SRC JavaScriptCore/kjs/RegExpConstructor.cpp generated_sources/kjs/RegExpConstructor.lut.h JavaScriptCore/kjs/RegExpConstructor.cpp)
CREATE_LUT (GENERATED_SRC JavaScriptCore/kjs/StringObject.cpp generated_sources/kjs/StringObject.lut.h JavaScriptCore/kjs/StringObject.cpp)
CREATE_LUT (GENERATED_SRC JavaScriptCore/kjs/StringPrototype.cpp generated_sources/kjs/StringPrototype.lut.h JavaScriptCore/kjs/StringPrototype.cpp)
CREATE_GENERATED_CPP_HELPER (generated_sources/WebCore/JSHTMLInputElementBaseTable.inl)
CREATE_LUT (GENERATED_SRC WebCore/bindings/js/JSEventTargetBase.cpp generated_sources/WebCore/JSEventTargetBase.lut.h WebCore/bindings/js/JSEventTargetBase.cpp)
CREATE_LUT (GENERATED_SRC WebCore/bindings/js/JSEventTargetNode.cpp generated_sources/WebCore/JSEventTargetNode.lut.h WebCore/bindings/js/JSEventTargetNode.cpp)
CREATE_LUT (GENERATED_SRC WebCore/bindings/js/JSEventListener.cpp generated_sources/WebCore/JSEventListener.lut.h WebCore/bindings/js/JSEventListener.cpp)
CREATE_LUT (GENERATED_SRC WebCore/bindings/js/JSDOMBinding.cpp generated_sources/WebCore/JSDOMBinding.lut.h WebCore/bindings/js/JSDOMBinding.cpp)
CREATE_LUT (GENERATED_SRC WebCore/bindings/js/JSPluginElementFunctions.cpp generated_sources/WebCore/JSPluginElementFunctions.lut.h WebCore/bindings/js/JSPluginElementFunctions.cpp)
CREATE_LUT (GENERATED_SRC WebCore/bindings/js/JSDOMWindowBase.cpp generated_sources/WebCore/JSDOMWindowBase.lut.h WebCore/bindings/js/JSDOMWindowBase.cpp)
CREATE_LUT (GENERATED_SRC WebCore/bindings/js/JSRGBColor.cpp generated_sources/WebCore/JSRGBColor.lut.h WebCore/bindings/js/JSRGBColor.cpp)
CREATE_LUT (GENERATED_SRC WebCore/bindings/js/JSImageConstructor.cpp generated_sources/WebCore/JSImageConstructor.lut.h WebCore/bindings/js/JSImageConstructor.cpp)

# Add CXXFLAGS here
ADD_DEFINITIONS (-include ${CMAKE_SOURCE_DIR}/BAL/Base/config.h)

# Directories to include
INCLUDE_DIRECTORIES (
    ${CMAKE_SOURCE_DIR}/BAL/Base
    ${CMAKE_SOURCE_DIR}/BAL/Base/wtf
    ${CMAKE_BINARY_DIR}/generated_link/OWBAL
    ${CMAKE_BINARY_DIR}/generated_link/WKAL

    ${CMAKE_SOURCE_DIR}/WebCore
    ${CMAKE_SOURCE_DIR}/WebCore/bindings/js
    ${CMAKE_SOURCE_DIR}/WebCore/svg/
    ${CMAKE_SOURCE_DIR}/WebCore/svg/graphics
    ${CMAKE_SOURCE_DIR}/WebCore/svg/graphics/filters
    ${CMAKE_SOURCE_DIR}/WebCore/svg/animation/
    ${CMAKE_SOURCE_DIR}/WebCore/css
    ${CMAKE_SOURCE_DIR}/WebCore/dom
    ${CMAKE_SOURCE_DIR}/WebCore/page
    ${CMAKE_SOURCE_DIR}/WebCore/html
    ${CMAKE_SOURCE_DIR}/WebCore/loader
    ${CMAKE_SOURCE_DIR}/WebCore/loader/archive
    ${CMAKE_SOURCE_DIR}/WebCore/loader/icon
    ${CMAKE_SOURCE_DIR}/WebCore/loader/appcache
    ${CMAKE_SOURCE_DIR}/WebCore/editing
    ${CMAKE_SOURCE_DIR}/WebCore/rendering
    ${CMAKE_SOURCE_DIR}/WebCore/rendering/style
    ${CMAKE_SOURCE_DIR}/WebCore/history
    ${CMAKE_SOURCE_DIR}/WebCore/bridge
    ${CMAKE_SOURCE_DIR}/WebCore/xml
    ${CMAKE_SOURCE_DIR}/WebCore/storage
    ${CMAKE_SOURCE_DIR}/WebCore/plugins
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/JavaScriptCore
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/kjs
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/ForwardingHeaders
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/bindings
    ${CMAKE_SOURCE_DIR}/JavaScriptCore/VM
    ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/
    ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/Api
    ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/WebCoreSupport
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/xml
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/svg
    
    ${XML2_INCLUDE_DIRS}
)
IF (WEBKIT_ENABLE_XSLT)
    INCLUDE_DIRECTORIES(
        ${XSLT_INCLUDE_DIRS}
    )
ENDIF (WEBKIT_ENABLE_XSLT)

IF (WEBKIT_USE_I18N STREQUAL "GENERIC")
    INCLUDE_DIRECTORIES (
        ${CMAKE_SOURCE_DIR}/JavaScriptCore/icu
    )
ENDIF (WEBKIT_USE_I18N STREQUAL "GENERIC")

IF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")
    INCLUDE_DIRECTORIES (${CAIRO_INCLUDE_DIRS})
ENDIF (${OWBAL_PLATFORM_GRAPHICS} STREQUAL "GTK")

IF (WEBKIT_USE_HTML_EXTENSION)
    INCLUDE_DIRECTORIES (
        ${CMAKE_SOURCE_DIR}/HtmlExt/HtmlElement
        ${CMAKE_SOURCE_DIR}/HtmlExt/Renderer
    )
ENDIF (WEBKIT_USE_HTML_EXTENSION)

LIST (APPEND GENERATED_SRC
    ${CMAKE_BINARY_DIR}/generated_sources/kjs/grammar.cpp
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSGrammar.cpp
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html/HTMLEntityNames.c
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html/HTMLNames.cpp
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html/JSHTMLElementWrapperFactory.cpp
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/xml/XMLNames.cpp
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/UserAgentStyleSheetsData.cpp
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSValueKeywords.h
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/CSSPropertyNames.h
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/html/DocTypeStrings.cpp
    ${CMAKE_BINARY_DIR}/generated_sources/WebCore/XLinkNames.cpp
)
IF (WEBKIT_ENABLE_XPATH)
    LIST (APPEND GENERATED_SRC
        ${CMAKE_BINARY_DIR}/generated_sources/WebCore/XPathGrammar.h
        ${CMAKE_BINARY_DIR}/generated_sources/WebCore/XPathGrammar.cpp
    )
ENDIF (WEBKIT_ENABLE_XPATH)
IF (WEBKIT_USE_SVG)
    LIST (APPEND GENERATED_SRC
        ${CMAKE_BINARY_DIR}/generated_sources/WebCore/svg/JSSVGElementWrapperFactory.cpp
        ${CMAKE_BINARY_DIR}/generated_sources/WebCore/svg/SVGElementFactory.cpp
        ${CMAKE_BINARY_DIR}/generated_sources/WebCore/svg/SVGNames.cpp
    )
ENDIF (WEBKIT_USE_SVG)

IF (BUILD_SHARED_LIBS)
    ADD_LIBRARY (generated-sources ${GENERATED_SRC})
ELSE (BUILD_SHARED_LIBS)
    ADD_LIBRARY (generated-sources STATIC ${GENERATED_SRC})
ENDIF (BUILD_SHARED_LIBS)

INSTALL (TARGETS generated-sources
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
