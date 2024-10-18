//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-10-18.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/Enums.hpp"

namespace argos
{
#define ENUM_CASE(x) case TextId::x: return #x

    std::string to_string(TextId textId)
    {
        switch (textId)
        {
        ENUM_CASE(INITIAL_TEXT);
        ENUM_CASE(USAGE_TITLE);
        ENUM_CASE(USAGE);
        ENUM_CASE(ABOUT);
        ENUM_CASE(SUBCOMMANDS_TITLE);
        ENUM_CASE(ARGUMENTS_TITLE);
        ENUM_CASE(OPTIONS_TITLE);
        ENUM_CASE(FINAL_TEXT);
        ENUM_CASE(ERROR_USAGE);
        ENUM_CASE(HELP);
        default:
            return "<Unknown>";
        }
    }
}
