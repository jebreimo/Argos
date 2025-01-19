//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-01-19.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <array>
#include <catch2/catch_test_macros.hpp>
#include "Argos/ArgumentParser.hpp"
#include "Argv.hpp"

TEST_CASE("Test option callback")
{
    using namespace argos;
    Argv argv{"test", "-a"};
    auto args = ArgumentParser()
        .auto_exit(false)
        .add(Option({"-b"}))
        .add(Option({"-c"}))
        .add(Option({"-a"}).callback(
            [](auto& oa) -> bool
            {
                oa.builder.assign("-b", "true").assign("-c", "true");
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
    auto args = ArgumentParser()
        .auto_exit(false)
        .add(Option({"-b"}))
        .add(Argument("arg").callback(
            [](auto aa) -> bool
            {
                aa.builder.assign("-b", "false");
                return true;
            }))
        .parse(argv.size(), argv.data());
    REQUIRE(args.result_code() == ParserResultCode::SUCCESS);
    REQUIRE(!args.value("-b").as_bool());
    REQUIRE(args.value("arg").as_string() == "abcd");
}

TEST_CASE("Callback for all options")
{
    using namespace argos;
    struct Callback
    {
        Callback(size_t& i) : i(i) {}
        void operator()(OptionCallbackArguments& oa)
        {
            REQUIRE(i != values.size());
            REQUIRE(oa.view.flags().size() == 1);
            REQUIRE(oa.view.flags()[0] == values[i]);
            ++i;
        }

        size_t& i;
        std::array<std::string, 2> values = {"-b", "-c"};
    };

    size_t i = 0;
    Argv argv{"test", "-b", "-c"};
    auto args = ArgumentParser()
        .auto_exit(false)
        .add(Option("-b"))
        .add(Option("-c"))
        .option_callback(Callback(i))
        .parse(argv.size(), argv.data());
    REQUIRE(i == 2);
}

TEST_CASE("Callback for all arguments")
{
    using namespace argos;
    struct Callback
    {
        Callback(size_t& i)
            : i(i)
        {
        }

        void operator()(ArgumentCallbackArguments& oa)
        {
            REQUIRE(i != values.size());
            REQUIRE(oa.view.name() == values[i]);
            ++i;
        }

        size_t& i;
        std::array<std::string, 2> values = {"FILE", "URL"};
    };

    size_t i = 0;
    Argv argv{"test", "bbb", "ccc"};
    auto args = ArgumentParser()
        .auto_exit(false)
        .add(Argument("FILE"))
        .add(Argument("URL"))
        .argument_callback(Callback(i))
        .parse(argv.size(), argv.data());
    REQUIRE(i == 2);
}

TEST_CASE("Test option callback adding new args")
{
    using namespace argos;
    Argv argv{"test", "-aC"};
    auto args = ArgumentParser()
        .auto_exit(false)
        .add(Option({"-b"}))
        .add(Option({"-B"}).alias("-b").constant(false))
        .add(Option({"-c"}))
        .add(Option({"-C"}).alias("-c").constant(false))
        .add(Option({"-d"}))
        .add(Option({"-D"}).alias("-d").constant(false))
        .add(Option({"-a"}).callback(
            [](auto& oa)
            {
                oa.new_arguments = {"-bcd"};
            }))
        .parse(argv.size(), argv.data());
    REQUIRE(args.result_code() == ParserResultCode::SUCCESS);
    REQUIRE(args.value("-a").as_bool());
    REQUIRE(args.value("-b").as_bool());
    REQUIRE_FALSE(args.value("-c").as_bool());
    REQUIRE(args.value("-d").as_bool());
}
