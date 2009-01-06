execute_process(
    COMMAND mkdir -p include
    WORKING_DIRECTORY ${OWB_BINARY_DIR}
    ERROR_QUIET
)
set(API_INCLUDE_DIRS ${OWB_BINARY_DIR}/include)
foreach(files ${API_HEADER})
    get_filename_component(filename ${files} NAME)
    configure_file(
        ${files}
        ${API_INCLUDE_DIRS}/${filename}
        COPYONLY
    )
endforeach(files)

include_directories(
    ${API_INCLUDE_DIRS}
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

if (ENABLE_ORIGYNSUITE)
    target_link_libraries(owb
        origynsuite
    )
endif (ENABLE_ORIGYNSUITE)


if(ENABLE_TESTS)
    add_executable(APITest Tests/APITests.cpp)
    target_link_libraries(APITest
        webkit-owb
        ${GRAPHICS_LIBRARIES}
	${CPPUNIT_LIBRARIES}
    )
    ADD_TEST(APITest ${OWB_BINARY_DIR}/bin/APITest)
endif(ENABLE_TESTS)

# IF (WEBKIT_USE_HTML_EXTENSION)
#     TARGET_LINK_LIBRARIES (owb
#         htmlext
#     )
# ENDIF (WEBKIT_USE_HTML_EXTENSION)
# 
# INSTALL (TARGETS owb 
#     RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
# )
