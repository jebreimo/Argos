//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-05.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/TextFormatter.hpp"

#include <sstream>
#include <catch2/catch_test_macros.hpp>
#include "U8Adapter.hpp"

TEST_CASE("Basic test of TextFormatter")
{
    std::stringstream ss;
    argos::TextFormatter formatter(&ss, 40);
    formatter.write_words("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    formatter.flush();
    REQUIRE(ss.str() == "Lorem ipsum dolor sit amet, consectetur\nadipiscing elit.");
}

TEST_CASE("Test TextFormatter with indentation")
{
    std::stringstream ss;
    argos::TextFormatter formatter(&ss, 40);
    formatter.write_words("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    formatter.push_indentation(17);
    formatter.write_words("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    formatter.pop_indentation();
    formatter.write_words(" ");
    formatter.write_words("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
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
    argos::TextFormatter formatter(&ss, 40);
    formatter.write_words("Lorem ipsum dolor\nsit amet, consectetur\nadipiscing elit.");
    formatter.flush();
    REQUIRE(ss.str() == "Lorem ipsum dolor\nsit amet, consectetur\nadipiscing elit.");
}

TEST_CASE("Indentation change")
{
    std::stringstream ss;
    argos::TextFormatter formatter(&ss, 40);
    formatter.write_words("ABCDEFGHIJ");
    formatter.push_indentation(20);
    formatter.write_words("ABCDEFGHIJ");
    formatter.flush();
    REQUIRE(ss.str() == "ABCDEFGHIJ          ABCDEFGHIJ");
}

TEST_CASE("Indentation change with preformatted text")
{
    std::stringstream ss;
    argos::TextFormatter formatter(&ss, 40);
    formatter.write_lines("ABCDEFGHIJ");
    formatter.push_indentation(20);
    formatter.write_lines("ABCDEFGHIJ");
    formatter.flush();
    REQUIRE(ss.str() == "ABCDEFGHIJ          ABCDEFGHIJ");
}

TEST_CASE("Keep whitespace at the start of a line.")
{
    std::stringstream ss;
    argos::TextFormatter formatter(&ss, 40);
    formatter.write_words(" Lorem ipsum dolor\n  sit amet, consectetur\n   adipiscing elit.");
    formatter.flush();
    REQUIRE(ss.str() == " Lorem ipsum dolor\n  sit amet, consectetur\n   adipiscing elit.");
}

TEST_CASE("Preformatted text across several lines")
{
    std::stringstream ss;
    argos::TextFormatter formatter(&ss, 30);
    formatter.push_indentation(5);
    formatter.write_lines("[abc efg]");
    formatter.write_words(" ");
    formatter.write_lines("[abc efg]");
    formatter.write_words(" ");
    formatter.write_lines("[abc efg]");
    formatter.write_words(" ");
    formatter.write_lines("[abc efg]");
    formatter.write_words(" ");
    formatter.write_lines("[abc efg]");
    formatter.flush();
    REQUIRE(ss.str() == "     [abc efg] [abc efg]\n     [abc efg] [abc efg]\n     [abc efg]");
}

TEST_CASE("TextFormatter with multi-byte characters")
{
    std::stringstream ss;
    argos::TextFormatter formatter(&ss, 40);
    formatter.write_words(U8("Lorem ipsum dålår sit åmet, consøctetur adipiscing elit."));
    formatter.flush();
    REQUIRE(ss.str() == U8("Lorem ipsum dålår sit åmet, consøctetur\nadipiscing elit."));
}

TEST_CASE("TextFormatter splitting word, no rule")
{
    std::stringstream ss;
    argos::TextFormatter formatter(&ss, 10);
    formatter.write_words(U8("Brønnøysundsregisteret"));
    formatter.flush();
    REQUIRE(ss.str() == U8("Brønnøysu-\nndsregist-\neret"));
}

TEST_CASE("TextFormatter splitting word with rule")
{
    std::stringstream ss;
    argos::TextFormatter formatter(&ss, 12);
    formatter.word_splitter().add_word("Brønn øy sunds registeret");
    formatter.write_words(U8("Til Brønnøysundsregisteret"));
    formatter.flush();
    REQUIRE(ss.str() == U8("Til Brønnøy-\nsunds-\nregisteret"));
}

TEST_CASE("Text alignment")
{
    std::stringstream ss;
    argos::TextFormatter formatter(&ss, 12);
    formatter.write_words("    Abcd efg hij klm");
    formatter.flush();
    REQUIRE(ss.str() == "    Abcd efg\n    hij klm");
}

TEST_CASE("List item alignment")
{
    std::stringstream ss;
    argos::TextFormatter formatter(&ss, 17);
    formatter.write_words(R"-(My list:
- Abcdef ghijk lmn
    * Abcdef ghij
    * Abcdef ghijk lmn
- Abcdef ghijk lmn
1. Abcdef ghijk lmn
    1. Abcdef ghijk lmn
Abcdef ghijk lmn opqrst)-");
    formatter.flush();
    REQUIRE(ss.str() == R"-(My list:
- Abcdef ghijk
  lmn
    * Abcdef ghij
    * Abcdef
      ghijk lmn
- Abcdef ghijk
  lmn
1. Abcdef ghijk
   lmn
    1. Abcdef
       ghijk lmn
Abcdef ghijk lmn
opqrst)-");
}

TEST_CASE("spaces before newline")
{
    std::stringstream ss;
    argos::TextFormatter formatter(&ss, 12);
    formatter.write_words("abcd \nefgh");
    formatter.flush();
    REQUIRE(ss.str() == "abcd\nefgh");
}
