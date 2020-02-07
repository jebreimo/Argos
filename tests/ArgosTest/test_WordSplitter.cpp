//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/WordSplitter.hpp"

#include <catch2/catch.hpp>

namespace
{
    void testDefaultSplit(
            std::string_view word, size_t startPos,
            size_t maxLength,
            std::tuple<std::string_view, char, std::string_view> result)
    {
        CAPTURE(word, startPos, maxLength);
        Argos::WordSplitter splitter;
        auto[txt, sep, rem] = splitter.split(word, startPos, maxLength, true);
        REQUIRE(txt == std::get<0>(result));
        REQUIRE(sep == std::get<1>(result));
        REQUIRE(rem == std::get<2>(result));
    }

    void testSplit(
            const std::string& rule,
            size_t startPos, size_t maxLength,
            std::tuple<std::string_view, char, std::string_view> result)
    {
        Argos::WordSplitter splitter;
        splitter.addWord(rule);
        std::string word = rule;
        word.erase(remove(word.begin(), word.end(), ' '), word.end());
        CAPTURE(word, startPos, maxLength);
        auto[txt, sep, rem] = splitter.split(word, startPos, maxLength, false);
        REQUIRE(txt == std::get<0>(result));
        REQUIRE(sep == std::get<1>(result));
        REQUIRE(rem == std::get<2>(result));
    }
}

TEST_CASE("Check default splitter")
{
    Argos::WordSplitter splitter;
    testDefaultSplit("decision", 0, 8, {"decision", '\0', {}});
    testDefaultSplit("decision", 0, 7, {"deci", '-', "sion"});
    testDefaultSplit("abcdef123456", 0, 9, {"abcdef", '-', "123456"});
    testDefaultSplit("transmitter", 0, 7, {"trans", '-', "mitter"});
}

TEST_CASE("Check splitter")
{
    Argos::WordSplitter splitter;
    testSplit("ono mato poe ti con", 2, 8, {"omato", '-', "poeticon"});
}
