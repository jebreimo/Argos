//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argument.hpp"

namespace Argos
{
    Argument::Argument(const std::string& name)
    {
        m_Argument.name = name;
    }

    Argument& Argument::text(const std::string& text)
    {
        m_Argument.text = text;
        return *this;
    }

    Argument& Argument::section(const std::string& name)
    {
        m_Argument.section = name;
        return *this;
    }

    Argument& Argument::valueName(const std::string& id)
    {
        m_Argument.valueName = id;
        return *this;
    }

    Argument& Argument::operation(ArgumentOperation operation)
    {
        m_Argument.operation = operation;
        return *this;
    }

    Argument& Argument::optional(bool optional)
    {
        m_Argument.minCount = optional ? 0 : 1;
        return *this;
    }

    Argument& Argument::count(int n)
    {
        m_Argument.minCount = m_Argument.maxCount = n;
        return *this;
    }

    Argument& Argument::count(int minCount, int maxCount)
    {
        m_Argument.minCount = minCount;
        m_Argument.maxCount = maxCount;
        return *this;
    }

    Argument& Argument::hidden(bool hidden)
    {
        m_Argument.hidden = hidden;
        return *this;
    }

    Argument& Argument::id(int id)
    {
        m_Argument.id = id;
        return *this;
    }

    ArgumentData&& Argument::get()
    {
        return std::move(m_Argument);
    }
}
