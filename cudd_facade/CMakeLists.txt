cmake_minimum_required(VERSION 2.8.2)

set(CUDD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cudd)
set(CUDD_LIBS "${CUDD_PATH}/cudd/libcudd.a ${CUDD_PATH}/epd/libepd.a ${CUDD_PATH}/mtr/libmtr.a ${CUDD_PATH}/st/libst.a ${CUDD_PATH}/util/libutil.a ${CUDD_PATH}/dddmp/libdddmp.a")

project(cudd_facade)

add_definitions(-std=c++98)
add_definitions(-pedantic-errors)
add_definitions(-Wextra)
add_definitions(-Wall)
add_definitions(-Wfloat-equal)
add_definitions(-Wctor-dtor-privacy)
add_definitions(-Weffc++)
add_definitions(-Wold-style-cast)        # dropped because some CUDD macros use C-style casting
add_definitions(-Woverloaded-virtual)
add_definitions(-finline-functions)
add_definitions(-fdiagnostics-show-option)
add_definitions(-O2)
add_definitions(-march=core2)
add_definitions(-mtune=core2)
add_definitions(-msse2)
add_definitions(-g)
#add_definitions(-pg)

include_directories(../include)
include_directories(${CUDD_PATH}/include)

add_library(cudd_facade_bare STATIC cudd_facade.cc)

add_custom_command(TARGET cudd_facade_bare POST_BUILD
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/merge_static_libraries.sh libcudd_facade.a "${CUDD_LIBS} libcudd_facade_bare.a")
