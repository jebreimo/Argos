//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/Argos.hpp"
#include <sstream>
#include <catch2/catch_test_macros.hpp>
#include "Argv.hpp"

TEST_CASE("Check that help text contains information about commands")
{
    std::stringstream stream;
    using namespace argos;
    auto parser = ArgumentParser("Test program")
        .auto_exit(false)
        .add(Command("foo").about("Does foo things"))
        .add(Command("bar").about("Does bar things"))
        .stream(&stream)
        .move();
    parser.write_help_text();
    auto help_text = stream.str();
    REQUIRE(help_text.find("foo|bar") != std::string::npos);
    REQUIRE(help_text.find("COMMANDS") != std::string::npos);
    REQUIRE(help_text.find("Does foo things") != std::string::npos);
    REQUIRE(help_text.find("Does bar things") != std::string::npos);
}

TEST_CASE("One subcommand with one argument")
{
    using namespace argos;
    Argv argv{{"test", "foo", "value"}};
    const auto args = ArgumentParser()
        .auto_exit(false)
        .add(Command("foo").about("Does foo things")
            .add(Arg("ARG").help("An argument")))
        .parse(argv.size(), argv.data());
    REQUIRE(args.commands().size() == 1);
}
