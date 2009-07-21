##################################################
# SunSpider target.                              #
##################################################

#FIXME: find a correct way to compare results
#Indeed comparison is done between results stored in tmp/baseline-filename.txt
#and current results.
#The problem is that you do not get a comparion result if you run sunspider
#and then v8 as it will try to compare unmatching tests.
#A solution may be to move results in a specific sunspider or v8 directory
#and move results in tmp when you try to compare. Another solution is to hack
#sunspider-compare-results to fit our requirements but it involves some
#maintenance.

add_custom_target(run-sunspider
    COMMAND ./sunspider --shell ${OWB_BINARY_DIR}/bin/jshell
    #COMMAND ./sunspider-compare-results --shell ${OWB_BINARY_DIR}/bin/jshell
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_dependencies(run-sunspider jshell)

add_custom_target(run-v8
    COMMAND ./sunspider --shell ${OWB_BINARY_DIR}/bin/jshell --v8-suite
    #COMMAND ./sunspider-compare-results --shell ${OWB_BINARY_DIR}/bin/jshell
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_dependencies(run-v8 jshell)
