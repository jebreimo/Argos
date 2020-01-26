//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-23.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <catch2/catch.hpp>
#include "Argos/ArgumentCounter.hpp"
#include "../../include/Argos/Argument.hpp"

TEST_CASE("Test non-deterministic counter.")
{
    std::vector<std::shared_ptr<Argos::ArgumentData>> args {
            Argos::Argument("1").count(0, 1).release(),
            Argos::Argument("2").count(2).release()
    };

    REQUIRE(Argos::ArgumentCounter::requiresArgumentCount(args));

    SECTION("Unknown number of arguments")
    {
        Argos::ArgumentCounter counter(args);
        REQUIRE(!counter.isComplete());
        REQUIRE(counter.nextArgument() == args[0].get());
        REQUIRE(!counter.isComplete());
        REQUIRE(counter.nextArgument() == args[1].get());
        REQUIRE(!counter.isComplete());
        REQUIRE(counter.nextArgument() == args[1].get());
        REQUIRE(counter.isComplete());
        REQUIRE(counter.nextArgument() == nullptr);
    }
    SECTION("Known number of arguments")
    {
        Argos::ArgumentCounter counter(args, 2);
        REQUIRE(!counter.isComplete());
        REQUIRE(counter.nextArgument() == args[1].get());
        REQUIRE(!counter.isComplete());
        REQUIRE(counter.nextArgument() == args[1].get());
        REQUIRE(counter.isComplete());
        REQUIRE(counter.nextArgument() == nullptr);
    }
}
