//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <Argos/ArgosException.hpp>
#include "Argos/ParsedArguments.hpp"

#include "ParsedArgumentsImpl.hpp"
#include "Argos/ParseValue.hpp"
#include "HelpWriter.hpp"

namespace Argos
{
    ParsedArguments::ParsedArguments() = default;

    ParsedArguments::ParsedArguments(std::unique_ptr<ParsedArgumentsImpl> impl)
        : m_Impl(move(impl))
    {}

    ParsedArguments::ParsedArguments(ParsedArguments&& rhs) noexcept
        : m_Impl(move(rhs.m_Impl))
    {}

    ParsedArguments::~ParsedArguments() = default;

    ParsedArguments& ParsedArguments::operator=(ParsedArguments&& rhs) noexcept
    {
        m_Impl = move(rhs.m_Impl);
        return *this;
    }

    std::vector<std::string> ParsedArguments::getStrings(
            const std::string& name,
            const std::vector<std::string>& defaultValue) const
    {
        auto id = m_Impl->getValueId(name);
        auto values = m_Impl->getValues(id);
        if (values.empty())
            return defaultValue;
        std::vector<std::string> result;
        for (auto sv : values)
            result.emplace_back(sv);
        return result;
    }

    ArgumentValue ParsedArguments::value(const std::string& name) const
    {
        auto id = m_Impl->getValueId(name);
        auto value = m_Impl->getValue(id);
        return {value, m_Impl, id};
    }

    ArgumentValues ParsedArguments::values(const std::string& name) const
    {
        auto id = m_Impl->getValueId(name);
        auto values = m_Impl->getValues(id);
        return {values, m_Impl, id};
    }

    bool ParsedArguments::has(const std::string& name) const
    {
        return m_Impl->has(m_Impl->getValueId(name));
    }

    ParserResultCode ParsedArguments::resultCode() const
    {
        return m_Impl->resultCode();
    }

    OptionView ParsedArguments::stopOption() const
    {
        auto option = m_Impl->breakingOption();
        if (!option)
            ARGOS_THROW("There is no special option.");
        return OptionView(option);
    }

    const std::vector<std::string>& ParsedArguments::arguments() const
    {
        return m_Impl->arguments();
    }

    const std::vector<std::string>& ParsedArguments::unprocessedArguments() const
    {
        return m_Impl->unprocessedArguments();
    }
}
