//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-03-01.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentParser.hpp"
#include <catch2/catch.hpp>

#include <sstream>

TEST_CASE("Separate lines for STOP options")
{
    using namespace Argos;
    std::stringstream ss;
    auto parser = ArgumentParser("prog")
            .add(Option({"--help"}).type(OptionType::HELP).visibility(Visibility::USAGE))
            .add(Option({"--version"}).type(OptionType::STOP).visibility(Visibility::USAGE))
            .add(Option({"--option"}).visibility(Visibility::USAGE))
            .add(Argument("arg").visibility(Visibility::USAGE))
            .outputStream(&ss)
            .move();
    parser.writeHelpText();
    REQUIRE(ss.str() == R"-(USAGE
  prog --help
  prog --version
  prog [--option] <arg>
)-");
}
