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
 * @file  BCXSLT.cpp
 *
 * @brief Bal Concretisation of BIXSLT
 *
 * Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date: 2006/05/11 13:44:56 $
 *
 */

#include "BCXSLT.h"
#include <libxslt/xsltutils.h>

#include <libxslt/xslt.h>
#include <libxslt/transform.h>
#include <libxslt/imports.h>
#include <libxslt/variables.h>

#include "BALConfiguration.h"
#include "../LIBXML/BCXML.h"

using namespace BC;
using namespace BAL;

namespace BAL {

    static BIXSLT *theXslt = NULL;
    
    BIXSLT* createBIXSLT(  )
    {
        if( !theXslt )
            theXslt = new BCXSLT( );
        return theXslt;
    }

}

BCXSLT::BCXSLT( )
{
    m_sheet = new BTXSLTStyleSheet();
    m_trans = new BTXSLTTransformContext();
    m_xml = static_cast<BCXML *>(createBIXML());
}

BCXSLT::~BCXSLT( )
{
    delete m_sheet;
    delete m_trans;
    
}

int BCXSLT::isBlank( unsigned char *str)
{
    return xsltIsBlank( str );
}

int BCXSLT::isXsltElem( BTXMLNode *node )
{
    return IS_XSLT_ELEM( node );
}

int BCXSLT::isXlstName( BTXMLNode *node, char *str )
{
    return IS_XSLT_NAME( node, str );
}

unsigned char *BCXSLT::xsltGetNsProp( BTXMLNode *node, const unsigned char *name, const unsigned char *nameSpace)
{
    return ::xsltGetNsProp( m_xml->m_node, name, nameSpace );
}

BTXSLTStyleSheet* BCXSLT::xsltLoadStylesheetPI( BTXMLDoc *doc )
{
    m_xsltsheet = ::xsltLoadStylesheetPI( m_xml->m_doc );
    
    m_sheet->doc = m_xml->convertDoc( m_xsltsheet->doc );
    m_sheet->omitXmlDeclaration = m_xsltsheet->omitXmlDeclaration;
    
    return m_sheet;
}

BTXSLTStyleSheet* BCXSLT::xsltParseStylesheetDoc( BTXMLDoc *doc )
{
    m_xsltsheet = ::xsltParseStylesheetDoc( m_xml->m_doc );
    
    m_sheet->doc = m_xml->convertDoc( m_xsltsheet->doc );
    m_sheet->omitXmlDeclaration = m_xsltsheet->omitXmlDeclaration;
    
    return m_sheet;
}

static BAL::BIXSLTDocLoaderFunc m_f;

void BCXSLT::setLoaderFunc( BIXSLTDocLoaderFunc f )
{
    m_f = f;
    xsltSetLoaderFunc( XSLTDocLoaderFunc );
}

xmlDocPtr BCXSLT::XSLTDocLoaderFunc( const xmlChar *URI,
                                    xmlDictPtr ,
                                    int options,
                                    void *ctxt,
                                    xsltLoadType type )
{
    if( m_f )
    {
        BTXMLDict *d = new BTXMLDict();
        BTXMLDoc *doc = m_f( URI, d, options, ctxt, (BIXsltLoadType)type );
        /*FIXME : return the xmlDoc*/
        return 0 ;
    }
    else
    {
        return 0;
    }
}

int BCXSLT::xsltSaveResultTo( BTXMLOutputBuffer *buf, BTXMLDoc *result, BTXSLTStyleSheet *style)
{
    m_xsltsheet->doc = m_xml->convertXmlDoc( m_xsltsheet->doc, style->doc );
    m_xsltsheet->omitXmlDeclaration = style->omitXmlDeclaration;
    
    return ::xsltSaveResultTo( m_xml->m_outbuf, m_xml->m_doc, m_xsltsheet);
}

void BCXSLT::getImportPtr( const unsigned char *res, BTXSLTStyleSheet *style )
{
    m_xsltsheet->doc = m_xml->convertXmlDoc( m_xsltsheet->doc, style->doc );
    m_xsltsheet->omitXmlDeclaration = style->omitXmlDeclaration;
    
    res = NULL;
    while (m_xsltsheet != NULL)
    {
        if ( m_xsltsheet->method != NULL ) { res = m_xsltsheet->method; break; }
        m_xsltsheet = xsltNextImport(m_xsltsheet);
    }
}

BTXSLTTransformContext *BCXSLT::xsltNewTransformContext( BTXSLTStyleSheet *style, BTXMLDoc *doc )
{
    m_xsltsheet->doc = m_xml->convertXmlDoc( m_xsltsheet->doc, style->doc );
    m_xsltsheet->omitXmlDeclaration = style->omitXmlDeclaration;
    
    m_xsltTransform = ::xsltNewTransformContext( m_xsltsheet, m_xml->m_doc );
    
    m_trans->document->doc = m_xml->convertDoc( m_xsltTransform->document->doc );
    
    m_trans->node = m_xml->convertNode( m_xsltTransform->node );
    
    return m_trans;
}

int BCXSLT::xsltQuoteUserParams( BTXSLTTransformContext *ctxt, const char **params)
{
    m_xsltTransform->document->doc = m_xml->convertXmlDoc( m_xsltTransform->document->doc, ctxt->document->doc );
    m_xsltTransform->node = m_xml->convertXmlNode( ctxt->node );
    
    return ::xsltQuoteUserParams( m_xsltTransform, params );
}

BTXMLDoc *BCXSLT::xsltApplyStylesheetUser( BTXSLTStyleSheet *style,
                                    BTXMLDoc *doc,
                                    const char **params,
                                    const char *output,
                                    FILE * profile,
                                    BTXSLTTransformContext *userCtxt)
{
    m_xsltsheet->doc = m_xml->convertXmlDoc( m_xsltsheet->doc, style->doc );
    m_xsltsheet->omitXmlDeclaration = style->omitXmlDeclaration;
    
    m_xsltTransform->document->doc = m_xml->convertXmlDoc( m_xsltTransform->document->doc, userCtxt->document->doc );
    m_xsltTransform->node = m_xml->convertXmlNode( userCtxt->node );
    
    return m_xml->convertDoc( ::xsltApplyStylesheetUser( m_xsltsheet, m_xml->m_doc, params, output, profile, m_xsltTransform ) );
}

void BCXSLT::xsltFreeTransformContext( BTXSLTTransformContext *ctxt )
{
    // TODO convert
    
    m_xsltTransform->document->doc = m_xml->convertXmlDoc( m_xsltTransform->document->doc, ctxt->document->doc );
    m_xsltTransform->node = m_xml->convertXmlNode( ctxt->node );
    
    ::xsltFreeTransformContext( m_xsltTransform );
}

void BCXSLT::xsltFreeStylesheet( BTXSLTStyleSheet *sheet )
{
    m_xsltsheet->doc = m_xml->convertXmlDoc( m_xsltsheet->doc, sheet->doc );
    m_xsltsheet->omitXmlDeclaration = sheet->omitXmlDeclaration;
    
    ::xsltFreeStylesheet( m_xsltsheet );
}

