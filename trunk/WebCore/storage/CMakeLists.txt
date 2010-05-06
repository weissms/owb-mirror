if(ENABLE_DATABASE)
    list(APPEND WEBCORE_SRC
        storage/ChangeVersionWrapper.cpp
        storage/Database.cpp
        storage/DatabaseAuthorizer.cpp
        storage/DatabaseSync.cpp
        storage/DatabaseTask.cpp
        storage/DatabaseThread.cpp
        storage/DatabaseTracker.cpp
        storage/OriginQuotaManager.cpp
        storage/OriginUsageRecord.cpp
        storage/SQLResultSet.cpp
        storage/SQLResultSetRowList.cpp
        storage/SQLStatement.cpp
        storage/SQLTransaction.cpp
        storage/SQLTransactionClient.cpp
        storage/SQLTransactionCoordinator.cpp
        storage/SQLTransactionSync.cpp
    )
    list(APPEND IDL_SRC
        storage/Database.idl
        storage/DatabaseCallback.idl
        storage/SQLError.idl
        storage/SQLResultSet.idl
        storage/SQLResultSetRowList.idl
        storage/SQLStatementCallback.idl
        storage/SQLStatementErrorCallback.idl
        storage/SQLTransaction.idl
        storage/SQLTransactionCallback.idl
        storage/SQLTransactionErrorCallback.idl
    )
endif(ENABLE_DATABASE)

if(ENABLE_DOM_STORAGE)
    list(APPEND WEBCORE_SRC
        storage/LocalStorageTask.cpp
        storage/LocalStorageThread.cpp
        storage/Storage.cpp
        storage/StorageAreaImpl.cpp
        storage/StorageAreaSync.cpp
        storage/StorageEvent.cpp
        storage/StorageEventDispatcher.cpp
        storage/StorageMap.cpp
        storage/StorageNamespace.cpp
        storage/StorageNamespaceImpl.cpp
        storage/StorageSyncManager.cpp
    )
    list(APPEND IDL_SRC
        storage/Storage.idl
        storage/StorageEvent.idl
    )
endif(ENABLE_DOM_STORAGE)

if(ENABLE_INDEXED_DATABASE)
    list(APPEND WEBCORE_SRC
        storage/IDBDatabase.cpp
        storage/IDBDatabaseRequest.cpp
        storage/IndexedDatabase.cpp
        storage/IndexedDatabaseImpl.cpp
        storage/IndexedDatabaseRequest.cpp
    )
    list(APPEND IDL_SRC
        storage/IDBDatabaseError.idl
        storage/IDBDatabaseException.idl
        storage/IDBDatabaseRequest.idl
        storage/IndexedDatabaseRequest.idl
    )
endif(ENABLE_INDEXED_DATABASE)
