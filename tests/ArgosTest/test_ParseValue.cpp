//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-05-19.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ParseValue.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("parse_integer on empty string")
{
    REQUIRE(!argos::parse_integer<int>({}, 0).has_value());
}

TEST_CASE("parse_floating_point on empty string")
{
    REQUIRE(!argos::parse_floating_point<double>({}).has_value());
}
