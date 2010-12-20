cmake_minimum_required(VERSION 2.8.2)

set(CMAKE_COLOR_MAKEFILE ON)
#set(CMAKE_VERBOSE_MAKEFILE ON)

project(reflib)

add_custom_target(reflib ALL "make" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
