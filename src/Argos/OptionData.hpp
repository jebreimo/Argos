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
#include "TextSource.hpp"

namespace argos
{
    struct OptionData
    {
        std::vector<std::string> flags;
        TextSource help;
        std::string section;
        std::string alias;
        std::string argument;
        std::string constant;
        std::string initial_value;
        OptionCallback callback;
        OptionOperation operation = OptionOperation::ASSIGN;
        OptionType type = OptionType::NORMAL;
        Visibility visibility = Visibility::NORMAL;
        bool optional = true;
        int id = 0;
        ArgumentId argument_id = {};
        ValueId value_id = {};
    };

    void validate_and_update(OptionData& option, OptionStyle style);
}
