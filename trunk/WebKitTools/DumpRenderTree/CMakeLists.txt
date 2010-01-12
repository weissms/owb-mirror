include_directories(
    ${BASE_INCLUDE_DIRS}
    ${JAVASCRIPTCORE_INCLUDE_DIRS}
    ${WEBKIT_INCLUDE_DIRS}
    ${EXTERNAL_DEPS_INCLUDE_DIRS}
    ${OWB_SOURCE_DIR}/WebCore
    ${OWB_SOURCE_DIR}/WebKitTools/DumpRenderTree/
    ${OWB_SOURCE_DIR}/WebKitTools/DumpRenderTree/bal
    ${OWB_SOURCE_DIR}/WebKitTools/DumpRenderTree/unix/TestNetscapePlugin/ForwardingHeaders
    ${OWB_SOURCE_DIR}/WebKitTools/DumpRenderTree/unix/TestNetscapePlugin/ForwardingHeaders/WebKit
    ${OWB_SOURCE_DIR}/WebKitTools/DumpRenderTree/TestNetscapePlugIn.subproj
)
link_directories(
    ${LIBRARY_OUTPUT_PATH}
    ${GRAPHICS_LIBRARY_DIRS}
)


include(AddDAECompilerFlags OPTIONAL)
include(AddWebCoreCompilerFlags)

list(APPEND DUMPRENDERTREE_SRC
    AccessibilityController.cpp
    AccessibilityUIElement.cpp
    LayoutTestController.cpp
    GCController.cpp
    WorkQueue.cpp
    bal/AccessibilityControllerBal.cpp
    bal/AccessibilityUIElementBal.cpp
    bal/DumpRenderTree.cpp
    bal/EditingDelegate.cpp
    bal/EventSender.cpp
    bal/GCControllerBal.cpp
    bal/LayoutTestControllerBal.cpp    
    bal/PolicyDelegate.cpp
    bal/ResourceLoadDelegate.cpp
    bal/WorkQueueItemBal.cpp
)

if(ENABLE_DAE)
    list(APPEND DUMPRENDERTREE_SRC
        bal/ApplicationTestController.cpp
    )
endif(ENABLE_DAE)


if(USE_GRAPHICS_GTK)
    list(APPEND DUMPRENDERTREE_SRC
        bal/gtk/DumpRenderTreeGtk.cpp
        bal/gtk/EventSenderGtk.cpp
    )
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_QT)
    list(APPEND DUMPRENDERTREE_SRC
        bal/qt/DumpRenderTreeQt.cpp
        bal/qt/EventSenderQt.cpp
    )
    owb_generate_moc(DUMPRENDERTREE_SRC ${OWB_SOURCE_DIR}/WebKitTools/DumpRenderTree/bal/qt/DumpRenderTreeQt.cpp DumpRenderTreeQt.moc)
endif(USE_GRAPHICS_QT)

if(USE_API_SDL)
    list(APPEND DUMPRENDERTREE_SRC
        bal/SDL/DumpRenderTreeSDL.cpp
        bal/SDL/EventSenderSDL.cpp
    )
endif(USE_API_SDL)

include(${OWB_SOURCE_DIR}/WebKitTools/DumpRenderTree/bal/Customer/CMakeLists.txt OPTIONAL)

set_source_files_properties(${DUMPRENDERTREE_SRC}
    PROPERTIES COMPILE_FLAGS "-fno-rtti -fno-exceptions"
)
add_executable(DumpRenderTree ${DUMPRENDERTREE_SRC})
target_link_libraries(DumpRenderTree webkit-owb ${GRAPHICS_LIBRARIES} ${EXTERNAL_DEPS_LIBRARIES})

if(ENABLE_NPAPI)
    add_definitions(-DTEST_PLUGIN_DIR=\"${CMAKE_BINARY_DIR}/lib\")

    list(APPEND TESTPLUGIN_SRC
        unix/TestNetscapePlugin/TestNetscapePlugin.cpp
        TestNetscapePlugIn.subproj/PluginObject.cpp
        TestNetscapePlugIn.subproj/TestObject.cpp
    )

    add_library(testnetscapeplugin SHARED ${TESTPLUGIN_SRC})
endif(ENABLE_NPAPI)
