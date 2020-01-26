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
    src/Argos/Argos.cpp
    src/Argos/Argos.hpp
    src/Argos/ArgumentBuilder.cpp
    src/Argos/ArgumentBuilder.hpp
    src/Argos/ParserResultImpl.cpp
    src/Argos/ParserResultImpl.hpp
    src/Argos/ArgumentIterator.cpp
    src/Argos/ArgumentIterator.hpp
    src/Argos/RawArg.cpp
    src/Argos/RawArg.hpp
    src/Argos/ArgumentData.hpp
    src/Argos/OptionData.hpp
    src/Argos/ArgumentOperation.hpp
    src/Argos/StandardOptionsIterator.cpp
    src/Argos/StandardOptionsIterator.hpp
    src/Argos/OptionBuilder.cpp
    src/Argos/OptionBuilder.hpp
    src/Argos/OptionType.hpp
    src/Argos/ParserBuilder.cpp
    src/Argos/ParserBuilder.hpp
    src/Argos/ParserData.hpp
    src/Argos/ArgosException.hpp
    src/Argos/StringUtilities.hpp
    src/Argos/StringUtilities.cpp
    src/Argos/VisitorUtilities.hpp
    src/Argos/OptionStyle.hpp
    src/Argos/HelpTextWriter.cpp
    src/Argos/HelpTextWriter.hpp
    src/Argos/ArgumentCounter.cpp
    src/Argos/ArgumentCounter.hpp
    )

target_include_directories(Argos
    PRIVATE
        include
    )

add_subdirectory(tests/ArgosTest)
