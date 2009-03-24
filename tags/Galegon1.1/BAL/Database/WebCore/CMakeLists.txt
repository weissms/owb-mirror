##################################################
# Database support part.                         #
##################################################

if(ENABLE_DATABASE)
    create_include_link(${BAL_DIR}/Database/WebCore/SQLite/BCSQLiteDatabaseSQLite.h BAL/SQLiteDatabase.h)
    create_include_link(${BAL_DIR}/Database/WebCore/SQLite/BCSQLiteStatementSQLite.h BAL/SQLiteStatement.h)
    create_include_link(${BAL_DIR}/Database/WebCore/SQLite/BCSQLiteTransactionSQLite.h BAL/SQLiteTransaction.h)
    create_include_link(${BAL_DIR}/Database/WebCore/SQLite/BCSQLValueSQLite.h BAL/SQLValue.h)

    aux_source_directory(${BAL_DIR}/Database/WebCore/SQLite WEBCORE_SRC)
endif(ENABLE_DATABASE)
