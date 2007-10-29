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
#ifndef BIXSLT_H_
#define BIXSLT_H_

//#include <libxml/xmlstring.h>
//#include <libxml/HTMLparser.h>

#include "BIXML.h"

#include "BTXSLT.h"

namespace BAL {


    /**
    * @brief the BIXSLT
    * 
    * The xslt base class
    * 
    */
    class BIXSLT {
        public:
        // this is mandatory
            virtual ~BIXSLT(){}
            
            /**
             *  Check if a string is ignorable
             *  Returns:  1 if the string is NULL or made of blanks chars, 0 otherwise
             */
            
            virtual int isBlank( unsigned char *) = 0;
            
            /**
            * isXsltElem:
            *
            * Checks that the element pertains to XSLT namespace.
            */
            virtual int isXsltElem( BTXMLNode *) = 0;
            
            /**
            * IS_XSLT_NAME:
            *
            * Checks the value of an element in XSLT namespace.
            */
            virtual int isXlstName( BTXMLNode *, char * ) = 0;
            
            /**
            * get the namespace properties
            */
            virtual unsigned char *xsltGetNsProp( BTXMLNode *node, const unsigned char *name, const unsigned char *nameSpace) = 0;
            
            /**
            * load the styleSheet
            */
            virtual BTXSLTStyleSheet* xsltLoadStylesheetPI( BTXMLDoc *doc ) = 0;
            
            /**
            * Parse the styleSheet
            */
            virtual BTXSLTStyleSheet* xsltParseStylesheetDoc( BTXMLDoc *doc ) = 0;
            
            /**
            * BIXSLTDocLoaderFunc:
            * @URI: the URI of the document to load
            * @dict: the dictionnary to use when parsing that document
            * @options: parsing options, a set of xmlParserOption
            * @ctxt: the context, either a stylesheet or a transformation context
            * @type: the BIXsltLoadType indicating the kind of loading required
            *
            * An BIXSLTDocLoaderFunc is a signature for a function which can be
            * registered to load document not provided by the compilation or
            * transformation API themselve, for example when an xsl:import,
            * xsl:include is found at compilation time or when a document()
            * call is made at runtime.
            *
            * Returns the pointer to the document (which will be modified and
            * freed by the engine later), or NULL in case of error.
            */

            virtual void setLoaderFunc( BIXSLTDocLoaderFunc f ) = 0;
            
            
            /**
            * Save the result obtained by applying the stylesheet to an I/O output channel buf
            */
            virtual int xsltSaveResultTo( BTXMLOutputBuffer *buf, BTXMLDoc *result, BTXSLTStyleSheet *style) = 0;
            
            /**
            * import pointers from the stylesheet cascading order.
            */
            virtual void getImportPtr( const unsigned char *res, BTXSLTStyleSheet *style ) = 0;
            
            /**
            * Export context to users.
            */
            virtual BTXSLTTransformContext *xsltNewTransformContext( BTXSLTStyleSheet *style, BTXMLDoc *doc ) = 0;
            
            /**
             * Apply the user style sheet
             */
            virtual BTXMLDoc *xsltApplyStylesheetUser( BTXSLTStyleSheet *style,
                                               BTXMLDoc *doc,
                                               const char **params,
                                               const char *output,
                                               FILE * profile,
                                               BTXSLTTransformContext *userCtxt) = 0;
            
            /**
            * Interfaces for the variable module.
            */
            virtual int xsltQuoteUserParams( BTXSLTTransformContext *ctxt, const char **params) = 0;
            
            
            /**
             * free the transform context
             */
            virtual void xsltFreeTransformContext( BTXSLTTransformContext *ctxt ) = 0;
            
            /**
             * free the style sheet
             */
            virtual void xsltFreeStylesheet( BTXSLTStyleSheet *sheet ) = 0;
    };

    
}

#endif // BIXLST_H_

