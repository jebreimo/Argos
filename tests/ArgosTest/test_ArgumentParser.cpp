//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <catch2/catch.hpp>
#include "Argos/ArgumentParser.hpp"

#include <sstream>
#include <string>
#include <vector>

struct Argv
{
    Argv(std::initializer_list<std::string> args) : strings(args)
    {
        for (auto& arg : strings)
            argv.push_back(arg.data());
    }

    size_t size() const
    {
        return argv.size();
    }

    char** data()
    {
        return argv.data();
    }

    std::vector<std::string> strings;
    std::vector<char*> argv;
};

TEST_CASE("Test help flag")
{
    Argos::ArgumentParser argos("test");
    std::stringstream ss;
    argos.autoExitEnabled(false).outputStream(&ss);
    REQUIRE(argos.programName() == "test");

    argos.add(Argos::Option({"-h", "--help"})
            .type(Argos::OptionType::HELP)
            .text("Show help message.")
            .id(10));
    Argv argv{"test", "--help"};
    auto result = argos.parse(argv.size(), argv.data());
    REQUIRE(result.has("--help"));
    REQUIRE(result.resultCode() == Argos::ParserResultCode::SPECIAL_OPTION);
    REQUIRE(result.breakingOption().id() == 10);
    REQUIRE(result.value("--help").boolValue());
}

TEST_CASE("Conflicting flags")
{
    Argos::ArgumentParser argos("test");
    argos.add(Argos::Option({"-h", "--help"})
                      .type(Argos::OptionType::HELP)
                      .text("Show help message."));
    argos.add(Argos::Option({"-h"})
                      .text("Output height."));
    Argv argv{"test", "--help"};
    REQUIRE_THROWS(argos.parse(argv.size(), argv.data()));
}

TEST_CASE("String arguments")
{
    using namespace Argos;
    Argv argv{"test", "test_file.txt"};
    auto args = Argos::ArgumentParser("test")
            .autoExitEnabled(false)
            .add(Argument("file"))
            .parse(argv.size(), argv.data());
    REQUIRE(args.value("file").stringValue() == "test_file.txt");
}

TEST_CASE("Section order in help text")
{
    using namespace Argos;
    Argv argv{"test", "-h"};
    std::stringstream ss;
    auto args = Argos::ArgumentParser("test")
            .autoExitEnabled(false)
            .add(Argument("file").section("Z"))
            .add(Option({"-h"}).section("A").type(OptionType::HELP))
            .add(Option({"-s"}).section("Z"))
            .add(Argument("device").section("A"))
            .outputStream(&ss)
            .text(TextId::USAGE, "")
            .parse(argv.size(), argv.data());
    REQUIRE(ss.str() == "Z\n  <file>\n  -s\nA\n  <device>\n  -h\n");
}

TEST_CASE("Two argument")
{
    using namespace Argos;
    Argv argv{"test", "foo", "bar"};
    auto args = Argos::ArgumentParser("test")
            .autoExitEnabled(false)
            .add(Argument("arg1"))
            .add(Argument("arg2"))
            .parse(argv.size(), argv.data());
    REQUIRE(args.value("arg1").stringValue() == "foo");
    REQUIRE(args.value("arg2").stringValue() == "bar");
}

TEST_CASE("Option that appends must have argument or value")
{
    using namespace Argos;
    ArgumentParser parser("test");
    REQUIRE_THROWS(parser.add(Option({"-a"})
                                      .operation(ArgumentOperation::APPEND)));
}

TEST_CASE("List argument")
{
    using namespace Argos;
    Argv argv{"test", "-n", "12", "--number", "20", "--number=6", "-n15"};
    auto args = Argos::ArgumentParser("test")
            .autoExitEnabled(false)
            .add(Option({"-n", "--number"})
                         .operation(ArgumentOperation::APPEND)
                         .argument("NUM"))
            .parse(argv.size(), argv.data());
    REQUIRE(args.resultCode() == ParserResultCode::NORMAL);
    auto numbers = args.values("-n").int32Values();
    REQUIRE(!numbers.empty());
    REQUIRE(numbers.size() == 4);
    REQUIRE(numbers[0] == 12);
    REQUIRE(numbers[1] == 20);
    REQUIRE(numbers[2] == 6);
    REQUIRE(numbers[3] == 15);
}

TEST_CASE("Incorrect standard options")
{
    using namespace Argos;
    REQUIRE_THROWS(ArgumentParser().add(Option({"a"})));
    REQUIRE_NOTHROW(ArgumentParser().add(Option({"-a"})));
    REQUIRE_NOTHROW(ArgumentParser().add(Option({"--"})));
    REQUIRE_NOTHROW(ArgumentParser().add(Option({"-="})));
    REQUIRE_THROWS(ArgumentParser().add(Option({"-ab"})));
    REQUIRE_NOTHROW(ArgumentParser().add(Option({"--="})));
    REQUIRE_NOTHROW(ArgumentParser().add(Option({"--a"})));
    REQUIRE_NOTHROW(ArgumentParser().add(Option({"--a="})));
    REQUIRE_THROWS(ArgumentParser().add(Option({"--a=b"})));
}

TEST_CASE("Tet dash options")
{
    using namespace Argos;
    Argv argv{"test", "-number", "12", "-number", "20", "-number=6", "-number", "15"};
    auto args = Argos::ArgumentParser("test")
            .autoExitEnabled(false)
            .optionStyle(OptionStyle::DASH)
            .add(Option({"-number"})
                         .operation(ArgumentOperation::APPEND)
                         .argument("NUM"))
            .parse(argv.size(), argv.data());
    REQUIRE(args.resultCode() == ParserResultCode::NORMAL);
    auto numbers = args.values("-number").int32Values();
    REQUIRE(!numbers.empty());
    REQUIRE(numbers.size() == 4);
}

TEST_CASE("Tet slash options")
{
    using namespace Argos;
    Argv argv{"test", "/number", "12", "/number", "20", "/number=6", "/number", "15"};
    auto args = Argos::ArgumentParser("test")
            .autoExitEnabled(false)
            .optionStyle(OptionStyle::SLASH)
            .add(Option({"/number"})
                         .operation(ArgumentOperation::APPEND)
                         .argument("NUM"))
            .parse(argv.size(), argv.data());
    REQUIRE(args.resultCode() == ParserResultCode::NORMAL);
    auto numbers = args.values("/number").int32Values();
    REQUIRE(!numbers.empty());
    REQUIRE(numbers.size() == 4);
}

TEST_CASE("Test incorrect slash option")
{
    using namespace Argos;
    Argv argv{"test", "/benny"};
    auto args = Argos::ArgumentParser("test")
            .autoExitEnabled(false)
            .optionStyle(OptionStyle::SLASH)
            .add(Option({"/bill"}))
            .add(Argument({"file"}))
            .parse(argv.size(), argv.data());
    REQUIRE(args.resultCode() == ParserResultCode::ERROR);
}
