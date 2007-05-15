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
 #include "BCXML.h"
#include "libxml/uri.h"
#include "libxml/parserInternals.h"

using namespace BC;
using namespace BAL;

namespace BAL {
    static BIXML *theXml = NULL;
    
    BIXML* createBIXML(  )
    {
        if( !theXml )
            theXml = new BCXML( );
        return theXml;
    }
}

BCXML::BCXML()
:m_doc(0), m_node(0), m_ns( 0 ), m_attr( 0 ), m_enc( 0 ), m_outbuf( 0 ), m_hashTable( 0 ), m_parser( 0 ), m_entity( 0 ), m_input (0 ), m_sax(0)
{
}

BCXML::~BCXML()
{
    m_doc = NULL;
    m_node = NULL;
    m_ns = NULL;
    m_attr = NULL;
    m_enc = NULL;
    m_outbuf = NULL;
    m_hashTable = NULL;
    m_sax = NULL;
    m_parser = NULL;
    m_entity = NULL;
    m_input = NULL;
        
}

void *BCXML::xmlMalloc( int size )
{
    return ::xmlMalloc( size );
}

void BCXML::xmlFreeDoc( BAL::BTXMLDoc * )
{
    ::xmlFreeDoc( m_doc );
}

BTXMLDoc *BCXML::convertDoc( xmlDoc *doc )
{
    if( doc != NULL )
        return new BTXMLDoc(doc->type, convertNode( doc->children ), doc->encoding ) ;
    return 0;
}

BTXMLNode *BCXML::convertNode( xmlNode *node )
{
    if( node != NULL )
        return new BTXMLNode( node->type, node->name, convertNode( node->children ), convertNode( node->next ), convertNS( node->ns ), node->content );
        
    return 0;
}

BTXMLNs *BCXML::convertNS( xmlNs *ns )
{
    if( ns != NULL )
        return new BTXMLNs( ns->href );
    return 0;
}

BTXMLAttr *BCXML::convertAttr( xmlAttr *attr)
{
    if( attr != NULL )
        return new BTXMLAttr( convertNode( attr->parent ) );
    return 0;
}

BTXMLCharEncodingHandler *BCXML::convertEncoding( xmlCharEncodingHandler *enc )
{
    if( enc != NULL )
        return new BTXMLCharEncodingHandler();
    return 0;
}

BTXMLOutputBuffer *BCXML::convertOutPut( xmlOutputBuffer *out )
{
    if( out != NULL )
        return new BTXMLOutputBuffer( out->context, out->writecallback );
    return 0;
}

BTXMLEntity *BCXML::convertEntity( xmlEntity *entity )
{
    if( entity != NULL )
        return new BTXMLEntity( entity->type, entity->name, entity->orig, entity->content, entity->length, entity->etype );
    return 0;
}

BTXMLParserInput *BCXML::convertParserInput( xmlParserInput *input )
{
    if( input != NULL )
        return new BTXMLParserInput( input->line, input->col );
    return 0;
}

BTXMLParserCtxt *BCXML::convertParser( xmlParserCtxt *parser )
{
    if( parser != NULL )
    {
        BTXMLParserCtxt *ctxt = new BTXMLParserCtxt( parser->_private, parser->replaceEntities, convertNode( parser->node ), convertDoc( parser->myDoc ), convertParserInput( parser->input ) );
        
        return ctxt;
    }
    
    return 0;
}

xmlDoc *BCXML::convertXmlDoc( xmlDoc *d, BTXMLDoc *doc )
{
    if( doc != 0 )
    {
        d->type = (xmlElementType)doc->type;
        if( doc->children != NULL )
            d->children = convertXmlNode( doc->children );
        else
            d->children = NULL;
        d->encoding = doc->encoding;
        
        return d;
    }
    
    return 0;
}

xmlNode *BCXML::convertXmlNode( BTXMLNode *node )
{
    if( node != NULL )
    {
        xmlNode *n = xmlNewNode( convertXmlNS( NULL, node->ns ), node->name );
        
        n->type = ( xmlElementType )node->type;
        n->name = node->name;
        n->children = convertXmlNode( node->children );
        n->next = convertXmlNode( node->next );
        n->ns = convertXmlNS( n, node->ns );
        n->content = node->content;
        
        return n;
    }
    
    return 0;
}

xmlNs *BCXML::convertXmlNS( xmlNode *node, BTXMLNs *ns )
{
    if( ns != NULL )
    {
        xmlNs *_ns = xmlNewNs( node, ns->href, (unsigned char * )"" );

        _ns->href = ns->href;
        
        return _ns;
    }
    
    return 0;
}

xmlOutputBuffer *BCXML::convertXmlOutPut( BTXMLOutputBuffer *out )
{
    if( out != NULL )
    {
        xmlOutputBuffer *out = new xmlOutputBuffer();
        out->context = out->context;
        out->writecallback = out->writecallback;
    
        return out;
    }
    
    return 0;
}

xmlEntity *BCXML::convertXmlEntity( BTXMLEntity *entity )
{
    if( entity != NULL )
    {
        xmlEntity *ent = new xmlEntity();
        ent->type = (xmlElementType)entity->type;
        ent->name = entity->name;
        ent->orig = entity->orig;
        ent->content = entity->content;
        ent->length = entity->length;
        ent->etype = (xmlEntityType)entity->etype;
        return ent;
    }
    
    return 0;
}



static void normalErrorHandler (void *, const char *msg, ... )
{
    BCXML *aXml = static_cast<BCXML*>(BAL::theXml);
    aXml->m_error( aXml->convertParser(aXml->m_parser), msg );
    
}

static void fatalErrorHandler (void *, const char *msg, ...)
{
    BCXML *aXml = static_cast<BCXML*>(BAL::theXml);
    aXml->m_fatalError( aXml->convertParser(aXml->m_parser), msg );
}

static void charactersHandler(void *, const unsigned char *s, int len)
{
    BCXML *aXml = static_cast<BCXML*>(BAL::theXml);
    aXml->m_characters( aXml->convertParser(aXml->m_parser), s, len );
}

static void processingInstructionHandler (void *, const unsigned char *target, const unsigned char *data)
{
    BCXML *aXml = static_cast<BCXML*>(BAL::theXml);
    aXml->m_processingInstruction( aXml->convertParser(aXml->m_parser), target, data );
}

static void cdataBlockHandler ( void *, const unsigned char *value, int len)
{
    BCXML *aXml = static_cast<BCXML*>(BAL::theXml);
    aXml->m_cdataBlock( aXml->convertParser(aXml->m_parser), value, len );
}

static void commentHandler (void *, const unsigned char *value)
{
    BCXML *aXml = static_cast<BCXML*>(BAL::theXml);
    aXml->m_comment( aXml->convertParser(aXml->m_parser), value );
}

static void warningHandler (void *,  const char *msg, ...)
{
    BCXML *aXml = static_cast<BCXML*>(BAL::theXml);
    aXml->m_warning( aXml->convertParser(aXml->m_parser), msg );
}

static void startElementNsHandler (void *,
                                   const unsigned char *localname,
                                   const unsigned char *prefix,
                                   const unsigned char *URI,
                                   int nb_namespaces,
                                   const unsigned char **namespaces,
                                   int nb_attributes,
                                   int nb_defaulted,
                                   const unsigned char **attributes)
{
    BCXML *aXml = static_cast<BCXML*>(BAL::theXml);
    aXml->m_startElementNs( aXml->convertParser(aXml->m_parser), localname, prefix, URI, nb_namespaces, namespaces, nb_attributes, nb_defaulted, attributes );
}

static void endElementNsHandler (void *,
                                 const unsigned char *localname,
                                 const unsigned char *prefix,
                                 const unsigned char *URI)
{
    BCXML *aXml = static_cast<BCXML*>(BAL::theXml);
    aXml->m_endElementNs( aXml->convertParser(aXml->m_parser), localname, prefix, URI );
}


static xmlEntity* getEntitySAX(void *, const unsigned char *name)
{
    BCXML *aXml = static_cast<BCXML*>(BAL::theXml);
    return aXml->convertXmlEntity( aXml->m_getEntity( aXml->convertParser(aXml->m_parser), name ) );
}

static void internalSubsetHandler (void *,
                                   const unsigned char *name,
                                   const unsigned char *ExternalID,
                                   const unsigned char *SystemID)
{
    BCXML *aXml = static_cast<BCXML*>(BAL::theXml);
    aXml->m_internalSubset( aXml->convertParser(aXml->m_parser), name, ExternalID, SystemID );
}

static void externalSubsetHandler (void *,
                                   const unsigned char *name,
                                   const unsigned char *ExternalID,
                                   const unsigned char *SystemID)
{
    BCXML *aXml = static_cast<BCXML*>(BAL::theXml);
    aXml->m_externalSubset( aXml->convertParser(aXml->m_parser), name, ExternalID, SystemID );
}

static void ignorableWhitespaceHandler (void *,
                                        const unsigned char *ch,
                                        int len)
{
    BCXML *aXml = static_cast<BCXML*>(BAL::theXml);
    aXml->m_ignorableWhitespace( aXml->convertParser(aXml->m_parser), ch, len );
}


BTXMLSAXHandler *BCXML::setSaxHandler( BTErrorSAXFunc error, 
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
                                       BTIgnorableWhitespaceSAXFunc ignorableWhitespace )
{
    m_error = error;
    m_fatalError = fatalError;
    m_characters = characters;
    m_processingInstruction = processingInstruction;
    m_cdataBlock = cdataBlock;
    m_comment = comment;
    m_warning = warning;
    m_startElementNs = startElementNs;
    m_endElementNs = endElementNs;
    m_getEntity = getEntity;
    m_internalSubset = internalSubset;
    m_externalSubset = externalSubset;
    m_ignorableWhitespace = ignorableWhitespace;



    if( m_sax == NULL )
        m_sax = new xmlSAXHandler();
    m_sax->error = normalErrorHandler;
    m_sax->fatalError = fatalErrorHandler;
    m_sax->characters = charactersHandler;
    m_sax->processingInstruction = processingInstructionHandler;
    m_sax->cdataBlock = cdataBlockHandler;
    m_sax->comment = commentHandler;
    m_sax->warning = warningHandler;
    m_sax->startElementNs = startElementNsHandler;
    m_sax->endElementNs = endElementNsHandler;
    m_sax->getEntity = getEntitySAX;
    m_sax->startDocument = ::xmlSAX2StartDocument;
    m_sax->internalSubset = internalSubsetHandler;
    m_sax->externalSubset = externalSubsetHandler;
    m_sax->ignorableWhitespace = ignorableWhitespaceHandler;
    m_sax->entityDecl = ::xmlSAX2EntityDecl;
    m_sax->initialized = BTXML_SAX2_MAGIC;
        
    return 0;
}

xmlParserInput *BCXML::convertXmlParserInput( xmlParserInput *in, BTXMLParserInput *input )
{
    if( input != NULL )
    {
        in->line = input->line;
        in->col = input->col;
        
        return in;
    }
    
    return 0;
}

xmlParserCtxt *BCXML::convertXmlParser( xmlParserCtxt *p, BTXMLParserCtxt *parser )
{
    if( parser != NULL )
    {
        p->_private = parser->_private;
        p->replaceEntities = parser->replaceEntities;
        if( parser->node != NULL )
            p->node = convertXmlNode( parser->node );
        if( parser->myDoc != NULL )
            p->myDoc = convertXmlDoc( p->myDoc, parser->myDoc );
        if( parser->input != NULL )
            p->input = convertXmlParserInput( p->input, parser->input );
        
        return p;
    }
    
    return p;
}








BAL::BTXMLDoc *BCXML::xmlReadMemory(const char * buffer,
                                int size,
                                const char * URL,
                                const char * encoding,
                                int options)
{
    m_doc = ::xmlReadMemory( buffer, size, URL, encoding, options );
    m_node = m_doc->children;
    return convertDoc( m_doc );
}

BAL::BTXMLAttr *BCXML::xmlGetID( BAL::BTXMLDoc *doc, const unsigned char *ID)
{
    m_attr = ::xmlGetID( m_doc, ID );
    return convertAttr( m_attr );
}

void BCXML::xmlFree(void * mem)
{
    ::xmlFree( mem );
}

unsigned char *BCXML::xmlNodeGetBase( BAL::BTXMLDoc *doc, BTXMLNode *cur)
{
    return ::xmlNodeGetBase( m_doc, m_node );
}

unsigned char *BCXML::xmlBuildURI( const unsigned char *URI, const unsigned char *base )
{
    return ::xmlBuildURI( URI, base );
}

int BCXML::xmlStrEqual( const unsigned char* str1, const unsigned char* str2)
{
    return ::xmlStrEqual( str1, str2 );
}

void BCXML::xmlSetGenericErrorFunc (void * ctx, BTXMLGenericErrorFunc handler)
{
    ::xmlSetGenericErrorFunc( ctx, handler );
}

BTXMLOutputBuffer *BCXML::xmlAllocOutputBuffer( BTXMLCharEncodingHandler * )
{
    m_outbuf = ::xmlAllocOutputBuffer( m_enc );
    
    return  convertOutPut( m_outbuf );
}

void BCXML::xmlOutputBufferClose(BTXMLOutputBuffer *)
{
    ::xmlOutputBufferClose( m_outbuf );
}

BTXMLHashTable *BCXML::xmlHashCreate( int size )
{
    m_hashTable = ::xmlHashCreate( size );
    return new BTXMLHashTable() ;
}

unsigned char *BCXML::xmlStrdup( const unsigned char *buf)
{
    return ::xmlStrdup( buf );
}

unsigned char *BCXML::xmlStrndup( const unsigned char *buf, int size )
{
    return ::xmlStrndup( buf, size );
}

void BCXML::xmlInitParser(void)
{
    ::xmlInitParser();
}

BTXMLParserCtxt *BCXML::xmlCreatePushParserCtxt( BTXMLSAXHandler *)
{
    m_parser = ::xmlCreatePushParserCtxt( m_sax, 0, 0, 0, 0 );
    
    return convertParser( m_parser );
}

int BCXML::xmlRegisterInputCallbacks (BTXMLInputMatchCallback matchFunc, 
                                BTXMLInputOpenCallback openFunc, 
                                BTXMLInputReadCallback readFunc, 
                                BTXMLInputCloseCallback closeFunc)
{
    return ::xmlRegisterInputCallbacks( matchFunc, openFunc, readFunc, closeFunc );
}

int BCXML::xmlRegisterOutputCallbacks (BTXMLOutputMatchCallback matchFunc, 
                                BTXMLOutputOpenCallback openFunc, 
                                BTXMLOutputWriteCallback writeFunc, 
                                BTXMLOutputCloseCallback closeFunc)
{
    return ::xmlRegisterOutputCallbacks( matchFunc, openFunc, writeFunc, closeFunc);
}

BTXMLParserCtxt * BCXML::xmlSwitchEncoding( BTXMLParserCtxt *ctxt, int enc)
{
    m_parser = convertXmlParser( m_parser, ctxt );
    
    ::xmlSwitchEncoding( m_parser, ( xmlCharEncoding )enc );
    
    return convertParser( m_parser );
}

BTXMLParserCtxt *BCXML::xmlParseChunk( BTXMLParserCtxt *ctxt, const char *chunk, int size, int terminate)
{
    m_parser = convertXmlParser( m_parser, ctxt );

    ::xmlParseChunk( m_parser, chunk, size, terminate );
    
    return convertParser( m_parser );
}

BTXMLEntity *BCXML::xmlGetPredefinedEntity(const unsigned char* name)
{
    m_entity = ::xmlGetPredefinedEntity( name );
    return convertEntity( m_entity );
}

BTXMLEntity *BCXML::xmlGetDocEntity( BTXMLDoc *, const unsigned char *name )
{
    m_entity = ::xmlGetDocEntity( m_parser->myDoc, name );
    return convertEntity( m_entity );
}

void BCXML::xmlSAX2InternalSubset( void * ctx, const unsigned char * name, const unsigned char * ExternalID, const unsigned char * SystemID)
{
    ::xmlSAX2InternalSubset( ctx, name, ExternalID, SystemID );
}

void BCXML::xmlFreeParserCtxt( BTXMLParserCtxt * )
{
    ::xmlFreeParserCtxt( m_parser );
}

void BCXML::xmlStopParser( BTXMLParserCtxt *)
{
    ::xmlStopParser( m_parser );
}

int BCXML::xmlParseBalancedChunkMemory( void * user_data, const unsigned char * string )
{
   
    return ::xmlParseBalancedChunkMemory( 0, m_sax, user_data, 0, string, 0 );
}

void *BCXML::getXMLGenericErrorContext()
{
    return xmlGenericErrorContext;
}


BTXMLGenericErrorFunc BCXML::getXMLGenericError()
{
    return xmlGenericError;
}

