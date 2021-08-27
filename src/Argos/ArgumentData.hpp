//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <string>
#include "Argos/Callbacks.hpp"
#include "Argos/Enums.hpp"

namespace argos
{
    struct ArgumentData
    {
        std::string name;
        std::string help;
        std::string section;
        std::string value;
        ArgumentCallback callback;
        unsigned min_count = 1;
        unsigned max_count = 1;
        Visibility visibility = Visibility::NORMAL;
        int id = 0;
        ValueId value_id = {};
        ArgumentId argument_id = {};
    };
}
