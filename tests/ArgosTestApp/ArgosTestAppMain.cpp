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
            .add(Option({"-h", "--help"}).type(OptionType::HELP).text("Show help."))
            .add(Argument("file").text("A file of some kind."))
            .parse(argc, argv);
    std::cout << "file: " << args.getString("file") << "\n";
    return 0;
}
