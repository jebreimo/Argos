# ===========================================================================
# Copyright © 2021 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2021-08-05.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.14)
project(whereis)

set(CMAKE_CXX_STANDARD 17)

add_executable(whereis
    whereis.cpp
    )

set(SINGLE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../single_src)

if (EXISTS ${SINGLE_SRC_DIR}/Argos/Argos.hpp)

    target_sources(whereis
        PRIVATE
            ${SINGLE_SRC_DIR}/Argos/Argos.cpp)
    target_include_directories(whereis
        PRIVATE
            ${SINGLE_SRC_DIR})

else()

    include(FetchContent)
    FetchContent_Declare(argos
        GIT_REPOSITORY "https://github.com/jebreimo/Argos.git"
        GIT_TAG "master"
        )
    FetchContent_MakeAvailable(argos)

    target_link_libraries(whereis
        PRIVATE
            Argos::Argos
        )

endif()
