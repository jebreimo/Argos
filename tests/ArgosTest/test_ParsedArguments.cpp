//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-04-20.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <catch2/catch.hpp>
#include "Argos/Argos.hpp"

#include "Argv.hpp"

TEST_CASE("Test filter argc/argv")
{
    using namespace Argos;
    auto parser = ArgumentParser("test")
        .autoExit(false)
        .ignoreUndefinedOptions(true)
        .ignoreUndefinedArguments(true)
        .add(Argument("FILE"))
        .add(Option{"-f"})
        .add(Option{"--g"})
        .move();
    SECTION("filter all")
    {
        Argv argv{{"test", "-f", "file"}};
        int c = argv.size();
        char** v = argv.data();
        auto args = parser.parse(c, v);
        args.filterParsedArguments(c, v);
        REQUIRE(c == 1);
        REQUIRE(std::string(v[0]) == "test");
    }
    SECTION("filter some")
    {
        Argv argv{{"test", "-f", "-p", "--g", "--q", "file", "text"}};
        int c = argv.size();
        char** v = argv.data();
        auto args = parser.parse(c, v);
        args.filterParsedArguments(c, v);
        REQUIRE(c == 4);
        REQUIRE(std::string(v[0]) == "test");
        REQUIRE(std::string(v[1]) == "-p");
        REQUIRE(std::string(v[2]) == "--q");
        REQUIRE(std::string(v[3]) == "text");
    }
}
