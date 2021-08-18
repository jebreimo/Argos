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

#ifndef ARGOS_EX_USAGE
    #ifdef EX_USAGE
        #define ARGOS_EX_USAGE EX_USAGE
    #else
        #define ARGOS_EX_USAGE 64
    #endif
#endif

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
        bool generateHelpOption = true;
        int errorExitCode = ARGOS_EX_USAGE;
    };

    struct HelpSettings
    {
        std::string programName;
        std::string version;
        std::map<TextId, std::string> texts;
        std::ostream* outputStream = nullptr;
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
