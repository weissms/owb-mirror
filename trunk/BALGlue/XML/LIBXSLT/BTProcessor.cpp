/*
 * Copyright (C) 2007 Pleyo.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Pleyo nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PLEYO AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PLEYO OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "BTProcessor.h"
#include <libxslt/imports.h>
#include <libxslt/variables.h>
#include <libxslt/xsltutils.h>

#include "Node.h"
#include "DeprecatedString.h"
#include "XSLTProcessor.h"
#include "DocLoader.h"
#include "ResourceError.h"
#include "ResourceResponse.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "CString.h"
#include "markup.h"
#include "ParseXML.h"
#include "XSLTExtensions.h"

namespace BAL {

using WebCore::Document;
using WebCore::Node;
using WebCore::DeprecatedString;
using WebCore::XSLTProcessor;
using WebCore::DocLoader;
using WebCore::ResourceError;
using WebCore::ResourceResponse;
using WebCore::Frame;
using WebCore::FrameLoader;
using WebCore::CString;

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

static XSLTProcessor *globalProcessor = 0;
static DocLoader *globalDocLoader = 0;
static xmlDocPtr docLoaderFunc(const xmlChar *uri,
                                    xmlDictPtr dict,
                                    int options,
                                    void* ctxt,
                                    xsltLoadType type)
{
    if (!globalProcessor)
        return 0;
    
    switch (type) {
        case XSLT_LOAD_DOCUMENT: {
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
            return doc;
        }
        case XSLT_LOAD_STYLESHEET:
            return globalProcessor->xslStylesheet()->btStyleSheet()->locateStylesheetSubResource(((xsltStylesheetPtr)ctxt)->doc, uri);
        default:
            break;
    }
    
    return 0;
}

static inline void setXSLTLoadCallBack(xsltDocLoaderFunc func, XSLTProcessor *processor, DocLoader *loader)
{
    xsltSetLoaderFunc(func);
    globalProcessor = processor;
    globalDocLoader = loader;
}

static int writeToQString(void *context, const char *buffer, int len)
{
    DeprecatedString &resultOutput = *static_cast<DeprecatedString *>(context);
    resultOutput += DeprecatedString::fromUtf8(buffer, len);
    return len;
}

static bool saveResultToString(xmlDocPtr resultDoc, xsltStylesheetPtr sheet, DeprecatedString &resultString)
{
    xmlOutputBufferPtr outputBuf = xmlAllocOutputBuffer(0);
    if (!outputBuf)
        return false;
    outputBuf->context = &resultString;
    outputBuf->writecallback = writeToQString;
    
    int retval = xsltSaveResultTo(outputBuf, resultDoc, sheet);
    xmlOutputBufferClose(outputBuf);
    
    return (retval >= 0);
}

static const char **xsltParamArrayFromParameterMap(XSLTProcessor::ParameterMap& parameters)
{
    if (parameters.isEmpty())
        return 0;

    const char **parameterArray = (const char **)fastMalloc(((parameters.size() * 2) + 1) * sizeof(char *));

    XSLTProcessor::ParameterMap::iterator end = parameters.end();
    unsigned index = 0;
    for (XSLTProcessor::ParameterMap::iterator it = parameters.begin(); it != end; ++it) {
        parameterArray[index++] = strdup(it->first.utf8().data());
        parameterArray[index++] = strdup(it->second.utf8().data());
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

static xsltStylesheetPtr xsltStylesheetPointer(RefPtr<XSLStyleSheet> &cachedStylesheet, Node *stylesheetRootNode)
{
    if (!cachedStylesheet && stylesheetRootNode) {
        cachedStylesheet = new XSLStyleSheet(stylesheetRootNode->parent() ? stylesheetRootNode->parent() : stylesheetRootNode, stylesheetRootNode->document()->URL());
        cachedStylesheet->parseString(WebCore::createMarkup(stylesheetRootNode));
    }
    
    if (!cachedStylesheet || !cachedStylesheet->btStyleSheet()->document())
        return 0;
    
    return cachedStylesheet->btStyleSheet()->compileStyleSheet();
}

static inline xmlDocPtr xmlDocPtrFromNode(Node *sourceNode, bool &shouldDelete)
{
    RefPtr<Document> ownerDocument = sourceNode->document();
    bool sourceIsDocument = (sourceNode == ownerDocument.get());
    
    xmlDocPtr sourceDoc = 0;
    if (sourceIsDocument)
        sourceDoc = (xmlDocPtr)ownerDocument->transformSource();
    if (!sourceDoc) {
        ParseXML *parse = new ParseXML(0);
        sourceDoc = (xmlDocPtr)parse->xmlDocPtrForString(ownerDocument->docLoader(), createMarkup(sourceNode), sourceIsDocument ? ownerDocument->URL() : DeprecatedString());
        shouldDelete = (sourceDoc != 0);
        delete parse;
    }
    return sourceDoc;
}

static inline DeprecatedString resultMIMEType(xmlDocPtr resultDoc, xsltStylesheetPtr sheet)
{
    // There are three types of output we need to be able to deal with:
    // HTML (create an HTML document), XML (create an XML document),
    // and text (wrap in a <pre> and create an XML document).

    const xmlChar *resultType = 0;
    XSLT_GET_IMPORT_PTR(resultType, sheet, method);
    if (resultType == 0 && resultDoc->type == XML_HTML_DOCUMENT_NODE)
        resultType = (const xmlChar *)"html";
    
    if (xmlStrEqual(resultType, (const xmlChar *)"html"))
        return DeprecatedString("text/html");
    else if (xmlStrEqual(resultType, (const xmlChar *)"text"))
        return DeprecatedString("text/plain");
        
    return DeprecatedString("application/xml");
}

BTProcessor::BTProcessor(XSLTProcessor *proc)
    :m_processor(proc)
{
}

BTProcessor::~BTProcessor()
{
    m_processor = 0;
}


bool BTProcessor::transformToString(Node *sourceNode, DeprecatedString &mimeType, DeprecatedString &resultString, DeprecatedString &resultEncoding)
{
    RefPtr<Document> ownerDocument = sourceNode->document();
    
    setXSLTLoadCallBack(docLoaderFunc, this->m_processor, ownerDocument->docLoader());
    xsltStylesheetPtr sheet = xsltStylesheetPointer(m_processor->m_stylesheet, m_processor->m_stylesheetRootNode.get());
    if (!sheet) {
        setXSLTLoadCallBack(0, 0, 0);
        return false;
    }
    m_processor->m_stylesheet->clearDocuments();

    xmlChar* origMethod = sheet->method;
    if (!origMethod && mimeType == "text/html")
        sheet->method = (xmlChar*)"html";

    bool success = false;
    bool shouldFreeSourceDoc = false;
    if (xmlDocPtr sourceDoc = xmlDocPtrFromNode(sourceNode, shouldFreeSourceDoc)) {
        // The XML declaration would prevent parsing the result as a fragment, and it's not needed even for documents, 
        // as the result of this function is always immediately parsed.
        sheet->omitXmlDeclaration = true;

        xsltTransformContextPtr transformContext = xsltNewTransformContext(sheet, sourceDoc);
        WebCore::registerXSLTExtensions(transformContext);

        // This is a workaround for a bug in libxslt. 
        // The bug has been fixed in version 1.1.13, so once we ship that this can be removed.
        if (transformContext->globalVars == NULL)
           transformContext->globalVars = xmlHashCreate(20);

        const char **params = xsltParamArrayFromParameterMap(m_processor->m_parameters);
        xsltQuoteUserParams(transformContext, params);
        xmlDocPtr resultDoc = xsltApplyStylesheetUser(sheet, sourceDoc, 0, 0, 0, transformContext);
        
        xsltFreeTransformContext(transformContext);        
        freeXsltParamArray(params);
        
        if (shouldFreeSourceDoc)
            xmlFreeDoc(sourceDoc);
        
        if (success = saveResultToString(resultDoc, sheet, resultString)) {
            mimeType = resultMIMEType(resultDoc, sheet);
            resultEncoding = (char *)resultDoc->encoding;
        }
        xmlFreeDoc(resultDoc);
    }
    
    sheet->method = origMethod;
    setXSLTLoadCallBack(0, 0, 0);
    xsltFreeStylesheet(sheet);
    m_processor->m_stylesheet = 0;

    return success;
}
/*RefPtr<Document> BTProcessor::documentFromXMLDocPtr(xmlDocPtr resultDoc, xsltStylesheetPtr sheet, Document *ownerDocument, bool sourceIsDocument)
{
}*/

}

