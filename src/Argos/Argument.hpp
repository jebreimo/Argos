//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "ArgumentData.hpp"

namespace Argos
{
    class Argument
    {
    public:
        Argument(const std::string& name);

        Argument& text(const std::string& text);

        Argument& section(const std::string& name);

        Argument& valueName(const std::string& id);

        Argument& operation(ArgumentOperation operation);

        Argument& optional(bool optional);

        Argument& count(int n);

        Argument& count(int minCount, int maxCount);

        Argument& hidden(bool hidden);

        Argument& id(int id);

        ArgumentData&& get();
    private:
        ArgumentData m_Argument;
    };
}
