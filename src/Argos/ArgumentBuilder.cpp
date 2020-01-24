//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ArgumentBuilder.hpp"

namespace Argos
{
    ArgumentBuilder::ArgumentBuilder(const std::string& name)
    {
        m_Argument.name = name;
    }

    ArgumentBuilder& ArgumentBuilder::text(const std::string& text)
    {
        m_Argument.text = text;
        return *this;
    }

    ArgumentBuilder& ArgumentBuilder::section(const std::string& name)
    {
        m_Argument.section = name;
        return *this;
    }

    ArgumentBuilder& ArgumentBuilder::valueName(const std::string& id)
    {
        m_Argument.valueName = id;
        return *this;
    }

    ArgumentBuilder& ArgumentBuilder::operation(ArgumentOperation operation)
    {
        m_Argument.operation = operation;
        return *this;
    }

    ArgumentBuilder& ArgumentBuilder::optional(bool optional)
    {
        m_Argument.minCount = optional ? 0 : 1;
        return *this;
    }

    ArgumentBuilder& ArgumentBuilder::count(int n)
    {
        m_Argument.minCount = m_Argument.maxCount = n;
        return *this;
    }

    ArgumentBuilder& ArgumentBuilder::count(int minCount, int maxCount)
    {
        m_Argument.minCount = minCount;
        m_Argument.maxCount = maxCount;
        return *this;
    }

    ArgumentBuilder& ArgumentBuilder::hidden(bool hidden)
    {
        m_Argument.hidden = hidden;
        return *this;
    }

    ArgumentBuilder& ArgumentBuilder::id(int n)
    {
        m_Argument.id = n;
        return *this;
    }

    Argument&& ArgumentBuilder::get()
    {
        return std::move(m_Argument);
    }
}
