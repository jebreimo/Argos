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

TEST_CASE("Test default splitter")
{
    Argos::WordSplitter splitter;
    testDefaultSplit("decision", 0, 8, {"decision", '\0', {}});
    testDefaultSplit("decision", 0, 7, {"decisi", '-', "on"});
    testDefaultSplit("abcdef123456", 0, 9, {"abcdef", '-', "123456"});
    testDefaultSplit("bbbbbbbbbbb", 0, 7, {"bbbbbb", '-', "bbbbb"});
    testDefaultSplit("bbbbbbccccccdddddd", 6, 7, {"cccccc", '-', "dddddd"});
}

TEST_CASE("Test default splitter with UTF-8")
{
    Argos::WordSplitter splitter;
    testDefaultSplit(u8"æøå•Ωé†µüıœπ˙äöﬁª√˛¸ƒ∂ß", 0, 14,
                     {u8"æøå•Ωé†µüıœπ˙", '-', u8"äöﬁª√˛¸ƒ∂ß"});
    testDefaultSplit(u8"Båidg-hølnow", 0, 8,
                     {u8"Båidg-", '\0', u8"hølnow"});
}

TEST_CASE("Test splitter")
{
    Argos::WordSplitter splitter;
    testSplit("ono mato poe ti con", 2, 8, {"omato", '-', "poeticon"});
    testSplit("ono mato poe ti con", 0, 8, {"onomato", '-', "poeticon"});
    testSplit("ono mato poe ti con", 0, 7, {"ono", '-', "matopoeticon"});
    testSplit("ono mato poe ti con", 7, 8, {"poeticon", '\0', {}});
    testSplit("ono mato poe ti con", 7, 7, {"poeti", '-', "con"});
}

TEST_CASE("Test splitter with hyphens")
{
    Argos::WordSplitter splitter;
    testSplit("multi- tasking", 0, 8, {"multi-", '\0', "tasking"});
}

TEST_CASE("Test splitter with UTF-8")
{
    Argos::WordSplitter splitter;
    testSplit(u8"Brønn øy sund", 0, 6, {u8"Brønn", '-', u8"øysund"});
    testSplit(u8"Brønn øy sund", 0, 7, {u8"Brønn", '-', u8"øysund"});
    testSplit(u8"Brønn øy sund", 0, 8, {u8"Brønnøy", '-', u8"sund"});
    testSplit(u8"Brønn øy sund", 0, 10, {u8"Brønnøy", '-', u8"sund"});
    testSplit(u8"Brønn øy sund", 0, 11, {u8"Brønnøysund", '\0', {}});
    testSplit(u8"Brønn øy sund", 4, 7, {u8"nnøy", '-', u8"sund"});
}
