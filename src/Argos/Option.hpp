//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <string>
#include <vector>
#include "ArgumentOperation.hpp"

namespace Argos
{
    enum class OptionType
    {
        NORMAL,
        HELP,
        BREAK,
        FINAL
    };

    struct Option
    {
        std::vector<std::string> flags;
        std::string text;
        std::string section;
        std::string id;
        std::string argument;
        std::string value;
        ArgumentOperation operation = ArgumentOperation::ASSIGN;
        OptionType optionType = OptionType::NORMAL;
        bool hidden = false;
        bool mandatory = false;
        int m_InternalId;
    };
}
