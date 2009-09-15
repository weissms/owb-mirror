list(APPEND WEBCORE_SRC
    loader/archive/ArchiveFactory.cpp
    loader/archive/ArchiveResource.cpp
    loader/archive/ArchiveResourceCollection.cpp
    loader/Cache.cpp
    loader/CachedCSSStyleSheet.cpp
    loader/CachedFont.cpp
    loader/CachedImage.cpp
    loader/CachedResource.cpp
    loader/CachedResourceClientWalker.cpp
    loader/CachedResourceHandle.cpp
    loader/CachedScript.cpp
    loader/CachedXSLStyleSheet.cpp
    loader/CrossOriginAccessControl.cpp
    loader/CrossOriginPreflightResultCache.cpp
    loader/DocLoader.cpp
    loader/DocumentLoader.cpp
    loader/DocumentThreadableLoader.cpp
    loader/FormState.cpp
    loader/FrameLoader.cpp
    loader/FTPDirectoryDocument.cpp
    loader/FTPDirectoryParser.cpp
    loader/ImageDocument.cpp
    loader/ImageLoader.cpp
    loader/loader.cpp
    loader/MainResourceLoader.cpp
    loader/NavigationAction.cpp
    loader/NetscapePlugInStreamLoader.cpp
    loader/PlaceholderDocument.cpp
    loader/PluginDocument.cpp
    loader/ProgressTracker.cpp
    loader/Request.cpp
    loader/ResourceLoader.cpp
    loader/SubresourceLoader.cpp
    loader/TextDocument.cpp
    loader/TextResourceDecoder.cpp
    loader/ThreadableLoader.cpp
    loader/WorkerThreadableLoader.cpp
)

if(ENABLE_OFFLINE_WEB_APPLICATIONS)
    list(APPEND WEBCORE_SRC
        loader/appcache/ApplicationCache.cpp
        loader/appcache/ApplicationCacheGroup.cpp
        loader/appcache/ApplicationCacheHost.cpp
        loader/appcache/ApplicationCacheResource.cpp
        loader/appcache/ApplicationCacheStorage.cpp
        loader/appcache/DOMApplicationCache.cpp
        loader/appcache/ManifestParser.cpp
    )
    list(APPEND IDL_SRC
        loader/appcache/DOMApplicationCache.idl
    )
endif(ENABLE_OFFLINE_WEB_APPLICATIONS)

if(ENABLE_VIDEO)
    list(APPEND WEBCORE_SRC
        loader/MediaDocument.cpp
    )
endif(ENABLE_VIDEO)

