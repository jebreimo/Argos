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
    src/Argos/Argument.cpp
    include/Argos/Argument.hpp
    src/Argos/ParsedArgumentsImpl.cpp
    src/Argos/ParsedArgumentsImpl.hpp
    src/Argos/ArgumentIteratorImpl.cpp
    src/Argos/ArgumentIteratorImpl.hpp
    src/Argos/RawArg.cpp
    src/Argos/RawArg.hpp
    src/Argos/ArgumentData.hpp
    src/Argos/OptionData.hpp
    src/Argos/StandardOptionIterator.cpp
    src/Argos/StandardOptionIterator.hpp
    src/Argos/Option.cpp
    include/Argos/Option.hpp
    src/Argos/ParserData.hpp
    include/Argos/ArgosException.hpp
    src/Argos/StringUtilities.hpp
    src/Argos/StringUtilities.cpp
    src/Argos/VisitorUtilities.hpp
    include/Argos/ArgosEnums.hpp
    src/Argos/HelpWriter.cpp
    src/Argos/HelpWriter.hpp
    src/Argos/ArgumentCounter.cpp
    src/Argos/ArgumentCounter.hpp
    src/Argos/ArgumentParser.cpp
    include/Argos/ArgumentParser.hpp
    src/Argos/ArgumentIterator.cpp
    include/Argos/ArgumentIterator.hpp include/Argos/IArgumentView.hpp src/Argos/ParsedArguments.cpp include/Argos/ParsedArguments.hpp src/Argos/ArgumentView.cpp include/Argos/ArgumentView.hpp src/Argos/OptionView.cpp include/Argos/OptionView.hpp src/Argos/ParsedArgumentsBuilder.cpp include/Argos/ParsedArgumentsBuilder.hpp)

target_include_directories(Argos
    PUBLIC
        include
    )

add_subdirectory(tests/ArgosTest)
