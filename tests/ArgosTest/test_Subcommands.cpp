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
        .add(Command("foo")
            .add(Arg("ARG")))
        .parse(argv.size(), argv.data());
    auto subcommands = args.subcommands();
    REQUIRE(subcommands.size() == 1);
    const auto& command = subcommands[0];
    REQUIRE(command.command_name() == "foo");
    REQUIRE(command.has("ARG"));
    REQUIRE(command.value("ARG").as_string() == "value");
}

TEST_CASE("Check help text for subcommand")
{
    std::stringstream stream;
    using namespace argos;
    Argv argv{{"test", "foo", "--help"}};
    auto parser = ArgumentParser()
        .auto_exit(false)
        .add(Command("foo").about("Does foo things"))
        .stream(&stream)
        .auto_exit(false)
        .parse(argv.size(), argv.data());
    auto help_text = stream.str();
    REQUIRE(help_text.find("test foo") != std::string::npos);
}

TEST_CASE("Command which requires a subcommand")
{
    using namespace argos;
    std::stringstream stream;
    Argv argv{{"test"}};
    auto args = ArgumentParser()
        .auto_exit(false)
        .stream(&stream)
        .add(Command("foo"))
        .parse(argv.size(), argv.data());
    REQUIRE(args.result_code() == ParserResultCode::FAILURE);
}

TEST_CASE("Check multi-command")
{
    using namespace argos;
    Argv argv{{"test", "foo", "bar", "foo"}};
    auto args = ArgumentParser()
        .auto_exit(false)
        .multi_command(true)
        .add(Command("foo"))
        .add(Command("bar"))
        .parse(argv.size(), argv.data());
    auto subcommands = args.subcommands();
    REQUIRE(subcommands.size() == 3);
}

TEST_CASE("Check multi-level multi-command")
{
    using namespace argos;
    Argv argv{{"test", "foo", "zap", "bar"}};
    auto args = ArgumentParser()
        .auto_exit(false)
        .multi_command(true)
        .add(Command("foo")
            .add(Command("zap"))
            .add(Command("zip")))
        .add(Command("bar"))
        .parse(argv.size(), argv.data());
    auto subcommands = args.subcommands();
    REQUIRE(subcommands.size() == 2);
}
