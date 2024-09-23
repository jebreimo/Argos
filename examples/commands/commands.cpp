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
    try
    {
        using namespace argos;
        const ParsedArguments args = ArgumentParser(argv[0])
            .about("Performs an operation of some kind.")
            .version("1.0.0")
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

        const auto subcommand = args.subcommands()[0];
        std::string a;
        if (subcommand.command_name() == "greet")
        {
            a = "Hello";
        }
        else if (subcommand.command_name() == "congratulate")
        {
            a = "Congratulations";
            if (const auto occasion = subcommand.value("--occasion"))
                a += " on your " + occasion.as_string();
        }

        const int n = subcommand.value("--number").as_int(1);
        const auto name = subcommand.value("NAME").as_string("world");
        for (int i = 0; i < n; ++i)
        {
            std::cout << a << ", " << name;
            std::cout << "!\n";
        }
    }
    catch (std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}
