//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-03-01.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentParser.hpp"
#include <catch2/catch_test_macros.hpp>

#include <sstream>

TEST_CASE("Separate lines for STOP options")
{
    using namespace argos;
    std::stringstream ss;
    auto parser = ArgumentParser("prog")
        .add(Option({"--help"}).type(OptionType::HELP).visibility(Visibility::USAGE))
        .add(Option({"--version"}).type(OptionType::STOP).visibility(Visibility::USAGE))
        .add(Option({"--do"}).argument("STUFF").type(OptionType::EXIT).visibility(Visibility::USAGE))
        .add(Option({"--option"}).visibility(Visibility::USAGE))
        .add(Argument("arg").visibility(Visibility::USAGE))
        .stream(&ss)
        .move();
    parser.write_help_text();
    REQUIRE(ss.str() == R"-(USAGE
  prog --help
  prog --version
  prog --do <STUFF>
  prog [--option] <arg>
)-");
}

TEST_CASE("No empty line after undocumented arguments and options.")
{
    using namespace argos;
    std::stringstream ss;
    auto parser = ArgumentParser("prog")
        .add(Option({"--option1"}).help(
            "A text that is too long to fit on a single line. This makes the help text appear on the next line."))
        .add(Option({"--option2"}))
        .add(Argument("arg"))
        .generate_help_option(false)
        .stream(&ss)
        .move();
    parser.write_help_text();
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

TEST_CASE("Callbacks for arguments and options.")
{
    using namespace argos;
    std::stringstream ss;
    auto parser = ArgumentParser("prog")
        .add(Opt({"--opt"}).help([](){return "Option";}))
        .add(Arg("arg").help([](){return "Argument";}))
        .generate_help_option(false)
        .stream(&ss)
        .move();
    parser.write_help_text();
    REQUIRE(ss.str() == R"-(USAGE
  prog [--opt] <arg>

ARGUMENTS
  <arg> Argument

OPTIONS
  --opt Option
)-");
}

TEST_CASE("Include ABOUT text.")
{
    using namespace argos;
    std::stringstream ss;
    auto parser = ArgumentParser("prog")
        .about("Does stuff.")
        .add(Arg("arg").help([](){return "Argument";}))
        .generate_help_option(false)
        .stream(&ss)
        .move();
    parser.write_help_text();
    REQUIRE(ss.str() == R"-(USAGE
  prog <arg>

Does stuff.

ARGUMENTS
  <arg> Argument
)-");
}

TEST_CASE("HELP is used if sub-commands lack ABOUT.")
{
    using namespace argos;
    std::stringstream ss;
    auto parser = ArgumentParser("prog")
        .add(Command("foo").help("Does foo things."))
        .add(Command("bar"))
        .stream(&ss)
        .move();

    SECTION("top-level help text")
    {
        parser.write_help_text();
        REQUIRE(ss.str() == R"-(USAGE
  prog --help
  prog foo|bar

COMMANDS
  foo        Does foo things.
  bar

OPTIONS
  -h, --help Display this help text.
)-");
    }
    SECTION("sub-command help text")
    {
        parser.write_subcommand_help_text({"foo"});
        REQUIRE(ss.str() == R"-(USAGE
  prog foo --help
  prog foo

Does foo things.

OPTIONS
  -h, --help Display this help text.
)-");
    }
}
