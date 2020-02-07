//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/TextFormatter.hpp"

#include <sstream>
#include <catch2/catch.hpp>

TEST_CASE("Basic test of TextFormatter")
{
    std::stringstream ss;
    Argos::TextFormatter formatter(&ss, 0, 40);
    formatter.writeText("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    // TODO Enable test.
    //REQUIRE(ss.str() == "Lorem ipsum dolor sit amet, consectetur");
}

