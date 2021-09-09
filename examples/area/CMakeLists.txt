# ===========================================================================
# Copyright © 2021 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2021-08-05.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.14)
project(area)

set(CMAKE_CXX_STANDARD 17)

set(SINGLE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../single_src)

if (EXISTS ${SINGLE_SRC_DIR}/Argos/Argos.hpp)

    function(add_argos TARGET)
        target_sources(${TARGET}
            PRIVATE
                ${SINGLE_SRC_DIR}/Argos/Argos.cpp
            )

        target_include_directories(${TARGET}
            PRIVATE
                ${SINGLE_SRC_DIR}
            )

        target_compile_definitions(${TARGET}
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
            )
    endfunction()

else()

    include(FetchContent)
    FetchContent_Declare(argos
        GIT_REPOSITORY "https://github.com/jebreimo/Argos.git"
        GIT_TAG "master"
        )
    FetchContent_MakeAvailable(argos)

    function(add_argos TARGET)
        target_link_libraries(${TARGET}
            PRIVATE
                Argos::Argos
            )

        target_compile_definitions(${TARGET}
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
            )
    endfunction()
endif()

add_executable(area area.cpp)
add_argos(area)
