//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-03-31.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <Argos/Argos.hpp>

/*
    ... more includes ...
*/

int main(int argc, char* argv[])
{
    using namespace Argos;
    /* Define the arguments and options and parse argc and argv */
    auto args = ArgumentParser(argv[0], true)
        .text("This program doesn't do anything apart from"
              " demonstrating command line argument parsing with"
              " Argos, but one can imagine it reads one or more data"
              " files, analyzes their contents and outputs"
              " a PNG file.")
        .add(Argument("data file").count(1, 100)
                 .text("Paths to the input files."))
        .add(Argument("output file")
                 .text("Path to the output file."))
        .add(Option{"-s", "--size"}.argument("<WIDTH>,<HEIGHT>")
                 .text("Width and height of the image, separated by a"
                       " comma. Default: 800,600."))
        .add(Option{"-c", "--color"}.argument("RGB")
                 .text("Foreground color as RGB integer value."
                       " Default: 0xFF0000 (red)."))
        .add(Option{"-b", "--bgcolor"}.argument("RGB")
                 .text("Background color as RGB integer value."
                       " Default: 0 (black)."))
        .add(Option{"--quiet"}.value("--verbose").constant(false)
                 .text("Opposite of verbose, the program will not"
                       " display additional run-time information."))
        .add(Option{"--verbose"}
                 .text("Display additional run-time information."))
        .parse(argc, argv);

    /* Read the values of the arguments and options */
    auto inputFiles = args.values("data file").asStrings();
    auto outputFile = args.value("output file").asString();
    auto verbose = args.value("--verbose").asBool();
    auto fgColor = args.value("--color").asUInt(0xFF0000, 0);
    auto bgColor = args.value("--bgcolor").asUInt(0, 0);
    auto size = args.value("--size").split(',', 2).asUInts({800, 600});

    /*
        ... the actual program code ...
    */

    return 0;
}
