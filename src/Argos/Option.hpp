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
    class Option
    {
    public:
        explicit Option(std::vector<std::string> flags);

        Option& argument(const std::string& name);

        Option& value(const std::string& value);

        Option& text(const std::string& text);

        Option& section(const std::string& name);

        Option& valueName(const std::string& id);

        Option& operation(ArgumentOperation operation);

        Option& type(OptionType type);

        Option& mandatory(bool mandatory);

        Option& hidden(bool hidden);

        Option& id(int id);

        OptionData&& get();
    private:
        OptionData m_Option;
    };
}
