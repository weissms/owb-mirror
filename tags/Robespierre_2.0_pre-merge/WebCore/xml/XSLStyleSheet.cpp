/**
 * This file is part of the XSL implementation.
 *
 * Copyright (C) 2004, 2005, 2006 Apple Computer, Inc.
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
#include "XSLStyleSheet.h"

#ifdef XSLT_SUPPORT

#include "CString.h"
#include "DocLoader.h"
#include "Document.h"
#include "Node.h"
#include "XSLImportRule.h"
#include "loader.h"
#include "XMLTokenizer.h"

#ifndef __OWB_BIXML__
#include <libxslt/xsltutils.h>
#include <libxml/uri.h>
#else
#include "BIXML.h"
#include "BALConfiguration.h"
using namespace BAL;
#endif

namespace WebCore {

#ifndef __OWB_BIXML__
#define IS_BLANK_NODE(n)                                                \
    (((n)->type == XML_TEXT_NODE) && (xsltIsBlank((n)->content)))
#else
#define IS_BLANK_NODE(n) \
    (((n)->type == BTXML_TEXT_NODE) && (m_xlst->isBlank((n)->content)))
#endif

XSLStyleSheet::XSLStyleSheet(XSLImportRule* parentRule, const String& href)
    : StyleSheet(parentRule, href)
    , m_ownerDocument(0)
    , m_stylesheetDoc(0)
    , m_embedded(false)
    , m_processed(false) // Child sheets get marked as processed when the libxslt engine has finally seen them.
    , m_stylesheetDocTaken(false)
{
#ifdef __OWB_BIXML__
    m_xlst = createBIXSLT();
    m_xml = createBIXML();
#endif
}

XSLStyleSheet::XSLStyleSheet(Node* parentNode, const String& href,  bool embedded)
    : StyleSheet(parentNode, href)
    , m_ownerDocument(parentNode->document())
    , m_stylesheetDoc(0)
    , m_embedded(embedded)
    , m_processed(true) // The root sheet starts off processed.
    , m_stylesheetDocTaken(false)
{
}

XSLStyleSheet::~XSLStyleSheet()
{
    if (!m_stylesheetDocTaken)
#ifdef __OWB_BIXML__
        m_xml->xmlFreeDoc(m_stylesheetDoc);
        delete m_xlst;
        delete m_xml;
#else
        xmlFreeDoc(m_stylesheetDoc);
#endif

}

bool XSLStyleSheet::isLoading()
{
    unsigned len = length();
    for (unsigned i = 0; i < len; ++i) {
        StyleBase* rule = item(i);
        if (rule->isImportRule()) {
            XSLImportRule* import = static_cast<XSLImportRule*>(rule);
            if (import->isLoading())
                return true;
        }
    }
    return false;
}

void XSLStyleSheet::checkLoaded()
{
    if (isLoading())
        return;
    if (parent())
        parent()->checkLoaded();
    if (m_parentNode)
        m_parentNode->sheetLoaded();
}

#ifdef __OWB_BIXML__
BTXMLDoc *XSLStyleSheet::document()
#else
xmlDocPtr XSLStyleSheet::document()
#endif
{
    if (m_embedded && ownerDocument())
#ifdef __OWB_BIXML__
        return (BTXMLDoc *)ownerDocument()->transformSource();
#else
        return (xmlDocPtr)ownerDocument()->transformSource();
#endif
    return m_stylesheetDoc;
}

void XSLStyleSheet::clearDocuments()
{
    m_stylesheetDoc = 0;
    unsigned len = length();
    for (unsigned i = 0; i < len; ++i) {
        StyleBase* rule = item(i);
        if (rule->isImportRule()) {
            XSLImportRule* import = static_cast<XSLImportRule*>(rule);
            if (import->styleSheet())
                import->styleSheet()->clearDocuments();
        }
    }
}

DocLoader* XSLStyleSheet::docLoader()
{
    if (!m_ownerDocument)
        return 0;
    return m_ownerDocument->docLoader();
}

bool XSLStyleSheet::parseString(const String& string, bool strict)
{
    // Parse in a single chunk into an xmlDocPtr
    const UChar BOM = 0xFEFF;
    const unsigned char BOMHighByte = *reinterpret_cast<const unsigned char*>(&BOM);
    setLoaderForLibXMLCallbacks(docLoader());
    if (!m_stylesheetDocTaken)
#ifdef __OWB_BIXML__
        m_xml->xmlFreeDoc(m_stylesheetDoc);
#else
        xmlFreeDoc(m_stylesheetDoc);
#endif

    m_stylesheetDocTaken = false;


#ifdef __OWB_BIXML__
    m_stylesheetDoc = m_xml->xmlReadMemory(reinterpret_cast<const char*>(string.characters()), string.length() * sizeof(UChar),
            m_ownerDocument->URL().ascii(),
            BOMHighByte == 0xFF ? "UTF-16LE" : "UTF-16BE",
            BTXML_PARSE_NOENT | BTXML_PARSE_DTDATTR | BTXML_PARSE_NOERROR | BTXML_PARSE_NOWARNING | BTXML_PARSE_NOCDATA);
#else
    m_stylesheetDoc = xmlReadMemory(reinterpret_cast<const char*>(string.characters()), string.length() * sizeof(UChar),
            m_ownerDocument->URL().ascii(),
            BOMHighByte == 0xFF ? "UTF-16LE" : "UTF-16BE",
            XML_PARSE_NOENT | XML_PARSE_DTDATTR | XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NOCDATA);
#endif

    loadChildSheets();
    setLoaderForLibXMLCallbacks(0);
    return m_stylesheetDoc;
}

void XSLStyleSheet::loadChildSheets()
{
    if (!document())
        return;

#ifdef __OWB_BIXML__
    BTXMLNode *stylesheetRoot = document()->children;
#else
    xmlNodePtr stylesheetRoot = document()->children;
#endif

    // Top level children may include other things such as DTD nodes, we ignore those.
#ifdef __OWB_BIXML__
    while (stylesheetRoot && stylesheetRoot->type != BTXML_ELEMENT_NODE)
#else
    while (stylesheetRoot && stylesheetRoot->type != XML_ELEMENT_NODE)
#endif
        stylesheetRoot = stylesheetRoot->next;

    if (m_embedded) {
        // We have to locate (by ID) the appropriate embedded stylesheet element, so that we can walk the
        // import/include list.
#ifdef __OWB_BIXML__
        BTXMLAttr *idNode = m_xml->xmlGetID(document(), (const unsigned char*)(href().deprecatedString().utf8().data()));
#else
        xmlAttrPtr idNode = xmlGetID(document(), (const xmlChar*)(href().utf8().data()));
#endif
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
#ifdef __OWB_BIXML__
        BTXMLNode *curr = stylesheetRoot->children;
#else
        xmlNodePtr curr = stylesheetRoot->children;
#endif
        while (curr) {
            if (IS_BLANK_NODE(curr)) {
                curr = curr->next;
                continue;
            }
#ifdef __OWB_BIXML__
            if (curr->type == BTXML_ELEMENT_NODE
             && m_xlst->isXsltElem( curr )
             && m_xlst->isXlstName( curr, "import" ) ) {
                unsigned char* uriRef = m_xlst->xsltGetNsProp(curr, (const unsigned char*)"href", BIXSLT_NAMESPACE);
#else
            if (curr->type == XML_ELEMENT_NODE && IS_XSLT_ELEM(curr) && IS_XSLT_NAME(curr, "import")) {
                xmlChar* uriRef = xsltGetNsProp(curr, (const xmlChar*)"href", XSLT_NAMESPACE);
#endif
                loadChildSheet(DeprecatedString::fromUtf8((const char*)uriRef));
#ifdef __OWB_BIXML__
                m_xml->xmlFree(uriRef);
#else
                xmlFree(uriRef);
#endif
            } else
                break;
            curr = curr->next;
        }

        // Now handle includes.
        while (curr) {
#ifdef __OWB_BIXML__
            if (curr->type == BTXML_ELEMENT_NODE
             && m_xlst->isXsltElem(curr)
             && m_xlst->isXlstName(curr, "include")) {
                unsigned char* uriRef = m_xlst->xsltGetNsProp(curr, (const unsigned char*)"href", BIXSLT_NAMESPACE);

                loadChildSheet(DeprecatedString::fromUtf8((const char*)uriRef));
                m_xml->xmlFree(uriRef);
#else
            if (curr->type == XML_ELEMENT_NODE && IS_XSLT_ELEM(curr) && IS_XSLT_NAME(curr, "include")) {
                xmlChar* uriRef = xsltGetNsProp(curr, (const xmlChar*)"href", XSLT_NAMESPACE);

                loadChildSheet(DeprecatedString::fromUtf8((const char*)uriRef));
                xmlFree(uriRef);
#endif
            }
            curr = curr->next;
        }
    }
}

void XSLStyleSheet::loadChildSheet(const DeprecatedString& href)
{
    RefPtr<XSLImportRule> childRule = new XSLImportRule(this, href);
    append(childRule);
    childRule->loadSheet();
}

#ifdef __OWB_BIXML__
BAL::BTXSLTStyleSheet* XSLStyleSheet::compileStyleSheet()
#else
xsltStylesheetPtr XSLStyleSheet::compileStyleSheet()
#endif
{
    // FIXME: Hook up error reporting for the stylesheet compilation process.
    if (m_embedded)
#ifdef __OWB_BIXML__
        return m_xlst->xsltLoadStylesheetPI(document());
#else
        return xsltLoadStylesheetPI(document());
#endif
    // xsltParseStylesheetDoc makes the document part of the stylesheet
    // so we have to release our pointer to it.
    ASSERT(!m_stylesheetDocTaken);

#ifdef __OWB_BIXML__
    BAL::BTXSLTStyleSheet* result = m_xlst->xsltParseStylesheetDoc(m_stylesheetDoc);
#else
    xsltStylesheetPtr result = xsltParseStylesheetDoc(m_stylesheetDoc);
#endif
    if (result)
        m_stylesheetDocTaken = true;
    return result;
}
#ifdef __OWB_BIXML__
BTXMLDoc *XSLStyleSheet::locateStylesheetSubResource(BTXMLDoc *parentDoc, const unsigned char* uri)
#else
xmlDocPtr XSLStyleSheet::locateStylesheetSubResource(xmlDocPtr parentDoc, const xmlChar* uri)
#endif
{
    bool matchedParent = (parentDoc == document());
    unsigned len = length();
    for (unsigned i = 0; i < len; ++i) {
        StyleBase* rule = item(i);
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
#ifdef __OWB_BIXML__
                unsigned char* base = m_xml->xmlNodeGetBase(parentDoc, (BTXMLNode *)parentDoc);
                unsigned char* childURI = m_xml->xmlBuildURI((const unsigned char*)(const char*)importHref, base);

                bool equalURIs = m_xml->xmlStrEqual(uri, childURI);
                m_xml->xmlFree(base);
                m_xml->xmlFree(childURI);
#else
                xmlChar* base = xmlNodeGetBase(parentDoc, (xmlNodePtr)parentDoc);
                xmlChar* childURI = xmlBuildURI((const xmlChar*)(const char*)importHref, base);

                bool equalURIs = xmlStrEqual(uri, childURI);
                xmlFree(base);
                xmlFree(childURI);
#endif

                if (equalURIs) {
                    child->markAsProcessed();
                    return child->document();
                }
            } else {
#ifdef __OWB_BIXML__
                BTXMLDoc *result = import->styleSheet()->locateStylesheetSubResource(parentDoc, uri);
#else
                xmlDocPtr result = import->styleSheet()->locateStylesheetSubResource(parentDoc, uri);
#endif
                if (result)
                    return result;
            }
        }
    }

    return 0;
}

void XSLStyleSheet::markAsProcessed()
{
    ASSERT(!m_processed);
    ASSERT(!m_stylesheetDocTaken);
    m_processed = true;
    m_stylesheetDocTaken = true;
}

} // namespace WebCore

#endif // XSLT_SUPPORT
