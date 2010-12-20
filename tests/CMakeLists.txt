cmake_minimum_required(VERSION 2.8.2)

project(tests)

add_test(UnionTest        "${CMAKE_CURRENT_SOURCE_DIR}/union_test.sh")
add_test(IntersectionTest "${CMAKE_CURRENT_SOURCE_DIR}/intersection_test.sh")
