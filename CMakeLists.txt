cmake_minimum_required(VERSION 2.8.2)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_COLOR_MAKEFILE ON)
#set(CMAKE_VERBOSE_MAKEFILE ON)


project(libsfta)

# find doxygen
find_package(Doxygen)
if (DOXYGEN_FOUND STREQUAL "NO")
    message(FATAL_ERROR "Doxygen not found.")
endif (DOXYGEN_FOUND STREQUAL "NO")

# Include CTest so that sophisticated testing can be done now
include(CTest)

add_subdirectory(cudd_facade/cudd)
add_subdirectory(cudd_facade)
add_subdirectory(src)
add_subdirectory(reflib)
add_subdirectory(unit_tests)
add_subdirectory(tests)


# prepare doxygen configuration file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# add doxygen as target
add_custom_target(doxygen ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# cleanup $build/doc on "make clean"
set_property(DIRECTORY APPEND PROPERTY
       ADDITIONAL_MAKE_CLEAN_FILES doc)

# add doxygen as dependency to doc-target
get_target_property(DOC_TARGET doc TYPE)
if(NOT DOC_TARGET)
  add_custom_target(doc)
endif()
add_dependencies(doc doxygen)

# install HTML API documentation and manual pages
set(DOC_PATH "share/doc/${CPACK_PACKAGE_NAME}-${VERSION}")

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/html
         DESTINATION ${DOC_PATH}
       )

# install man pages into packages, scope is now project root..
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/man/man3
         DESTINATION share/man/man3/
       )
