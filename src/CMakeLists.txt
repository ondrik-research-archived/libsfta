cmake_minimum_required(VERSION 2.8.2)

set(CMAKE_COLOR_MAKEFILE ON)
#set(CMAKE_VERBOSE_MAKEFILE ON)

project(sfta)

add_definitions(-std=c++98)
add_definitions(-pedantic-errors)
add_definitions(-Wextra)
add_definitions(-Wall)
add_definitions(-Wfloat-equal)
add_definitions(-Wctor-dtor-privacy)
add_definitions(-Weffc++)
add_definitions(-Wold-style-cast)
add_definitions(-Woverloaded-virtual)
add_definitions(-finline-functions)
add_definitions(-fdiagnostics-show-option)
add_definitions(-O2)
add_definitions(-march=core2)
add_definitions(-mtune=core2)
add_definitions(-msse2)
add_definitions(-g)

# uncomment for profiling
#add_definitions(-pg)
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")

find_package(Log4CPP REQUIRED)
find_package(Loki REQUIRED)

include_directories(../include)

add_library(libsfta STATIC
  convert.cc
  fake_file.cc
  formula_parser.cc
  td_tree_automaton_cover.cc)
set_target_properties(libsfta PROPERTIES
   OUTPUT_NAME sfta
   CLEAN_DIRECT_OUTPUT 1
)

add_executable(sfta sfta.cc)

add_library(libcudd_facade STATIC IMPORTED)
set_property(TARGET libcudd_facade PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/cudd_facade/libcudd_facade.a)

target_link_libraries(sfta libcudd_facade)
target_link_libraries(sfta libsfta)
target_link_libraries(sfta ${LOG4CPP_LIBRARIES})
target_link_libraries(sfta ${LOKI_LIBRARY})
target_link_libraries(sfta rt)
