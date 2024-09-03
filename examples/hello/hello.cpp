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
    const argos::ParsedArguments args = argos::ArgumentParser(argv[0])
        .about("Displays a greeting to someone or something.")
        .add(argos::Arg("NAME").optional(true)
            .help("The person or thing to greet."))
        .add(argos::Opt("-n", "--number").argument("NUM")
            .help("The number of times to repeat the greeting."))
        .parse(argc, argv);

    int n = args.value("--number").as_int(1);
    for (int i = 0; i < n; ++i)
    {
        std::cout << "Hello "
                  << args.value("NAME").as_string("world")
                  << "!\n";
    }

    return 0;
}
