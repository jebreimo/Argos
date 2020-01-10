//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Option.hpp"

namespace Argos
{
    class OptionBuilder
    {
    public:
        OptionBuilder(Option* option);

        OptionBuilder& text(const std::string& text);

        OptionBuilder& section(const std::string& name);

        OptionBuilder& id(const std::string& id);

        OptionBuilder& operation(ArgumentOperation operation);

        OptionBuilder& mandatory(bool mandatory);

        OptionBuilder& type();

        OptionBuilder& count(int minCount, int maxCount);

        OptionBuilder& hidden(bool hidden);
    private:
        Option* m_Option;
    };
}
