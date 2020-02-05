# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-01-07.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.15)

include(FetchContent)
FetchContent_Declare(catch
    GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
    GIT_TAG "v2.11.1"
    )
FetchContent_GetProperties(catch)
if (NOT catch_POPULATED)
    FetchContent_Populate(catch)
    add_subdirectory(${catch_SOURCE_DIR} ${catch_BINARY_DIR})
endif ()

add_executable(ArgosTest
    ArgosTestMain.cpp
    test_ArgumentParser.cpp
    test_StandardOptionIterator.cpp
    test_StringUtilities.cpp
    test_ArgumentCounter.cpp
    test_ParseInteger.cpp
    )

target_link_libraries(ArgosTest
    PRIVATE
        Argos
        Catch2::Catch2
    )

target_include_directories(ArgosTest
    PRIVATE
        ../../src
    )
