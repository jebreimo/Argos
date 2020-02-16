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

    double ParsedArguments::getDouble(const std::string& name, double defaultValue) const
    {
        return getValue<double>(name, defaultValue);
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

    //std::pair<std::vector<std::string>, ArgumentValue>
    //ParsedArguments::getVector(const std::string& name,
    //                           const std::vector<std::string>& defaultValue) const
    //{
    //    auto id = m_Impl->getValueId(name);
    //    auto values = m_Impl->getValues(id);
    //    if (values.empty())
    //        return {defaultValue, ArgumentValue(m_Impl->parserData(), id)};
    //    std::pair result(std::vector<std::string>(),
    //                     ArgumentValue(m_Impl->parserData(), id));
    //    for (auto sv : values)
    //        result.first.emplace_back(sv);
    //    return result;
    //}

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
        auto v = parseSingleValue<T>(std::string(*value));
        if (!v)
            m_Impl->error("Invalid value: " + std::string(*value) + ".", id);
        return *v;
    }
}
