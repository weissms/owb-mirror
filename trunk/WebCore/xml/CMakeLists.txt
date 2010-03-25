LIST (APPEND WEBCORE_SRC
    xml/DOMParser.cpp
    xml/XMLHttpRequest.cpp
    xml/XMLHttpRequestProgressEventThrottle.cpp
    xml/XMLHttpRequestUpload.cpp
    xml/XMLSerializer.cpp
)
list(APPEND IDL_SRC
    xml/DOMParser.idl
    xml/XMLHttpRequest.idl
    xml/XMLHttpRequestProgressEvent.idl
    xml/XMLHttpRequestException.idl
    xml/XMLHttpRequestUpload.idl
    xml/XMLSerializer.idl
    xml/XSLTProcessor.idl
)

if(ENABLE_XPATH)
    list(APPEND WEBCORE_SRC
        xml/NativeXPathNSResolver.cpp
        xml/XPathEvaluator.cpp
        xml/XPathExpression.cpp
        xml/XPathExpressionNode.cpp
        xml/XPathFunctions.cpp
        xml/XPathNSResolver.cpp
        xml/XPathNamespace.cpp
        xml/XPathNodeSet.cpp
        xml/XPathParser.cpp
        xml/XPathPath.cpp
        xml/XPathPredicate.cpp
        xml/XPathResult.cpp
        xml/XPathStep.cpp
        xml/XPathUtil.cpp
        xml/XPathValue.cpp
        xml/XPathVariableReference.cpp
    )
    list(APPEND IDL_SRC
        xml/XPathEvaluator.idl
        xml/XPathException.idl
        xml/XPathExpression.idl
        xml/XPathNSResolver.idl
        xml/XPathResult.idl
    )
endif(ENABLE_XPATH)

if(ENABLE_XSLT)
    list(APPEND WEBCORE_SRC
        xml/XSLImportRule.cpp
        xml/XSLTExtensions.cpp
        xml/XSLTProcessor.cpp
        xml/XSLTUnicodeSort.cpp
    )
    
if(USE_GRAPHICS_QT)
    list(APPEND WEBCORE_SRC
        xml/XSLStyleSheetQt.cpp
        xml/XSLTProcessorQt.cpp
    )
else(USE_GRAPHICS_QT)
    list(APPEND WEBCORE_SRC
        xml/XSLStyleSheetLibxslt.cpp
        xml/XSLTProcessorLibxslt.cpp
    )
endif(USE_GRAPHICS_QT)

endif(ENABLE_XSLT)
