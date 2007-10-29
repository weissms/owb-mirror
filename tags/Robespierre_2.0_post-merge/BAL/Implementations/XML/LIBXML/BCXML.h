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
 * @file  BCXML.h
 *
 * @brief Concretisation of BIXML
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date: 2006/05/11 13:44:56 $
 *
 * This header file is private. Only IDL interface is public.
 *
 */

#include "BIXML.h"
#include <libxml/tree.h>
#include <libxml/xmlIO.h>
#include <libxml/encoding.h>

using namespace BAL;

namespace BC
{
    /**
    * @brief the XML Implementation
    * 
    */
    class BCXML : public BAL::BIXML 
    {
        public:
            BCXML();
            ~BCXML();
            
            void *xmlMalloc( int size );
            void xmlFreeDoc( BAL::BTXMLDoc *doc );
            BAL::BTXMLDoc *xmlReadMemory(const char * buffer,
                                         int size,
                                         const char * URL,
                                         const char * encoding,
                                         int options);
            BAL::BTXMLAttr *xmlGetID( BAL::BTXMLDoc *doc, const unsigned char *ID);
            void xmlFree(void * mem);
            unsigned char *xmlNodeGetBase( BAL::BTXMLDoc *doc, BTXMLNode *cur);
            unsigned char *xmlBuildURI( const unsigned char *URI, const unsigned char *base );
            int xmlStrEqual( const unsigned char* str1, const unsigned char* str2);
            void xmlSetGenericErrorFunc (void * ctx, BTXMLGenericErrorFunc handler);
            BTXMLOutputBuffer *xmlAllocOutputBuffer( BTXMLCharEncodingHandler *encoder );
            void xmlOutputBufferClose(BTXMLOutputBuffer *outputBuf);
            BTXMLHashTable *xmlHashCreate( int size );
            unsigned char *xmlStrdup( const unsigned char *buf);
            unsigned char *xmlStrndup( const unsigned char *buf, int size );
            void xmlInitParser(void);
            BTXMLParserCtxt *xmlCreatePushParserCtxt( BTXMLSAXHandler *sax );
            int xmlRegisterInputCallbacks (BTXMLInputMatchCallback matchFunc, 
                                           BTXMLInputOpenCallback openFunc, 
                                           BTXMLInputReadCallback readFunc, 
                                           BTXMLInputCloseCallback closeFunc);

            int xmlRegisterOutputCallbacks (BTXMLOutputMatchCallback matchFunc, 
                                            BTXMLOutputOpenCallback openFunc, 
                                            BTXMLOutputWriteCallback writeFunc, 
                                            BTXMLOutputCloseCallback closeFunc);
            BTXMLParserCtxt * xmlSwitchEncoding( BTXMLParserCtxt *ctxt, int enc);
            BTXMLParserCtxt * xmlParseChunk( BTXMLParserCtxt *ctxt, const char *chunk, int size, int terminate);
            BTXMLEntity *xmlGetPredefinedEntity(const unsigned char* name);
            BTXMLEntity *xmlGetDocEntity( BTXMLDoc *doc, const unsigned char *name );
            void xmlSAX2InternalSubset( void * ctx, const unsigned char * name, const unsigned char * ExternalID, const unsigned char * SystemID);
            void xmlFreeParserCtxt( BTXMLParserCtxt *ctxt );
            void xmlStopParser( BTXMLParserCtxt *ctxt);
            
            int xmlParseBalancedChunkMemory( void * user_data, const unsigned char * string );
            
            void *getXMLGenericErrorContext();
            BTXMLSAXHandler *setSaxHandler( BTErrorSAXFunc error, 
                                     BTFatalErrorSAXFunc fatalError, 
                                     BTCharactersSAXFunc characters,
                                     BTProcessingInstructionSAXFunc processingInstruction, 
                                     BTCdataBlockSAXFunc cdataBlock, 
                                     BTCommentSAXFunc comment, 
                                     BTWarningSAXFunc warning,
                                     BTStartElementNsSAX2Func startElementNs,
                                     BTEndElementNsSAX2Func endElementNs,
                                     BTGetEntitySAXFunc getEntity,
                                     BTInternalSubsetSAXFunc internalSubset,
                                     BTExternalSubsetSAXFunc externalSubset,
                                     BTIgnorableWhitespaceSAXFunc ignorableWhitespace );
                                     
            BTXMLGenericErrorFunc getXMLGenericError();
            
            BTXMLDoc *convertDoc( xmlDoc *doc );
            BTXMLNode *convertNode( xmlNode *node );
            BTXMLNs *convertNS( xmlNs *ns );
            BTXMLAttr *convertAttr( xmlAttr *attr);
            BTXMLCharEncodingHandler *convertEncoding( xmlCharEncodingHandler *m_enc );
            BTXMLOutputBuffer *convertOutPut( xmlOutputBuffer *out );
            BTXMLEntity *convertEntity( xmlEntity *entity );
            BTXMLParserCtxt *convertParser( xmlParserCtxt *parser );
            BTXMLParserInput *convertParserInput( xmlParserInput *input );
            
            xmlDoc *convertXmlDoc( xmlDoc *d, BTXMLDoc *doc );
            xmlNode *convertXmlNode( BTXMLNode *node );
            xmlNs *convertXmlNS( xmlNode *node, BTXMLNs *ns );
            xmlOutputBuffer *convertXmlOutPut( BTXMLOutputBuffer *out );
            xmlParserCtxt *convertXmlParser( xmlParserCtxt *p, BTXMLParserCtxt *parser );
            xmlParserInput *convertXmlParserInput( xmlParserInput *in, BTXMLParserInput *input );
            
            static xmlEntity *convertXmlEntity( BTXMLEntity *entity );
            static xmlDoc *convertXmlDocS( BTXMLDoc *doc );
            
            BTErrorSAXFunc m_error;
            BTFatalErrorSAXFunc m_fatalError;
            BTCharactersSAXFunc m_characters;
            BTProcessingInstructionSAXFunc m_processingInstruction;
            BTCdataBlockSAXFunc m_cdataBlock;
            BTCommentSAXFunc m_comment;
            BTWarningSAXFunc m_warning;
            BTStartElementNsSAX2Func m_startElementNs;
            BTEndElementNsSAX2Func m_endElementNs;
            BTGetEntitySAXFunc m_getEntity;
            BTInternalSubsetSAXFunc m_internalSubset;
            BTExternalSubsetSAXFunc m_externalSubset;
            BTIgnorableWhitespaceSAXFunc m_ignorableWhitespace;
            
            xmlParserCtxt *m_parser;
            xmlDoc *m_doc;
            xmlNode *m_node;
            xmlOutputBuffer *m_outbuf;
        private :
            
            
            
            xmlNs *m_ns;
            xmlAttr *m_attr;
            xmlCharEncodingHandler *m_enc;
            xmlHashTable *m_hashTable;
            xmlSAXHandler *m_sax;
            
            xmlEntity *m_entity;
            xmlParserInput *m_input;
    };

}

