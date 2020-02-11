//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-04.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <Argos/ArgumentParser.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    using namespace Argos;
    auto args = ArgumentParser()
            .allowAbbreviatedOptions(true)
            .add(Argument("file").text("A file of some kind."))
            .add(Option({"-h", "--help"}).type(OptionType::HELP).text("Show help."))
            .add(Option({"-r", "--resolution"}).argument("HOR,VER").text("Set screen resolution."))
            .add(Option({"--fullscreen"}).value(1).text("Run in fullscreen mode."))
            .add(Option({"--windowed"}).valueName("--fullscreen").value(0).text("Run in windowed mode."))
            .add(Option({"--list-interfaces"}).type(OptionType::BREAK).text("Display list of available graphics interfaces."))
            .add(Option({"--"}).type(OptionType::FINAL).text("Mark end of options. Allows arguments starting with '-'."))
            .add(Option({"-a", "--anonymous"}))
            .parse(argc, argv);
    std::cout << "file: " << args.getString("file") << "\n";
    return 0;
}
