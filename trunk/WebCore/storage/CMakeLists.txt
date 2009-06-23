if(ENABLE_DATABASE)
    list(APPEND WEBCORE_SRC
        storage/ChangeVersionWrapper.cpp
        storage/Database.cpp
        storage/DatabaseAuthorizer.cpp
        storage/DatabaseTask.cpp
        storage/DatabaseThread.cpp
        storage/DatabaseTracker.cpp
        storage/OriginQuotaManager.cpp
        storage/OriginUsageRecord.cpp
        storage/SQLResultSet.cpp
        storage/SQLResultSetRowList.cpp
        storage/SQLStatement.cpp
        storage/SQLTransaction.cpp
    )
    list(APPEND IDL_SRC
        storage/Database.idl
        storage/SQLError.idl
        storage/SQLResultSet.idl
        storage/SQLResultSetRowList.idl
        storage/SQLTransaction.idl
    )
endif(ENABLE_DATABASE)

if(ENABLE_DOM_STORAGE)
    list(APPEND WEBCORE_SRC
        storage/LocalStorage.cpp
        storage/LocalStorageArea.cpp
        storage/LocalStorageTask.cpp
        storage/LocalStorageThread.cpp
        storage/SessionStorage.cpp
        storage/SessionStorageArea.cpp
        storage/Storage.cpp
        storage/StorageArea.cpp
        storage/StorageEvent.cpp
        storage/StorageMap.cpp
        storage/StorageSyncManager.cpp
    )
    list(APPEND IDL_SRC
        storage/Storage.idl
        storage/StorageEvent.idl
    )
endif(ENABLE_DOM_STORAGE)
