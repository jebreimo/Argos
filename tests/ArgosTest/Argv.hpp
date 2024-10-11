//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-04-20.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <string>
#include <vector>

struct Argv
{
    Argv(std::initializer_list<std::string> args) : strings(args)
    {
        for (auto& arg : strings)
            argv.push_back(arg.data());
    }

    [[nodiscard]] int size() const
    {
        return static_cast<int>(argv.size());
    }

    char** data()
    {
        return argv.data();
    }

    std::vector<std::string> strings;
    std::vector<char*> argv;
};
