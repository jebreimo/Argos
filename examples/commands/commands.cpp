//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-08-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iostream>
#include <Argos/Argos.hpp>

int main(int argc, char* argv[])
{
    using namespace argos;
    const ParsedArguments args = ArgumentParser(argv[0])
        .about("Performs an operation of some kind.")
        .add(Command("greet")
            .about("Prints a greeting.")
            .add(Arg("NAME").optional(true)
                .help("The person or thing to greet."))
            .add(Opt("-n", "--number").argument("NUM")
                .help("The number of times to repeat the greeting.")))
        .add(Command("congratulate")
            .about("Prints a congratulation.")
            .add(Arg("NAME").optional(true)
                .help("The person or thing to greet."))
            .add(Opt("-o", "--occasion").argument("OCCASION")
                .help("The occasion for the congratulation."))
            .add(Opt("-n", "--number").argument("NUM")
                .help("The number of times to repeat the congratulation.")))
        .parse(argc, argv);

    return 0;
}
