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

#include "ParseXML.h"

#include "config.h"
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#if ENABLE(XSLT)
#include <libxslt/xslt.h>
#endif
#include "DeprecatedPtrList.h"
#include "XMLTokenizer.h"
#include "ResourceError.h"
#include "ResourceResponse.h"
#include "Frame.h"
#include "FrameView.h"
#include "FrameLoader.h"
#include "TextEncoding.h"
#include "QualifiedName.h"
#include "HTMLNames.h"
#include "HTMLScriptElement.h"
#include "HTMLStyleElement.h"
#include "HTMLTableSectionElement.h"
#include "HTMLTokenizer.h"
#include "CString.h"
#include "Node.h"
#include "CachedScript.h"
#include "CharacterData.h"
#include "Text.h"
#include "ProcessingInstruction.h"
#include "CDATASection.h"
#include "Comment.h"
#include "DocumentType.h"

using WebCore::Document;
using WebCore::String;
using WebCore::DeprecatedPtrList;
using WebCore::ResourceError;
using WebCore::ResourceResponse;
using WebCore::Frame;
using WebCore::FrameView;
using WebCore::UTF8Encoding;
using std::min;
using WebCore::QualifiedName;
using WebCore::HTMLNames::scriptTag;
using WebCore::HTMLNames::srcAttr;
using WebCore::HTMLNames::charsetAttr;
using WebCore::HTMLScriptElement;
using WebCore::HTMLStyleElement;
using WebCore::Node;
using WebCore::CachedScript;
using WebCore::CharacterData;
using WebCore::Text;
using WebCore::ProcessingInstruction;
using WebCore::CDATASection;
using WebCore::Comment;
using WebCore::DocumentType;
using WebCore::CString;

namespace BAL {

    typedef int ExceptionCode;

    class PendingCallbacks {
    public:
        PendingCallbacks()
        {
            m_callbacks.setAutoDelete(true);
        }
        
        void appendStartElementNSCallback(const xmlChar* xmlLocalName, const xmlChar* xmlPrefix, const xmlChar* xmlURI, int nb_namespaces,
                                        const xmlChar** namespaces, int nb_attributes, int nb_defaulted, const xmlChar** attributes)
        {
            PendingStartElementNSCallback* callback = new PendingStartElementNSCallback;
            
            callback->xmlLocalName = xmlStrdup(xmlLocalName);
            callback->xmlPrefix = xmlStrdup(xmlPrefix);
            callback->xmlURI = xmlStrdup(xmlURI);
            callback->nb_namespaces = nb_namespaces;
            callback->namespaces = reinterpret_cast<xmlChar**>(xmlMalloc(sizeof(xmlChar*) * nb_namespaces * 2));
            for (int i = 0; i < nb_namespaces * 2 ; i++)
                callback->namespaces[i] = xmlStrdup(namespaces[i]);
            callback->nb_attributes = nb_attributes;
            callback->nb_defaulted = nb_defaulted;
            callback->attributes =  reinterpret_cast<xmlChar**>(xmlMalloc(sizeof(xmlChar*) * nb_attributes * 5));
            for (int i = 0; i < nb_attributes; i++) {
                // Each attribute has 5 elements in the array:
                // name, prefix, uri, value and an end pointer.
                
                for (int j = 0; j < 3; j++)
                    callback->attributes[i * 5 + j] = xmlStrdup(attributes[i * 5 + j]);
                
                int len = attributes[i * 5 + 4] - attributes[i * 5 + 3];
    
                callback->attributes[i * 5 + 3] = xmlStrndup(attributes[i * 5 + 3], len);
                callback->attributes[i * 5 + 4] = callback->attributes[i * 5 + 3] + len;
            }
            
            m_callbacks.append(callback);
        }
    
        void appendEndElementNSCallback()
        {
            PendingEndElementNSCallback* callback = new PendingEndElementNSCallback;
            
            m_callbacks.append(callback);
        }
        
        void appendCharactersCallback(const xmlChar* s, int len)
        {
            PendingCharactersCallback* callback = new PendingCharactersCallback;
            
            callback->s = xmlStrndup(s, len);
            callback->len = len;
            
            m_callbacks.append(callback);        
        }
        
        void appendProcessingInstructionCallback(const xmlChar* target, const xmlChar* data)
        {
            PendingProcessingInstructionCallback* callback = new PendingProcessingInstructionCallback;
            
            callback->target = xmlStrdup(target);
            callback->data = xmlStrdup(data);
            
            m_callbacks.append(callback);
        }
        
        void appendCDATABlockCallback(const xmlChar* s, int len)
        {
            PendingCDATABlockCallback* callback = new PendingCDATABlockCallback;
            
            callback->s = xmlStrndup(s, len);
            callback->len = len;
            
            m_callbacks.append(callback);        
        }
    
        void appendCommentCallback(const xmlChar* s)
        {
            PendingCommentCallback* callback = new PendingCommentCallback;
            
            callback->s = xmlStrdup(s);
            
            m_callbacks.append(callback);        
        }
    
        void appendInternalSubsetCallback(const xmlChar* name, const xmlChar* externalID, const xmlChar* systemID)
        {
            PendingInternalSubsetCallback* callback = new PendingInternalSubsetCallback;
            
            callback->name = xmlStrdup(name);
            callback->externalID = xmlStrdup(externalID);
            callback->systemID = xmlStrdup(systemID);
            
            m_callbacks.append(callback);        
        }
        
        void appendErrorCallback(XMLTokenizer::ErrorType type, const char* message, int lineNumber, int columnNumber)
        {
            PendingErrorCallback* callback = new PendingErrorCallback;
            
            callback->message = strdup(message);
            callback->type = type;
            callback->lineNumber = lineNumber;
            callback->columnNumber = columnNumber;
            
            m_callbacks.append(callback);
        }
    
        void callAndRemoveFirstCallback(ParseXML* parser)
        {
            PendingCallback* cb = m_callbacks.getFirst();
                
            cb->call(parser);
            m_callbacks.removeFirst();
        }
        
        bool isEmpty() const { return m_callbacks.isEmpty(); }
        
    private:    
        struct PendingCallback {        
            
            virtual ~PendingCallback() { } 
    
            virtual void call(ParseXML* parser) = 0;
        };  
        
        struct PendingStartElementNSCallback : public PendingCallback {        
            virtual ~PendingStartElementNSCallback() {
                xmlFree(xmlLocalName);
                xmlFree(xmlPrefix);
                xmlFree(xmlURI);
                for (int i = 0; i < nb_namespaces * 2; i++)
                    xmlFree(namespaces[i]);
                xmlFree(namespaces);
                for (int i = 0; i < nb_attributes; i++)
                    for (int j = 0; j < 4; j++) 
                        xmlFree(attributes[i * 5 + j]);
                xmlFree(attributes);
            }
            
            virtual void call(ParseXML* parser) {
                parser->startElementNs(xmlLocalName, xmlPrefix, xmlURI, 
                                        nb_namespaces, (const xmlChar**)namespaces,
                                        nb_attributes, nb_defaulted, (const xmlChar**)(attributes));
            }
    
            xmlChar* xmlLocalName;
            xmlChar* xmlPrefix;
            xmlChar* xmlURI;
            int nb_namespaces;
            xmlChar** namespaces;
            int nb_attributes;
            int nb_defaulted;
            xmlChar** attributes;
        };
        
        struct PendingEndElementNSCallback : public PendingCallback {
            virtual void call(ParseXML* parser) 
            {
                parser->endElementNs();
            }
        };
        
        struct PendingCharactersCallback : public PendingCallback {
            virtual ~PendingCharactersCallback() 
            {
                xmlFree(s);
            }
        
            virtual void call(ParseXML* parser) 
            {
                parser->characters(s, len);
            }
            
            xmlChar* s;
            int len;
        };
    
        struct PendingProcessingInstructionCallback : public PendingCallback {
            virtual ~PendingProcessingInstructionCallback() 
            {
                xmlFree(target);
                xmlFree(data);
            }
            
            virtual void call(ParseXML* parser) 
            {
                parser->processingInstruction(target, data);
            }
            
            xmlChar* target;
            xmlChar* data;
        };
        
        struct PendingCDATABlockCallback : public PendingCallback {
            virtual ~PendingCDATABlockCallback() 
            {
                xmlFree(s);
            }
            
            virtual void call(ParseXML* parser) 
            {
                parser->cdataBlock(s, len);
            }
            
            xmlChar* s;
            int len;
        };
    
        struct PendingCommentCallback : public PendingCallback {
            virtual ~PendingCommentCallback() 
            {
                xmlFree(s);
            }
            
            virtual void call(ParseXML* parser) 
            {
                parser->comment(s);
            }
    
            xmlChar* s;
        };
        
        struct PendingInternalSubsetCallback : public PendingCallback {
            virtual ~PendingInternalSubsetCallback() 
            {
                xmlFree(name);
                xmlFree(externalID);
                xmlFree(systemID);
            }
            
            virtual void call(ParseXML* parser)
            {
                parser->internalSubset(name, externalID, systemID);
            }
            
            xmlChar* name;
            xmlChar* externalID;
            xmlChar* systemID;        
        };
        
        struct PendingErrorCallback: public PendingCallback {
            virtual ~PendingErrorCallback() 
            {
                free (message);
            }
            
            virtual void call(ParseXML* parser) 
            {
                parser->tokenizer()->handleError(type, message, lineNumber, columnNumber);
            }
            
            XMLTokenizer::ErrorType type;
            char* message;
            int lineNumber;
            int columnNumber;
        };
        
    public:
        DeprecatedPtrList<PendingCallback> m_callbacks;
    };

    ParseXML::ParseXML(XMLTokenizer *t)
    : m_pendingCallbacks(new PendingCallbacks)
    , m_tokenizer(t)
    , m_context(0)
    {
    }

    ParseXML::~ParseXML()
    {
        m_tokenizer = 0;
    }

    static int globalDescriptor = 0;

    static int matchFunc(const char* uri)
    {
        return 1; // Match everything.
    }

    static DocLoader* globalDocLoader = 0;

    class OffsetBuffer {
    public:
        OffsetBuffer(const Vector<char>& b) : m_buffer(b), m_currentOffset(0) { }
        
        int readOutBytes(char* outputBuffer, unsigned askedToRead) {
            unsigned bytesLeft = m_buffer.size() - m_currentOffset;
            unsigned lenToCopy = min(askedToRead, bytesLeft);
            if (lenToCopy) {
                memcpy(outputBuffer, m_buffer.data() + m_currentOffset, lenToCopy);
                m_currentOffset += lenToCopy;
            }
            return lenToCopy;
        }
    
    private:
        Vector<char> m_buffer;
        unsigned m_currentOffset;
    };

    static bool shouldAllowExternalLoad(const char* inURI)
    {
        if (strstr(inURI, "/etc/xml/catalog")
                || strstr(inURI, "http://www.w3.org/Graphics/SVG") == inURI
                || strstr(inURI, "http://www.w3.org/TR/xhtml") == inURI)
            return false;
        return true;
    }
    static void* openFunc(const char* uri)
    {
        if (!globalDocLoader || !shouldAllowExternalLoad(uri))
            return &globalDescriptor;
    
        ResourceError error;
        ResourceResponse response;
        Vector<char> data;
        
        if (globalDocLoader->frame()) 
            globalDocLoader->frame()->loader()->loadResourceSynchronously(KURL(uri), error, response, data);
    
        return new OffsetBuffer(data);
    }
    
    static int readFunc(void* context, char* buffer, int len)
    {
        // Do 0-byte reads in case of a null descriptor
        if (context == &globalDescriptor)
            return 0;
            
        OffsetBuffer* data = static_cast<OffsetBuffer*>(context);
        return data->readOutBytes(buffer, len);
    }
    
    static int writeFunc(void* context, const char* buffer, int len)
    {
        // Always just do 0-byte writes
        return 0;
    }
    
    static int closeFunc(void* context)
    {
        if (context != &globalDescriptor) {
            OffsetBuffer* data = static_cast<OffsetBuffer*>(context);
            delete data;
        }
        return 0;
    }
    
    static void errorFunc(void*, const char*, ...)
    {
        // FIXME: It would be nice to display error messages somewhere.
    }
    
    void setLoaderForLibXMLCallbacks(DocLoader* docLoader)
    {
        globalDocLoader = docLoader;
    }
    
    static xmlParserCtxtPtr createStringParser(xmlSAXHandlerPtr handlers, void* userData)
    {
        static bool didInit = false;
        if (!didInit) {
            xmlInitParser();
            xmlRegisterInputCallbacks(matchFunc, openFunc, readFunc, closeFunc);
            xmlRegisterOutputCallbacks(matchFunc, openFunc, writeFunc, closeFunc);
            didInit = true;
        }
    
        xmlParserCtxtPtr parser = xmlCreatePushParserCtxt(handlers, 0, 0, 0, 0);
        parser->_private = userData;
        parser->replaceEntities = true;
        const UChar BOM = 0xFEFF;
        const unsigned char BOMHighByte = *reinterpret_cast<const unsigned char*>(&BOM);
        xmlSwitchEncoding(parser, BOMHighByte == 0xFF ? XML_CHAR_ENCODING_UTF16LE : XML_CHAR_ENCODING_UTF16BE);
        return parser;
    }

    void ParseXML::parse(String parseString)
    {
        if (!m_context)
            initializeParserContext();
        // libXML throws an error if you try to switch the encoding for an empty string.
        if (parseString.length()) {
            // Hack around libxml2's lack of encoding overide support by manually
            // resetting the encoding to UTF-16 before every chunk.  Otherwise libxml
            // will detect <?xml version="1.0" encoding="<encoding name>"?> blocks 
            // and switch encodings, causing the parse to fail.
            const UChar BOM = 0xFEFF;
            const unsigned char BOMHighByte = *reinterpret_cast<const unsigned char*>(&BOM);
            xmlSwitchEncoding(m_context, BOMHighByte == 0xFF ? XML_CHAR_ENCODING_UTF16LE : XML_CHAR_ENCODING_UTF16BE);
    
            xmlParseChunk(m_context, reinterpret_cast<const char*>(parseString.characters()), sizeof(UChar) * parseString.length(), 0);
        }
    }

    static inline String toString(const xmlChar* str, unsigned len)
    {
        return UTF8Encoding().decode(reinterpret_cast<const char*>(str), len);
    }
    
    static inline String toString(const xmlChar* str)
    {
        if (!str)
            return String();
        
        return UTF8Encoding().decode(reinterpret_cast<const char*>(str), strlen(reinterpret_cast<const char*>(str)));
    }
    
    struct _xmlSAX2Namespace {
        const xmlChar* prefix;
        const xmlChar* uri;
    };
    typedef struct _xmlSAX2Namespace xmlSAX2Namespace;
    
    static inline void handleElementNamespaces(Element* newElement, const xmlChar** libxmlNamespaces, int nb_namespaces, ExceptionCode& ec)
    {
        xmlSAX2Namespace* namespaces = reinterpret_cast<xmlSAX2Namespace*>(libxmlNamespaces);
        for(int i = 0; i < nb_namespaces; i++) {
            String namespaceQName = "xmlns";
            String namespaceURI = toString(namespaces[i].uri);
            if (namespaces[i].prefix)
                namespaceQName = "xmlns:" + toString(namespaces[i].prefix);
            newElement->setAttributeNS("http://www.w3.org/2000/xmlns/", namespaceQName, namespaceURI, ec);
            if (ec) // exception setting attributes
                return;
        }
    }
    
    struct _xmlSAX2Attributes {
        const xmlChar* localname;
        const xmlChar* prefix;
        const xmlChar* uri;
        const xmlChar* value;
        const xmlChar* end;
    };
    typedef struct _xmlSAX2Attributes xmlSAX2Attributes;
    
    static inline void handleElementAttributes(Element* newElement, const xmlChar** libxmlAttributes, int nb_attributes, ExceptionCode& ec)
    {
        xmlSAX2Attributes* attributes = reinterpret_cast<xmlSAX2Attributes*>(libxmlAttributes);
        for(int i = 0; i < nb_attributes; i++) {
            String attrLocalName = toString(attributes[i].localname);
            int valueLength = (int) (attributes[i].end - attributes[i].value);
            String attrValue = toString(attributes[i].value, valueLength);
            String attrPrefix = toString(attributes[i].prefix);
            String attrURI = attrPrefix.isEmpty() ? String() : toString(attributes[i].uri);
            String attrQName = attrPrefix.isEmpty() ? attrLocalName : attrPrefix + ":" + attrLocalName;
            
            newElement->setAttributeNS(attrURI, attrQName, attrValue, ec);
            if (ec) // exception setting attributes
                return;
        }
    }
    
    void ParseXML::startElementNs(const xmlChar* xmlLocalName, const xmlChar* xmlPrefix, const xmlChar* xmlURI, int nb_namespaces,
                                    const xmlChar** libxmlNamespaces, int nb_attributes, int nb_defaulted, const xmlChar** libxmlAttributes)
    {
        if (m_tokenizer->m_parserStopped)
            return;
        
        if (m_tokenizer->m_parserPaused) {
            m_pendingCallbacks->appendStartElementNSCallback(xmlLocalName, xmlPrefix, xmlURI, nb_namespaces, libxmlNamespaces,
                                                            nb_attributes, nb_defaulted, libxmlAttributes);
            return;
        }
        
        m_tokenizer->m_sawFirstElement = true;
    
        m_tokenizer->exitText();
    
        String localName = toString(xmlLocalName);
        String uri = toString(xmlURI);
        String prefix = toString(xmlPrefix);
        
        if (m_tokenizer->m_parsingFragment && uri.isNull()) {
            if (!prefix.isNull())
                uri = m_tokenizer->m_prefixToNamespaceMap.get(prefix);
            else
                uri = m_tokenizer->m_defaultNamespaceURI;
        }
    
        ExceptionCode ec = 0;
        QualifiedName qName(prefix, localName, uri);
        RefPtr<Element> newElement = m_tokenizer->m_doc->createElement(qName, true, ec);
        if (!newElement) {
            stopParsing();
            return;
        }
        
        handleElementNamespaces(newElement.get(), libxmlNamespaces, nb_namespaces, ec);
        if (ec) {
            stopParsing();
            return;
        }
        
        handleElementAttributes(newElement.get(), libxmlAttributes, nb_attributes, ec);
        if (ec) {
            stopParsing();
            return;
        }
    
        if (newElement->hasTagName(scriptTag))
            static_cast<HTMLScriptElement*>(newElement.get())->setCreatedByParser(true);
        else if (newElement->hasTagName(WebCore::HTMLNames::styleTag))
            static_cast<HTMLStyleElement*>(newElement.get())->setCreatedByParser(true);
    #if ENABLE(SVG)
        else if (newElement->hasTagName(WebCore::SVGNames::styleTag))
            static_cast<SVGStyleElement*>(newElement.get())->setCreatedByParser(true);
    #endif
        
        if (newElement->hasTagName(WebCore::HTMLNames::scriptTag)
    #if ENABLE(SVG)
            || newElement->hasTagName(WebCore::SVGNames::scriptTag)
    #endif
            )
            m_tokenizer->m_scriptStartLine = lineNumber();
        
        if (!m_tokenizer->m_currentNode->addChild(newElement.get())) {
            stopParsing();
            return;
        }
        
        m_tokenizer->m_currentNode = newElement.get();
        if (m_tokenizer->m_view && !newElement->attached())
            newElement->attach();
    }
    
    void ParseXML::endElementNs()
    {
        if (m_tokenizer->m_parserStopped)
            return;
    
        if (m_tokenizer->m_parserPaused) {
            m_pendingCallbacks->appendEndElementNSCallback();
            return;
        }
        
        m_tokenizer->exitText();
    
        Node* n = m_tokenizer->m_currentNode;
        RefPtr<Node> parent = n->parentNode();
        n->finishedParsing();
        
        // don't load external scripts for standalone documents (for now)
        if (n->isElementNode() && m_tokenizer->m_view && (static_cast<Element*>(n)->hasTagName(scriptTag) 
    #if ENABLE(SVG)
                                            || static_cast<Element*>(n)->hasTagName(SVGNames::scriptTag)
    #endif
                                            )) {
    
                                            
            ASSERT(!m_tokenizer->m_pendingScript);
            
            m_tokenizer->m_requestingScript = true;
            
            Element* scriptElement = static_cast<Element*>(n);        
            String scriptHref;
            
            if (static_cast<Element*>(n)->hasTagName(scriptTag))
                scriptHref = scriptElement->getAttribute(srcAttr);
    #if ENABLE(SVG)
            else if (static_cast<Element*>(n)->hasTagName(SVGNames::scriptTag))
                scriptHref = scriptElement->getAttribute(XLinkNames::hrefAttr);
    #endif
            
            if (!scriptHref.isEmpty()) {
                // we have a src attribute 
                const AtomicString& charset = scriptElement->getAttribute(charsetAttr);
                if ((m_tokenizer->m_pendingScript = m_tokenizer->m_doc->docLoader()->requestScript(scriptHref, charset))) {
                    m_tokenizer->m_scriptElement = scriptElement;
                    m_tokenizer->m_pendingScript->ref(m_tokenizer);
                        
                    // m_pendingScript will be 0 if script was already loaded and ref() executed it
                    if (m_tokenizer->m_pendingScript)
                        m_tokenizer->pauseParsing();
                } else 
                    m_tokenizer->m_scriptElement = 0;
    
            } else {
                String scriptCode = "";
                for (Node* child = scriptElement->firstChild(); child; child = child->nextSibling()) {
                    if (child->isTextNode() || child->nodeType() == Node::CDATA_SECTION_NODE)
                        scriptCode += static_cast<CharacterData*>(child)->data();
                }
    #ifdef __OWB_JS__
                m_tokenizer->m_view->frame()->loader()->executeScript(m_tokenizer->m_doc->URL(), m_tokenizer->m_scriptStartLine - 1, scriptCode);
    #endif //__OWB_JS__
            }
            
            m_tokenizer->m_requestingScript = false;
        }
    
        m_tokenizer->m_currentNode = parent.get();
    }
    
    void ParseXML::characters(const xmlChar* s, int len)
    {
        if (m_tokenizer->m_parserStopped)
            return;
        
        if (m_tokenizer->m_parserPaused) {
            m_pendingCallbacks->appendCharactersCallback(s, len);
            return;
        }
        
        if (m_tokenizer->m_currentNode->isTextNode() || m_tokenizer->enterText()) {
            ExceptionCode ec = 0;
            static_cast<Text*>(m_tokenizer->m_currentNode)->appendData(toString(s, len), ec);
        }
    }
    
    void ParseXML::error(ErrorType type, const char* message, va_list args)
    {
        if (m_tokenizer->m_parserStopped)
            return;
    
    #if PLATFORM(WIN_OS)
        char m[1024];
        vsnprintf(m, sizeof(m) - 1, message, args);
    #else
        char* m;
        vasprintf(&m, message, args);
    #endif
        
        if (m_tokenizer->m_parserPaused)
            m_pendingCallbacks->appendErrorCallback((XMLTokenizer::ErrorType)type, m, lineNumber(), columnNumber());
        else
            m_tokenizer->handleError((XMLTokenizer::ErrorType)type, m, lineNumber(), columnNumber());
    
    #if !PLATFORM(WIN_OS)
        free(m);
    #endif
    }
    
    void ParseXML::processingInstruction(const xmlChar* target, const xmlChar* data)
    {
        if (m_tokenizer->m_parserStopped)
            return;
    
        if (m_tokenizer->m_parserPaused) {
            m_pendingCallbacks->appendProcessingInstructionCallback(target, data);
            return;
        }
        
        m_tokenizer->exitText();
    
        // ### handle exceptions
        int exception = 0;
        RefPtr<ProcessingInstruction> pi = m_tokenizer->m_doc->createProcessingInstruction(
            toString(target), toString(data), exception);
        if (exception)
            return;
    
        if (!m_tokenizer->m_currentNode->addChild(pi.get()))
            return;
        if (m_tokenizer->m_view && !pi->attached())
            pi->attach();
    
        // don't load stylesheets for standalone documents
        if (m_tokenizer->m_doc->frame()) {
            m_tokenizer->m_sawXSLTransform = !m_tokenizer->m_sawFirstElement && !pi->checkStyleSheet();
    #if ENABLE(XSLT)
            // Pretend we didn't see this PI if we're the result of a transform.
            if (m_tokenizer->m_sawXSLTransform && !m_tokenizer->m_doc->transformSourceDocument())
    #else
            if (m_tokenizer->m_sawXSLTransform)
    #endif
                // Stop the SAX parser.
                stopParsing();
        }
    }
    
    void ParseXML::cdataBlock(const xmlChar* s, int len)
    {
        if (m_tokenizer->m_parserStopped)
            return;
    
        if (m_tokenizer->m_parserPaused) {
            m_pendingCallbacks->appendCDATABlockCallback(s, len);
            return;
        }
        
        m_tokenizer->exitText();
    
        RefPtr<Node> newNode = new CDATASection(m_tokenizer->m_doc, toString(s, len));
        if (!m_tokenizer->m_currentNode->addChild(newNode.get()))
            return;
        if (m_tokenizer->m_view && !newNode->attached())
            newNode->attach();
    }
    
    void ParseXML::comment(const xmlChar* s)
    {
        if (m_tokenizer->m_parserStopped)
            return;
    
        if (m_tokenizer->m_parserPaused) {
            m_pendingCallbacks->appendCommentCallback(s);
            return;
        }
        
        m_tokenizer->exitText();
    
        RefPtr<Node> newNode = new Comment(m_tokenizer->m_doc, toString(s));
        m_tokenizer->m_currentNode->addChild(newNode.get());
        if (m_tokenizer->m_view && !newNode->attached())
            newNode->attach();
    }
    
    void ParseXML::startDocument(const xmlChar* version, const xmlChar* encoding, int standalone)
    {
        ExceptionCode ec = 0;
    
        if (version)
            m_tokenizer->m_doc->setXMLVersion(toString(version), ec);
        m_tokenizer->m_doc->setXMLStandalone(standalone == 1, ec); // possible values are 0, 1, and -1
        if (encoding)
            m_tokenizer->m_doc->setXMLEncoding(toString(encoding));
    }
    
    void ParseXML::internalSubset(const xmlChar* name, const xmlChar* externalID, const xmlChar* systemID)
    {
        if (m_tokenizer->m_parserStopped)
            return;
    
        if (m_tokenizer->m_parserPaused) {
            m_pendingCallbacks->appendInternalSubsetCallback(name, externalID, systemID);
            return;
        }
        
        Document* doc = m_tokenizer->m_doc;
        if (!doc)
            return;
    
        doc->setDocType(new DocumentType(doc, toString(name), toString(externalID), toString(systemID)));
    }
    
    static inline ParseXML* getTokenizer(void* closure)
    {
        xmlParserCtxtPtr ctxt = static_cast<xmlParserCtxtPtr>(closure);
        return static_cast<ParseXML*>(ctxt->_private);
    }
    
    // This is a hack around http://bugzilla.gnome.org/show_bug.cgi?id=159219
    // Otherwise libxml seems to call all the SAX callbacks twice for any replaced entity.
    static inline bool hackAroundLibXMLEntityBug(void* closure)
    {
    #if LIBXML_VERSION >= 20627
        // This bug has been fixed in libxml 2.6.27.
        return false;
    #else
        return static_cast<xmlParserCtxtPtr>(closure)->node;
    #endif
    }
    
    static void startElementNsHandler(void* closure, const xmlChar* localname, const xmlChar* prefix, const xmlChar* uri, int nb_namespaces, const xmlChar** namespaces, int nb_attributes, int nb_defaulted, const xmlChar** libxmlAttributes)
    {
        if (hackAroundLibXMLEntityBug(closure))
            return;
    
        getTokenizer(closure)->startElementNs(localname, prefix, uri, nb_namespaces, namespaces, nb_attributes, nb_defaulted, libxmlAttributes);
    }
    
    static void endElementNsHandler(void* closure, const xmlChar* localname, const xmlChar* prefix, const xmlChar* uri)
    {
        if (hackAroundLibXMLEntityBug(closure))
            return;
        
        getTokenizer(closure)->endElementNs();
    }
    
    static void charactersHandler(void* closure, const xmlChar* s, int len)
    {
        if (hackAroundLibXMLEntityBug(closure))
            return;
        
        getTokenizer(closure)->characters(s, len);
    }
    
    static void processingInstructionHandler(void* closure, const xmlChar* target, const xmlChar* data)
    {
        if (hackAroundLibXMLEntityBug(closure))
            return;
        
        getTokenizer(closure)->processingInstruction(target, data);
    }
    
    static void cdataBlockHandler(void* closure, const xmlChar* s, int len)
    {
        if (hackAroundLibXMLEntityBug(closure))
            return;
        
        getTokenizer(closure)->cdataBlock(s, len);
    }
    
    static void commentHandler(void* closure, const xmlChar* comment)
    {
        if (hackAroundLibXMLEntityBug(closure))
            return;
        
        getTokenizer(closure)->comment(comment);
    }
    
    static void warningHandler(void* closure, const char* message, ...)
    {
        va_list args;
        va_start(args, message);
        getTokenizer(closure)->error(ParseXML::warning, message, args);
        va_end(args);
    }
    
    static void fatalErrorHandler(void* closure, const char* message, ...)
    {
        va_list args;
        va_start(args, message);
        getTokenizer(closure)->error(ParseXML::fatal, message, args);
        va_end(args);
    }
    
    static void normalErrorHandler(void* closure, const char* message, ...)
    {
        va_list args;
        va_start(args, message);
        getTokenizer(closure)->error(ParseXML::nonFatal, message, args);
        va_end(args);
    }
    
    // Using a global variable entity and marking it XML_INTERNAL_PREDEFINED_ENTITY is
    // a hack to avoid malloc/free. Using a global variable like this could cause trouble
    // if libxml implementation details were to change
    static xmlChar sharedXHTMLEntityResult[5] = {0,0,0,0,0};
    static xmlEntity sharedXHTMLEntity = {
        0, XML_ENTITY_DECL, 0, 0, 0, 0, 0, 0, 0, 
        sharedXHTMLEntityResult, sharedXHTMLEntityResult, 0,
        XML_INTERNAL_PREDEFINED_ENTITY, 0, 0, 0, 0, 0
    };
    
    static xmlEntityPtr getXHTMLEntity(const xmlChar* name)
    {
        UChar c = WebCore::decodeNamedEntity(reinterpret_cast<const char*>(name));
        if (!c)
            return 0;
    
        CString value = String(&c, 1).utf8();
        ASSERT(value.length() < 5);
        sharedXHTMLEntity.length = value.length();
        sharedXHTMLEntity.name = name;
        memcpy(sharedXHTMLEntityResult, value.data(), sharedXHTMLEntity.length + 1);
    
        return &sharedXHTMLEntity;
    }
    
    static xmlEntityPtr getEntityHandler(void* closure, const xmlChar* name)
    {
        xmlParserCtxtPtr ctxt = static_cast<xmlParserCtxtPtr>(closure);
        xmlEntityPtr ent = xmlGetPredefinedEntity(name);
        if (ent) {
            ent->etype = XML_INTERNAL_PREDEFINED_ENTITY;
            return ent;
        }
    
        ent = xmlGetDocEntity(ctxt->myDoc, name);
        if (!ent && getTokenizer(closure)->tokenizer()->isXHTMLDocument()) {
            ent = getXHTMLEntity(name);
            if (ent)
                ent->etype = XML_INTERNAL_GENERAL_ENTITY;
        }
        
        return ent;
    }
    
    static void startDocumentHandler(void* closure)
    {
        xmlParserCtxt* ctxt = static_cast<xmlParserCtxt*>(closure);
        getTokenizer(closure)->startDocument(ctxt->version, ctxt->encoding, ctxt->standalone);
        xmlSAX2StartDocument(closure);
    }
    
    static void internalSubsetHandler(void* closure, const xmlChar* name, const xmlChar* externalID, const xmlChar* systemID)
    {
        getTokenizer(closure)->internalSubset(name, externalID, systemID);
        xmlSAX2InternalSubset(closure, name, externalID, systemID);
    }
    
    static void externalSubsetHandler(void* closure, const xmlChar* name, const xmlChar* externalId, const xmlChar* systemId)
    {
        String extId = toString(externalId);
        if ((extId == "-//W3C//DTD XHTML 1.0 Transitional//EN")
            || (extId == "-//W3C//DTD XHTML 1.1//EN")
            || (extId == "-//W3C//DTD XHTML 1.0 Strict//EN")
            || (extId == "-//W3C//DTD XHTML 1.0 Frameset//EN")
            || (extId == "-//W3C//DTD XHTML Basic 1.0//EN")
            || (extId == "-//W3C//DTD XHTML 1.1 plus MathML 2.0//EN")
            || (extId == "-//W3C//DTD XHTML 1.1 plus MathML 2.0 plus SVG 1.1//EN")
            || (extId == "-//WAPFORUM//DTD XHTML Mobile 1.0//EN"))
            getTokenizer(closure)->tokenizer()->setIsXHTMLDocument(true); // controls if we replace entities or not.
    }
    
    static void ignorableWhitespaceHandler(void* ctx, const xmlChar* ch, int len)
    {
        // nothing to do, but we need this to work around a crasher
        // http://bugzilla.gnome.org/show_bug.cgi?id=172255
        // http://bugs.webkit.org/show_bug.cgi?id=5792
    }
    
    void ParseXML::initializeParserContext()
    {
        xmlSAXHandler sax;
        memset(&sax, 0, sizeof(sax));
        sax.error = normalErrorHandler;
        sax.fatalError = fatalErrorHandler;
        sax.characters = charactersHandler;
        sax.processingInstruction = processingInstructionHandler;
        sax.cdataBlock = cdataBlockHandler;
        sax.comment = commentHandler;
        sax.warning = warningHandler;
        sax.startElementNs = startElementNsHandler;
        sax.endElementNs = endElementNsHandler;
        sax.getEntity = getEntityHandler;
        sax.startDocument = startDocumentHandler;
        sax.internalSubset = internalSubsetHandler;
        sax.externalSubset = externalSubsetHandler;
        sax.ignorableWhitespace = ignorableWhitespaceHandler;
        sax.entityDecl = xmlSAX2EntityDecl;
        sax.initialized = XML_SAX2_MAGIC;
        
        m_context = createStringParser(&sax, this);
    }
    
    void ParseXML::end()
    {
        if (m_context) {
            // Tell libxml we're done.
            xmlParseChunk(m_context, 0, 0, 1);
            
            if (m_context->myDoc)
                xmlFreeDoc(m_context->myDoc);
            xmlFreeParserCtxt(m_context);
            m_context = 0;
        }
    }

    int ParseXML::lineNumber() const
    {
        return m_context ? m_context->input->line : 1;
    }
    
    int ParseXML::columnNumber() const
    {
        return m_context ? m_context->input->col : 1;
    }

    void ParseXML::stopParsing()
    {
        xmlStopParser(m_context);
    }

    void ParseXML::resumeParsing()
    {
        while (!m_pendingCallbacks->isEmpty()) {
            m_pendingCallbacks->callAndRemoveFirstCallback(this);
            
            // A callback paused the parser
            if (m_tokenizer->m_parserPaused)
                return;
        }
    }

    static void balancedStartElementNsHandler(void* closure, const xmlChar* localname, const xmlChar* prefix,
                                          const xmlChar* uri, int nb_namespaces, const xmlChar** namespaces,
                                          int nb_attributes, int nb_defaulted, const xmlChar** libxmlAttributes)
    {
    static_cast<ParseXML*>(closure)->startElementNs(localname, prefix, uri, nb_namespaces, namespaces, nb_attributes, nb_defaulted, libxmlAttributes);
    }

    
    static void balancedEndElementNsHandler(void* closure, const xmlChar* localname, const xmlChar* prefix, const xmlChar* uri)
    {
        static_cast<ParseXML*>(closure)->endElementNs();
    }
    
    static void balancedCharactersHandler(void* closure, const xmlChar* s, int len)
    {
        static_cast<ParseXML*>(closure)->characters(s, len);
    }
    
    static void balancedProcessingInstructionHandler(void* closure, const xmlChar* target, const xmlChar* data)
    {
        static_cast<ParseXML*>(closure)->processingInstruction(target, data);
    }
    
    static void balancedCdataBlockHandler(void* closure, const xmlChar* s, int len)
    {
        static_cast<ParseXML*>(closure)->cdataBlock(s, len);
    }
    
    static void balancedCommentHandler(void* closure, const xmlChar* comment)
    {
        static_cast<ParseXML*>(closure)->comment(comment);
    }
    
    static void balancedWarningHandler(void* closure, const char* message, ...)
    {
        va_list args;
        va_start(args, message);
        static_cast<ParseXML*>(closure)->error(ParseXML::warning, message, args);
        va_end(args);
    }

    bool ParseXML::parseXMLDocumentFragment(const String& string, DocumentFragment* fragment, Element* parent)
    {
        xmlSAXHandler sax;
        memset(&sax, 0, sizeof(sax));
    
        sax.characters = balancedCharactersHandler;
        sax.processingInstruction = balancedProcessingInstructionHandler;
        sax.startElementNs = balancedStartElementNsHandler;
        sax.endElementNs = balancedEndElementNsHandler;
        sax.cdataBlock = balancedCdataBlockHandler;
        sax.ignorableWhitespace = balancedCdataBlockHandler;
        sax.comment = balancedCommentHandler;
        sax.warning = balancedWarningHandler;
        sax.initialized = XML_SAX2_MAGIC;
        
        int result = xmlParseBalancedChunkMemory(0, &sax, this, 0, (const xmlChar*)string.utf8().data(), 0);
        return result == 0;
    }

struct AttributeParseState {
    HashMap<String, String> attributes;
    bool gotAttributes;
};


    static void attributesStartElementNsHandler(void* closure, const xmlChar* xmlLocalName, const xmlChar* xmlPrefix,
                                                const xmlChar* xmlURI, int nb_namespaces, const xmlChar** namespaces,
                                                int nb_attributes, int nb_defaulted, const xmlChar** libxmlAttributes)
    {
        if (strcmp(reinterpret_cast<const char*>(xmlLocalName), "attrs") != 0)
            return;
        
        xmlParserCtxtPtr ctxt = static_cast<xmlParserCtxtPtr>(closure);
        AttributeParseState* state = static_cast<AttributeParseState*>(ctxt->_private);
        
        state->gotAttributes = true;
        
        xmlSAX2Attributes* attributes = reinterpret_cast<xmlSAX2Attributes*>(libxmlAttributes);
        for(int i = 0; i < nb_attributes; i++) {
            String attrLocalName = toString(attributes[i].localname);
            int valueLength = (int) (attributes[i].end - attributes[i].value);
            String attrValue = toString(attributes[i].value, valueLength);
            String attrPrefix = toString(attributes[i].prefix);
            String attrQName = attrPrefix.isEmpty() ? attrLocalName : attrPrefix + ":" + attrLocalName;
            
            state->attributes.set(attrQName, attrValue);
        }
    }

    HashMap<String, String> ParseXML::parseAttributes(const String& string, bool& attrsOK)
    {
        AttributeParseState state;
        state.gotAttributes = false;
        xmlSAXHandler sax;
        memset(&sax, 0, sizeof(sax));
        sax.startElementNs = attributesStartElementNsHandler;
        sax.initialized = XML_SAX2_MAGIC;
        xmlParserCtxtPtr parser = createStringParser(&sax, &state);
        String parseString = "<?xml version=\"1.0\"?><attrs " + string + " />";
        xmlParseChunk(parser, reinterpret_cast<const char*>(parseString.characters()), parseString.length() * sizeof(UChar), 1);
        if (parser->myDoc)
            xmlFreeDoc(parser->myDoc);
        xmlFreeParserCtxt(parser);
        attrsOK = state.gotAttributes;
        return state.attributes;
    }

    void* ParseXML::xmlDocPtrForString(DocLoader* docLoader, const String& source, const DeprecatedString& url)
    {
        if (source.isEmpty())
            return 0;
    
        // Parse in a single chunk into an xmlDocPtr
        // FIXME: Hook up error handlers so that a failure to parse the main document results in
        // good error messages.
        const UChar BOM = 0xFEFF;
        const unsigned char BOMHighByte = *reinterpret_cast<const unsigned char*>(&BOM);
    
        xmlGenericErrorFunc oldErrorFunc = xmlGenericError;
        void* oldErrorContext = xmlGenericErrorContext;
        
        setLoaderForLibXMLCallbacks(docLoader);
        xmlSetGenericErrorFunc(0, errorFunc);
        
        xmlDocPtr sourceDoc = xmlReadMemory(reinterpret_cast<const char*>(source.characters()),
                                            source.length() * sizeof(UChar),
                                            url.ascii(),
                                            BOMHighByte == 0xFF ? "UTF-16LE" : "UTF-16BE", 
                                            XSLT_PARSE_OPTIONS);
        
        setLoaderForLibXMLCallbacks(0);
        xmlSetGenericErrorFunc(oldErrorContext, oldErrorFunc);
        
        return sourceDoc;
    }

}
