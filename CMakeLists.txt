cmake_minimum_required(VERSION 2.8.2)

project(libsfta)

set(CMAKE_COLOR_MAKEFILE ON)
#set(CMAKE_VERBOSE_MAKEFILE ON)

add_subdirectory(cudd_facade)
add_subdirectory(src)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")


#ADD_EXECUTABLE(cudd_facade_test ../unit_tests/cudd_facade_test.cc)
#ADD_TEST(cudd_facade_test cudd_facade_test)

