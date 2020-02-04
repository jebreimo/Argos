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

    bool ParsedArguments::getBool(const std::string& name, bool defaultValue) const
    {
        auto id = m_Impl->getValueId(name);
        auto value = m_Impl->getValue(id);
        if (!value)
            return defaultValue;
        return parseValue<int>({*value, ArgumentValueRef(m_Impl->parserData(), id)}).first;
    }

    bool ParsedArguments::has(const std::string& name) const
    {
        return m_Impl->has(m_Impl->getValueId(name));
    }

    ParserResultCode ParsedArguments::resultCode() const
    {
        return m_Impl->resultCode();
    }

    OptionView ParsedArguments::specialOption() const
    {
        auto option = m_Impl->specialOption();
        if (!option)
            ARGOS_THROW("There is no special option.");
        return OptionView(option);
    }
}
