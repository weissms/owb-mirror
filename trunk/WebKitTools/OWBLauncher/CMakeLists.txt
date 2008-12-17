include_directories(
    ${BASE_INCLUDE_DIRS}
    ${WEBKIT_INCLUDE_DIRS}
    ${EXTERNAL_DEPS_INCLUDE_DIRS}
)
#FIXME
link_directories(${LIBRARY_OUTPUT_PATH})
if(USE_GRAPHICS_GTK)
    add_executable(owb EXCLUDE_FROM_ALL gtk/main.cpp)
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_SDL)
    add_executable(owb EXCLUDE_FROM_ALL SDL/main.cpp)
endif(USE_GRAPHICS_SDL)

include(Customer/CMakeLists.txt OPTIONAL)

target_link_libraries(owb
    webkit-owb
    ${GRAPHICS_LIBRARIES}
)

if (HAS_ADDONS)
    target_link_libraries(owb
        bindingjs
    )
endif (HAS_ADDONS)

# IF (WEBKIT_USE_HTML_EXTENSION)
#     TARGET_LINK_LIBRARIES (owb
#         htmlext
#     )
# ENDIF (WEBKIT_USE_HTML_EXTENSION)
# 
# INSTALL (TARGETS owb 
#     RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
# )
