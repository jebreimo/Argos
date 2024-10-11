//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-13.
//
// This file is distributed under the Zero-Clause BSD License.
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
    std::stringstream stream;
    auto parser = ArgumentParser()
        .auto_exit(false)
        .multi_command(true)
        .stream(&stream)
        .add(Command("foo")
            .add(Command("zap")))
        .add(Command("bar"))
        .move();
    SECTION("Continue with subcommand of grandparent")
    {
        Argv argv{"test", "foo", "zap", "bar"};
        auto args = parser.parse(argv.size(), argv.data());
        auto subcommands = args.subcommands();
        REQUIRE(subcommands.size() == 2);
    }
    SECTION("Missing subcommand for child")
    {
        Argv argv{"test", "foo"};
        auto args = parser.parse(argv.size(), argv.data());
        REQUIRE(args.result_code() == ParserResultCode::FAILURE);
    }
}

TEST_CASE("Check that error text refers to the correct command")
{
    using namespace argos;
    std::stringstream stream;
    auto parser = ArgumentParser()
        .auto_exit(false)
        .stream(&stream)
        .add(Command("foo")
            .add(Arg("ARG")))
        .move();
    Argv argv{"test", "foo"};
    auto args = parser.parse(argv.size(), argv.data());
    REQUIRE(args.result_code() == ParserResultCode::FAILURE);
    auto error_text = stream.str();
    REQUIRE(error_text.find("test foo") == 0);
}

TEST_CASE("Check that copy_from copies options and arguments")
{
    using namespace argos;
    auto command = Command()
        .add(Arg("FILE"))
        .add(Opt("-c", "--compress").argument("ALGO"));
    auto parser = ArgumentParser()
        .auto_exit(false)
        .add(Command("add").copy_from(command))
        .add(Command("update").copy_from(command))
        .move();
    SECTION("Test add command")
    {
        Argv argv{"test", "add", "file.txt", "--compress", "gzip"};
        auto args = parser.parse(argv.size(), argv.data());
        REQUIRE(args.subcommands().size() == 1);
        args = args.subcommands()[0];
        REQUIRE(args.command_name() == "add");
        REQUIRE(args.has("FILE"));
        REQUIRE(args.value("FILE").as_string() == "file.txt");
        REQUIRE(args.has("-c"));
        REQUIRE(args.value("-c").as_string() == "gzip");
    }
    SECTION("Test update command")
    {
        Argv argv{"test", "update", "file1.txt", "--compress", "lzh"};
        auto args = parser.parse(argv.size(), argv.data());
        REQUIRE(args.subcommands().size() == 1);
        args = args.subcommands()[0];
        REQUIRE(args.command_name() == "update");
        REQUIRE(args.has("FILE"));
        REQUIRE(args.value("FILE").as_string() == "file1.txt");
        REQUIRE(args.has("-c"));
        REQUIRE(args.value("-c").as_string() == "lzh");
    }
}

TEST_CASE("It's illegal to add unnamed commands")
{
    using namespace argos;
    REQUIRE_THROWS_AS(ArgumentParser().add(Command()), ArgosException);
    Command cmd("a");
    REQUIRE_THROWS_AS(cmd.add(Command()), ArgosException);
}

TEST_CASE("Final argument option and multi-commands")
{
    using namespace argos;
    auto common = Command()
        .add(Arg("NAME").count(0, 1000))
        .add(Opt("--").type(OptionType::LAST_ARGUMENT));
    auto parser = ArgumentParser()
        .auto_exit(false)
        .multi_command(true)
        .add(Command("foo")
            .copy_from(common))
        .add(Command("bar")
            .copy_from(common)
            .add(Opt("--baz").argument("S")))
        .move();
    auto args = parser.parse({"foo", "name1", "--", "bar", "name2", "--baz=qux"});
    auto commands = args.subcommands();
    REQUIRE(commands.size() == 2);
    auto cmd = commands[0];
    REQUIRE(cmd.command_name() == "foo");
    REQUIRE(cmd.values("NAME").size() == 1);
    REQUIRE(cmd.values("NAME")[0].as_string() == "name1");
    cmd = commands[1];
    REQUIRE(cmd.command_name() == "bar");
    REQUIRE(cmd.values("NAME").size() == 1);
    REQUIRE(cmd.values("NAME")[0].as_string() == "name2");
    REQUIRE(cmd.value("--baz").as_string() == "qux");
}
