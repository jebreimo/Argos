# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-03-15.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.13)

#
# Contents are based on https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/
#

find_package(Doxygen)

if(Doxygen_NOT_FOUND)
    return()
endif()

# Find all the public headers
get_target_property(ARGOS_PUBLIC_HEADER_DIR Argos INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE ARGOS_PUBLIC_HEADERS "${ARGOS_PUBLIC_HEADER_DIR}/*.hpp")

set(DOXYGEN_START_PAGE "${CMAKE_SOURCE_DIR}/README.md")
set(DOXYGEN_INPUT_DIRS "${CMAKE_SOURCE_DIR}/include/Argos ${CMAKE_CURRENT_BINARY_DIR}/../../include/Argos")
set(DOXYGEN_INPUT "${DOXYGEN_START_PAGE} ${DOXYGEN_INPUT_DIRS}")
set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doxygen")
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

# Doxygen won't create this for us
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

set(DOXYGEN_INDEX_FILE "${DOXYGEN_OUTPUT_DIR}/xml/index.xml")

# Only regenerate Doxygen when the Doxyfile or public headers change
add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
    DEPENDS ${ARGOS_PUBLIC_HEADERS}
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
    MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
    COMMENT "Generating documentation with Doxygen")

# Nice named target so we can run the job easily
add_custom_target(ArgosDoxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})
