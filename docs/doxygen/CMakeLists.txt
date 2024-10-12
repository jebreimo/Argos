# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-03-15.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.13)

find_package(Doxygen)

if(Doxygen_NOT_FOUND)
    return()
endif()

# Find all the public headers
file(GLOB_RECURSE ARGOS_DOXYGEN_INPUT "${CMAKE_SOURCE_DIR}/include/Argos/*.hpp")

list(APPEND ARGOS_DOXYGEN_INPUT "${CMAKE_CURRENT_BINARY_DIR}/../../ArgosVersion.hpp")

# Use README.md as the main page
set(ARGOS_MARKDOWN_MAIN_PAGE "${CMAKE_SOURCE_DIR}/README.md")
list(INSERT ARGOS_DOXYGEN_INPUT 0 "${ARGOS_MARKDOWN_MAIN_PAGE}")

string(REPLACE ";" " \\\n    " DOXYGEN_INPUT "${ARGOS_DOXYGEN_INPUT}")
set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

set(DOXYGEN_INDEX_FILE "${DOXYGEN_OUTPUT_DIR}/xml/index.xml")

# Only regenerate Doxygen when the Doxyfile or public headers change
add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
    DEPENDS ${ARGOS_DOXYGEN_INPUT}
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
    MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
    COMMENT "Generating documentation with Doxygen")

# Nice named target so we can run the job easily
add_custom_target(ArgosDoxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})
