//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgosException.hpp"
#include "Argos/ParsedArguments.hpp"

#include "ParsedArgumentsImpl.hpp"

namespace Argos
{
    ParsedArguments::ParsedArguments() = default;

    ParsedArguments::ParsedArguments(std::shared_ptr<ParsedArgumentsImpl> impl)
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

    bool ParsedArguments::has(const std::string& name) const
    {
        return m_Impl->has(m_Impl->getValueId(name));
    }

    bool ParsedArguments::has(const IArgumentView& arg) const
    {
        return m_Impl->has(arg.valueId());
    }

    ArgumentValue ParsedArguments::value(const std::string& name) const
    {
        auto id = m_Impl->getValueId(name);
        auto value = m_Impl->getValue(id);
        return {value, m_Impl, id};
    }

    ArgumentValue ParsedArguments::value(const IArgumentView& arg) const
    {
        auto value = m_Impl->getValue(arg.valueId());
        return {value, m_Impl, arg.valueId()};
    }

    ArgumentValues ParsedArguments::values(const std::string& name) const
    {
        auto id = m_Impl->getValueId(name);
        auto values = m_Impl->getValues(id);
        return {values, m_Impl, id};
    }

    ArgumentValues ParsedArguments::values(const IArgumentView& arg) const
    {
        auto values = m_Impl->getValues(arg.valueId());
        return {values, m_Impl, arg.valueId()};
    }

    std::vector<std::unique_ptr<ArgumentView>>
    ParsedArguments::allArguments() const
    {
        std::vector<std::unique_ptr<ArgumentView>> result;
        for (auto& a : m_Impl->parserData()->arguments)
            result.emplace_back(std::make_unique<ArgumentView>(a.get()));
        return result;
    }

    std::vector<std::unique_ptr<OptionView>>
    ParsedArguments::allOptions() const
    {
        std::vector<std::unique_ptr<OptionView>> result;
        for (auto& o : m_Impl->parserData()->options)
            result.emplace_back(std::make_unique<OptionView>(o.get()));
        return result;
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

    const std::vector<std::string>& ParsedArguments::unprocessedArguments() const
    {
        return m_Impl->unprocessedArguments();
    }
}
