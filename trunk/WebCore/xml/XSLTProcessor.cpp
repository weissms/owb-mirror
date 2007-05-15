/**
 * This file is part of the XSL implementation.
 *
 * Copyright (C) 2004, 2005, 2006 Apple Computer, Inc.
 * Copyright (C) 2005, 2006 Alexey Proskuryakov <ap@webkit.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"

#ifdef XSLT_SUPPORT

#include "XSLTProcessor.h"

#include "CString.h"
#include "Cache.h"
#include "DOMImplementation.h"
#include "DocLoader.h"
#include "DocumentFragment.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "FrameView.h"
#include "HTMLDocument.h"
#include "HTMLTokenizer.h"
#include "ResourceHandle.h"
#include "ResourceRequest.h"
#include "ResourceResponse.h"
#include "Text.h"
#include "TextResourceDecoder.h"
#include "XMLTokenizer.h"
#include "loader.h"
#include "markup.h"

#ifndef __OWB_BIXML__
#include <libxslt/imports.h>
#include <libxslt/variables.h>
#include <libxslt/xsltutils.h>
#endif

#include <wtf/Assertions.h>
#include <wtf/Platform.h>
#include <wtf/Vector.h>

namespace WebCore {

static void parseErrorFunc(void *ctxt, const char *msg, ...)
{
    // FIXME: It would be nice to display error messages somewhere.
#if !PLATFORM(WIN_OS)
    // FIXME: No vasprintf support.
#ifndef ERROR_DISABLED
    char *errorMessage = 0;
    va_list args;
    va_start(args, msg);
    vasprintf(&errorMessage, msg, args);
    LOG_ERROR("%s", errorMessage);
    if (errorMessage)
        free(errorMessage);
    va_end(args);
#endif
#endif
}

// FIXME: There seems to be no way to control the ctxt pointer for loading here, thus we have globals.
static XSLTProcessor *globalProcessor = 0;
static DocLoader *globalDocLoader = 0;
#ifdef __OWB_BIXML__
static BTXMLDoc *docLoaderFunc(const unsigned char *uri,
                                    BTXMLDict *dict,
                                    int options,
                                    void* ctxt,
                                    BAL::BIXsltLoadType type)
#else
static xmlDocPtr docLoaderFunc(const xmlChar *uri,
                                    xmlDictPtr dict,
                                    int options,
                                    void* ctxt,
                                    xsltLoadType type)
#endif
{
    if (!globalProcessor)
        return 0;

    switch (type) {
        case XSLT_LOAD_DOCUMENT: {
#ifdef __OWB_BIXML__
            BAL::BIXML *m_xml = createBIXML();
            BTXSLTTransformContext *context = (BTXSLTTransformContext *)ctxt;
            unsigned char *base = m_xml->xmlNodeGetBase(context->document->doc, context->node);
            KURL url((const char*)base, (const char*)uri);
            m_xml->xmlFree(base);
            ResourceRequest request(url);
            KURL finalURL;

            DeprecatedString headers;
            BTXMLGenericErrorFunc oldErrorFunc = m_xml->getXMLGenericError();
            void *oldErrorContext = m_xml->getXMLGenericErrorContext();

            Vector<char> data = ServeSynchronousRequest(cache()->loader(), globalDocLoader, request, finalURL, headers);

            m_xml->xmlSetGenericErrorFunc(0, parseErrorFunc);

            // We don't specify an encoding here. Neither Gecko nor WinIE respects
            // the encoding specified in the HTTP headers.

            BTXMLDoc *doc = m_xml->xmlReadMemory(data.data(), data.size(), (const char*)uri, 0, options);
            m_xml->xmlSetGenericErrorFunc(oldErrorContext, oldErrorFunc);
#else
            xsltTransformContextPtr context = (xsltTransformContextPtr)ctxt;
            xmlChar *base = xmlNodeGetBase(context->document->doc, context->node);
            KURL url((const char*)base, (const char*)uri);
            xmlFree(base);
            ResourceError error;
            ResourceResponse response;
            xmlGenericErrorFunc oldErrorFunc = xmlGenericError;
            void *oldErrorContext = xmlGenericErrorContext;

            Vector<char> data;
                
            if (globalDocLoader->frame()) 
                globalDocLoader->frame()->loader()->loadResourceSynchronously(url, error, response, data);

            xmlSetGenericErrorFunc(0, parseErrorFunc);

            // We don't specify an encoding here. Neither Gecko nor WinIE respects
            // the encoding specified in the HTTP headers.
            xmlDocPtr doc = xmlReadMemory(data.data(), data.size(), (const char*)uri, 0, options);
            xmlSetGenericErrorFunc(oldErrorContext, oldErrorFunc);
#endif

            return doc;
        }
        case XSLT_LOAD_STYLESHEET:
#ifdef __OWB_BIXML__
            return globalProcessor->xslStylesheet()->locateStylesheetSubResource(((BTXSLTStyleSheet *)ctxt)->doc, uri);
#else
            return globalProcessor->xslStylesheet()->locateStylesheetSubResource(((xsltStylesheetPtr)ctxt)->doc, uri);
#endif
        default:
            break;
    }

    return 0;
}

#ifdef __OWB_BIXML__
static inline void setXSLTLoadCallBack(BIXSLTDocLoaderFunc func, XSLTProcessor *processor, DocLoader *loader)
{
    BAL::BIXSLT *m_xlst = createBIXSLT();
    m_xlst->setLoaderFunc(func);
#else
static inline void setXSLTLoadCallBack(xsltDocLoaderFunc func, XSLTProcessor *processor, DocLoader *loader)
{
    xsltSetLoaderFunc(func);
#endif
    globalProcessor = processor;
    globalDocLoader = loader;
}
static int writeToQString(void *context, const char *buffer, int len)
{
    DeprecatedString &resultOutput = *static_cast<DeprecatedString *>(context);
    resultOutput += DeprecatedString::fromUtf8(buffer, len);
    return len;
}

#ifdef __OWB_BIXML__
static bool saveResultToString(BTXMLDoc *resultDoc, BTXSLTStyleSheet *sheet, DeprecatedString &resultString)
{
    BAL::BIXML *m_xml = createBIXML();
    BTXMLOutputBuffer *outputBuf = m_xml->xmlAllocOutputBuffer(0);
#else
static bool saveResultToString(xmlDocPtr resultDoc, xsltStylesheetPtr sheet, DeprecatedString &resultString)
{
    xmlOutputBufferPtr outputBuf = xmlAllocOutputBuffer(0);
#endif

    if (!outputBuf)
        return false;
    outputBuf->context = &resultString;
    outputBuf->writecallback = writeToQString;

#ifdef __OWB_BIXML__
    BAL::BIXSLT *m_xlst = createBIXSLT();
    int retval = m_xlst->xsltSaveResultTo(outputBuf, resultDoc, sheet);
    m_xml->xmlOutputBufferClose(outputBuf);
#else
    int retval = xsltSaveResultTo(outputBuf, resultDoc, sheet);
    xmlOutputBufferClose(outputBuf);
#endif

    return (retval >= 0);
}

static inline void transformTextStringToXHTMLDocumentString(String &text)
{
    // Modify the output so that it is a well-formed XHTML document with a <pre> tag enclosing the text.
    text.replace('&', "&amp;");
    text.replace('<', "&lt;");
    text = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
        "<head><title/></head>\n"
        "<body>\n"
        "<pre>" + text + "</pre>\n"
        "</body>\n"
        "</html>\n";
}

static const char **xsltParamArrayFromParameterMap(XSLTProcessor::ParameterMap& parameters)
{
    if (parameters.isEmpty())
        return 0;

    const char **parameterArray = (const char **)fastMalloc(((parameters.size() * 2) + 1) * sizeof(char *));

    XSLTProcessor::ParameterMap::iterator end = parameters.end();
    unsigned index = 0;
    for (XSLTProcessor::ParameterMap::iterator it = parameters.begin(); it != end; ++it) {
        parameterArray[index++] = strdup(it->first.utf8());
        parameterArray[index++] = strdup(it->second.utf8());
    }
    parameterArray[index] = 0;

    return parameterArray;
}

static void freeXsltParamArray(const char **params)
{
    const char **temp = params;
    if (!params)
        return;

    while (*temp) {
        free((void *)*(temp++)); // strdup returns malloc'd blocks, so we have to use free() here
        free((void *)*(temp++));
    }
    fastFree(params);
}


RefPtr<Document> XSLTProcessor::createDocumentFromSource(const DeprecatedString& sourceString,
    const DeprecatedString& sourceEncoding, const DeprecatedString& sourceMIMEType, Node* sourceNode, FrameView* view)
{
    RefPtr<Document> ownerDocument = sourceNode->document();
    bool sourceIsDocument = (sourceNode == ownerDocument.get());
    String documentSource = sourceString;

    RefPtr<Document> result;
    if (sourceMIMEType == "text/html")
        result = ownerDocument->implementation()->createHTMLDocument(view);
    else {
        result = ownerDocument->implementation()->createDocument(view);
        if (sourceMIMEType == "text/plain")
            transformTextStringToXHTMLDocumentString(documentSource);
    }

    // Before parsing, we need to save & detach the old document and get the new document
    // in place. We have to do this only if we're rendering the result document.
    if (view) {
        view->clear();
        result->setTransformSourceDocument(view->frame()->document());
        view->frame()->setDocument(result.get());
    }

    result->open();
    if (sourceIsDocument) {
        result->setURL(ownerDocument->URL());
        result->setBaseURL(ownerDocument->baseURL());
    }
    result->determineParseMode(documentSource); // Make sure we parse in the correct mode.

    RefPtr<TextResourceDecoder> decoder = new TextResourceDecoder(sourceMIMEType);
    decoder->setEncoding(sourceEncoding.isEmpty() ? UTF8Encoding() : TextEncoding(sourceEncoding), TextResourceDecoder::EncodingFromXMLHeader);
    result->setDecoder(decoder.get());

    result->write(documentSource);
    result->finishParsing();
    if (view)
        view->frame()->loader()->checkCompleted();
    else
        result->close(); // FIXME: Even viewless docs can load subresources. onload will fire too early.
                         // This is probably a bug in XMLHttpRequestObjects as well.
    return result;
}

static inline RefPtr<DocumentFragment> createFragmentFromSource(DeprecatedString sourceString, DeprecatedString sourceMIMEType, Node *sourceNode, Document *outputDoc)
{
    RefPtr<DocumentFragment> fragment = new DocumentFragment(outputDoc);

    if (sourceMIMEType == "text/html")
        parseHTMLDocumentFragment(sourceString, fragment.get());
    else if (sourceMIMEType == "text/plain")
        fragment->addChild(new Text(outputDoc, sourceString));
    else {
        bool successfulParse = parseXMLDocumentFragment(sourceString, fragment.get(), outputDoc->documentElement());
        if (!successfulParse)
            return 0;
    }

    // FIXME: Do we need to mess with URLs here?

    return fragment;
}

#ifdef __OWB_BIXML__
static BTXSLTStyleSheet *xsltStylesheetPointer(RefPtr<XSLStyleSheet> &cachedStylesheet, Node *stylesheetRootNode)
#else
static xsltStylesheetPtr xsltStylesheetPointer(RefPtr<XSLStyleSheet> &cachedStylesheet, Node *stylesheetRootNode)
#endif
{
    if (!cachedStylesheet && stylesheetRootNode) {
        cachedStylesheet = new XSLStyleSheet(stylesheetRootNode->parent() ? stylesheetRootNode->parent() : stylesheetRootNode, stylesheetRootNode->document()->URL());
        cachedStylesheet->parseString(createMarkup(stylesheetRootNode));
    }

    if (!cachedStylesheet || !cachedStylesheet->document())
        return 0;

    return cachedStylesheet->compileStyleSheet();
}

#ifdef __OWB_BIXML__
static inline BTXMLDoc *xmlDocPtrFromNode(Node *sourceNode, bool &shouldDelete)
#else
static inline xmlDocPtr xmlDocPtrFromNode(Node *sourceNode, bool &shouldDelete)
#endif
{
    RefPtr<Document> ownerDocument = sourceNode->document();
    bool sourceIsDocument = (sourceNode == ownerDocument.get());

#ifdef __OWB_BIXML__
    BTXMLDoc *sourceDoc = 0;
    if (sourceIsDocument)
        sourceDoc = (BTXMLDoc *)ownerDocument->transformSource();
    if (!sourceDoc) {
        sourceDoc = (BTXMLDoc *)xmlDocPtrForString(ownerDocument->docLoader(), createMarkup(sourceNode), sourceIsDocument ? ownerDocument->URL() : DeprecatedString());
        shouldDelete = (sourceDoc != 0);
    }
#else
    xmlDocPtr sourceDoc = 0;
    if (sourceIsDocument)
        sourceDoc = (xmlDocPtr)ownerDocument->transformSource();
    if (!sourceDoc) {
        sourceDoc = (xmlDocPtr)xmlDocPtrForString(ownerDocument->docLoader(), createMarkup(sourceNode), sourceIsDocument ? ownerDocument->URL() : DeprecatedString());
        shouldDelete = (sourceDoc != 0);
    }
#endif

    return sourceDoc;
}

#ifdef __OWB_BIXML__
static inline DeprecatedString resultMIMEType(BTXMLDoc *resultDoc, BTXSLTStyleSheet *sheet)
#else
static inline DeprecatedString resultMIMEType(xmlDocPtr resultDoc, xsltStylesheetPtr sheet)
#endif
{
    // There are three types of output we need to be able to deal with:
    // HTML (create an HTML document), XML (create an XML document),
    // and text (wrap in a <pre> and create an XML document).

#ifdef __OWB_BIXML__
    const unsigned char *resultType = 0;
    BAL::BIXSLT *m_xlst = createBIXSLT();
    m_xlst->getImportPtr( resultType, sheet );

    if (resultType == 0 && resultDoc->type == BTXML_HTML_DOCUMENT_NODE)
        resultType = (const unsigned char *)"html";

    BAL::BIXML *m_xml = createBIXML();
    if (m_xml->xmlStrEqual(resultType, (const unsigned char *)"html"))
        return DeprecatedString("text/html");
    else if (m_xml->xmlStrEqual(resultType, (const unsigned char *)"text"))
        return DeprecatedString("text/plain");
#else
    const xmlChar *resultType = 0;
    XSLT_GET_IMPORT_PTR(resultType, sheet, method);

    if (resultType == 0 && resultDoc->type == XML_HTML_DOCUMENT_NODE)
        resultType = (const xmlChar *)"html";

    if (xmlStrEqual(resultType, (const xmlChar *)"html"))
        return DeprecatedString("text/html");
    else if (xmlStrEqual(resultType, (const xmlChar *)"text"))
        return DeprecatedString("text/plain");
#endif

    return DeprecatedString("application/xml");
}

bool XSLTProcessor::transformToString(Node *sourceNode, DeprecatedString &mimeType, DeprecatedString &resultString, DeprecatedString &resultEncoding)
{
    RefPtr<Document> ownerDocument = sourceNode->document();

    setXSLTLoadCallBack(docLoaderFunc, this, ownerDocument->docLoader());
#ifdef __OWB_BIXML__
    BAL::BIXSLT *m_xlst = createBIXSLT();
    BTXSLTStyleSheet *sheet = xsltStylesheetPointer(m_stylesheet, m_stylesheetRootNode.get());
#else
    xsltStylesheetPtr sheet = xsltStylesheetPointer(m_stylesheet, m_stylesheetRootNode.get());
#endif
    if (!sheet) {
        setXSLTLoadCallBack(0, 0, 0);
        return false;
    }
    m_stylesheet->clearDocuments();

    xmlChar* origMethod = sheet->method;
    if (!origMethod && mimeType == "text/html")
        sheet->method = (xmlChar*)"html";

    bool success = false;
    bool shouldFreeSourceDoc = false;
#ifdef __OWB_BIXML__
    if (BTXMLDoc *sourceDoc = xmlDocPtrFromNode(sourceNode, shouldFreeSourceDoc)) {
#else
    if (xmlDocPtr sourceDoc = xmlDocPtrFromNode(sourceNode, shouldFreeSourceDoc)) {
#endif
        // The XML declaration would prevent parsing the result as a fragment, and it's not needed even for documents,
        // as the result of this function is always immediately parsed.
        sheet->omitXmlDeclaration = true;

#ifdef __OWB_BIXML__
        BAL::BIXML *m_xml = createBIXML();
        BTXSLTTransformContext *transformContext = m_xlst->xsltNewTransformContext(sheet, sourceDoc);

        // This is a workaround for a bug in libxslt.
        // The bug has been fixed in version 1.1.13, so once we ship that this can be removed.
        if (transformContext->globalVars == NULL)
           transformContext->globalVars = m_xml->xmlHashCreate(20);
#else
        xsltTransformContextPtr transformContext = xsltNewTransformContext(sheet, sourceDoc);

        // This is a workaround for a bug in libxslt.
        // The bug has been fixed in version 1.1.13, so once we ship that this can be removed.
        if (transformContext->globalVars == NULL)
           transformContext->globalVars = xmlHashCreate(20);
#endif


        const char **params = xsltParamArrayFromParameterMap(m_parameters);
#ifdef __OWB_BIXML__
        m_xlst->xsltQuoteUserParams(transformContext, params);
        BTXMLDoc *resultDoc = m_xlst->xsltApplyStylesheetUser(sheet, sourceDoc, 0, 0, 0, transformContext);
        m_xlst->xsltFreeTransformContext(transformContext);
#else
        xsltQuoteUserParams(transformContext, params);
        xmlDocPtr resultDoc = xsltApplyStylesheetUser(sheet, sourceDoc, 0, 0, 0, transformContext);
        xsltFreeTransformContext(transformContext);
#endif


        freeXsltParamArray(params);

        if (shouldFreeSourceDoc)
#ifdef __OWB_BIXML__
            m_xml->xmlFreeDoc(sourceDoc);
#else
            xmlFreeDoc(sourceDoc);
#endif

        if (success = saveResultToString(resultDoc, sheet, resultString)) {
            mimeType = resultMIMEType(resultDoc, sheet);
            resultEncoding = (char *)resultDoc->encoding;
        }
#ifdef __OWB_BIXML__
        m_xml->xmlFreeDoc(resultDoc);
#else
        xmlFreeDoc(resultDoc);
#endif

    }

    sheet->method = origMethod;
    setXSLTLoadCallBack(0, 0, 0);

#ifdef __OWB_BIXML__
    m_xlst->xsltFreeStylesheet(sheet);
#else
    xsltFreeStylesheet(sheet);
#endif
    m_stylesheet = 0;

    return success;
}

RefPtr<Document> XSLTProcessor::transformToDocument(Node *sourceNode)
{
    DeprecatedString resultMIMEType;
    DeprecatedString resultString;
    DeprecatedString resultEncoding;
    if (!transformToString(sourceNode, resultMIMEType, resultString, resultEncoding))
        return 0;
    return createDocumentFromSource(resultString, resultEncoding, resultMIMEType, sourceNode);
}

RefPtr<DocumentFragment> XSLTProcessor::transformToFragment(Node* sourceNode, Document* outputDoc)
{
    DeprecatedString resultMIMEType;
    DeprecatedString resultString;
    DeprecatedString resultEncoding;

    // If the output document is HTML, default to HTML method.
    if (outputDoc->isHTMLDocument())
        resultMIMEType = "text/html";
    
    if (!transformToString(sourceNode, resultMIMEType, resultString, resultEncoding))
        return 0;
    return createFragmentFromSource(resultString, resultMIMEType, sourceNode, outputDoc);
}

void XSLTProcessor::setParameter(const String& namespaceURI, const String& localName, const String& value)
{
    // FIXME: namespace support?
    // should make a QualifiedName here but we'd have to expose the impl
    m_parameters.set(localName, value);
}

String XSLTProcessor::getParameter(const String& namespaceURI, const String& localName) const
{
    // FIXME: namespace support?
    // should make a QualifiedName here but we'd have to expose the impl
    return m_parameters.get(localName);
}

void XSLTProcessor::removeParameter(const String& namespaceURI, const String& localName)
{
    // FIXME: namespace support?
    m_parameters.remove(localName);
}

} // namespace WebCore

#endif // XSLT_SUPPORT
