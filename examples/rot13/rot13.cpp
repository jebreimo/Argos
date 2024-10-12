//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-08-29.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <climits>
#include <iostream>
#include <Argos/Argos.hpp>

void rotate_and_print(const std::string& word, int n)
{
    for (auto c: word)
    {
        if ('A' <= c && c <= 'Z')
            c = char('A' + ((c - 'A') + n) % 26);
        else if ('a' <= c && c <= 'z')
            c = char('a' + ((c - 'a') + n) % 26);
        std::cout << c;
    }
}

int main(int argc, char* argv[])
{
    using namespace argos;
    const argos::ParsedArguments args = ArgumentParser("example2")
        .about("Obfuscates (or reveals) text with the rot-13 algorithm.")
        .add(Arg("WORD").count(1, INT_MAX).help("One or more words."))
        .add(Opt{"-v", "--verbose"}
            .help("Display additional information."))
        .add(Opt{"-n", "--number"}.argument("NUM")
            .help("Set the number letters are rotated by. Default is 13."))
        .add(Opt{"--"}.type(OptionType::LAST_OPTION)
             .help("Marks the end of the options. Allows words to"
                   " start with dashes ('-')."))
        .parse(argc, argv);

    const std::vector<std::string> words = args.values("WORD").as_strings();
    const bool verbose = args.value("--verbose").as_bool();
    const int n = args.value("--number").as_int(13);

    if (verbose)
        std::cout << "n = " << n << "\n";

    bool first = true;
    for (const auto& word : words)
    {
        if (first)
            first = false;
        else
            std::cout << ' ';
        rotate_and_print(word, n);
    }
    std::cout << std::endl;

    return 0;
}
