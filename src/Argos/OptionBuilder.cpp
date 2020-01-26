//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "OptionBuilder.hpp"

namespace Argos
{
    OptionBuilder::OptionBuilder(std::vector<std::string> flags)
    {
        m_Option.flags = move(flags);
    }

    OptionBuilder& OptionBuilder::argument(const std::string& name)
    {
        m_Option.argument = name;
        return *this;
    }

    OptionBuilder& OptionBuilder::value(const std::string& value)
    {
        m_Option.value = value;
        return *this;
    }

    OptionBuilder& OptionBuilder::text(const std::string& text)
    {
        m_Option.text = text;
        return *this;
    }

    OptionBuilder& OptionBuilder::section(const std::string& name)
    {
        m_Option.section = name;
        return *this;
    }

    OptionBuilder& OptionBuilder::valueName(const std::string& id)
    {
        m_Option.valueName = id;
        return *this;
    }

    OptionBuilder& OptionBuilder::operation(ArgumentOperation operation)
    {
        m_Option.operation = operation;
        return *this;
    }

    OptionBuilder& OptionBuilder::type(OptionType type)
    {
        m_Option.optionType = type;
        return *this;
    }

    OptionBuilder& OptionBuilder::mandatory(bool mandatory)
    {
        m_Option.mandatory = mandatory;
        return *this;
    }

    OptionBuilder& OptionBuilder::hidden(bool hidden)
    {
        m_Option.hidden = hidden;
        return *this;
    }

    OptionBuilder& OptionBuilder::id(int id)
    {
        m_Option.id = id;
        return *this;
    }

    OptionData&& OptionBuilder::get()
    {
        return std::move(m_Option);
    }
}
