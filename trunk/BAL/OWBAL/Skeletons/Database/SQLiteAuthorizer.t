/*
 * Copyright (C) 2008 Pleyo.  All rights reserved.
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
#ifndef SQLiteAuthorizer_h
#define SQLiteAuthorizer_h
/**
 *  @file  SQLiteAuthorizer.t
 *  SQLiteAuthorizer description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include <wtf/PassRefPtr.h>
#include <wtf/Threading.h>

namespace OWBAL {

class String;

extern const int SQLAuthAllow;
extern const int SQLAuthIgnore;
extern const int SQLAuthDeny;

class SQLiteAuthorizer : public OWBALBase, public ThreadSafeShared<SQLiteAuthorizer> {
public:
    /**
     * Create a SQLiteAuthorizer.
     * @param[out] : returns a PassRefPtr to a SQLiteAuthorizer.
     * @code
     * @endcode
     */
    static PassRefPtr<SQLiteAuthorizer> create() ;

    /**
     * Destructor for SQLiteAuthorizer.
     * @code
     * @endcode
     */
    virtual ~SQLiteAuthorizer() ;

    /**
     * Create a new table in database.
     * @param[in] : a table name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int createTable(const String& tableName) ;

    /**
     * Create a temporary table in database.
     * @param[in] : a tableName.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int createTempTable(const String& tableName) ;

    /**
     * Drop a table.
     * @param[in] : a table name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int dropTable(const String& tableName) ;

    /**
     * Drop a temporary table.
     * @param[in] : a table name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int dropTempTable(const String& tableName) ;

    /**
     * Check credential to alter a table.
     * @param[in] : a database name.
     * @param[in] : a table name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int allowAlterTable(const String& databaseName, const String& tableName) ;

    /**
     * Create an index.
     * @param[in] : an index name.
     * @param[in] : a table name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int createIndex(const String& indexName, const String& tableName) ;

    /**
     * Create a temporary index.
     * @param[in] : an index name.
     * @param[in] : a table name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int createTempIndex(const String& indexName, const String& tableName) ;

    /**
     * Drop an index.
     * @param[in] : an index name.
     * @param[in] : a table name
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int dropIndex(const String& indexName, const String& tableName) ;

    /**
     * Drop a temporary index.
     * @param[in] : an index name.
     * @param[in] : a table name
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int dropTempIndex(const String& indexName, const String& tableName) ;

    /**
     * Create a trigger.
     * @param[in] : a trigger name.
     * @param[in] : a table name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int createTrigger(const String& triggerName, const String& tableName) ;

    /**
     * Create a temporary trigger.
     * @param[in] : a trigger name.
     * @param[in] : a table name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int createTempTrigger(const String& triggerName, const String& tableName) ;

    /**
     * Drop a trigger.
     * @param[in] : a trigger name.
     * @param[in] : a table name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int dropTrigger(const String& triggerName, const String& tableName) ;

    /**
     * Drop a temporary trigger.
     * @param[in] : a trigger name.
     * @param[in] : a table name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int dropTempTrigger(const String& triggerName, const String& tableName) ;

    /**
     * Create a view.
     * @param[in] : a view name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int createView(const String& viewName) ;

    /**
     * Create a temporary view.
     * @param[in] : a view name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int createTempView(const String& viewName) ;

    /**
     * Drop a view.
     * @param[in] : a view name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int dropView(const String& viewName) ;

    /**
     * Drop a temporary view.
     * @param[in] : a view name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int dropTempView(const String& viewName) ;

    /**
     * Create a vtable.
     * @param[in] : a table name.
     * @param[in] : a module name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int createVTable(const String& tableName, const String& moduleName) ;

    /**
     * Drop a vtable.
     * @param[in] : a table name.
     * @param[in] : a module name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int dropVTable(const String& tableName, const String& moduleName) ;

    /**
     * Check credential to delete an item in a database.
     * @param[in] : a table name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int allowDelete(const String& tableName) ;

    /**
     * Check credential to insert an item in a database.
     * @param[in] : a table name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int allowInsert(const String& tableName) ;

    /**
     * Check credential for an update.
     * @param[in] : a table name.
     * @param[in] : a column name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int allowUpdate(const String& tableName, const String& columnName) ;

    /**
     * Check credential for a transaction.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int allowTransaction() ;

    /**
     * Check credential for a select.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int allowSelect() ;

    /**
     * Check credential for a read.
     * @param[in] : a table name.
     * @param[in] : a column name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int allowRead(const String& tableName, const String& columnName) ;

    /**
     * Check credential for an attach.
     * @param[in] : a file name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int allowAttach(const String& filename) ;

    /**
     * Check credential for a detach.
     * @param[in] : a database name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int allowDetach(const String& databaseName) ;

    /**
     * Check credential to reindex.
     * @param[in] : an index name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int allowReindex(const String& indexName) ;

    /**
     * Check credential to analyze.
     * @param[in] : a table name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int allowAnalyze(const String& tableName) ;

    /**
     * Check credential to allow a function.
     * @param[in] : a function name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int allowFunction(const String& functionName) ;

    /**
     * Check credential for pragma.
     * @param[in] : a pragma name.
     * @param[out] : returns an integer which can be either equal to SQLAuthAllow or equal to SQLAuthDeny.
     * @code
     * @endcode
     */
    virtual int allowPragma(const String& pragmaName, const String& firstArgument) ;
    
protected:
    /**
     * Constructor for SQLiteAuthorizer.
     * @code
     * @endcode
     */
    SQLiteAuthorizer() ;
};

} // namespace OWBAL

#endif // SQLiteAuthorizer_h




