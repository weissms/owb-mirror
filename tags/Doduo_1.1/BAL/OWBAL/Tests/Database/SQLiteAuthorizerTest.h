#ifndef SQLiteAuthorizer_h_CPPUNIT
#define SQLiteAuthorizer_h_CPPUNIT

#include <cppunit/extensions/HelperMacros.h>
#include "BCSQLiteAuthorizer.h"
class SQLiteAuthorizerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( SQLiteAuthorizerTest );
//register each method:
    CPPUNIT_TEST( constructors );
    CPPUNIT_TEST( destructors );
    CPPUNIT_TEST( create );
    CPPUNIT_TEST( createTable);
    CPPUNIT_TEST( createTempTable);
    CPPUNIT_TEST( dropTable);
    CPPUNIT_TEST( dropTempTable);
    CPPUNIT_TEST( allowAlterTable);

    CPPUNIT_TEST( createIndex);
    CPPUNIT_TEST( createTempIndex);
    CPPUNIT_TEST( dropIndex);
    CPPUNIT_TEST( dropTempIndex);

    CPPUNIT_TEST( createTrigger);
    CPPUNIT_TEST( createTempTrigger);
    CPPUNIT_TEST( dropTrigger);
    CPPUNIT_TEST( dropTempTrigger);

    CPPUNIT_TEST( createView);
    CPPUNIT_TEST( createTempView);
    CPPUNIT_TEST( dropView);
    CPPUNIT_TEST( dropTempView);

    CPPUNIT_TEST( createVTable);
    CPPUNIT_TEST( dropVTable);

    CPPUNIT_TEST( allowDelete);
    CPPUNIT_TEST( allowInsert);
    CPPUNIT_TEST( allowUpdate);
    CPPUNIT_TEST( allowTransaction);

    CPPUNIT_TEST( allowSelect);
    CPPUNIT_TEST( allowRead);

    CPPUNIT_TEST( allowAttach);
    CPPUNIT_TEST( allowDetach);

    CPPUNIT_TEST( allowReindex);
    CPPUNIT_TEST( allowAnalyze);
    CPPUNIT_TEST( allowFunction);
    CPPUNIT_TEST( allowPragma);

    CPPUNIT_TEST_SUITE_END();

public:
    void constructors()CPPU_NOT_IMPLEMENTED
    void destructors()CPPU_NOT_IMPLEMENTED
    void create()CPPU_NOT_IMPLEMENTED


    void createTable() CPPU_NOT_IMPLEMENTED
    void createTempTable() CPPU_NOT_IMPLEMENTED
    void dropTable() CPPU_NOT_IMPLEMENTED
    void dropTempTable() CPPU_NOT_IMPLEMENTED
    void allowAlterTable() CPPU_NOT_IMPLEMENTED

    void createIndex() CPPU_NOT_IMPLEMENTED
    void createTempIndex() CPPU_NOT_IMPLEMENTED
    void dropIndex() CPPU_NOT_IMPLEMENTED
    void dropTempIndex() CPPU_NOT_IMPLEMENTED

    void createTrigger() CPPU_NOT_IMPLEMENTED
    void createTempTrigger() CPPU_NOT_IMPLEMENTED
    void dropTrigger() CPPU_NOT_IMPLEMENTED
    void dropTempTrigger() CPPU_NOT_IMPLEMENTED

    void createView() CPPU_NOT_IMPLEMENTED
    void createTempView() CPPU_NOT_IMPLEMENTED
    void dropView() CPPU_NOT_IMPLEMENTED
    void dropTempView() CPPU_NOT_IMPLEMENTED

    void createVTable() CPPU_NOT_IMPLEMENTED
    void dropVTable() CPPU_NOT_IMPLEMENTED

    void allowDelete() CPPU_NOT_IMPLEMENTED
    void allowInsert() CPPU_NOT_IMPLEMENTED
    void allowUpdate() CPPU_NOT_IMPLEMENTED
    void allowTransaction() CPPU_NOT_IMPLEMENTED

    void allowSelect() CPPU_NOT_IMPLEMENTED
    void allowRead() CPPU_NOT_IMPLEMENTED

    void allowAttach() CPPU_NOT_IMPLEMENTED
    void allowDetach() CPPU_NOT_IMPLEMENTED

    void allowReindex() CPPU_NOT_IMPLEMENTED
    void allowAnalyze() CPPU_NOT_IMPLEMENTED
    void allowFunction() CPPU_NOT_IMPLEMENTED
    void allowPragma() CPPU_NOT_IMPLEMENTED

};

#endif // SQLiteAuthorizer_h
