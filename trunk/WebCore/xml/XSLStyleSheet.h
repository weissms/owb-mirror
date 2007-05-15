/*
 * This file is part of the XSL implementation.
 *
 * Copyright (C) 2004, 2006 Apple Computer, Inc.
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
 *
 */

#ifndef XSLStyleSheet_h
#define XSLStyleSheet_h

#ifdef XSLT_SUPPORT

#include "StyleSheet.h"

#ifdef __OWB_BIXML__
#include <BIXSLT.h>
#include <BIXML.h>
namespace BAL {
class BIXSLT;
}
#else
#include <libxml/parser.h>
#include <libxslt/transform.h>
#endif

namespace WebCore {

class DocLoader;
class Document;
class XSLImportRule;


class XSLStyleSheet : public StyleSheet {
public:
    XSLStyleSheet(Node* parentNode, const String& href = String(), bool embedded = false);
    XSLStyleSheet(XSLImportRule* parentImport, const String& href = String());
    ~XSLStyleSheet();

    virtual bool isXSLStyleSheet() const { return true; }

    virtual String type() const { return "text/xml"; }

    virtual bool parseString(const String &string, bool strict = true);

    virtual bool isLoading();
    virtual void checkLoaded();

    void loadChildSheets();
    void loadChildSheet(const DeprecatedString& href);

#ifdef __OWB_BIXML__
    BAL::BTXSLTStyleSheet* compileStyleSheet();
#else
    xsltStylesheetPtr compileStyleSheet();
#endif
    DocLoader* docLoader();

    Document* ownerDocument() { return m_ownerDocument; }
    void setOwnerDocument(Document* doc) { m_ownerDocument = doc; }

#ifdef __OWB_BIXML__
    BAL::BTXMLDoc *document();
#else
    xmlDocPtr document();
#endif
    void clearDocuments();

#ifdef __OWB_BIXML__
    BAL::BTXMLDoc *locateStylesheetSubResource(BAL::BTXMLDoc *parentDoc, const unsigned char* uri);
#else
    xmlDocPtr locateStylesheetSubResource(xmlDocPtr parentDoc, const xmlChar* uri);
#endif

    void markAsProcessed();
    bool processed() const { return m_processed; }

protected:
    Document* m_ownerDocument;
#ifdef __OWB_BIXML__
    BAL::BIXSLT *m_xlst;
    BAL::BIXML *m_xml;
    BAL::BTXMLDoc *m_stylesheetDoc;
#else
    xmlDocPtr m_stylesheetDoc;
#endif
    bool m_embedded;
    bool m_processed;
    bool m_stylesheetDocTaken;
};

} // namespace WebCore

#endif // XSLT_SUPPORT

#endif // XSLStyleSheet_h
