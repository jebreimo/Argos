//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <catch2/catch.hpp>
#include "Argos/StandardOptionIterator.hpp"

std::vector<std::string_view> makeArgs(
        const std::vector<std::string>& strings)
{
    std::vector<std::string_view> result;
    for (auto& s : strings)
        result.emplace_back(s);
    return result;
}

TEST_CASE("Test empty list of arguments.")
{
    std::vector<std::string> strings;
    Argos::StandardOptionIterator it(makeArgs(strings));
    REQUIRE(!it.next());
    REQUIRE(!it.nextValue());
}

TEST_CASE("Test empty argument.")
{
    std::vector<std::string> strings{"", "bc"};
    Argos::StandardOptionIterator it(makeArgs(strings));
    auto value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(value->empty());
    value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(*value == "bc");
}

TEST_CASE("Test short options.")
{
    std::vector<std::string> strings {"-a", "-bc"};
    Argos::StandardOptionIterator it(makeArgs(strings));
    auto value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(*value == "-a");
    value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(*value == "-b");
    value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(*value == "-c");
    value = it.next();
    REQUIRE(!value.has_value());
}

TEST_CASE("Test short options arguments.")
{
    std::vector<std::string> strings{"-abc", "-d", "-efg"};
    Argos::StandardOptionIterator it(makeArgs(strings));
    auto value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(*value == "-a");
    value = it.nextValue();
    REQUIRE(value.has_value());
    REQUIRE(*value == "bc");
    value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(*value == "-d");
    value = it.nextValue();
    REQUIRE(value.has_value());
    REQUIRE(*value == "-efg");
    value = it.next();
    REQUIRE(!value.has_value());
}

TEST_CASE("Test long options.")
{
    std::vector<std::string> strings{"--abc", "--def=ghi", "--jklmno=", "--pq"};
    Argos::StandardOptionIterator it(makeArgs(strings));
    auto value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(*value == "--abc");
    value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(*value == "--def=");
    value = it.nextValue();
    REQUIRE(value.has_value());
    REQUIRE(*value == "ghi");
    value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(*value == "--jklmno=");
    value = it.nextValue();
    REQUIRE(value.has_value());
    REQUIRE(value->empty());
    value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(*value == "--pq");
    value = it.next();
    REQUIRE(!value.has_value());
}

TEST_CASE("Test skipping value after =.")
{
    std::vector<std::string> strings{"--def=ghi", "--jkl=", "--p"};
    Argos::StandardOptionIterator it(makeArgs(strings));
    auto value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(*value == "--def=");
    value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(*value == "--jkl=");
    value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(*value == "--p");
    value = it.next();
    REQUIRE(!value.has_value());
}
