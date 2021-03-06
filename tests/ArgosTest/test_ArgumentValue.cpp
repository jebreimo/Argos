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
    auto values = args.value("--f=").split(',', 2, 2).asInts();
    REQUIRE(values == std::vector<int>{34, 45});
}

TEST_CASE("Test ArgumentValues split")
{
    using namespace Argos;
    auto args = ArgumentParser("test")
            .autoExit(false)
            .add(Option{"--f"}.argument("PATHS").operation(OptionOperation::APPEND))
            .parse({"--f", "/a/b:/a/c", "--f", "/b/c:/b/d/e:/c/a"});
    auto values = args.values("--f").split(':').asStrings();
    REQUIRE(values == std::vector<std::string>{"/a/b", "/a/c", "/b/c", "/b/d/e", "/c/a"});
}

TEST_CASE("Double values")
{
    using namespace Argos;
    auto args = ArgumentParser("test")
        .autoExit(false)
        .add(Argument("ARG1"))
        .add(Option{"-v"}.argument("N"))
        .parse({"1.567"});
    REQUIRE(args.value("ARG1").asDouble() == 1.567);
    REQUIRE(args.value("-v").asDouble(1e-10) == 1e-10);
}
