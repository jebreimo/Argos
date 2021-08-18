//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-04.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <Argos/Argos.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
    using namespace Argos;
    auto args = ArgumentParser("ArgosTestApp")
            .allowAbbreviatedOptions(true)
            .add(Argument("file").count(1, UINT_MAX)
                .help("A file of some kind."))
            .add(Option({"-r", "--resolution"}).argument("HOR,VER")
                .help("Set screen resolution."))
            .add(Option({"--fullscreen"}).constant(true)
                .help("Run in fullscreen mode."))
            .add(Option({"--windowed"}).alias("--fullscreen").constant(false)
                .help("Run in windowed mode."))
            .add(Option({"--list-interfaces"}).type(OptionType::LAST_ARGUMENT)
                .help("Display list of available graphics interfaces."))
            .add(Option({"--"}).type(OptionType::LAST_OPTION)
                .help("Mark end of options. Allows arguments starting with '-'."))
            .add(Option({"--version"}).type(OptionType::STOP)
                .help("Show version."))
            .add(Option({"--none"}).operation(OptionOperation::NONE)
                .help("Nothing."))
            .add(Option({"-a", "--anonymous"}).visibility(Visibility::USAGE))
            .parse(argc, argv);

    print(std::cout, args);

    return 0;
}
