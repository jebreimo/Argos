//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <map>
#include <memory>
#include <variant>
#include "Argos/Enums.hpp"
#include "ArgumentData.hpp"
#include "OptionData.hpp"
#include "TextFormatter.hpp"

namespace Argos
{
    struct ParserSettings
    {
        ArgumentCallback argumentCallback;
        OptionCallback optionCallback;
        OptionStyle optionStyle = OptionStyle::STANDARD;
        bool autoExit = true;
        bool allowAbbreviatedOptions = false;
        bool ignoreUndefinedOptions = false;
        bool ignoreUndefinedArguments = false;
        bool caseInsensitive = false;
    };

    struct HelpSettings
    {
        std::string programName;
        std::map<TextId, std::string> texts;
    };

    struct ParserData
    {
        std::vector<std::unique_ptr<ArgumentData>> arguments;
        std::vector<std::unique_ptr<OptionData>> options;

        ParserSettings parserSettings;
        HelpSettings helpSettings;

        TextFormatter textFormatter;
    };
}
