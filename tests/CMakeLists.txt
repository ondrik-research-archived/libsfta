cmake_minimum_required(VERSION 2.8.2)

project(tests)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

add_test(UnionTest        "${CMAKE_CURRENT_SOURCE_DIR}/union_test.sh")
add_test(IntersectionTest "${CMAKE_CURRENT_SOURCE_DIR}/intersection_test.sh")
