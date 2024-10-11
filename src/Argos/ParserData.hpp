//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-13.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <map>
#include <memory>
#include <variant>
#include "Argos/Enums.hpp"
#include "ArgumentData.hpp"
#include "OptionData.hpp"
#include "CommandData.hpp"
#include "TextFormatter.hpp"

#ifndef ARGOS_EX_USAGE
    #ifdef EX_USAGE
        #define ARGOS_EX_USAGE EX_USAGE
    #else
        #define ARGOS_EX_USAGE 64
    #endif
#endif

namespace argos
{
    struct ParserSettings
    {
        ArgumentCallback argument_callback;
        OptionCallback option_callback;
        OptionStyle option_style = OptionStyle::STANDARD;
        bool auto_exit = true;
        bool allow_abbreviated_options = false;
        bool ignore_undefined_options = false;
        bool ignore_undefined_arguments = false;
        bool case_insensitive = false;
        bool generate_help_option = true;
        int normal_exit_code = 0;
        int error_exit_code = ARGOS_EX_USAGE;
    };

    struct HelpSettings
    {
        std::ostream* output_stream = nullptr;
        unsigned line_width = 0;
        std::vector<std::string> word_split_rules;
    };

    struct ParserData
    {
        CommandData command;
        ParserSettings parser_settings;
        HelpSettings help_settings;
        std::string version;
    };

    void finish_initialization(ParserData& data);
}
