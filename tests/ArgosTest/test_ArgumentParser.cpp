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
    REQUIRE(result.specialOption().id() == 10);
    REQUIRE(result.getBool("--help"));
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
    REQUIRE(args.getString("file") == "test_file.txt");
}
