//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <catch2/catch.hpp>
#include "Argos/StringUtilities.hpp"

TEST_CASE("Test starts_with")
{
    REQUIRE(argos::starts_with("ABCDEF", "A"));
    REQUIRE(argos::starts_with("ABCDEF", "ABC"));
    REQUIRE(argos::starts_with("ABCDEF", "ABCDEF"));
    REQUIRE_FALSE(argos::starts_with("ABCDEF", "abc"));
    REQUIRE_FALSE(argos::starts_with("ABCDEF", "ABCDEFG"));
}

TEST_CASE("Test starts_with_ci")
{
    REQUIRE(argos::starts_with_ci("ABCDEF", "A"));
    REQUIRE(argos::starts_with_ci("ABCDEF", "ABC"));
    REQUIRE(argos::starts_with_ci("ABCDEF", "ABcDEF"));
    REQUIRE(argos::starts_with_ci("ABCDEF", "abc"));
    REQUIRE_FALSE(argos::starts_with_ci("ABCDEF", "ABCDEFG"));
    REQUIRE(argos::starts_with_ci("@ABCDYZ", "@abcdyz"));
    REQUIRE_FALSE(argos::starts_with_ci("@ABCDYZ", "`abcdyz"));
    REQUIRE_FALSE(argos::starts_with_ci("{ABCDYZ", "[abcdyz"));
}

TEST_CASE("Test is_less_ci")
{
    REQUIRE(argos::is_less_ci("abc", "ABD"));
    REQUIRE_FALSE(argos::is_less_ci("abc", "ABC"));
    REQUIRE(argos::is_less_ci("abc", "ABCD"));
    REQUIRE_FALSE(argos::is_less_ci("aBCD", "ABC"));
}

TEST_CASE("Test count_code_points")
{
    REQUIRE(argos::count_code_points(u8"Bæ bæ bø må.") == 12);
}

TEST_CASE("Test find_nth_code_point")
{
    REQUIRE(argos::find_nth_code_point(u8"Bæ bæ bø må.", 8) == 11);
}

TEST_CASE("Test find_nth_code_point non-UTF8")
{
    REQUIRE(argos::find_nth_code_point("Ba boo\200 boo ma.", 8) == 8);
}
