# IF (WEBKIT_USE_HTML_EXTENSION)
#     TARGET_LINK_LIBRARIES (jsc
#         htmlext
#     )
# ENDIF (WEBKIT_USE_HTML_EXTENSION)

##################################################
# Generate sources for JavaScriptCore library.   #
##################################################

add_custom_command(
    OUTPUT ${OWB_BINARY_DIR}/generated_sources/JavaScriptCore/Grammar.cpp ${OWB_BINARY_DIR}/generated_sources/JavaScriptCore/Grammar.h
    WORKING_DIRECTORY ${OWB_BINARY_DIR}
    COMMAND mkdir -p ${OWB_BINARY_DIR}/generated_sources/JavaScriptCore
    COMMAND ${BISON_EXECUTABLE} -d -p jscyy ${CMAKE_CURRENT_SOURCE_DIR}/parser/Grammar.y -o generated_sources/JavaScriptCore/Grammar.cpp 
    COMMAND cat generated_sources/JavaScriptCore/Grammar.hpp > generated_sources/JavaScriptCore/Grammar.h
    COMMAND rm -f generated_sources/JavaScriptCore/Grammar.hpp bison_out.txt
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parser/Grammar.y
)

if(NOT EXISTS ${OWB_BINARY_DIR}/generated_sources/JavaScriptCore/chartables.c)
    exec_program(mkdir
        ARGS -p ${OWB_BINARY_DIR}/generated_sources/JavaScriptCore
    )
    exec_program(${CMAKE_CURRENT_SOURCE_DIR}/pcre/dftables
        ARGS ${OWB_BINARY_DIR}/generated_sources/JavaScriptCore/chartables.c
    )
endif(NOT EXISTS ${OWB_BINARY_DIR}/generated_sources/JavaScriptCore/chartables.c)


##################################################
# Add sources for JavaScriptore library.         #
##################################################

list(APPEND JSC_SRC
    ${OWB_BINARY_DIR}/generated_sources/JavaScriptCore/Grammar.cpp
)

include(API/CMakeLists.txt)
include(bytecode/CMakeLists.txt)
include(bytecompiler/CMakeLists.txt)
include(debugger/CMakeLists.txt)
include(interpreter/CMakeLists.txt)
include(jit/CMakeLists.txt)
include(parser/CMakeLists.txt)
include(pcre/CMakeLists.txt)
include(profiler/CMakeLists.txt)
include(runtime/CMakeLists.txt)
include(wrec/CMakeLists.txt)


##################################################
# Compilation settings.                          #
##################################################

include_directories(
    ${BASE_INCLUDE_DIRS}
    ${WTF_INCLUDE_DIRS}
    ${BAL_INCLUDE_DIRS}
    ${JAVASCRIPTCORE_INCLUDE_DIRS}
    ${OWB_BINARY_DIR}/generated_sources/JavaScriptCore
    ${I18N_INCLUDE_DIRS}
    ${THREADS_INCLUDE_DIRS}
)

if(NOT ENABLE_DEBUG)
    include_directories(
        ${OWB_SOURCE_DIR}/BAL/Memory/WTF
        ${OWB_SOURCE_DIR}/BAL/Types/WTF
    )
    set_source_files_properties(${OWB_BINARY_DIR}/generated_sources/JavaScriptCore/Grammar.cpp
        PROPERTIES HEADER_FILE_ONLY TRUE)
endif(NOT ENABLE_DEBUG)

include(AddJavaScriptCoreCompilerFlags)

set_source_files_properties(${JSC_SRC}
    PROPERTIES COMPILE_FLAGS "-fno-rtti -fno-exceptions"
)
add_library(jsc STATIC ${JSC_SRC})
add_dependencies(jsc wtf)


##################################################
# Tests compilation.                             #
##################################################

link_directories(${THREADS_LIBRARY_DIRS})
add_executable(jshell EXCLUDE_FROM_ALL jsc.cpp)
set_target_properties(jshell
    PROPERTIES COMPILE_FLAGS "-fno-rtti -fno-exceptions"
)
target_link_libraries(jshell
    ${KJS_LD_FLAGS}
    ${I18N_LIBRARIES}
    ${THREADS_LIBRARIES}
    jsc
)
add_custom_target(jsctests
    COMMAND ${PERL_EXECUTABLE} mklistpage.pl > menubody.html
    COMMAND cat menuhead.html menubody.html menufoot.html > menu.html
    COMMAND ${PERL_EXECUTABLE} jsDriver.pl -e squirrelfish -s ${OWB_BINARY_DIR}/bin/jshell -f results.html
    COMMAND ${PERL_EXECUTABLE} report.pl
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/mozilla
)
add_dependencies(jsctests jshell)
