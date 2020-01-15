//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <string>
#include "ArgumentOperation.hpp"

namespace Argos
{
    struct Argument
    {
        std::string name;
        std::string text;
        std::string section;
        std::string id;
        ArgumentOperation operation = ArgumentOperation::ASSIGN;
        int minCount = 1;
        int maxCount = 1;
        bool hidden = false;
        int m_InternalId;
    };
}
