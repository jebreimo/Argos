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
#include "Argos/OptionStyle.hpp"

namespace Argos
{
    struct ParserSettings
    {
        OptionStyle optionStyle = OptionStyle::STANDARD;
        bool autoExit = true;
        bool allowAbbreviatedOptions = false;
        bool ignoreUndefinedArguments = false;
        bool ignoreMissingArguments = false;
        bool caseInsensitive = false;
    };

    struct HelpSettings
    {
        std::string programName;
    };

    struct ParserData
    {
        std::vector<std::shared_ptr<ArgumentData>> arguments;
        std::vector<std::shared_ptr<OptionData>> options;

        ParserSettings parserSettings;
        HelpSettings helpSettings;

        //std::string programName;
        //
        //OptionStyle optionStyle = OptionStyle::STANDARD;
        //bool autoExit = true;
        //bool allowAbbreviatedOptions = false;
        //bool ignoreUndefinedArguments = false;
        //bool ignoreMissingArguments = false;
        //bool caseInsensitive = false;
    };
}
