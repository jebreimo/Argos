# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-02-04.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.14)
project(ArgosTestApp)

set(CMAKE_CXX_STANDARD 17)

add_executable(ArgosTestApp
    ArgosTestAppMain.cpp
    )

set(SINGLE_SRC_DIR ../../../single_src)

if (EXISTS ${SINGLE_SRC_DIR}/Argos/Argos.hpp)
    target_sources(ArgosTestApp
        PRIVATE
            ${SINGLE_SRC_DIR}/Argos/Argos.cpp)
    target_include_directories(ArgosTestApp
        PRIVATE
            ${SINGLE_SRC_DIR})
endif()

#target_link_libraries(ArgosTestApp
#    PRIVATE
#        Argos::Argos
#    )
