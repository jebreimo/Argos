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
#include "Argos/Callbacks.hpp"
#include "Argos/Enums.hpp"

namespace Argos
{
    struct OptionData
    {
        std::vector<std::string> flags;
        std::string text;
        std::string section;
        std::string valueName;
        std::string argument;
        std::string value;
        OptionCallback callback;
        OptionOperation operation = OptionOperation::ASSIGN;
        OptionType type = OptionType::NORMAL;
        Visibility visibility = Visibility::NORMAL;
        bool mandatory = false;
        int id = 0;
        int valueId_ = 0;
    };
}
