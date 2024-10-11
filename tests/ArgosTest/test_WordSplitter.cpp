//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-07.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/WordSplitter.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include "U8Adapter.hpp"

namespace
{
    void test_default_split(
            std::string_view word, size_t start_pos,
            size_t max_length,
            std::tuple<std::string_view, char, std::string_view> result)
    {
        CAPTURE(word, start_pos, max_length);
        argos::WordSplitter splitter;
        auto[txt, sep, rem] = splitter.split(word, start_pos, max_length, true);
        REQUIRE(txt == std::get<0>(result));
        REQUIRE(sep == std::get<1>(result));
        REQUIRE(rem == std::get<2>(result));
    }

    void test_split(
        const std::string& rule,
        size_t start_pos, size_t max_length,
        std::tuple<std::string_view, char, std::string_view> result)
    {
        argos::WordSplitter splitter;
        splitter.add_word(rule);
        std::string word = rule;
        word.erase(remove(word.begin(), word.end(), ' '), word.end());
        CAPTURE(word, start_pos, max_length);
        auto[txt, sep, rem] = splitter.split(word, start_pos, max_length, false);
        REQUIRE(txt == std::get<0>(result));
        REQUIRE(sep == std::get<1>(result));
        REQUIRE(rem == std::get<2>(result));
    }
}

TEST_CASE("Test default splitter")
{
    argos::WordSplitter splitter;
    test_default_split("decision", 0, 8, {"decision", '\0', {}});
    test_default_split("decision", 0, 7, {"decisi", '-', "on"});
    test_default_split("abcdef123456", 0, 9, {"abcdef", '-', "123456"});
    test_default_split("bbbbbbbbbbb", 0, 7, {"bbbbbb", '-', "bbbbb"});
    test_default_split("bbbbbbccccccdddddd", 6, 7, {"cccccc", '-', "dddddd"});
}

TEST_CASE("Test split double-dash option")
{
    // I'm adding this test case to highlight a behavior that I'm not
    // entirely sure I want.
    test_default_split("--help", 0, 5, {"--", '\0', "help"});
}

TEST_CASE("Test default splitter with UTF-8")
{
    argos::WordSplitter splitter;
    test_default_split(U8("æøå•Ωé†µüıœπ˙äöﬁª√˛¸ƒ∂ß"), 0, 14,
                       {U8("æøå•Ωé†µüıœπ˙"), '-', U8("äöﬁª√˛¸ƒ∂ß")});
    test_default_split(U8("Båidg-hølnow"), 0, 8,
                       {U8("Båidg-"), '\0', U8("hølnow")});
}

TEST_CASE("Test splitter")
{
    argos::WordSplitter splitter;
    test_split("ono mato poe ti con", 2, 8, {"omato", '-', "poeticon"});
    test_split("ono mato poe ti con", 0, 8, {"onomato", '-', "poeticon"});
    test_split("ono mato poe ti con", 0, 7, {"ono", '-', "matopoeticon"});
    test_split("ono mato poe ti con", 7, 8, {"poeticon", '\0', {}});
    test_split("ono mato poe ti con", 7, 7, {"poeti", '-', "con"});
}

TEST_CASE("Test splitter with hyphens")
{
    argos::WordSplitter splitter;
    test_split("multi- tasking", 0, 8, {"multi-", '\0', "tasking"});
}

TEST_CASE("Test splitter with UTF-8")
{
    argos::WordSplitter splitter;
    test_split(U8("Brønn øy sund"), 0, 6, {U8("Brønn"), '-', U8("øysund")});
    test_split(U8("Brønn øy sund"), 0, 7, {U8("Brønn"), '-', U8("øysund")});
    test_split(U8("Brønn øy sund"), 0, 8, {U8("Brønnøy"), '-', U8("sund")});
    test_split(U8("Brønn øy sund"), 0, 10, {U8("Brønnøy"), '-', U8("sund")});
    test_split(U8("Brønn øy sund"), 0, 11, {U8("Brønnøysund"), '\0', {}});
    test_split(U8("Brønn øy sund"), 4, 7, {U8("nnøy"), '-', U8("sund")});
}

TEST_CASE("Test splitter handles case and punctuation.")
{
    argos::WordSplitter splitter;
    splitter.add_word("in compre hensi bili ties");
    auto [txt, sep, rem] = splitter.split("Incomprehensibilities.", 0, 12, false);
    REQUIRE(txt == "Incompre");
    REQUIRE(sep == '-');
    REQUIRE(rem == "hensibilities.");
}
