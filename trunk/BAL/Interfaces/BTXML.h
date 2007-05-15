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
#ifndef BTXML_H_
#define BTXML_H_


namespace BAL {
    
    class BTXMLNode;
    class BTXMLDoc;
    class BTXMLNs;
    class BTXMLDict;
    
    #define BTXML_ELEMENT_NODE                 1
    #define BTXML_TEXT_NODE                    3
    #define BTXML_HTML_DOCUMENT_NODE          13
    #define BTXML_CHAR_ENCODING_UTF16LE        2
    #define BTXML_CHAR_ENCODING_UTF16BE        3
    #define BTXML_ENTITY_DECL                 17
    #define BTXML_INTERNAL_PREDEFINED_ENTITY   6
    #define BTXML_SAX2_MAGIC 0xDEEDBEAF
     
    typedef enum {
        BTXML_PARSE_RECOVER   = 1<<0, /* recover on errors */
        BTXML_PARSE_NOENT     = 1<<1, /* substitute entities */
        BTXML_PARSE_DTDLOAD   = 1<<2, /* load the external subset */
        BTXML_PARSE_DTDATTR   = 1<<3, /* default DTD attributes */
        BTXML_PARSE_DTDVALID  = 1<<4, /* validate with the DTD */
        BTXML_PARSE_NOERROR   = 1<<5, /* suppress error reports */
        BTXML_PARSE_NOWARNING = 1<<6, /* suppress warning reports */
        BTXML_PARSE_PEDANTIC  = 1<<7, /* pedantic error reporting */
        BTXML_PARSE_NOBLANKS  = 1<<8, /* remove blank nodes */
        BTXML_PARSE_SAX1      = 1<<9, /* use the SAX1 interface internally */
        BTXML_PARSE_XINCLUDE  = 1<<10,/* Implement XInclude substitition  */
        BTXML_PARSE_NONET     = 1<<11,/* Forbid network access */
        BTXML_PARSE_NODICT    = 1<<12,/* Do not reuse the context dictionnary */
        BTXML_PARSE_NSCLEAN   = 1<<13,/* remove redundant namespaces declarations */
        BTXML_PARSE_NOCDATA   = 1<<14,/* merge CDATA as text nodes */
        BTXML_PARSE_NOXINCNODE= 1<<15,/* do not generate XINCLUDE START/END nodes */
        BTXML_PARSE_COMPACT   = 1<<16 /* compact small text nodes */
    } BTXMLParserOption;
    
    class BTXMLDoc
    {
        public:
            BTXMLDoc();
            BTXMLDoc( int t, BTXMLNode *c, const unsigned char *e );
            ~BTXMLDoc();
       
            int  type;        /* XML_DOCUMENT_NODE, must be second ! */
            BTXMLNode *children;  /* the document tree */
            const unsigned char *encoding;   /* external initial encoding, if any */
    };
    
    class BTXMLNs
    {
        public:
            BTXMLNs();
            BTXMLNs(const unsigned char *ref);
            ~BTXMLNs();
            
            const unsigned char *href;        /* URL for the namespace */
    };
    
    
    class BTXMLAttr
    {
        public:
            BTXMLAttr();
            BTXMLAttr( BTXMLNode *p );
            ~BTXMLAttr();
            
            BTXMLNode *parent;    /* child->parent link */
};

    
    class BTXMLNode
    {
        public:
            BTXMLNode();
            BTXMLNode(int t, const unsigned char *na, BTXMLNode *c, BTXMLNode *n, BTXMLNs *_ns, unsigned char *co );
            ~BTXMLNode();
            
            int  type;      /* type number, must be second ! */
            const unsigned char *name;      /* the name of the node, or the entity */
            BTXMLNode *children;  /* parent->childs link */
            BTXMLNode *next;      /* next sibling link  */
        
            /* End of common part */
            BTXMLNs         *ns;        /* pointer to the associated namespace */
            unsigned char   *content;   /* the content */
    };
    
    class BTXMLHashTable
    {
        public:
            BTXMLHashTable(){};
            ~BTXMLHashTable(){};
       
    };
    
    class BTXMLDict
    {
        public:
            BTXMLDict(){};
            ~BTXMLDict(){};
       
    };
    
    /**
    * xmlCharEncodingInputFunc:
    * @param out  a pointer to an array of bytes to store the UTF-8 result
    * @param outlen  the length of @out
    * @param in  a pointer to an array of chars in the original encoding
    * @param inlen  the length of @in
    *
    * Take a block of chars in the original encoding and try to convert
    * it to an UTF-8 block of chars out.
    *
    * Returns the number of bytes written, -1 if lack of space, or -2
    *     if the transcoding failed.
    * The value of @inlen after return is the number of octets consumed
    *     if the return value is positive, else unpredictiable.
    * The value of @outlen after return is the number of octets consumed.
    */
    typedef int (* BTXMLCharEncodingInputFunc)(unsigned char *out, int *outlen,
                                            const unsigned char *in, int *inlen);

    /**
    * xmlCharEncodingOutputFunc:
    * @param out  a pointer to an array of bytes to store the result
    * @param outlen  the length of @out
    * @param in  a pointer to an array of UTF-8 chars
    * @param inlen  the length of @in
    *
    * Take a block of UTF-8 chars in and try to convert it to another
    * encoding.
    * Note: a first call designed to produce heading info is called with
    * in = NULL. If stateful this should also initialize the encoder state.
    *
    * Returns the number of bytes written, -1 if lack of space, or -2
    *     if the transcoding failed.
    * The value of @inlen after return is the number of octets consumed
    *     if the return value is positive, else unpredictiable.
    * The value of @outlen after return is the number of octets produced.
    */
    typedef int (* BTXMLCharEncodingOutputFunc)(unsigned char *out, int *outlen,
                                            const unsigned char *in, int *inlen);

    class BTXMLCharEncodingHandler
    {
        public:
            BTXMLCharEncodingHandler();
            ~BTXMLCharEncodingHandler();

    };


    /**
    * xmlOutputWriteCallback:
    * @param context  an Output context
    * @param buffer  the buffer of data to write
    * @param len  the length of the buffer in bytes
    *
    * Callback used in the I/O Output API to write to the resource
    *
    * Returns the number of bytes written or -1 in case of error
    */
    typedef int (* BTXMLOutputWriteCallback) (void * context, const char * buffer, int len);

    
    class BTXMLBuffer 
    {
        public:
            BTXMLBuffer();
            BTXMLBuffer( unsigned char *c );
            ~BTXMLBuffer();
        
            unsigned char *content;     /* The buffer content UTF8 */
    };

    class BTXMLOutputBuffer
    {
        public:
            BTXMLOutputBuffer();
            BTXMLOutputBuffer( void *c, BTXMLOutputWriteCallback wc );
            ~BTXMLOutputBuffer();
            
            void*                   context;
            BTXMLOutputWriteCallback  writecallback;
    };
    
    
    class BTXMLParserInput
    {
        public:
            BTXMLParserInput();
            BTXMLParserInput( int l, int c);
            ~BTXMLParserInput();
            
            int line;                         /* Current line */
            int col;                          /* Current column */
    };
    
    
    /**
    * BTXMLGenericErrorFunc:
    * @param ctx  a parsing context
    * @param msg  the message
    * @param ...  the extra arguments of the varags to format the message
    *
    * Signature of the function to use when there is an error and
    * no parsing or validity context available .
    */
    typedef void (*BTXMLGenericErrorFunc) (void *ctx, const char *msg, ...);

    class BTXMLParserCtxt
    {
        public :
        
            BTXMLParserCtxt();
            BTXMLParserCtxt(void *_p, int r, BTXMLNode *n, BTXMLDoc *d, BTXMLParserInput *pi );
            ~BTXMLParserCtxt();
            
            void        *_private;      /* For user data, libxml won't touch it */
            int         replaceEntities;        /* shall we replace entities ? */
            BTXMLNode   *node;          /* Current parsed Node */
            BTXMLDoc    *myDoc;        /* the document being built */
            BTXMLParserInput  *input;         /* Current input stream */
    };
    
    /**
    * BTErrorSAXFunc:
    * @param ctx  an XML parser context
    * @param msg  the message to display/transmit
    * @param ...  extra parameters for the message display
    *
    * Display and format an error messages, callback.
    */
    typedef void (*BTErrorSAXFunc) (void *ctx, const char *msg, ...);

    /**
    * BTFatalErrorSAXFunc:
    * @param ctx  an XML parser context
    * @param msg  the message to display/transmit
    * @param ...  extra parameters for the message display
    *
    * Display and format fatal error messages, callback.
    * Note: so far fatalError() SAX callbacks are not used, error()
    *       get all the callbacks for errors.
    */
    typedef void (*BTFatalErrorSAXFunc) (void *ctx, const char *msg, ...);
    
    /**
    * BTCharactersSAXFunc:
    * @param ctx  the user data (XML parser context)
    * @param ch  a xmlChar string
    * @param len the number of xmlChar
    *
    * Receiving some chars from the parser.
    */
    typedef void (*BTCharactersSAXFunc) (void *ctx, const unsigned char *ch, int len);

    /**
    * BTProcessingInstructionSAXFunc:
    * @param ctx  the user data (XML parser context)
    * @param target  the target name
    * @param data the PI data's
    *
    * A processing instruction has been parsed.
    */
    typedef void (*BTProcessingInstructionSAXFunc) (void *ctx, const unsigned char *target, const unsigned char *data);
    
    
    /**
    * BTCdataBlockSAXFunc:
    * @param ctx:  the user data (XML parser context)
    * @param cvalue:  The pcdata content
    * @param clen:  the block length
    *
    * Called when a pcdata block has been parsed.
    */
    typedef void (*BTCdataBlockSAXFunc) ( void *ctx, const unsigned char *value, int len);
    
    /**
    * BTCommentSAXFunc:
    * @param cctx:  the user data (XML parser context)
    * @param cvalue:  the comment content
    *
    * A comment has been parsed.
    */
    typedef void (*BTCommentSAXFunc) (void *ctx, const unsigned char *value);
    
    /**
    * BTWarningSAXFunc:
    * @param cctx:  an XML parser context
    * @param cmsg:  the message to display/transmit
    * @param c...:  extra parameters for the message display
    *
    * Display and format a warning messages, callback.
    */
    typedef void (*BTWarningSAXFunc) (void *ctx,  const char *msg, ...);
    
    /**
    * BTStartElementNsSAX2Func:
    * @param cctx:  the user data (XML parser context)
    * @param clocalname:  the local name of the element
    * @param cprefix:  the element namespace prefix if available
    * @param cURI:  the element namespace name if available
    * @param cnb_namespaces:  number of namespace definitions on that node
    * @param cnamespaces:  pointer to the array of prefix/URI pairs namespace definitions
    * @param cnb_attributes:  the number of attributes on that node
    * @param cnb_defaulted:  the number of defaulted attributes. The defaulted
    *                  ones are at the end of the array
    * @param cattributes:  pointer to the array of (localname/prefix/URI/value/end)
    *               attribute values.
    *
    * SAX2 callback when an element start has been detected by the parser.
    * It provides the namespace informations for the element, as well as
    * the new namespace declarations on the element.
    */
    
    typedef void (*BTStartElementNsSAX2Func) (void *ctx,
                                            const unsigned char *localname,
                                            const unsigned char *prefix,
                                            const unsigned char *URI,
                                            int nb_namespaces,
                                            const unsigned char **namespaces,
                                            int nb_attributes,
                                            int nb_defaulted,
                                            const unsigned char **attributes);

    /**
    * BTEndElementNsSAX2Func:
    * @param cctx:  the user data (XML parser context)
    * @param clocalname:  the local name of the element
    * @param cprefix:  the element namespace prefix if available
    * @param cURI:  the element namespace name if available
    *
    * SAX2 callback when an element end has been detected by the parser.
    * It provides the namespace informations for the element.
    */
    
    typedef void (*BTEndElementNsSAX2Func)   (void *ctx,
                                              const unsigned char *localname,
                                              const unsigned char *prefix,
                                              const unsigned char *URI);

    /**
    * BTStartDocumentSAXFunc:
    * @param cctx:  the user data (XML parser context)
    *
    * Called when the document start being processed.
    */
    typedef void (*BTStartDocumentSAXFunc) (void *ctx);

    class BTXMLEntity;
    /**
    * BTGetEntitySAXFunc:
    * @param cctx:  the user data (XML parser context)
    * @param cname: The entity name
    *
    * Get an entity by name.
    *
    * Returns the xmlEntityPtr if found.
    */
    typedef BTXMLEntity* (*BTGetEntitySAXFunc) (void *ctx, const unsigned char *name);

    /**
    * BTInternalSubsetSAXFunc:
    * @param cctx:  the user data (XML parser context)
    * @param cname:  the root element name
    * @param cExternalID:  the external ID
    * @param cSystemID:  the SYSTEM ID (e.g. filename or URL)
    *
    * Callback on internal subset declaration.
    */
    typedef void (*BTInternalSubsetSAXFunc) (void *ctx,
                                    const unsigned char *name,
                                    const unsigned char *ExternalID,
                                    const unsigned char *SystemID);

    /**
    * BTExternalSubsetSAXFunc:
    * @param cctx:  the user data (XML parser context)
    * @param cname:  the root element name
    * @param cExternalID:  the external ID
    * @param cSystemID:  the SYSTEM ID (e.g. filename or URL)
    *
    * Callback on external subset declaration.
    */
    typedef void (*BTExternalSubsetSAXFunc) (void *ctx,
                                    const unsigned char *name,
                                    const unsigned char *ExternalID,
                                    const unsigned char *SystemID);

    /**
    * BTIgnorableWhitespaceSAXFunc:
    * @param cctx:  the user data (XML parser context)
    * @param cch:  a xmlChar string
    * @param clen: the number of xmlChar
    *
    * Receiving some ignorable whitespaces from the parser.
    * UNUSED: by default the DOM building will use characters.
    */
    typedef void (*BTIgnorableWhitespaceSAXFunc) (void *ctx,
                                    const unsigned char *ch,
                                    int len);

    /**
    * BTEntityDeclSAXFunc:
    * @param cctx:  the user data (XML parser context)
    * @param cname:  the entity name
    * @param ctype:  the entity type
    * @param cpublicId: The public ID of the entity
    * @param csystemId: The system ID of the entity
    * @param ccontent: the entity value (without processing).
    *
    * An entity definition has been parsed.
    */
    typedef void (*BTEntityDeclSAXFunc) (void *ctx,
                                    const unsigned char *name,
                                    int type,
                                    const unsigned char *publicId,
                                    const unsigned char *systemId,
                                    unsigned char *content);


    class BTXMLSAXHandler
    {
        public:
            BTXMLSAXHandler();
            ~BTXMLSAXHandler();
            
            BTErrorSAXFunc error;
            BTFatalErrorSAXFunc fatalError;
            BTCharactersSAXFunc characters;
            BTProcessingInstructionSAXFunc processingInstruction;
            BTCdataBlockSAXFunc cdataBlock;
            BTCommentSAXFunc comment;
            BTWarningSAXFunc warning;
            BTStartElementNsSAX2Func startElementNs;
            BTEndElementNsSAX2Func endElementNs;
            BTGetEntitySAXFunc getEntity;
            BTStartDocumentSAXFunc startDocument;
            BTInternalSubsetSAXFunc internalSubset;
            BTExternalSubsetSAXFunc externalSubset;
            BTIgnorableWhitespaceSAXFunc ignorableWhitespace;
            BTEntityDeclSAXFunc entityDecl;
            unsigned int initialized;
    };
    
    /**
    * BTXMLInputMatchCallback:
    * @param cfilename: the filename or URI
    *
    * Callback used in the I/O Input API to detect if the current handler
    * can provide input fonctionnalities for this resource.
    *
    * Returns 1 if yes and 0 if another Input module should be used
    */

    typedef int (*BTXMLInputMatchCallback) (char const *filename);
    
    /**
    * BTXMLInputOpenCallback:
    * @param filename: the filename or URI
    *
    * Callback used in the I/O Input API to open the resource
    *
    * Returns an Input context or NULL in case or error
    */

    typedef void * (*BTXMLInputOpenCallback) (char const *filename);
    
    /**
    * BTXMLInputReadCallback:
    * @param context:  an Input context
    * @param buffer:  the buffer to store data read
    * @param len:  the length of the buffer in bytes
    *
    * Callback used in the I/O Input API to read the resource
    *
    * Returns the number of bytes read or -1 in case of error
    */
    typedef int (*BTXMLInputReadCallback) (void * context, char * buffer, int len);
    
    /**
    * BTXMLInputCloseCallback:
    * @param context:  an Input context
    *
    * Callback used in the I/O Input API to close the resource
    *
    * Returns 0 or -1 in case of error
    */
    typedef int (*BTXMLInputCloseCallback) (void * context);
    
    
    /**
    * BTXMLOutputMatchCallback:
    * @param filename: the filename or URI
    *
    * Callback used in the I/O Output API to detect if the current handler
    * can provide output fonctionnalities for this resource.
    *
    * Returns 1 if yes and 0 if another Output module should be used
    */

    typedef int (*BTXMLOutputMatchCallback) (char const *filename);
    /**
    * BTXMLOutputOpenCallback:
    * @param filename: the filename or URI
    *
    * Callback used in the I/O Output API to open the resource
    *
    * Returns an Output context or NULL in case or error
    */
    typedef void * (*BTXMLOutputOpenCallback) (char const *filename);
    /**
    * BTXMLOutputWriteCallback:
    * @param context:  an Output context
    * @param buffer:  the buffer of data to write
    * @param len:  the length of the buffer in bytes
    *
    * Callback used in the I/O Output API to write to the resource
    *
    * Returns the number of bytes written or -1 in case of error
    */
    typedef int (*BTXMLOutputWriteCallback) (void * context, const char * buffer, int len);
    /**
    * BTXMLOutputCloseCallback:
    * @param context:  an Output context
    *
    * Callback used in the I/O Output API to close the resource
    *
    * Returns 0 or -1 in case of error
    */
    typedef int (*BTXMLOutputCloseCallback) (void * context);
    
    
    class BTXMLEntity
    {
        public:
            BTXMLEntity();
            BTXMLEntity( int t, const unsigned char *n, unsigned char *o, unsigned char *ct, int l, int et );
            ~BTXMLEntity();
            
            int                 type;       /* XML_ENTITY_DECL, must be second ! */
            const unsigned char *name;       /* Entity name */
            unsigned char       *orig;       /* content without ref substitution */
            unsigned char       *content;       /* content or ndata if unparsed */
            int                 length;       /* the content length */
            int                 etype;       /* The entity type */
    };
    
}

#endif // BTXML_H_

