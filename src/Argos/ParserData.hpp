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
#include "ArgumentData.hpp"
#include "OptionData.hpp"
#include "OptionStyle.hpp"

namespace Argos
{
    //using ArgumentVariant = std::variant<const Argument*, const Option*>;
    struct ParserData
    {
        std::vector<ArgumentData> arguments;
        std::vector<OptionData> options;
        //std::map<std::string, ArgumentVariant> argumentMap;
        std::string programName;

        OptionStyle optionStyle = OptionStyle::STANDARD;
        bool autoExit = true;
        bool allowAbbreviatedOptions = false;
        bool ignoreUndefinedArguments = false;
        bool ignoreMissingArguments = false;
        bool caseInsensitive = false;
    };
}
