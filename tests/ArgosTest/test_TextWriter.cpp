//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-27.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/TextWriter.hpp"
#include <catch2/catch_test_macros.hpp>

#include <sstream>

TEST_CASE("Basic TextWriter")
{
    argos::TextWriter writer(20);
    std::stringstream ss;
    writer.set_stream(&ss);
    writer.write("Word", false);
    writer.set_spaces(1);
    writer.write("word", false);
    writer.write("word", false);
    writer.flush();
    REQUIRE(ss.str() == "Word wordword");
}
