//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <catch2/catch_test_macros.hpp>
#include "Argos/ArgumentParser.hpp"

#include <sstream>
#include "Argv.hpp"

TEST_CASE("Test help flag")
{
    argos::ArgumentParser parser("test");
    std::stringstream ss;
    parser.auto_exit(false).stream(&ss);
    REQUIRE(parser.program_name() == "test");

    parser.add(argos::Option({"-h", "--help"})
        .type(argos::OptionType::HELP)
        .help("Show help message.")
        .id(10));
    Argv argv{"test", "--help"};
    auto result = parser.parse(argv.size(), argv.data());
    REQUIRE(result.has("--help"));
    REQUIRE(result.result_code() == argos::ParserResultCode::STOP);
    REQUIRE(result.stop_option().id() == 10);
    REQUIRE(result.value("--help").as_bool());
}

TEST_CASE("Conflicting flags")
{
    argos::ArgumentParser parser("test");
    parser.add(argos::Option({"-h", "--help"})
        .type(argos::OptionType::HELP)
        .help("Show help message."));
    parser.add(argos::Option({"-h"})
        .help("Output height."));
    Argv argv{"test", "--help"};
    REQUIRE_THROWS(parser.parse(argv.size(), argv.data()));
}

TEST_CASE("String arguments")
{
    using namespace argos;
    Argv argv{"test", "test_file.txt"};
    auto args = argos::ArgumentParser("test")
        .auto_exit(false)
        .add(Argument("file"))
        .parse(argv.size(), argv.data());
    REQUIRE(args.value("file").as_string() == "test_file.txt");
}

TEST_CASE("Section order in help text")
{
    using namespace argos;
    Argv argv{"test", "-h"};
    std::stringstream ss;
    auto args = argos::ArgumentParser("test")
        .auto_exit(false)
        .add(Argument("file").section("Z"))
        .add(Option({"-h"}).section("A").type(OptionType::HELP))
        .add(Option({"-s"}).section("Z"))
        .add(Argument("device").section("A"))
        .stream(&ss)
        .text(TextId::USAGE_TITLE, "")
        .text(TextId::USAGE, "")
        .parse(argv.size(), argv.data());
    REQUIRE(ss.str() == "Z\n  <file>\n  -s\n\nA\n  <device>\n  -h\n");
}

TEST_CASE("Test text callback")
{
    struct MakeText
    {
        std::string operator()()
        {
            return "Something.";
        }
    };
    using namespace argos;
    Argv argv{"test", "-h"};
    std::stringstream ss;
    auto args = argos::ArgumentParser("test")
        .auto_exit(false)
        .add(Argument("file"))
        .add(Option({"-h"}).type(OptionType::HELP))
        .add(Argument("device").section("A"))
        .stream(&ss)
        .text(TextId::USAGE_TITLE, "")
        .text(TextId::USAGE, "")
        .text(TextId::FINAL_TEXT, MakeText())
        .parse(argv.size(), argv.data());
    REQUIRE(ss.str() == "ARGUMENTS\n  <file>\n\nA\n  <device>\n\nOPTIONS\n  -h\n\nSomething.\n");
}

TEST_CASE("Two argument")
{
    using namespace argos;
    Argv argv{"test", "foo", "bar"};
    auto args = argos::ArgumentParser("test")
        .auto_exit(false)
        .add(Argument("arg1"))
        .add(Argument("arg2"))
        .parse(argv.size(), argv.data());
    REQUIRE(args.value("arg1").as_string() == "foo");
    REQUIRE(args.value("arg2").as_string() == "bar");
}

TEST_CASE("Option that appends must have argument or value")
{
    using namespace argos;
    ArgumentParser parser("test");
    REQUIRE_THROWS(parser.add(Option({"-a"})
        .operation(OptionOperation::APPEND)).parse({}));
}

TEST_CASE("List argument")
{
    using namespace argos;
    Argv argv{"test", "-n", "12", "--number", "20", "--number=6", "-n15"};
    auto args = argos::ArgumentParser("test")
        .auto_exit(false)
        .add(Option("-n", "--number")
            .operation(OptionOperation::APPEND)
            .argument("NUM"))
        .parse(argv.size(), argv.data());
    REQUIRE(args.result_code() == ParserResultCode::SUCCESS);
    auto numbers = args.values("-n").as_ints();
    REQUIRE(!numbers.empty());
    REQUIRE(numbers.size() == 4);
    REQUIRE(numbers[0] == 12);
    REQUIRE(numbers[1] == 20);
    REQUIRE(numbers[2] == 6);
    REQUIRE(numbers[3] == 15);
}

TEST_CASE("Incorrect standard options")
{
    using namespace argos;
    REQUIRE_THROWS(ArgumentParser().add(Opt("a")).parse({}));
    REQUIRE_NOTHROW(ArgumentParser().add(Opt("-a")).parse({}));
    REQUIRE_NOTHROW(ArgumentParser().add(Opt("--")).parse({}));
    REQUIRE_NOTHROW(ArgumentParser().add(Opt("-=")).parse({}));
    REQUIRE_THROWS(ArgumentParser().add(Opt("-ab")).parse({}));
    REQUIRE_THROWS(ArgumentParser().add(Opt("--=")).parse({}));
    REQUIRE_NOTHROW(ArgumentParser().add(Opt("--=").argument("A")).parse({}));
    REQUIRE_NOTHROW(ArgumentParser().add(Opt("--a")).parse({}));
    REQUIRE_THROWS(ArgumentParser().add(Opt("--a=")).parse({}));
    REQUIRE_NOTHROW(ArgumentParser().add(Opt("--a=").argument("A")).parse({}));
    REQUIRE_THROWS(ArgumentParser().add(Opt("--a=b")).parse({}));
}

TEST_CASE("Test dash options")
{
    using namespace argos;

    REQUIRE_NOTHROW(ArgumentParser().option_style(OptionStyle::DASH)
        .add(Option({"-="})).parse({}));
    REQUIRE_THROWS(ArgumentParser().option_style(OptionStyle::DASH)
        .add(Option({"-a="})).parse({}));
    REQUIRE_NOTHROW(ArgumentParser().option_style(OptionStyle::DASH)
        .add(Option({"-a="}).argument("A")).parse({}));

    Argv argv{"test", "-number", "12", "-number", "20", "-number=6", "-number", "15"};
    auto args = argos::ArgumentParser("test")
        .auto_exit(false)
        .option_style(OptionStyle::DASH)
        .add(Option({"-number"})
            .operation(OptionOperation::APPEND)
            .argument("NUM"))
        .parse(argv.size(), argv.data());
    REQUIRE(args.result_code() == ParserResultCode::SUCCESS);
    auto numbers = args.values("-number").as_ints();
    REQUIRE(!numbers.empty());
    REQUIRE(numbers.size() == 4);
}

TEST_CASE("Tet slash options")
{
    using namespace argos;
    Argv argv{"test", "/number", "12", "/number", "20", "/number=6", "/number", "15"};
    auto args = argos::ArgumentParser("test")
        .auto_exit(false)
        .option_style(OptionStyle::SLASH)
        .add(Option({"/number"})
            .operation(OptionOperation::APPEND)
            .argument("NUM"))
        .parse(argv.size(), argv.data());
    REQUIRE(args.result_code() == ParserResultCode::SUCCESS);
    auto numbers = args.values("/number").as_longs();
    REQUIRE(!numbers.empty());
    REQUIRE(numbers.size() == 4);
}

TEST_CASE("Test incorrect slash option")
{
    using namespace argos;
    Argv argv{"test", "/benny"};
    std::stringstream ss;
    auto args = argos::ArgumentParser("test")
        .auto_exit(false)
        .option_style(OptionStyle::SLASH)
        .stream(&ss)
        .add(Option({"/bill"}))
        .add(Argument("file"))
        .parse(argv.size(), argv.data());
    REQUIRE(args.result_code() == ParserResultCode::FAILURE);
}

TEST_CASE("Can't change option style after options have been added.")
{
    using namespace argos;
    ArgumentParser parser("test");
    parser.add(Argument("file"));
    REQUIRE_NOTHROW(parser.option_style(OptionStyle::DASH));
    parser.add(Option({"-p"}));
    REQUIRE_NOTHROW(parser.option_style(OptionStyle::DASH));
    REQUIRE_THROWS(parser.option_style(OptionStyle::STANDARD));
}

TEST_CASE("Test argument iterator")
{
    using namespace argos;
    Argv argv{"test", "foo", "bar", "baz"};
    auto it = argos::ArgumentParser("test")
        .auto_exit(false)
        .add(Argument("arg1").count(0, 9).id(1))
        .add(Argument("arg2").id(2))
        .make_iterator(argv.size(), argv.data());
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
    using namespace argos;
    Argv argv{"test", "--version", "arg 1", "arg 2"};
    auto args = argos::ArgumentParser("test")
        .auto_exit(false)
        .add(Argument("arg"))
        .add(Option("--version").type(OptionType::STOP))
        .parse(argv.size(), argv.data());
    REQUIRE(args.result_code() == ParserResultCode::STOP);
    auto option = args.stop_option();
    REQUIRE(option.flags().front() == "--version");
    REQUIRE(args.unprocessed_arguments().size() == 2);
    REQUIRE(args.unprocessed_arguments()[0] == "arg 1");
    REQUIRE(args.unprocessed_arguments()[1] == "arg 2");
}

TEST_CASE("LAST_ARGUMENT option")
{
    using namespace argos;
    Argv argv{"test", "--", "arg 1"};
    std::stringstream ss;
    auto args = argos::ArgumentParser("test")
        .auto_exit(false)
        .stream(&ss)
        .add(Argument("arg"))
        .add(Option("--").type(OptionType::LAST_ARGUMENT))
        .parse(argv.size(), argv.data());
    REQUIRE(args.result_code() == ParserResultCode::FAILURE);
}

TEST_CASE("LAST_OPTION option")
{
    using namespace argos;
    Argv argv{"test", "--bar", "--", "--bar"};
    auto args = argos::ArgumentParser("test")
        .auto_exit(false)
        .add(Argument("arg"))
        .add(Option("--bar"))
        .add(Option("--").type(OptionType::LAST_OPTION))
        .parse(argv.size(), argv.data());
    REQUIRE(args.result_code() == ParserResultCode::SUCCESS);
    REQUIRE(args.value("--bar").as_bool());
    REQUIRE(args.value("--").as_bool());
    REQUIRE(args.value("arg").as_string() == "--bar");
}

TEST_CASE("Argument with variable count")
{
    using namespace argos;
    Argv argv{"test", "ab", "cd", "ef", "gh"};
    SECTION("Variable count first")
    {
        auto args = argos::ArgumentParser("test")
            .auto_exit(false)
            .add(Argument("arg1").count(1, 4))
            .add(Argument("arg2").count(2))
            .parse(argv.size(), argv.data());
        REQUIRE(args.result_code() == ParserResultCode::SUCCESS);
        auto arg1 = args.values("arg1").as_strings();
        REQUIRE(arg1 == std::vector<std::string>{"ab", "cd"});
        auto arg2 = args.values("arg2").as_strings();
        REQUIRE(arg2 == std::vector<std::string>{"ef", "gh"});
    }
    SECTION("Variable count last")
    {
        auto args = argos::ArgumentParser("test")
            .auto_exit(false)
            .add(Argument("arg1").count(2))
            .add(Argument("arg2").count(1, 4))
            .parse(argv.size(), argv.data());
        REQUIRE(args.result_code() == ParserResultCode::SUCCESS);
        auto arg1 = args.values("arg1").as_strings();
        REQUIRE(arg1 == std::vector<std::string>{"ab", "cd"});
        auto arg2 = args.values("arg2").as_strings();
        REQUIRE(arg2 == std::vector<std::string>{"ef", "gh"});
    }
}

TEST_CASE("CLEAR option")
{
    using namespace argos;
    Argv argv{"test", "--bar=12", "--bud", "--bar", "34", "--ben"};
    auto it = argos::ArgumentParser("test")
        .auto_exit(false)
        .add(Option("--bar").argument("N")
            .operation(OptionOperation::APPEND)
            .id(1))
        .add(Option("--ben")
            .alias("--bar")
            .operation(OptionOperation::CLEAR)
            .id(2))
        .add(Option("--bud")
            .id(3))
        .make_iterator(argv.size(), argv.data());
    std::unique_ptr<IArgumentView> arg;
    std::string_view value;
    REQUIRE(it.next(arg, value));
    REQUIRE(arg->id() == 1);
    REQUIRE(value == "12");
    REQUIRE(it.next(arg, value));
    REQUIRE(arg->id() == 3);
    REQUIRE(value.empty());
    REQUIRE(it.next(arg, value));
    REQUIRE(arg->id() == 1);
    REQUIRE(value == "34");
    auto bars = it.parsed_arguments().values("--bar").as_ulongs();
    REQUIRE(bars == std::vector<unsigned long>{12, 34});
    REQUIRE(it.next(arg, value));
    REQUIRE(arg->id() == 2);
    bars = it.parsed_arguments().values("--bar").as_ulongs();
    REQUIRE(bars.empty());
    REQUIRE(it.parsed_arguments().value("--bud").as_bool());
}

TEST_CASE("Conflicting case-insensitive options")
{
    argos::ArgumentParser argos("test");
    argos.case_insensitive(true)
        .add(argos::Option("-h", "--help")
            .type(argos::OptionType::HELP)
            .help("Show help message."))
        .add(argos::Option("-H")
            .help("Output height."));
    Argv argv{"test", "--help"};
    REQUIRE_THROWS(argos.parse(argv.size(), argv.data()));
}

TEST_CASE("Case-insensitive options")
{
    using namespace argos;
    Argv argv{"test", "/PEnnY"};
    auto args = ArgumentParser("test")
        .case_insensitive(true)
        .option_style(OptionStyle::SLASH)
        .add(argos::Option("/penny"))
        .add(argos::Option("/lane"))
        .parse(argv.size(), argv.data());
    REQUIRE(args.result_code() == ParserResultCode::SUCCESS);
    REQUIRE(args.value("/penny").as_bool());
    REQUIRE(!args.value("/lane").as_bool());
}

TEST_CASE("Abbreviated options")
{
    using namespace argos;
    ArgumentParser parser("test");
    std::stringstream ss;
    parser.auto_exit(false)
        .allow_abbreviated_options(true)
        .case_insensitive(true)
        .option_style(OptionStyle::SLASH)
        .stream(&ss)
        .add(argos::Option("/penny"))
        .add(argos::Option("/pentagram"));
    SECTION("Valid flag 1")
    {
        Argv argv{"test", "/PenN"};
        auto args = parser.parse(argv.size(), argv.data());
        REQUIRE(args.result_code() == ParserResultCode::SUCCESS);
        REQUIRE(args.value("/penny").as_bool());
        REQUIRE(!args.value("/pentagram").as_bool());
    }
    SECTION("Valid flag 2")
    {
        Argv argv{"test", "/peNT"};
        auto args = parser.parse(argv.size(), argv.data());
        REQUIRE(args.result_code() == ParserResultCode::SUCCESS);
        REQUIRE(!args.value("/penny").as_bool());
        REQUIRE(args.value("/pentagram").as_bool());
    }
    SECTION("Invalid flag")
    {
        Argv argv{"test", "/peN"};
        auto args = parser.parse(argv.size(), argv.data());
        REQUIRE(args.result_code() == ParserResultCode::FAILURE);
        REQUIRE(!args.value("/penny").as_bool());
        REQUIRE(!args.value("/pentagram").as_bool());
    }
}

TEST_CASE("Test option callback")
{
    using namespace argos;
    Argv argv{"test", "-a"};
    ArgumentParser parser("test");
    auto args = parser.auto_exit(false)
        .add(Option({"-b"}))
        .add(Option({"-c"}))
        .add(Option({"-a"}).callback(
            [](auto opt, auto, auto builder) -> bool
            {
                builder.assign("-b", "true").assign("-c", "true");
                return true;
            }))
        .parse(argv.size(), argv.data());
    REQUIRE(args.result_code() == ParserResultCode::SUCCESS);
    REQUIRE(args.value("-a").as_bool());
    REQUIRE(args.value("-b").as_bool());
    REQUIRE(args.value("-c").as_bool());
}

TEST_CASE("Test argument callback")
{
    using namespace argos;
    Argv argv{"test", "-b", "abcd"};
    ArgumentParser parser("test");
    auto args = parser.auto_exit(false)
        .add(Option({"-b"}))
        .add(Argument("arg").callback(
            [](auto arg, auto, auto builder) -> bool
            {
                builder.assign("-b", "false");
                return true;
            }))
        .parse(argv.size(), argv.data());
    REQUIRE(args.result_code() == ParserResultCode::SUCCESS);
    REQUIRE(!args.value("-b").as_bool());
    REQUIRE(args.value("arg").as_string() == "abcd");
}

TEST_CASE("Two arguments with the same name")
{
    using namespace argos;
    Argv argv{"test", "aa", "bb", "cc"};
    ArgumentParser parser("test");
    auto args = parser.auto_exit(false)
        .add(Argument("arg"))
        .add(Argument("arg").count(0, 10))
        .parse(argv.size(), argv.data());
    REQUIRE(args.result_code() == ParserResultCode::SUCCESS);
    auto vals = args.values("arg").as_strings();
    REQUIRE(vals.size() == 3);
    REQUIRE(vals[0] == "aa");
    REQUIRE(vals[1] == "bb");
    REQUIRE(vals[2] == "cc");
}

TEST_CASE("Options ending with =")
{
    using namespace argos;
    Argv argv{"test", "--f=", "--f=2", "--f"};
    auto it = ArgumentParser("test")
        .auto_exit(false)
        .add(Option({"--f="}).alias("f").argument("N"))
        .add(Option({"--f"}).alias("f").operation(OptionOperation::CLEAR))
        .make_iterator(argv.size(), argv.data());
    std::unique_ptr<IArgumentView> arg;
    std::string_view value;
    REQUIRE(it.next(arg, value));
    REQUIRE(it.parsed_arguments().value("f").as_string(" ").empty());
    REQUIRE(it.next(arg, value));
    REQUIRE(it.parsed_arguments().value("f").as_string() == "2");
    REQUIRE(it.next(arg, value));
    REQUIRE(!it.parsed_arguments().has("f"));
    REQUIRE(!it.next(arg, value));
}

TEST_CASE("NONE option")
{
    using namespace argos;
    REQUIRE_THROWS(ArgumentParser("p").add(
        Option("-o").constant("f").operation(OptionOperation::NONE)).parse({}));
    REQUIRE_THROWS(ArgumentParser("p").add(
        Option("-o").alias("f").operation(OptionOperation::NONE)).parse({}));
    auto args = ArgumentParser("test")
        .auto_exit(false)
        .add(Option("--f").argument("N").operation(OptionOperation::NONE))
        .add(Option("--g").operation(OptionOperation::NONE))
        .add(Option("--h"))
        .parse({"--f=12", "--g", "--h"});
    REQUIRE_THROWS(args.value("--f"));
    REQUIRE_THROWS(args.value("--g"));
    REQUIRE(args.value("--h").as_bool());
}

TEST_CASE("Mandatory option")
{
    using namespace argos;
    std::stringstream ss;
    auto args = ArgumentParser("test")
        .auto_exit(false)
        .stream(&ss)
        .add(Option({"--f"}).argument("N").mandatory())
        .add(Argument("arg"))
        .parse({"abcd"});
    REQUIRE(args.result_code() == ParserResultCode::FAILURE);
}

TEST_CASE("Unknown options and arguments.")
{
    using namespace argos;
    Argv argv{{"test", "-o", "arg", "man"}};
    auto it = ArgumentParser("test")
        .auto_exit(false)
        .ignore_undefined_options(true)
        .ignore_undefined_arguments(true)
        .add(Argument("FILE"))
        .make_iterator(argv.size(), argv.data());
    std::unique_ptr<IArgumentView> arg;
    std::string_view value;
    REQUIRE(it.next(arg, value));
    REQUIRE(!arg);
    REQUIRE(value == "-o");
    REQUIRE(it.next(arg, value));
    REQUIRE(arg);
    REQUIRE(value == "arg");
    REQUIRE(it.next(arg, value));
    REQUIRE(!arg);
    REQUIRE(value == "man");
    REQUIRE(!it.next(arg, value));
    REQUIRE(it.parsed_arguments().result_code() == ParserResultCode::SUCCESS);
}

TEST_CASE("Unknown option, invalid argument.")
{
    using namespace argos;
    Argv argv{{"test", "--opera=foo", "arg", "man"}};
    std::stringstream ss;
    auto it = ArgumentParser("test")
        .auto_exit(false)
        .ignore_undefined_options(true)
        .stream(&ss)
        .add(Argument("FILE"))
        .make_iterator(argv.size(), argv.data());
    std::unique_ptr<IArgumentView> arg;
    std::string_view value;
    REQUIRE(it.next(arg, value));
    REQUIRE(!arg);
    REQUIRE(value == "--opera=foo");
    REQUIRE(it.next(arg, value));
    REQUIRE(arg);
    REQUIRE(value == "arg");
    REQUIRE(!it.next(arg, value));
    REQUIRE(it.parsed_arguments().result_code() == ParserResultCode::FAILURE);
}

TEST_CASE("Unknown argument, invalid option.")
{
    using namespace argos;
    Argv argv{{"test", "arg", "man", "-o"}};
    std::stringstream ss;
    auto it = ArgumentParser("test")
        .auto_exit(false)
        .ignore_undefined_arguments(true)
        .stream(&ss)
        .add(Argument("FILE"))
        .make_iterator(argv.size(), argv.data());
    std::unique_ptr<IArgumentView> arg;
    std::string_view value;
    REQUIRE(it.next(arg, value));
    REQUIRE(arg);
    REQUIRE(value == "arg");
    REQUIRE(it.next(arg, value));
    REQUIRE(!arg);
    REQUIRE(value == "man");
    REQUIRE(!it.next(arg, value));
    REQUIRE(it.parsed_arguments().result_code() == ParserResultCode::FAILURE);
}

TEST_CASE("Unknown argument, invalid short option.")
{
    using namespace argos;
    Argv argv{{"test", "-o", "-pq", "-op"}};
    std::stringstream ss;
    auto it = ArgumentParser("test")
        .auto_exit(false)
        .ignore_undefined_options(true)
        .stream(&ss)
        .add(Option{"-o"}.id(1))
        .make_iterator(argv.size(), argv.data());
    std::unique_ptr<IArgumentView> arg;
    std::string_view value;
    REQUIRE(it.next(arg, value));
    REQUIRE(arg);
    REQUIRE(arg->id() == 1);
    REQUIRE(it.next(arg, value));
    REQUIRE(!arg);
    REQUIRE(value == "-pq");
    REQUIRE(!it.next(arg, value));
    REQUIRE(it.parsed_arguments().result_code() == ParserResultCode::FAILURE);
}

TEST_CASE("Default help")
{
    using namespace argos;
    std::stringstream ss;
    auto args = ArgumentParser("test")
        .auto_exit(false)
        .stream(&ss)
        .add(Argument("arg"))
        .parse({"--help"});
    REQUIRE(args.result_code() == ParserResultCode::STOP);
}

TEST_CASE("Add option with reference before the option that is referred to")
{
    using namespace argos;
    auto parser = ArgumentParser("test");
    parser.add(Option{"-f"}.alias("-h").constant(2));
    parser.add(Option{"-g"}.alias("-h").constant(1));
    parser.add(Option{"-h"}.constant(3));
    auto args = parser.parse({"-h"});
    REQUIRE(args.value("-h").as_int() == 3);
}

TEST_CASE("Test using char* string as constant")
{
    using namespace argos;
    auto args = ArgumentParser("test")
        .add(Option{"-f"}.constant("foo"))
        .add(Option{"-g"}.alias("-f").constant("goo"))
        .parse({"-f"});
    REQUIRE(args.value("-f").as_string() == "foo");
}

TEST_CASE("Short options with argument")
{
    argos::ArgumentParser parser("test");
    parser.add(argos::Option({"-e"}).argument("FOO"));
    parser.add(argos::Option({"-x"}));
    parser.add(argos::Option({"-y"}));
    SECTION("Separate flags, joined value")
    {
        Argv argv{"test", "-x", "-e10"};
        auto args = parser.parse(argv.size(), argv.data());
        REQUIRE(args.value("-x").as_bool());
        REQUIRE(!args.value("-y").as_bool());
        REQUIRE(args.value("-e").as_int() == 10);
    }
    SECTION("Separate flags, separate value")
    {
        Argv argv{"test", "-x", "-e", "10"};
        auto args = parser.parse(argv.size(), argv.data());
        REQUIRE(args.value("-x").as_bool());
        REQUIRE(!args.value("-y").as_bool());
        REQUIRE(args.value("-e").as_int() == 10);
    }
    SECTION("Joined flags, joined value")
    {
        Argv argv{"test", "-xe10"};
        auto args = parser.parse(argv.size(), argv.data());
        REQUIRE(args.value("-x").as_bool());
        REQUIRE(!args.value("-y").as_bool());
        REQUIRE(args.value("-e").as_int() == 10);
    }
    SECTION("Joined flags, separate value")
    {
        Argv argv{"test", "-xe", "10"};
        auto args = parser.parse(argv.size(), argv.data());
        REQUIRE(args.value("-x").as_bool());
        REQUIRE(!args.value("-y").as_bool());
        REQUIRE(args.value("-e").as_int() == 10);
    }
}

TEST_CASE("Test options with initial_value")
{
    using namespace argos;
    using V = std::vector<std::string>;
    auto parser = ArgumentParser("test")
        .auto_exit(false)
        .add(Option{"-a"}
            .initial_value("a:b")
            .argument("A"))
        .add(Option{"-b"}
            .initial_value("o:p")
            .argument("B")
            .operation(OptionOperation::APPEND))
        .move();
    SECTION("Defaults")
    {
        Argv argv{"test"};
        auto args = parser.parse(argv.size(), argv.data());
        REQUIRE(args.value("-a").split(':').as_strings() == V{"a", "b"});
        REQUIRE(args.values("-b").split(':').as_strings() == V{"o", "p"});
    }
    SECTION("With options")
    {
        Argv argv{"test", "-ac:d", "-bq:r"};
        auto args = parser.parse(argv.size(), argv.data());
        REQUIRE(args.value("-a").split(':').as_strings() == V{"c", "d"});
        REQUIRE(args.values("-b").split(':').as_strings() == V{"o", "p", "q", "r"});
    }
}

TEST_CASE("Version option")
{
    using namespace argos;
    Argv argv{"test", "--version"};
    std::stringstream ss;
    auto args = ArgumentParser("test")
        .stream(&ss)
        .version("1.2.3")
        .auto_exit(false)
        .parse(argv.size(), argv.data());
    REQUIRE(args.result_code() == ParserResultCode::STOP);
    REQUIRE(ss.str() == "test 1.2.3\n");
}

TEST_CASE("Check that '-' is treated as an argument")
{
    using namespace argos;
    Argv argv{"test", "-"};
    auto args = ArgumentParser("test")
        .add(Argument("FOO"))
        .parse(argv.size(), argv.data());
    REQUIRE(args.value("FOO").as_string() == "-");
}

TEST_CASE("Check that program name is taken from argv[0]")
{
    using namespace argos;
    std::stringstream ss;
    Argv argv{"test", "-h"};
    auto args = ArgumentParser()
        .auto_exit(false)
        .stream(&ss)
        .parse(argv.size(), argv.data());
    auto help_text = ss.str();
    REQUIRE(help_text.substr(8, 4) == "test");
    REQUIRE(help_text.substr(22, 4) == "test");
}

TEST_CASE("Check that word-splitting rules are used")
{
    using namespace argos;
    ArgumentParser parser("test");
    parser.about("Full of weird incomprehensibilities.")
        .add_word_splitting_rule("in compre hensi bili ties")
        .generate_help_option(false);
    std::stringstream ss;
    parser.stream(&ss);
    parser.line_width(20);
    parser.write_help_text();
    auto help_text = ss.str();
    REQUIRE(help_text == "USAGE\n  test\n\nFull of weird in-\ncomprehensibilities.\n");
}
