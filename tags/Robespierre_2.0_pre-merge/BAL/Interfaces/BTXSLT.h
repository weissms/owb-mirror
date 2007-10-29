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
#ifndef BTXSLT_H_
#define BTXSLT_H_

#include "BIXML.h"

namespace BAL {

    #define BIXSLT_PARSE_OPTIONS \
        BTXML_PARSE_NOENT | BTXML_PARSE_DTDLOAD | BTXML_PARSE_DTDATTR | BTXML_PARSE_NOCDATA
    
    #define BIXSLT_NAMESPACE ((unsigned char *) "http://www.w3.org/1999/XSL/Transform")
    
    typedef enum {
        XSLT_LOAD_START = 0,        /* loading for a top stylesheet */
        XSLT_LOAD_STYLESHEET = 1,   /* loading for a stylesheet include/import */
        XSLT_LOAD_DOCUMENT = 2      /* loading document at transformation time */
    } BIXsltLoadType;

    class BTXSLTDocument
    {
        public:
            BTXSLTDocument(){};
            ~BTXSLTDocument(){};
            
            BTXMLDoc *doc;
    };



    class BTXSLTTransformContext
    {
        public:
            BTXSLTTransformContext() { document = new BTXSLTDocument(); };
            BTXSLTTransformContext(void *c)
            { 
                ctxt = c;
                document = new BTXSLTDocument();
            };
            ~BTXSLTTransformContext(){};
            
            
            void *ctxt;
            BTXSLTDocument *document;           /* the current document */
            BTXMLNode *node;                    /* the current node being processed */
            BTXMLHashTable *globalVars;       /* the global variables and params */
    };

    class BTXSLTStyleSheet
    {
        public:
            // this is mandatory
            BTXSLTStyleSheet(){};
            ~BTXSLTStyleSheet(){};
            
            
            BTXMLDoc *doc;              /* the parsed XML stylesheet */
            int omitXmlDeclaration;     /* omit-xml-declaration = "yes" | "no" */
    };
    
    
    typedef BTXMLDoc* (*BIXSLTDocLoaderFunc) ( const unsigned char *URI,
                                               BTXMLDict *dict,
                                               int options,
                                               void *ctxt,
                                               BIXsltLoadType type );

}

#endif // BTXSLT_H_

