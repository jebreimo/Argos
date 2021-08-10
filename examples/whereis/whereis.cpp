//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-08-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <filesystem>
#include <iostream>
#include <Argos/Argos.hpp>

const char VERSION[] = "1.0.0";

int main(int argc, char* argv[])
{
    auto pathEnv = std::getenv("PATH");

    using namespace Argos;
    auto args = ArgumentParser(argv[0])
        .about("Displays the path (or paths) where the given commands are"
               " located.")
        .add(Argument("COMMAND").count(1, UINT_MAX)
            .text("The command or commands to locate."))
        .add(Option{"-q", "--quiet"}.alias("--verbose").constant(false)
            .text("Do not show additional information (negates --verbose)."))
        .add(Option{"-v", "--verbose"}
            .text("Show additional information."))
        .add(Option{"-p", "--paths"}.argument("<PATH>[:<PATH>]...")
            .initialValue(pathEnv ? pathEnv : std::string())
            .text("Search the given path or paths rather than the ones in"
                  " the PATH environment variable."))
        .add(Option{"--version"}
            .callback([](auto, auto, auto pab)
            {
                std::cout << pab.programName() << " v" << VERSION << "\n";
                return true;
            })
            .type(OptionType::STOP)
            .text("Display the program version and exit."))
        .parse(argc, argv);

    auto dirs = args.values("--paths").split(':').asStrings();
    auto verbose = args.value("--verbose").asBool();

    for (const auto& dir : dirs)
    {
        for (auto command : args.values("COMMAND").asStrings())
        {
            auto path = std::filesystem::path(dir) / command;
            if (std::filesystem::exists(path))
                std::cout << path.string() << '\n';
            else if (verbose)
                std::cout << "not found: " << path.string() << '\n';
        }
    }
    return 0;
}
