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

namespace Argos
{
    struct ArgumentData
    {
        std::string name;
        std::string text;
        std::string section;
        std::string valueName;
        ArgumentCallback callback;
        unsigned minCount = 1;
        unsigned maxCount = 1;
        Visibility visibility = Visibility::NORMAL;
        int id = 0;
        int valueId_ = 0;
    };
}
