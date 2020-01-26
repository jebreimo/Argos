//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "OptionData.hpp"

namespace Argos
{
    class OptionBuilder
    {
    public:
        explicit OptionBuilder(std::vector<std::string> flags);

        OptionBuilder& argument(const std::string& name);

        OptionBuilder& value(const std::string& value);

        OptionBuilder& text(const std::string& text);

        OptionBuilder& section(const std::string& name);

        OptionBuilder& valueName(const std::string& id);

        OptionBuilder& operation(ArgumentOperation operation);

        OptionBuilder& type(OptionType type);

        OptionBuilder& mandatory(bool mandatory);

        OptionBuilder& hidden(bool hidden);

        OptionBuilder& id(int id);

        OptionData&& get();
    private:
        OptionData m_Option;
    };
}
