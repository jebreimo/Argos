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
        return getValue<int>(name, defaultValue ? 1 : 0) != 0;
    }

    int32_t ParsedArguments::getInt32(const std::string& name,
                                      int32_t defaultValue) const
    {
        return getValue<int32_t>(name, defaultValue);
    }

    int64_t ParsedArguments::getInt64(const std::string& name,
                                      int64_t defaultValue) const
    {
        return getValue<int64_t>(name, defaultValue);
    }

    std::string ParsedArguments::getString(
            const std::string& name,
            const std::string& defaultValue) const
    {
        auto id = m_Impl->getValueId(name);
        auto value = m_Impl->getValue(id);
        if (!value)
            return defaultValue;
        return std::string(*value);
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

    template <typename T>
    T ParsedArguments::getValue(const std::string& name,
                                const T& defaultValue) const
    {
        auto id = m_Impl->getValueId(name);
        auto value = m_Impl->getValue(id);
        if (!value)
            return defaultValue;
        return parseValue<T>({*value, ArgumentValueRef(m_Impl->parserData(), id)}).first;
    }
}
