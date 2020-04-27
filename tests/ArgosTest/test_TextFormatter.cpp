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
    Argos::TextFormatter formatter(&ss, 40);
    formatter.writeWords("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    formatter.flush();
    REQUIRE(ss.str() == "Lorem ipsum dolor sit amet, consectetur\nadipiscing elit.");
}

TEST_CASE("Test TextFormatter with indentation")
{
    std::stringstream ss;
    Argos::TextFormatter formatter(&ss, 40);
    formatter.writeWords("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    formatter.pushIndentation(17);
    formatter.writeWords("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    formatter.popIndentation();
    formatter.writeWords(" ");
    formatter.writeWords("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    formatter.flush();
    REQUIRE(ss.str() == R"*(Lorem ipsum dolor sit amet, consectetur
adipiscing elit. Lorem ipsum dolor sit
                 amet, consectetur
                 adipiscing elit. Lorem
ipsum dolor sit amet, consectetur
adipiscing elit.)*");
}

TEST_CASE("Text with newlines")
{
    std::stringstream ss;
    Argos::TextFormatter formatter(&ss, 40);
    formatter.writeWords("Lorem ipsum dolor\nsit amet, consectetur\nadipiscing elit.");
    formatter.flush();
    REQUIRE(ss.str() == "Lorem ipsum dolor\nsit amet, consectetur\nadipiscing elit.");
}

TEST_CASE("Indentation change")
{
    std::stringstream ss;
    Argos::TextFormatter formatter(&ss, 40);
    formatter.writeWords("ABCDEFGHIJ");
    formatter.pushIndentation(20);
    formatter.writeWords("ABCDEFGHIJ");
    formatter.flush();
    REQUIRE(ss.str() == "ABCDEFGHIJ          ABCDEFGHIJ");
}

TEST_CASE("Indentation change with preformatted text")
{
    std::stringstream ss;
    Argos::TextFormatter formatter(&ss, 40);
    formatter.writeLines("ABCDEFGHIJ");
    formatter.pushIndentation(20);
    formatter.writeLines("ABCDEFGHIJ");
    formatter.flush();
    REQUIRE(ss.str() == "ABCDEFGHIJ          ABCDEFGHIJ");
}

TEST_CASE("Keep whitespace at the start of a line.")
{
    std::stringstream ss;
    Argos::TextFormatter formatter(&ss, 40);
    formatter.writeWords(" Lorem ipsum dolor\n  sit amet, consectetur\n   adipiscing elit.");
    formatter.flush();
    REQUIRE(ss.str() == " Lorem ipsum dolor\n  sit amet, consectetur\n   adipiscing elit.");
}

TEST_CASE("Preformatted text across several lines")
{
    std::stringstream ss;
    Argos::TextFormatter formatter(&ss, 30);
    formatter.pushIndentation(5);
    formatter.writeLines("[abc efg]");
    formatter.writeWords(" ");
    formatter.writeLines("[abc efg]");
    formatter.writeWords(" ");
    formatter.writeLines("[abc efg]");
    formatter.writeWords(" ");
    formatter.writeLines("[abc efg]");
    formatter.writeWords(" ");
    formatter.writeLines("[abc efg]");
    formatter.flush();
    REQUIRE(ss.str() == "     [abc efg] [abc efg]\n     [abc efg] [abc efg]\n     [abc efg]");
}
