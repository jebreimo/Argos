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

add_library(Argos
    include/Argos/Argument.hpp
    include/Argos/Option.hpp
    include/Argos/ArgosException.hpp
    include/Argos/ArgosEnums.hpp
    include/Argos/ArgumentParser.hpp
    include/Argos/ArgumentIterator.hpp
    include/Argos/IArgumentView.hpp
    include/Argos/ParsedArguments.hpp
    include/Argos/ArgumentView.hpp
    include/Argos/OptionView.hpp
    include/Argos/ParsedArgumentsBuilder.hpp
    include/Argos/ArgumentValue.hpp
    include/Argos/ParseValue.hpp
    src/Argos/Argument.cpp
    src/Argos/ParsedArgumentsImpl.cpp
    src/Argos/ParsedArgumentsImpl.hpp
    src/Argos/ArgumentIteratorImpl.cpp
    src/Argos/ArgumentIteratorImpl.hpp
    src/Argos/ArgumentData.hpp
    src/Argos/OptionData.hpp
    src/Argos/StandardOptionIterator.cpp
    src/Argos/StandardOptionIterator.hpp
    src/Argos/Option.cpp
    src/Argos/ParserData.hpp
    src/Argos/StringUtilities.hpp
    src/Argos/StringUtilities.cpp
    src/Argos/VisitorUtilities.hpp
    src/Argos/HelpWriter.cpp
    src/Argos/HelpWriter.hpp
    src/Argos/ArgumentCounter.cpp
    src/Argos/ArgumentCounter.hpp
    src/Argos/ArgumentParser.cpp
    src/Argos/ArgumentIterator.cpp
    src/Argos/ParsedArguments.cpp
    src/Argos/ArgumentView.cpp
    src/Argos/OptionView.cpp
    src/Argos/ParsedArgumentsBuilder.cpp
    src/Argos/ArgumentValue.cpp
    src/Argos/TextFormatter.cpp
    src/Argos/TextFormatter.hpp
    src/Argos/WordSplitter.cpp
    src/Argos/WordSplitter.hpp
    src/Argos/ConsoleWidth.cpp
    src/Argos/ConsoleWidth.hpp
    src/Argos/ParseValue.cpp src/Argos/ArgumentValues.cpp include/Argos/ArgumentValues.hpp src/Argos/OptionIterator.cpp src/Argos/OptionIterator.hpp src/Argos/IOptionIterator.hpp)

target_include_directories(Argos
    PUBLIC
        include
    )

add_subdirectory(tests/ArgosTest)
add_subdirectory(tests/ArgosTestApp)
