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
    bal/DumpRenderTree.cpp
    bal/LayoutTestControllerBal.cpp    
    bal/WorkQueueItemBal.cpp
    bal/GCControllerBal.cpp
)

if(USE_GRAPHICS_GTK)
    list(APPEND DUMPRENDERTREE_SRC
        bal/gtk/DumpRenderTreeGtk.cpp
    )
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_QT)
    list(APPEND DUMPRENDERTREE_SRC
        bal/qt/DumpRenderTreeQt.cpp
    )
endif(USE_GRAPHICS_QT)

if(USE_GRAPHICS_SDL)
    list(APPEND DUMPRENDERTREE_SRC
        bal/SDL/DumpRenderTreeSDL.cpp
    )
endif(USE_GRAPHICS_SDL)

set_source_files_properties(${DUMPRENDERTREE_SRC}
    PROPERTIES COMPILE_FLAGS "-fno-rtti -fno-exceptions"
)
add_executable(DumpRenderTree ${DUMPRENDERTREE_SRC})
target_link_libraries(DumpRenderTree webkit-owb ${GRAPHICS_LIBRARIES} ${EXTERNAL_DEPS_LIBRARIES})

