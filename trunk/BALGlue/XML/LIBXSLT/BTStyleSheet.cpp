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

#include "BTStyleSheet.h"
#include <libxml/uri.h>
#include <libxslt/xsltutils.h>
#include "XSLStyleSheet.h"
#include "Document.h"
#include "CString.h"
#include "StyleBase.h"
#include "Node.h"
#include "XSLImportRule.h"


namespace BAL {

using WebCore::Document;
using WebCore::CString;
using WebCore::StyleBase;
using WebCore::Node;
using WebCore::XSLImportRule;

BTStyleSheet::BTStyleSheet(XSLStyleSheet *style, bool embedded)
    : m_stylesheetDoc(0)
    , m_embedded(embedded)
    , m_stylesheetDocTaken(false)
    , m_style(style)
{
}

BTStyleSheet::~BTStyleSheet()
{
    if (!m_stylesheetDocTaken)
        xmlFreeDoc(m_stylesheetDoc);
}

xsltStylesheetPtr BTStyleSheet::compileStyleSheet()
{
    // FIXME: Hook up error reporting for the stylesheet compilation process.
    if (m_embedded)
        return xsltLoadStylesheetPI(document());
    
    // xsltParseStylesheetDoc makes the document part of the stylesheet
    // so we have to release our pointer to it.
    ASSERT(!m_stylesheetDocTaken);
    xsltStylesheetPtr result = xsltParseStylesheetDoc(m_stylesheetDoc);
    if (result)
        m_stylesheetDocTaken = true;
    return result;
}

xmlDocPtr BTStyleSheet::document()
{
    if (m_embedded && m_style->ownerDocument())
        return (xmlDocPtr)m_style->ownerDocument()->transformSource();
    return m_stylesheetDoc;
}

xmlDocPtr BTStyleSheet::locateStylesheetSubResource(xmlDocPtr parentDoc, const xmlChar* uri)
{
    bool matchedParent = (parentDoc == document());
    unsigned len = m_style->length();
    for (unsigned i = 0; i < len; ++i) {
        StyleBase* rule = m_style->item(i);
        if (rule->isImportRule()) {
            XSLImportRule* import = static_cast<XSLImportRule*>(rule);
            XSLStyleSheet* child = import->styleSheet();
            if (!child)
                continue;
            if (matchedParent) {
                if (child->processed())
                    continue; // libxslt has been given this sheet already.
                
                // Check the URI of the child stylesheet against the doc URI.
                // In order to ensure that libxml canonicalized both URLs, we get the original href
                // string from the import rule and canonicalize it using libxml before comparing it
                // with the URI argument.
                CString importHref = import->href().utf8();
                xmlChar* base = xmlNodeGetBase(parentDoc, (xmlNodePtr)parentDoc);
                xmlChar* childURI = xmlBuildURI((const xmlChar*)importHref.data(), base);
                bool equalURIs = xmlStrEqual(uri, childURI);
                xmlFree(base);
                xmlFree(childURI);
                if (equalURIs) {
                    child->markAsProcessed();
                    return child->btStyleSheet()->document();
                }
            } else {
                xmlDocPtr result = import->styleSheet()->btStyleSheet()->locateStylesheetSubResource(parentDoc, uri);
                if (result)
                    return result;
            }
        }
    }
    
    return 0;
}

void BTStyleSheet::markAsProcessed()
{
    ASSERT(!m_stylesheetDocTaken);
    m_stylesheetDocTaken = true;
}

void BTStyleSheet::clearDocuments()
{
    m_stylesheetDoc = 0;
}

bool BTStyleSheet::parseString(const String& string, bool strict)
{
    const UChar BOM = 0xFEFF;
    const unsigned char BOMHighByte = *reinterpret_cast<const unsigned char*>(&BOM);
    if (!m_stylesheetDocTaken)
        xmlFreeDoc(m_stylesheetDoc);
    m_stylesheetDocTaken = false;
    m_stylesheetDoc = xmlReadMemory(reinterpret_cast<const char*>(string.characters()), string.length() * sizeof(UChar),
        m_style->ownerDocument()->URL().ascii(),
        BOMHighByte == 0xFF ? "UTF-16LE" : "UTF-16BE", 
        XML_PARSE_NOENT | XML_PARSE_DTDATTR | XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NOCDATA);

    return m_stylesheetDoc;
}

void BTStyleSheet::loadChildSheets()
{
    if (!document())
        return;
    
    xmlNodePtr stylesheetRoot = document()->children;
    
    // Top level children may include other things such as DTD nodes, we ignore those.
    while (stylesheetRoot && stylesheetRoot->type != XML_ELEMENT_NODE)
        stylesheetRoot = stylesheetRoot->next;
    
    if (m_embedded) {
        // We have to locate (by ID) the appropriate embedded stylesheet element, so that we can walk the 
        // import/include list.
        xmlAttrPtr idNode = xmlGetID(document(), (const xmlChar*)(m_style->href().utf8().data()));
        if (!idNode)
            return;
        stylesheetRoot = idNode->parent;
    } else {
        // FIXME: Need to handle an external URI with a # in it.  This is a pretty minor edge case, so we'll deal
        // with it later.
    }
    
    if (stylesheetRoot) {
        // Walk the children of the root element and look for import/include elements.
        // Imports must occur first.
        xmlNodePtr curr = stylesheetRoot->children;
        while (curr) {
            if (curr->type != XML_ELEMENT_NODE) {
                curr = curr->next;
                continue;
            }
            if (IS_XSLT_ELEM(curr) && IS_XSLT_NAME(curr, "import")) {
                xmlChar* uriRef = xsltGetNsProp(curr, (const xmlChar*)"href", XSLT_NAMESPACE);                
                m_style->loadChildSheet(DeprecatedString::fromUtf8((const char*)uriRef));
                xmlFree(uriRef);
            } else
                break;
            curr = curr->next;
        }

        // Now handle includes.
        while (curr) {
            if (curr->type == XML_ELEMENT_NODE && IS_XSLT_ELEM(curr) && IS_XSLT_NAME(curr, "include")) {
                xmlChar* uriRef = xsltGetNsProp(curr, (const xmlChar*)"href", XSLT_NAMESPACE);
                m_style->loadChildSheet(DeprecatedString::fromUtf8((const char*)uriRef));
                xmlFree(uriRef);
            }
            curr = curr->next;
        }
    }
}


}
