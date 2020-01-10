//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <string>
#include "ArgumentBuilder.hpp"
#include "ParserResult.hpp"
#include "Option.hpp"

namespace Argos
{
    class ArgumentParser
    {
    public:
        ArgumentParser(const std::vector<Argument>& arguments,
                       const std::vector<Option>& options);

        ParserResult parse(int argc, char* argv[]);
    private:
        std::vector<std::pair<Argument, int>> m_Arguments;
        std::vector<std::pair<Option, int>> m_Options;
        std::map<std::string, int> m_NameIds;
    };
}
