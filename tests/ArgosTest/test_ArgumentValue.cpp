//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-03-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <catch2/catch.hpp>
#include "Argos/ArgumentParser.hpp"

TEST_CASE("Test ArgumentValue split")
{
    using namespace Argos;
    auto args = ArgumentParser("test")
            .autoExit(false)
            .add(Option({"--f="}).argument("M,N"))
            .parse({"--f=34,45"});
    auto values = args.value("--f=").split(',', 2, 2).asInt32s();
    REQUIRE(values == std::vector<int32_t>{34, 45});
}

TEST_CASE("Test ArgumentValues split")
{
    using namespace Argos;
    auto args = ArgumentParser("test")
            .autoExit(false)
            .add(Option({"--f"}).argument("PATHS").operation(OptionOperation::APPEND))
            .parse({"--f", "/a/b:/a/c", "--f", "/b/c:/b/d/e:/c/a"});
    auto values = args.values("--f").split(':').asStrings();
    REQUIRE(values == std::vector<std::string>{"/a/b", "/a/c", "/b/c", "/b/d/e", "/c/a"});
}