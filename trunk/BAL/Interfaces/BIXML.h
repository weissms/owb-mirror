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
#ifndef BIXML_H_
#define BIXML_H_

#include "BTXML.h"

namespace BAL {


    /**
    * @brief the BIXML
    * 
    * The xslt base class
    * 
    */
    class BIXML {
        public:
        // this is mandatory
            virtual ~BIXML(){}
            
            /**
             * xmlMalloc:
             * @size:  number of bytes to allocate
             *
             * Returns the pointer to the allocated area or NULL in case of error
             */
            virtual void *xmlMalloc( int size ) = 0;
            
            /**
             * xmlFreeDoc :
             * Free up all the structures used by a document, tree included.
             * cur:  pointer to the document
             */
            virtual void xmlFreeDoc( BAL::BTXMLDoc *doc ) = 0;
            
            /**
             * xmlReadMemory:
             * parse an XML in-memory document and build a tree.
             * buffer:  a pointer to a char array
             * size:  the size of the array
             * URL:  the base URL to use for the document
             * encoding:  the document encoding, or NULL
             * options:  a combination of xmlParserOption
             * Returns:  the resulting document tree
             */
            virtual BAL::BTXMLDoc *xmlReadMemory(const char * buffer,
                                         int size,
                                         const char * URL,
                                         const char * encoding,
                                         int options) = 0;
            
            /**
             * xmlGetID:
             * Search the attribute declaring the given ID
             * doc:  pointer to the document
             * ID:  the ID value
             * Returns:  NULL if not found, otherwise the BTXMLAttrPtr defining the ID
             */
            virtual BAL::BTXMLAttr *xmlGetID( BAL::BTXMLDoc *doc, const unsigned char *ID) = 0;
            
            /**
             * xmlFree:
             * free the memory
             */
            virtual void xmlFree(void * mem) = 0;
            
            /**
             * xmlNodeGetBase:
             * Searches for the BASE URL. The code should work on both XML and HTML document even if base mechanisms are completely different. It returns the base as defined in RFC 2396 sections 5.1.1. Base URI within Document Content and 5.1.2. Base URI from the Encapsulating Entity However it does not return the document base (5.1.3).
             * doc:  the document the node pertains to
             * cur:  the node being checked
             * Returns:  a pointer to the base URL, or NULL if not found It's up to the caller to free the memory with xmlFree().
             */
            virtual unsigned char *xmlNodeGetBase( BAL::BTXMLDoc *doc, BTXMLNode *cur) = 0;
            
            /**
             * xmlBuildURI:
             * Computes he final URI of the reference done by checking that the given URI is valid, and building the final URI using the base URI. This is processed according to section 5.2 of the RFC 2396 5.2. Resolving Relative References to Absolute Form
             * URI:  the URI instance found in the document
             * base:  the base value
             * Returns:  a new URI string (to be freed by the caller) or NULL in case of error.
             */
            virtual unsigned char *xmlBuildURI( const unsigned char *URI, const unsigned char *base ) = 0;
            
            /**
             * xmlStrEqual:
             * Check if both strings are equal of have same content. Should be a bit more readable and faster than xmlStrcmp()
             * str1:  the first xmlChar *
             * str2:  the second xmlChar *
             * Returns:  1 if they are equal, 0 if they are different
             */
            virtual int xmlStrEqual( const unsigned char* str1, const unsigned char* str2) = 0;
            
            /**
             * xmlSetGenericErrorFunc:
             * Function to reset the handler and the error context for out of context error messages. This simply means that @handler will be called for subsequent error messages while not parsing nor validating. And @ctx will be passed as first argument to @handler One can simply force messages to be emitted to another FILE * than stderr by setting @ctx to this file handle and @handler to NULL. For multi-threaded applications, this must be set separately for each thread.
             * ctx:  the new error handling context
             * handler:  the new handler function
             */
            virtual void xmlSetGenericErrorFunc (void * ctx, BTXMLGenericErrorFunc handler) = 0;
            
            /**
             * xmlAllocOutputBuffer:
             * Create a buffered parser output
             * encoder:  the encoding converter or NULL
             * Returns:  the new parser output or NULL
             */
            virtual BTXMLOutputBuffer *xmlAllocOutputBuffer( BTXMLCharEncodingHandler *encoder ) = 0;
            /**
             * xmlOutputBufferClose:
             * flushes and close the output I/O channel and free up all the associated resources
             * out:  a buffered output
             * Returns:  the number of byte written or -1 in case of error.
             */
            virtual void xmlOutputBufferClose(BTXMLOutputBuffer *outputBuf) = 0;
            
            
            /**
             * xmlHashCreate:
             * Create a new BTXMLHashTable *
             * size:  the size of the hash table
             * Returns:  the newly created object, or NULL if an error occured.
             */ 
            virtual BTXMLHashTable *xmlHashCreate( int size ) = 0;
            
            /**
             * xmlStrdup:
             * a strdup for array of xmlChar's. Since they are supposed to be encoded in UTF-8 or an encoding with 8bit based chars, we assume a termination mark of '0'.
             * cur:  the input xmlChar *
             * Returns:  a new xmlChar * or NULL
             */
            virtual unsigned char *xmlStrdup( const unsigned char *buf) = 0;
            
            /**
             * xmlStrndup
             * a strndup for array of xmlChar's
             * cur:  the input xmlChar *
             * len:  the len of @cur
             * Returns:  a new xmlChar * or NULL
             */
            virtual unsigned char *xmlStrndup( const unsigned char *buf, int size ) = 0;
            
            /**
             * xmlInitParser
             * Initialization function for the XML parser. This is not reentrant. Call once before processing in case of use in multithreaded programs.
             */
            virtual void xmlInitParser(void) = 0;
            
            /**
             * xmlCreatePushParserCtxt:
             * Create a parser context for using the XML parser in push mode. If @buffer and @size are non-NULL, the data is used to detect the encoding. The remaining characters will be parsed so they don't need to be fed in again through xmlParseChunk. To allow content encoding detection, @size should be >= 4 The value of @filename is used for fetching external entities and error/warning reports.
             * sax:  a SAX handler
             * user_data:  The user data returned on SAX callbacks
             * chunk:  a pointer to an array of chars
             * size:  number of chars in the array
             * filename:  an optional file name or URI
             * Returns:  the new parser context or NULL
             */
            virtual BTXMLParserCtxt *xmlCreatePushParserCtxt( BTXMLSAXHandler *sax ) = 0;
            
            /**
             * xmlRegisterInputCallbacks:
             * Register a new set of I/O callback for handling parser input.
             * matchFunc:  the BTXMLInputMatchCallback
             * openFunc:  the BTXMLInputOpenCallback
             * readFunc:  the BTXMLInputReadCallback
             * closeFunc:  the BTXMLInputCloseCallback
             * Returns:  the registered handler number or -1 in case of error
             */
            virtual int xmlRegisterInputCallbacks (BTXMLInputMatchCallback matchFunc, 
                                           BTXMLInputOpenCallback openFunc, 
                                           BTXMLInputReadCallback readFunc, 
                                           BTXMLInputCloseCallback closeFunc) = 0;
            
            /**
             * xmlRegisterOutputCallbacks:
             * Register a new set of I/O callback for handling output.
             * matchFunc:  the BTXMLOutputMatchCallback
             * openFunc:  the BTXMLOutputOpenCallback
             * writeFunc:  the BTXMLOutputWriteCallback
             * closeFunc:  the BTXMLOutputCloseCallback
             * Returns:  the registered handler number or -1 in case of error 
             */
            virtual int xmlRegisterOutputCallbacks (BTXMLOutputMatchCallback matchFunc, 
                                            BTXMLOutputOpenCallback openFunc, 
                                            BTXMLOutputWriteCallback writeFunc, 
                                            BTXMLOutputCloseCallback closeFunc) = 0;
            /**
             * xmlSwitchEncoding:
             * change the input functions when discovering the character encoding of a given entity.
             * ctxt:  the parser context
             * enc:  the encoding value (number)
             * Returns:  0 in case of success, -1 otherwise
             */
            virtual BTXMLParserCtxt * xmlSwitchEncoding( BTXMLParserCtxt *ctxt, int enc) = 0;
            
            /**
             * xmlParseChunk:
             * Parse a Chunk of memory
             * ctxt:  an XML parser context
             * chunk:  an char array
             * size:  the size in byte of the chunk
             * terminate:  last chunk indicator
             * Returns:  zero if no error, the Errors otherwise.
             */
            virtual BTXMLParserCtxt * xmlParseChunk( BTXMLParserCtxt *ctxt, const char *chunk, int size, int terminate) = 0;
            
            /**
             * xmlGetPredefinedEntity:
             * Check whether this name is an predefined entity.
             * name:  the entity name
             * Returns:  NULL if not, otherwise the entity
             */ 
            virtual BTXMLEntity *xmlGetPredefinedEntity(const unsigned char* name) = 0;
            
            /**
             * xmlGetDocEntity:
             * Do an entity lookup in the document entity hash table
             * doc:  the document referencing the entity
             * name:  the entity name
             * Returns:  the corresponding entity, otherwise a lookup is done in the predefined entities too. Returns A pointer to the entity structure or NULL if not found.
             */
            virtual BTXMLEntity *xmlGetDocEntity( BTXMLDoc *doc, const unsigned char *name ) = 0;
            
            /**
             * xmlSAX2InternalSubset:
             * Callback on internal subset declaration.
             * ctx:  the user data (XML parser context)
             * name:  the root element name
             * ExternalID:  the external ID
             * SystemID:  the SYSTEM ID (e.g. filename or URL)
             */
            virtual void xmlSAX2InternalSubset( void * ctx, const unsigned char * name, const unsigned char * ExternalID, const unsigned char * SystemID) = 0;
            
            /**
             * xmlFreeParserCtxt
             * Free all the memory used by a parser context. However the parsed document in ctxt->myDoc is not freed.
             * ctxt:  an XML parser context
             */
            virtual void xmlFreeParserCtxt( BTXMLParserCtxt *ctxt ) = 0;
            
            /**
             * BTXMLGenericErrorFunc:
             * return a BTXMLGenericErrorFunc
             */
            virtual BTXMLGenericErrorFunc getXMLGenericError() = 0;
            
            /**
             * xmlStopParser:
             * Blocks further parser processing
             * ctxt:  an XML parser context
             */
            virtual void xmlStopParser( BTXMLParserCtxt *ctxt) = 0;
            
            /**
             * xmlParseBalancedChunkMemory:
             * Parse a well-balanced chunk of an XML document called by the parser The allowed sequence for the Well Balanced Chunk is the one defined by the content production in the XML grammar: [43] content ::= (element | CharData | Reference | CDSect | PI | Comment)*
             * doc:  the document the chunk pertains to
             * sax:  the SAX handler bloc (possibly NULL)
             * user_data:  The user data returned on SAX callbacks (possibly NULL)
             * depth:  Used for loop detection, use 0
             * string:  the input string in UTF8 or ISO-Latin (zero terminated)
             * lst:  the return value for the set of parsed nodes
             * Returns:  0 if the chunk is well balanced, -1 in case of args problem and the parser error code otherwise
             */
            virtual int xmlParseBalancedChunkMemory( void * user_data, const unsigned char * string ) = 0;

            /**
             * BTXMLSAXHandler:
             * setSaxHandler: set all handler to parse the document
             */
            virtual BTXMLSAXHandler *setSaxHandler( BTErrorSAXFunc error, 
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
                                     BTIgnorableWhitespaceSAXFunc ignorableWhitespace ) =0;
            
            /**
             * getXMLGenericErrorContext:
             * return the GenericErrorContext
             */
            virtual void* getXMLGenericErrorContext() = 0;
    };

    
}

#endif // BIXML_H_

