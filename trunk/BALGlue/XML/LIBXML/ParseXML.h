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

/**
 * @file  ParseXML.h
 *
 * Header file for ParseXML.
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

#ifndef PARSEXML_H
#define PARSEXML_H

#include <libxml/tree.h>
#include <libxml/xmlstring.h>
#include "DocLoader.h"
#include "Document.h"
#include "DocumentFragment.h"
#include "Element.h"
#include "PlatformString.h"

namespace WebCore {
    class Document;
    class String;
    class XMLTokenizer;
}

namespace BAL {
class PendingCallbacks;
struct AttributeParseState;
using WebCore::Document;
using WebCore::String;
using WebCore::DocumentFragment;
using WebCore::Element;
using WebCore::DocLoader;
using WebCore::XMLTokenizer;

class ParseXML {
    public:
        ParseXML(XMLTokenizer *t);
        ~ParseXML();

        enum ErrorType { warning, nonFatal, fatal };

        void parse(String parseString);
        void end();
        int lineNumber() const;
        int columnNumber() const;
        void stopParsing();
        void resumeParsing();
        bool parseXMLDocumentFragment(const String& string, DocumentFragment* fragment, Element* parent);
        HashMap<String, String> parseAttributes(const String& string, bool& attrsOK);
        void* xmlDocPtrForString(DocLoader*, const String& source, const DeprecatedString& URL);

        XMLTokenizer *tokenizer() {return m_tokenizer;}
        
        // callbacks from parser SAX
        void error(ErrorType, const char* message, va_list args);
        void startElementNs(const xmlChar* xmlLocalName, const xmlChar* xmlPrefix, const xmlChar* xmlURI, int nb_namespaces,
                            const xmlChar** namespaces, int nb_attributes, int nb_defaulted, const xmlChar** libxmlAttributes);
        void endElementNs();
        void characters(const xmlChar* s, int len);
        void processingInstruction(const xmlChar* target, const xmlChar* data);
        void cdataBlock(const xmlChar* s, int len);
        void comment(const xmlChar* s);
        void startDocument(const xmlChar* version, const xmlChar* encoding, int standalone);
        void internalSubset(const xmlChar* name, const xmlChar* externalID, const xmlChar* systemID);

    private:
        void initializeParserContext();

        xmlParserCtxtPtr m_context;
        OwnPtr<PendingCallbacks> m_pendingCallbacks;
        XMLTokenizer *m_tokenizer;
};

void setLoaderForLibXMLCallbacks(DocLoader*);

}

#endif
