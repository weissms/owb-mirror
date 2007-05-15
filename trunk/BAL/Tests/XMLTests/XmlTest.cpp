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

#include "config.h"
#include "BALConfiguration.h"
#include "BIXSLT.h"
#include "BIXML.h"

#include "TestManager/TestManager.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "PlatformString.h"

using namespace BAL;
using namespace WebCore;


static BTXMLDoc *docLoaderFunc(const unsigned char *uri,
                                    BTXMLDict *dict,
                                    int options,
                                    void* ctxt,
                                    BAL::BIXsltLoadType type)
{
    printf( "toto\n" );

    switch (type) {
        case XSLT_LOAD_DOCUMENT: {
            printf( "XSLT_LOAD_DOCUMENT\n");
            return 0;
        }
        case XSLT_LOAD_STYLESHEET: {
            printf( "XSLT_LOAD_STYLESHEET\n");
            return 0;
        }
        default:
            break;
    }
    
    return 0;
}






class XmlTest {
public:

    static void setUp()
    {
        // do nothing
    }

    static void tearDown()
    {
        // do nothing
    }

    static void testXslt()
    {
        BAL::BIXSLT *m_xlst = createBIXSLT();
        BAL::BIXML *m_xml = createBIXML();
       
        std::string xslFile = TestManager::GetInstance().getPath() + "XMLTests/Res/xslbench1.xsl";
        int fd = open(xslFile.c_str(), O_RDONLY);
	TestManager::AssertTrue(xslFile.c_str(), fd >= 0);
        if( fd < 0 )
            return;
        
        int size = lseek( fd, 0, SEEK_END );
        lseek( fd, SEEK_SET, 0 );
        
        char buf[size];
        
        int readSize = read( fd, buf, size );
        buf[ size ] = '\0';
        
        close( fd );
        
        if( readSize != size )
            return ;
        
        String xslDoc( buf, size );
        
        
        std::string xmlFile = TestManager::GetInstance().getPath() + "XMLTests/Res/xslbench1.xml";
        fd = open(xmlFile.c_str(), O_RDONLY);
	TestManager::AssertTrue(xmlFile.c_str(), fd >= 0);
        if( fd < 0 )
            return;
        
        int sizeXml = lseek( fd, 0, SEEK_END );
        lseek( fd, SEEK_SET, 0 );
        
        char bufXml[size];
        
        readSize = read( fd, bufXml, size );
        bufXml[ size ] = '\0';
        
        close( fd );
        
        if( readSize != sizeXml )
            return ;
        
        String xmlDoc( bufXml, sizeXml );
        
        
        BTXMLDoc *m_stylesheetDoc = m_xml->xmlReadMemory(reinterpret_cast<const char*>(xslDoc.characters()), xslDoc.length() * sizeof(UChar),
        "./XMLTests/Res/xslbench1.xsl",
        NULL, 
        BTXML_PARSE_NOENT | BTXML_PARSE_DTDATTR | BTXML_PARSE_NOERROR | BTXML_PARSE_NOWARNING | BTXML_PARSE_NOCDATA);
        
        TestManager::AssertTrue("test xslsheetDoc", m_stylesheetDoc != NULL );
        
        
        printf( "m_stylesheetDoc->type = %d, m_stylesheetDoc->encoding = 0x%x\n", m_stylesheetDoc->type, m_stylesheetDoc->encoding );
        
        
        m_xlst->setLoaderFunc( docLoaderFunc );
        
        BTXSLTStyleSheet* sheet = m_xlst->xsltParseStylesheetDoc(m_stylesheetDoc);
        
        TestManager::AssertTrue("test sheet", sheet != NULL );
        
        BTXMLDoc *m_xmlDoc = m_xml->xmlReadMemory(reinterpret_cast<const char*>(xmlDoc.characters()), xmlDoc.length() * sizeof(UChar),
        "./XMLTests/Res/xslbench1.xml",
        NULL, 
        BTXML_PARSE_NOENT | BTXML_PARSE_DTDATTR | BTXML_PARSE_NOERROR | BTXML_PARSE_NOWARNING | BTXML_PARSE_NOCDATA);
        
        TestManager::AssertTrue("test xml sheetDoc", m_xmlDoc != NULL );
        
        BTXSLTTransformContext *transformContext = m_xlst->xsltNewTransformContext(sheet, m_xmlDoc);
        
        TestManager::AssertTrue("test transformContext", transformContext != NULL );
        
        TestManager::AssertTrue("test xsltQuoteUserParams", m_xlst->xsltQuoteUserParams(transformContext, NULL) == 0 );
        
        BTXMLDoc *resultDoc = m_xlst->xsltApplyStylesheetUser(sheet, m_xmlDoc, 0, 0, 0, transformContext);
        
        TestManager::AssertTrue("test xsltApplyStylesheetUser", resultDoc != NULL );
        
        m_xlst->xsltFreeTransformContext(transformContext);
        
        delete m_xlst;
        delete m_xml;
    }

private:
};

static TestNode gtestXslt =
 { "testXslt", "testXslt", TestNode::AUTO, XmlTest::testXslt, NULL };
 
TestNode* gXmlTestNodeList[] = {
  &gtestXslt,
  	NULL
};

TestNode gTestSuiteXml = {
    "TestXml",
    "test xml",
    TestNode::TEST_SUITE,
    NULL, /* no function, it's a test suite */
		gXmlTestNodeList 
};

