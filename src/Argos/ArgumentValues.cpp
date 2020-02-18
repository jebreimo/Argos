//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentValues.hpp"
#include "ParsedArgumentsImpl.hpp"

namespace Argos
{
    ArgumentValues::ArgumentValues(std::vector<std::string_view> values,
                                   std::shared_ptr<ParsedArgumentsImpl> args,
                                   int valueId)
        : m_Values(move(values)),
          m_Args(move(args)),
          m_ValueId(valueId)
    {}

    ArgumentValues::ArgumentValues(const ArgumentValues&) = default;

    ArgumentValues::ArgumentValues(ArgumentValues&&) noexcept = default;

    ArgumentValues::~ArgumentValues() = default;

    ArgumentValues&
    ArgumentValues::operator=(const ArgumentValues&) = default;

    ArgumentValues&
    ArgumentValues::operator=(ArgumentValues&&) noexcept = default;

    std::vector<std::unique_ptr<IArgumentView>>
    ArgumentValues::arguments() const
    {
        return m_Args->getArgumentViews(m_ValueId);
    }

    void ArgumentValues::error(const std::string& message) const
    {
        m_Args->error(message, m_ValueId);
    }

    bool ArgumentValues::empty() const
    {
        return m_Values.empty();
    }

    size_t ArgumentValues::size() const
    {
        return m_Values.size();
    }

    const std::vector<std::string_view>& ArgumentValues::values() const
    {
        return m_Values;
    }
}
