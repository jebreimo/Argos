//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-08-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iostream>
#include <Argos/Argos.hpp>

/**
 * @file
 * Demonstrates how a program can have an option, e.g. --include, where
 * "--include=VALUE" adds "VALUE" to a list, while just "--include" empties
 * the same list.
 */

int main(int argc, char* argv[])
{
    using namespace argos;
    auto args = ArgumentParser(argv[0])
        .about("Demonstrates APPEND and CLEAR options.")
        .add(Option{"-i", "--include="}.argument("VALUE")
                 .operation(OptionOperation::APPEND)
                 .alias("--include")
                 .help("Appends VALUE to the list of values."))
        .add(Option{"--include"}
                 .operation(OptionOperation::CLEAR)
                 .help("Clears the list of values."))
        .parse(argc, argv);

    for (auto str : args.values("--include").as_strings())
        std::cout << str << "\n";

    return 0;
}
