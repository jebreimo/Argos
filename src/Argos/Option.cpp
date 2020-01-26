//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Option.hpp"

namespace Argos
{
    Option::Option(std::vector<std::string> flags)
    {
        m_Option.flags = move(flags);
    }

    Option& Option::argument(const std::string& name)
    {
        m_Option.argument = name;
        return *this;
    }

    Option& Option::value(const std::string& value)
    {
        m_Option.value = value;
        return *this;
    }

    Option& Option::text(const std::string& text)
    {
        m_Option.text = text;
        return *this;
    }

    Option& Option::section(const std::string& name)
    {
        m_Option.section = name;
        return *this;
    }

    Option& Option::valueName(const std::string& id)
    {
        m_Option.valueName = id;
        return *this;
    }

    Option& Option::operation(ArgumentOperation operation)
    {
        m_Option.operation = operation;
        return *this;
    }

    Option& Option::type(OptionType type)
    {
        m_Option.optionType = type;
        return *this;
    }

    Option& Option::mandatory(bool mandatory)
    {
        m_Option.mandatory = mandatory;
        return *this;
    }

    Option& Option::hidden(bool hidden)
    {
        m_Option.hidden = hidden;
        return *this;
    }

    Option& Option::id(int id)
    {
        m_Option.id = id;
        return *this;
    }

    OptionData&& Option::get()
    {
        return std::move(m_Option);
    }
}
