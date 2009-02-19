include_directories(
    ${BASE_INCLUDE_DIRS}
    ${JAVASCRIPTCORE_INCLUDE_DIRS}
    ${WEBKIT_INCLUDE_DIRS}
    ${EXTERNAL_DEPS_INCLUDE_DIRS}
    ${OWB_SOURCE_DIR}/WebKitTools/DumpRenderTree/
    ${OWB_SOURCE_DIR}/WebKitTools/DumpRenderTree/bal
)


list(APPEND DUMPRENDERTREE_SRC
    LayoutTestController.cpp
    GCController.cpp
    WorkQueue.cpp
#    AccessibilityController.cpp
#    AccessibilityUIElement.cpp
    bal/DumpRenderTree.cpp
    bal/LayoutTestControllerBal.cpp    
    bal/WorkQueueItemBal.cpp
    bal/GCControllerBal.cpp
)

if(USE_GRAPHICS_GTK)
    list(APPEND DUMPRENDERTREE_SRC
        bal/gtk/DumpRenderTreeGtk.cpp
    )
elseif(USE_GRAPHICS_SDL)
    list(APPEND DUMPRENDERTREE_SRC
        bal/SDL/DumpRenderTreeSDL.cpp
    )
endif(USE_GRAPHICS_GTK)

INCLUDE (bal/Customer/CMakeLists.txt OPTIONAL)

add_executable(DumpRenderTree ${DUMPRENDERTREE_SRC})
target_link_libraries(DumpRenderTree webkit-owb ${GRAPHICS_LIBRARIES} ${EXTERNAL_DEPS_LIBRARIES})

INCLUDE (bal/Customer/CMakeLinks.txt OPTIONAL)
