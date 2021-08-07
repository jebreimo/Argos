//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-03-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <catch2/catch.hpp>
#include "Argos/ArgumentParser.hpp"

#include <cstring>

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
    REQUIRE(args.value("ARG1"));
    REQUIRE(args.value("ARG1").asDouble() == 1.567);
    REQUIRE(!args.value("-v"));
    REQUIRE(args.value("-v").asDouble(1e-10) == 1e-10);
}

TEST_CASE("ArgumentValueIterator")
{
    using namespace Argos;
    auto args = ArgumentParser("test")
        .autoExit(false)
        .add(Option{"-i"}
            .argument("STR[:STR]*")
            .operation(Argos::OptionOperation::APPEND))
        .parse({"-i", "A:B", "-i", "C:D:E"});
    const char expected[] = "ABCDE";
    auto values = args.values("-i").split(':');
    REQUIRE(bool(values));
    REQUIRE(values.size() == strlen(expected));
    int i = 0;
    for (auto v : values)
    {
        CAPTURE(i);
        REQUIRE(v.asString() == std::string(1, expected[i++]));
    }
    REQUIRE(i == strlen(expected));
}
