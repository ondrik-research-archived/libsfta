cmake_minimum_required(VERSION 2.8.2)

project(tests)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

add_test(UnionTest "${CMAKE_CURRENT_SOURCE_DIR}/union_test.sh")

#set(TESTS "cudd_facade_test" "cudd_shared_mtbdd_cc_test" "cudd_shared_mtbdd_uv_test")
#foreach (TEST ${TESTS})
#  add_executable(${TEST} ${TEST}.cc)
#
#  target_link_libraries(${TEST} libcudd_facade)
#  target_link_libraries(${TEST} libsfta)
#  target_link_libraries(${TEST} tests)
#  target_link_libraries(${TEST} ${LOG4CPP_LIBRARIES})
#  target_link_libraries(${TEST} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
#
#  add_test(${TEST} ${CMAKE_CURRENT_BINARY_DIR}/${TEST})
#endforeach(TEST)
