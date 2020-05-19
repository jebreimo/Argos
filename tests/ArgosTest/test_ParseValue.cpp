//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-05-19.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ParseValue.hpp"
#include <catch2/catch.hpp>

TEST_CASE("parseInteger on empty string")
{
    REQUIRE(!Argos::parseInteger<int>({}, 0).has_value());
}

TEST_CASE("parseFloatingPoint on empty string")
{
    REQUIRE(!Argos::parseFloatingPoint<double>({}).has_value());
}
