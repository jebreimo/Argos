//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <catch2/catch_test_macros.hpp>
#include "Argos/StandardOptionIterator.hpp"

std::vector<std::string_view> make_args(
        const std::vector<std::string>& strings)
{
    std::vector<std::string_view> result;
    result.reserve(strings.size());
    for (auto& s : strings)
        result.emplace_back(s);
    return result;
}

TEST_CASE("Test empty list of arguments.")
{
    std::vector<std::string> strings;
    argos::StandardOptionIterator it(make_args(strings));
    REQUIRE(!it.next());
    REQUIRE(!it.next_value());
}

TEST_CASE("Test empty argument.")
{
    std::vector<std::string> strings{"", "bc"};
    argos::StandardOptionIterator it(make_args(strings));
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
    argos::StandardOptionIterator it(make_args(strings));
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
    argos::StandardOptionIterator it(make_args(strings));
    auto value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(*value == "-a");
    value = it.next_value();
    REQUIRE(value.has_value());
    REQUIRE(*value == "bc");
    value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(*value == "-d");
    value = it.next_value();
    REQUIRE(value.has_value());
    REQUIRE(*value == "-efg");
    value = it.next();
    REQUIRE(!value.has_value());
}

TEST_CASE("Test long options.")
{
    std::vector<std::string> strings{"--abc", "--def=ghi", "--jklmno=", "--pq"};
    argos::StandardOptionIterator it(make_args(strings));
    auto value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(*value == "--abc");
    value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(*value == "--def=");
    value = it.next_value();
    REQUIRE(value.has_value());
    REQUIRE(*value == "ghi");
    value = it.next();
    REQUIRE(value.has_value());
    REQUIRE(*value == "--jklmno=");
    value = it.next_value();
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
    argos::StandardOptionIterator it(make_args(strings));
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
