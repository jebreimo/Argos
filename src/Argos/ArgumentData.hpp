//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <map>
#include <variant>
#include "Argument.hpp"
#include "Option.hpp"

namespace Argos
{
    //using ArgumentVariant = std::variant<const Argument*, const Option*>;
    struct ArgumentData
    {
        std::vector<Argument> arguments;
        std::vector<Option> options;
        //std::map<std::string, ArgumentVariant> argumentMap;
        std::multimap<int, std::string> values;
        std::string programName;
        bool autoExit = true;
        bool allowAbbreviatedOptions = false;
        bool ignoreUndefinedArguments = false;
        bool caseInsensitive = false;
    };
}
