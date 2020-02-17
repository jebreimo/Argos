//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-01.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <catch2/catch.hpp>
#include "Argos/ParseValue.hpp"

TEST_CASE("Parse signed decimal integers")
{
    REQUIRE(Argos::parseValue<int>("0") == 0);
    REQUIRE(Argos::parseValue<int>("-1") == -1);
    REQUIRE(Argos::parseValue<int>("1") == 1);
    REQUIRE(Argos::parseValue<int>(std::to_string(INT_MAX)) == INT_MAX);
    REQUIRE(!Argos::parseValue<int>(std::to_string(int64_t(INT_MAX) + 1)).has_value());
    REQUIRE(Argos::parseValue<int>(std::to_string(INT_MIN)) == INT_MIN);
    REQUIRE(!Argos::parseValue<int>(std::to_string(int64_t(INT_MIN) - 1)).has_value());
    REQUIRE(Argos::parseValue<int>("123_456_78") == 12'345'678);
    REQUIRE(Argos::parseValue<int8_t>(std::to_string(INT8_MIN)) == INT8_MIN);
    REQUIRE(!Argos::parseValue<int8_t>(std::to_string(INT8_MIN - 1)).has_value());
}

TEST_CASE("Parse non-decimals")
{
    REQUIRE(Argos::parseValue<uint32_t>("0xFF2277CC") == 0xFF2277CCu);
    REQUIRE(Argos::parseValue<int>("-0xABCD") == -0xABCD);
    REQUIRE(Argos::parseValue<int>("0o671") == 0671);
    REQUIRE(!Argos::parseValue<int>("0o681").has_value());
    REQUIRE(Argos::parseValue<int>("0b1100_1111") == 0b1100'1111);
}
