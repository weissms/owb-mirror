list(APPEND WEBCORE_SRC
    plugins/MimeType.cpp
    plugins/MimeTypeArray.cpp
    plugins/Plugin.cpp
    plugins/PluginArray.cpp
    plugins/PluginData.cpp
    plugins/PluginDatabase.cpp
    plugins/PluginInfoStore.cpp
    plugins/PluginMainThreadScheduler.cpp
    plugins/PluginPackage.cpp
    plugins/PluginStream.cpp
    plugins/PluginView.cpp
    plugins/PluginDebug.cpp
)
list(APPEND IDL_SRC
    plugins/MimeType.idl
    plugins/MimeTypeArray.idl
    plugins/Plugin.idl
    plugins/PluginArray.idl)

if(ENABLE_NPAPI)
    list(APPEND WEBCORE_SRC
        plugins/npapi.cpp
    )
    if(USE_GRAPHICS_GTK)
        list(APPEND WEBCORE_SRC
            plugins/gtk/PluginPackageGtk.cpp
            plugins/gtk/PluginDataGtk.cpp
            plugins/gtk/PluginViewGtk.cpp
            plugins/gtk/gtk2xtbin.c
        )
    endif(USE_GRAPHICS_GTK)

    if(USE_GRAPHICS_QT)
        list(APPEND WEBCORE_SRC
            plugins/qt/PluginContainerQt.cpp
            plugins/qt/PluginPackageQt.cpp
            plugins/qt/PluginDataQt.cpp
            plugins/qt/PluginViewQt.cpp
        )
        owb_generate_moc(WEBCORE_SRC ${OWB_SOURCE_DIR}/WebCore/plugins/qt/PluginContainerQt.h moc_PluginContainerQt.cpp)
    endif(USE_GRAPHICS_QT)

    if(USE_GRAPHICS_SDL)
        list(APPEND WEBCORE_SRC
            plugins/SDL/PluginPackageSDL.cpp
            plugins/SDL/PluginDataSDL.cpp
            plugins/SDL/PluginViewSDL.cpp
        )
    endif(USE_GRAPHICS_SDL)
else(ENABLE_NPAPI)
     list(APPEND WEBCORE_SRC
         plugins/PluginDataNone.cpp
         plugins/PluginPackageNone.cpp
         plugins/PluginViewNone.cpp
     )
endif(ENABLE_NPAPI) 

include(plugins/Customer/CMakeLists.txt OPTIONAL)
