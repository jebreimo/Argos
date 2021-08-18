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

#ifdef _WIN32
    #define PATH_SEPARATOR ";"
    const std::vector<std::string> EXTENSIONS = {"", ".exe", ".com",
                                                   ".bat", ".cmd"};
#else
    #define PATH_SEPARATOR ":"
    const std::vector<std::string> EXTENSIONS = {""};
#endif


int main(int argc, char* argv[])
{
    auto pathEnv = std::getenv("PATH");

    using namespace Argos;
    auto args = ArgumentParser("whereis")
        .about("Searches the directories in the PATH environment variable"
               " for the given file (or files).")
        .version(VERSION)
        .add(Argument("FILE").count(1, UINT_MAX)
            .help("The file or files to locate."))
        .add(Option{"-p", "--paths"}
            .argument("<PATH>[" PATH_SEPARATOR "<PATH>]...")
            .initialValue(pathEnv ? pathEnv : std::string())
            .help("Search the given path or paths rather than the ones in"
                  " the PATH environment variable. Use " PATH_SEPARATOR
                  " as separator between the different paths."))
        .add(Option{"-e", "--extensions"}
            .argument("<EXT>[" PATH_SEPARATOR "<EXT>]...")
            .operation(OptionOperation::APPEND)
            .help("File name extensions to test while looking for FILE. Must"
                  " include the leading '.'. This option can be used"
                  " multiple times, multiple extensions can be set at once"
                  " by separating them with '" PATH_SEPARATOR "'."))
        .add(Option{"-q", "--quiet"}.alias("--verbose")
            .constant(false)
            .help("Do not show additional information (negates --verbose)."))
        .add(Option{"-v", "--verbose"}
            .help("Show additional information."))
        .add(Option{"--"}.type(OptionType::LAST_OPTION)
             .help("Marks the end of the options. Makes it possible to look"
                   " for file names starting with dashes ('-')."))
        .parse(argc, argv);

    auto dirs = args.values("--paths").split(PATH_SEPARATOR[0]).asStrings();
    auto verbose = args.value("--verbose").asBool();
    auto extensions = args.values("--extensions").split(PATH_SEPARATOR[0])
                          .asStrings(EXTENSIONS);

    for (const auto& dir : dirs)
    {
        std::filesystem::path dirPath(dir);
        for (auto cmd : args.values("FILE").asStrings())
        {
            bool found = false;
            for (const auto& extension : extensions)
            {
                auto path = dirPath / (cmd + extension);
                if (std::filesystem::exists(path))
                {
                    std::cout << path.string() << '\n';
                    found = true;
                }
            }
            if (!found && verbose)
                std::cout << "not found: " << dirPath.string() << '\n';
        }
    }
    return 0;
}
