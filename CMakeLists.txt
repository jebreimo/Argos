# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-01-07.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.15)
project(Argos)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|.*Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

add_library(Argos
    include/Argos/ArgosException.hpp
    include/Argos/Argument.hpp
    include/Argos/ArgumentIterator.hpp
    include/Argos/ArgumentParser.hpp
    include/Argos/ArgumentValue.hpp
    include/Argos/ArgumentValues.hpp
    include/Argos/ArgumentView.hpp
    include/Argos/Callbacks.hpp
    include/Argos/Enums.hpp
    include/Argos/IArgumentView.hpp
    include/Argos/Option.hpp
    include/Argos/OptionView.hpp
    include/Argos/ParsedArguments.hpp
    include/Argos/ParsedArgumentsBuilder.hpp
    src/Argos/ArgosThrow.hpp
    src/Argos/Argument.cpp
    src/Argos/ArgumentCounter.cpp
    src/Argos/ArgumentCounter.hpp
    src/Argos/ArgumentData.hpp
    src/Argos/ArgumentIterator.cpp
    src/Argos/ArgumentIteratorImpl.cpp
    src/Argos/ArgumentIteratorImpl.hpp
    src/Argos/ArgumentParser.cpp
    src/Argos/ArgumentValue.cpp
    src/Argos/ArgumentValues.cpp
    src/Argos/ArgumentView.cpp
    src/Argos/ConsoleWidth.cpp
    src/Argos/ConsoleWidth.hpp
    src/Argos/HelpText.cpp
    src/Argos/HelpText.hpp
    src/Argos/IOptionIterator.hpp
    src/Argos/Option.cpp
    src/Argos/OptionData.hpp
    src/Argos/OptionIterator.cpp
    src/Argos/OptionIterator.hpp
    src/Argos/OptionView.cpp
    src/Argos/ParseValue.cpp
    src/Argos/ParseValue.hpp
    src/Argos/ParsedArguments.cpp
    src/Argos/ParsedArgumentsImpl.cpp
    src/Argos/ParsedArgumentsImpl.hpp
    src/Argos/ParsedArgumentsBuilder.cpp
    src/Argos/ParserData.hpp
    src/Argos/StandardOptionIterator.cpp
    src/Argos/StandardOptionIterator.hpp
    src/Argos/StringUtilities.cpp
    src/Argos/StringUtilities.hpp
    src/Argos/TextFormatter.cpp
    src/Argos/TextFormatter.hpp
    src/Argos/TextWriter.cpp
    src/Argos/TextWriter.hpp
    src/Argos/WordSplitter.cpp
    src/Argos/WordSplitter.hpp
    )

target_include_directories(Argos
    PUBLIC
        include
    )

target_compile_definitions(Argos
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>
    )

enable_testing()

add_subdirectory(tests/ArgosTest)
add_subdirectory(tests/ArgosTestApp)
