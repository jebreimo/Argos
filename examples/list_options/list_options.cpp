//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-09-04.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iostream>
#include <Argos/Argos.hpp>

int main(int argc, char* argv[])
{
    using namespace argos;
    auto args = ArgumentParser(argv[0])
        .about("A small example that demonstrates different ways to work"
               " with options that build lists of values.")
        .add(Opt{"-s", "--add-single"}.argument("VALUE")
            .operation(OptionOperation::APPEND)
            .help("Add a single value to a list."))
        .add(Opt{"-m", "--add-multi"}.argument("VALUE[:VALUE]...")
            .operation(OptionOperation::APPEND)
            .help("Add one or more values to a list. Use colon, ':', as"
                  " separator when supplying more than one value."))
        .parse(argc, argv);

    std::cout << "single (one value at a time):\n";
    for (const auto value : args.values("--add-single"))
        std::cout << "  " << value.as_string() << "\n";

    std::cout << "single (all at once):\n";
    for (const auto value : args.values("--add-single").as_strings())
        std::cout << "  " << value << "\n";

    std::cout << "multi (one value at a time):\n";
    for (const auto value : args.values("--add-multi").split(':'))
        std::cout << "  " << value.as_string() << "\n";

    std::cout << "multi (all at once):\n";
    for (const auto value : args.values("--add-multi").split(':').as_strings())
        std::cout << "  " << value << "\n";

    return 0;
}
