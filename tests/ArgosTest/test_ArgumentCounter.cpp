//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-23.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <catch2/catch_test_macros.hpp>
#include "Argos/ArgumentCounter.hpp"
#include "../../include/Argos/Argument.hpp"

TEST_CASE("Test non-deterministic counter.")
{
    argos::CommandData command;
    auto& args = command.arguments;
    args.push_back(argos::Argument("1").count(0, 1).release());
    args.push_back(argos::Argument("2").count(2).release());

    REQUIRE(argos::ArgumentCounter::requires_argument_count(command));

    SECTION("Unknown number of arguments")
    {
        argos::ArgumentCounter counter(command);
        REQUIRE(!counter.is_complete());
        REQUIRE(counter.next_argument() == args[0].get());
        REQUIRE(!counter.is_complete());
        REQUIRE(counter.next_argument() == args[1].get());
        REQUIRE(!counter.is_complete());
        REQUIRE(counter.next_argument() == args[1].get());
        REQUIRE(counter.is_complete());
        REQUIRE(counter.next_argument() == nullptr);
    }
    SECTION("Known number of arguments")
    {
        argos::ArgumentCounter counter(command, 2);
        REQUIRE(!counter.is_complete());
        REQUIRE(counter.next_argument() == args[1].get());
        REQUIRE(!counter.is_complete());
        REQUIRE(counter.next_argument() == args[1].get());
        REQUIRE(counter.is_complete());
        REQUIRE(counter.next_argument() == nullptr);
    }
}
