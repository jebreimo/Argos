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
        .add(Option({"--do"}).argument("STUFF").type(OptionType::EXIT).visibility(Visibility::USAGE))
        .add(Option({"--option"}).visibility(Visibility::USAGE))
        .add(Argument("arg").visibility(Visibility::USAGE))
        .stream(&ss)
        .move();
    parser.writeHelpText();
    REQUIRE(ss.str() == R"-(USAGE
  prog --help
  prog --version
  prog --do <STUFF>
  prog [--option] <arg>
)-");
}

TEST_CASE("No empty line after undocumented arguments and options.")
{
    using namespace Argos;
    std::stringstream ss;
    auto parser = ArgumentParser("prog")
        .add(Option({"--option1"}).text("A text that is too long to fit on a single line. This makes the help text appear on the next line."))
        .add(Option({"--option2"}))
        .add(Argument("arg"))
        .generateHelpOption(false)
        .stream(&ss)
        .move();
    parser.writeHelpText();
    REQUIRE(ss.str() == R"-(USAGE
  prog [--option1] [--option2] <arg>

ARGUMENTS
  <arg>

OPTIONS
  --option1
        A text that is too long to fit on a single line. This makes the help
        text appear on the next line.
  --option2
)-");
}
