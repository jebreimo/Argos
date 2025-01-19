# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-01-07.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.14)

include(FetchContent)
FetchContent_Declare(catch
    GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
    GIT_TAG "v3.4.0"
    )
FetchContent_MakeAvailable(catch)

add_executable(ArgosTest
    Argv.hpp
    U8Adapter.hpp
    test_ArgumentCounter.cpp
    test_ArgumentParser.cpp
    test_ArgumentValue.cpp
    test_Callbacks.cpp
    test_HelpWriter.cpp
    test_ParseValue.cpp
    test_ParsedArguments.cpp
    test_StandardOptionIterator.cpp
    test_StringUtilities.cpp
    test_Subcommands.cpp
    test_TextFormatter.cpp
    test_TextWriter.cpp
    test_WordSplitter.cpp
)

target_link_libraries(ArgosTest
    PRIVATE
        Argos::Argos
        Catch2::Catch2WithMain
    )

target_compile_options(ArgosTest
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    )

target_include_directories(ArgosTest
    PRIVATE
        ../../src
    )

TargetEnableCodeCoverage(ArgosTest "${ARGOS_CODE_COVERAGE}")
TargetEnableAllWarnings(ArgosTest)

add_test(NAME ArgosTest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/ArgosTest)
