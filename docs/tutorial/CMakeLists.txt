# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-04-26.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.13)
project(Tutorial)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(argos
    GIT_REPOSITORY "https://github.com/jebreimo/Argos.git"
    )
FetchContent_GetProperties(argos)
if (NOT argos_POPULATED)
    FetchContent_Populate(argos)
    add_subdirectory(${argos_SOURCE_DIR} ${argos_BINARY_DIR})
endif ()

add_executable(tutorial
    Tutorial.cpp
    )

target_link_libraries(tutorial
    PRIVATE
        Argos::Argos
    )
