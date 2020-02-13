//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <catch2/catch.hpp>
#include <cfloat>
#include "Argos/ParseFloatingPoint.hpp"

TEST_CASE("Parse doubles")
{
    REQUIRE(Argos::parseFloatingPoint<double>("0") == 0.0);
    REQUIRE(Argos::parseFloatingPoint<double>("-1.234") == -1.234);
    REQUIRE(Argos::parseFloatingPoint<double>("1.234") == 1.234);
    REQUIRE(Argos::parseFloatingPoint<double>(std::to_string(DBL_MAX)) == DBL_MAX);
}
