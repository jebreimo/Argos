//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include <catch2/catch.hpp>
#include "Argos/Utilities.hpp"

TEST_CASE("Test startsWith")
{
    REQUIRE(Argos::startsWith("ABCDEF", "A"));
    REQUIRE(Argos::startsWith("ABCDEF", "ABC"));
    REQUIRE(Argos::startsWith("ABCDEF", "ABCDEF"));
    REQUIRE_FALSE(Argos::startsWith("ABCDEF", "abc"));
    REQUIRE_FALSE(Argos::startsWith("ABCDEF", "ABCDEFG"));
}

TEST_CASE("Test startsWithCI")
{
    REQUIRE(Argos::startsWithCI("ABCDEF", "A"));
    REQUIRE(Argos::startsWithCI("ABCDEF", "ABC"));
    REQUIRE(Argos::startsWithCI("ABCDEF", "ABcDEF"));
    REQUIRE(Argos::startsWithCI("ABCDEF", "abc"));
    REQUIRE_FALSE(Argos::startsWithCI("ABCDEF", "ABCDEFG"));
    REQUIRE(Argos::startsWithCI("@ABCDEF", "@abcdef"));
    REQUIRE_FALSE(Argos::startsWithCI("@ABCDEF", "`abcdef"));
    REQUIRE_FALSE(Argos::startsWithCI("{ABCDEF", "[abcdef"));
}
