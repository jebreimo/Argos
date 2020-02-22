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
    argos.autoExit(false).outputStream(&ss);
    REQUIRE(argos.programName() == "test");

    argos.add(Argos::Option({"-h", "--help"})
            .type(Argos::OptionType::HELP)
            .text("Show help message.")
            .id(10));
    Argv argv{"test", "--help"};
    auto result = argos.parse(argv.size(), argv.data());
    REQUIRE(result.has("--help"));
    REQUIRE(result.resultCode() == Argos::ParserResultCode::STOP);
    REQUIRE(result.stopOption().id() == 10);
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
            .autoExit(false)
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
            .autoExit(false)
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
            .autoExit(false)
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
                                      .operation(OptionOperation::APPEND)));
}

TEST_CASE("List argument")
{
    using namespace Argos;
    Argv argv{"test", "-n", "12", "--number", "20", "--number=6", "-n15"};
    auto args = Argos::ArgumentParser("test")
            .autoExit(false)
            .add(Option({"-n", "--number"})
                         .operation(OptionOperation::APPEND)
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
            .autoExit(false)
            .optionStyle(OptionStyle::DASH)
            .add(Option({"-number"})
                         .operation(OptionOperation::APPEND)
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
            .autoExit(false)
            .optionStyle(OptionStyle::SLASH)
            .add(Option({"/number"})
                         .operation(OptionOperation::APPEND)
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
            .autoExit(false)
            .optionStyle(OptionStyle::SLASH)
            .add(Option({"/bill"}))
            .add(Argument({"file"}))
            .parse(argv.size(), argv.data());
    REQUIRE(args.resultCode() == ParserResultCode::ERROR);
}

TEST_CASE("Can't change option style after options have been added.")
{
    using namespace Argos;
    ArgumentParser parser("test");
    parser.add(Argument("file"));
    REQUIRE_NOTHROW(parser.optionStyle(OptionStyle::DASH));
    parser.add(Option({"-p"}));
    REQUIRE_NOTHROW(parser.optionStyle(OptionStyle::DASH));
    REQUIRE_THROWS(parser.optionStyle(OptionStyle::STANDARD));
}

TEST_CASE("Test argument iterator")
{
    using namespace Argos;
    Argv argv{"test", "foo", "bar", "baz"};
    auto it = Argos::ArgumentParser("test")
            .autoExit(false)
            .add(Argument("arg1").count(0, 9).id(1))
            .add(Argument("arg2").id(2))
            .makeIterator(argv.size(), argv.data());
    std::unique_ptr<IArgumentView> arg;
    std::string_view value;

    REQUIRE(it.next(arg, value));
    REQUIRE(arg);
    REQUIRE(arg->id() == 1);
    auto argumentView = dynamic_cast<const ArgumentView*>(arg.get());
    REQUIRE(argumentView);
    REQUIRE(argumentView->name() == "arg1");
    REQUIRE(value == "foo");

    REQUIRE(it.next(arg, value));
    REQUIRE(arg);
    REQUIRE(arg->id() == 1);
    argumentView = dynamic_cast<const ArgumentView*>(arg.get());
    REQUIRE(argumentView);
    REQUIRE(argumentView->name() == "arg1");
    REQUIRE(value == "bar");

    REQUIRE(it.next(arg, value));
    REQUIRE(arg);
    REQUIRE(arg->id() == 2);
    argumentView = dynamic_cast<const ArgumentView*>(arg.get());
    REQUIRE(argumentView);
    REQUIRE(argumentView->name() == "arg2");
    REQUIRE(value == "baz");

    REQUIRE_FALSE(it.next(arg, value));
    REQUIRE(!arg);
    REQUIRE(value.empty());
}

TEST_CASE("STOP option")
{
    using namespace Argos;
    Argv argv{"test", "--version", "arg 1", "arg 2"};
    auto args = Argos::ArgumentParser("test")
            .autoExit(false)
            .add(Argument("arg"))
            .add(Option({"--version"}).type(OptionType::STOP))
            .parse(argv.size(), argv.data());
    REQUIRE(args.resultCode() == ParserResultCode::STOP);
    auto option = args.stopOption();
    REQUIRE(option.flags().front() == "--version");
    REQUIRE(args.unprocessedArguments().size() == 2);
    REQUIRE(args.unprocessedArguments()[0] == "arg 1");
    REQUIRE(args.unprocessedArguments()[1] == "arg 2");
}

TEST_CASE("LAST_ARGUMENT option")
{
    using namespace Argos;
    Argv argv{"test", "--", "arg 1"};
    auto args = Argos::ArgumentParser("test")
            .autoExit(false)
            .add(Argument("arg"))
            .add(Option({"--"}).type(OptionType::LAST_ARGUMENT))
            .parse(argv.size(), argv.data());
    REQUIRE(args.resultCode() == ParserResultCode::ERROR);
}

TEST_CASE("LAST_OPTION option")
{
    using namespace Argos;
    Argv argv{"test", "--bar", "--", "--bar"};
    auto args = Argos::ArgumentParser("test")
            .autoExit(false)
            .add(Argument("arg"))
            .add(Option({"--bar"}))
            .add(Option({"--"}).type(OptionType::LAST_OPTION))
            .parse(argv.size(), argv.data());
    REQUIRE(args.resultCode() == ParserResultCode::NORMAL);
    REQUIRE(args.value("--bar").boolValue());
    REQUIRE(args.value("--").boolValue());
    REQUIRE(args.value("arg").stringValue() == "--bar");
}

TEST_CASE("Argument with variable count")
{
    using namespace Argos;
    Argv argv{"test", "ab", "cd", "ef", "gh"};
    SECTION("Variable count first")
    {
        auto args = Argos::ArgumentParser("test")
                .autoExit(false)
                .add(Argument("arg1").count(1, 4))
                .add(Argument("arg2").count(2))
                .parse(argv.size(), argv.data());
        REQUIRE(args.resultCode() == ParserResultCode::NORMAL);
        auto arg1 = args.values("arg1").stringValues();
        REQUIRE(arg1 == std::vector<std::string>{"ab", "cd"});
        auto arg2 = args.values("arg2").stringValues();
        REQUIRE(arg2 == std::vector<std::string>{"ef", "gh"});
    }
    SECTION("Variable count last")
    {
        auto args = Argos::ArgumentParser("test")
                .autoExit(false)
                .add(Argument("arg1").count(2))
                .add(Argument("arg2").count(1, 4))
                .parse(argv.size(), argv.data());
        REQUIRE(args.resultCode() == ParserResultCode::NORMAL);
        auto arg1 = args.values("arg1").stringValues();
        REQUIRE(arg1 == std::vector<std::string>{"ab", "cd"});
        auto arg2 = args.values("arg2").stringValues();
        REQUIRE(arg2 == std::vector<std::string>{"ef", "gh"});
    }
}

TEST_CASE("CLEAR option")
{
    using namespace Argos;
    Argv argv{"test", "--bar=12", "--bar", "34", "--ben"};
    auto it = Argos::ArgumentParser("test")
            .autoExit(false)
            .add(Option({"--bar"}).argument("N")
                         .operation(OptionOperation::APPEND)
                         .id(1))
            .add(Option({"--ben"})
                         .valueName("--bar")
                         .operation(OptionOperation::CLEAR)
                         .id(2))
            .makeIterator(argv.size(), argv.data());
    std::unique_ptr<IArgumentView> arg;
    std::string_view value;
    REQUIRE(it.next(arg, value));
    REQUIRE(arg->id() == 1);
    REQUIRE(value == "12");
    REQUIRE(it.next(arg, value));
    REQUIRE(arg->id() == 1);
    REQUIRE(value == "34");
    auto bars = it.parsedArguments().values("--bar").int32Values();
    REQUIRE(bars == std::vector<int32_t>{12, 34});
    REQUIRE(it.next(arg, value));
    REQUIRE(arg->id() == 2);
    bars = it.parsedArguments().values("--bar").int32Values();
    REQUIRE(bars.empty());
}

TEST_CASE("Conflicting case-insensitive options")
{
    Argos::ArgumentParser argos("test");
    argos.caseInsensitive(true)
            .add(Argos::Option({"-h", "--help"})
                         .type(Argos::OptionType::HELP)
                         .text("Show help message."))
            .add(Argos::Option({"-H"})
                         .text("Output height."));
    Argv argv{"test", "--help"};
    REQUIRE_THROWS(argos.parse(argv.size(), argv.data()));
}

TEST_CASE("Case-insensitive options")
{
    using namespace Argos;
    Argv argv{"test", "/PEnnY"};
    auto args = ArgumentParser("test")
            .caseInsensitive(true)
            .optionStyle(OptionStyle::SLASH)
            .add(Argos::Option({"/penny"}))
            .add(Argos::Option({"/lane"}))
            .parse(argv.size(), argv.data());
    REQUIRE(args.resultCode() == ParserResultCode::NORMAL);
    REQUIRE(args.value("/penny").boolValue());
    REQUIRE(!args.value("/lane").boolValue());
}

TEST_CASE("Abbreviated options")
{
    using namespace Argos;
    ArgumentParser parser("test");
    parser.autoExit(false)
            .allowAbbreviatedOptions(true)
            .caseInsensitive(true)
            .optionStyle(OptionStyle::SLASH)
            .add(Argos::Option({"/penny"}))
            .add(Argos::Option({"/pentagram"}));
    SECTION("Valid flag 1")
    {
        Argv argv{"test", "/PenN"};
        auto args = parser.parse(argv.size(), argv.data());
        REQUIRE(args.resultCode() == ParserResultCode::NORMAL);
        REQUIRE(args.value("/penny").boolValue());
        REQUIRE(!args.value("/pentagram").boolValue());
    }
    SECTION("Valid flag 2")
    {
        Argv argv{"test", "/peNT"};
        auto args = parser.parse(argv.size(), argv.data());
        REQUIRE(args.resultCode() == ParserResultCode::NORMAL);
        REQUIRE(!args.value("/penny").boolValue());
        REQUIRE(args.value("/pentagram").boolValue());
    }
    SECTION("Invalid flag")
    {
        Argv argv{"test", "/peN"};
        auto args = parser.parse(argv.size(), argv.data());
        REQUIRE(args.resultCode() == ParserResultCode::ERROR);
        REQUIRE(!args.value("/penny").boolValue());
        REQUIRE(!args.value("/pentagram").boolValue());
    }
}
