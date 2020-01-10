//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Argument.hpp"

namespace Argos
{
    class ArgumentBuilder
    {
    public:
        ArgumentBuilder(Argument* argument);

        ArgumentBuilder& text(const std::string& text);

        ArgumentBuilder& section(const std::string& name);

        ArgumentBuilder& id(const std::string& id);

        ArgumentBuilder& operation(ArgumentOperation operation);

        ArgumentBuilder& optional(bool optional);

        ArgumentBuilder& count(int n);

        ArgumentBuilder& count(int minCount, int maxCount);

        ArgumentBuilder& hidden(bool hidden);
    private:
        Argument* m_Argument;
    };
}
