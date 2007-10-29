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
 * @file  BCXSLT.h
 *
 * @brief Concretisation of BIXLST
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date: 2006/05/11 13:44:56 $
 *
 * This header file is private. Only IDL interface is public.
 *
 */

#include "config.h"
#include "PlatformString.h"

#include "BIXSLT.h"
#include "BTXML.h"
#include <libxslt/documents.h>
#include <libxml/tree.h>

namespace BC
{
    class BCXML;
    
    /**
    * @brief the XSLT Implementation
    * 
    */
    class BCXSLT : public BAL::BIXSLT {
        public:
            BCXSLT();
            ~BCXSLT();
            
            int isBlank( unsigned char *);
            int isXsltElem( BAL::BTXMLNode *);
            int isXlstName( BAL::BTXMLNode *, char * );
            unsigned char *xsltGetNsProp( BAL::BTXMLNode *node, const unsigned char *name, const unsigned char *nameSpace);
            BAL::BTXSLTStyleSheet* xsltLoadStylesheetPI( BAL::BTXMLDoc *doc );
            BAL::BTXSLTStyleSheet* xsltParseStylesheetDoc( BAL::BTXMLDoc *doc );
            void setLoaderFunc( BAL::BIXSLTDocLoaderFunc f );
            int xsltSaveResultTo( BAL::BTXMLOutputBuffer *buf, BAL::BTXMLDoc *result, BAL::BTXSLTStyleSheet *style);
            void getImportPtr( const unsigned char *res, BAL::BTXSLTStyleSheet *style );
            BAL::BTXSLTTransformContext *xsltNewTransformContext( BAL::BTXSLTStyleSheet *style, BAL::BTXMLDoc *doc );
            int xsltQuoteUserParams( BAL::BTXSLTTransformContext *ctxt, const char **params);
            BAL::BTXMLDoc *xsltApplyStylesheetUser( BAL::BTXSLTStyleSheet *style,
                                               BAL::BTXMLDoc *doc,
                                               const char **params,
                                               const char *output,
                                               FILE * profile,
                                               BAL::BTXSLTTransformContext *userCtxt);
            void xsltFreeTransformContext( BAL::BTXSLTTransformContext *ctxt );
            void xsltFreeStylesheet( BAL::BTXSLTStyleSheet *sheet );
            static xmlDocPtr XSLTDocLoaderFunc( const xmlChar *URI,
                                                xmlDictPtr dict,
                                                int options,
                                                void *ctxt,
                                                xsltLoadType type );
       private:
       
            //void convertBTXMLNode( BAL::BTXMLNode *node );
       
            BAL::BTXSLTStyleSheet *m_sheet;
            BAL::BTXSLTTransformContext *m_trans;
            xsltStylesheetPtr m_xsltsheet;
            xsltTransformContextPtr m_xsltTransform;
            BCXML *m_xml;
    
    };
}
