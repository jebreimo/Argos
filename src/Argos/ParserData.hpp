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
#include "Argos/ArgosEnums.hpp"

namespace Argos
{
    struct ParserSettings
    {
        OptionStyle optionStyle = OptionStyle::STANDARD;
        bool autoExit = true;
        bool allowAbbreviatedOptions = false;
        bool ignoreUndefinedOptions = false;
        bool ignoreUndefinedArguments = false;
        bool caseInsensitive = false;
    };

    enum class TextId
    {
        USAGE_TITLE,
        USAGE,
        USAGE_DESCRIPTION,
        ARGUMENTS_TITLE,
        ARGUMENTS,
        OPTIONS_TITLE,
        OPTIONS,
        END_TEXT
    };

    struct HelpSettings
    {
        std::ostream* stream = nullptr;
        std::string programName;
        std::map<TextId, std::string> m_Texts;
    };

    struct ParserData
    {
        std::vector<std::unique_ptr<ArgumentData>> arguments;
        std::vector<std::unique_ptr<OptionData>> options;

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
