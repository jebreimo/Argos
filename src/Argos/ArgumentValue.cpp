//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-31.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentValue.hpp"

#include "Argos/ArgumentValues.hpp"
#include "ParseValue.hpp"
#include "ParsedArgumentsImpl.hpp"
#include "StringUtilities.hpp"

namespace Argos
{
    namespace
    {
        template <typename T>
        T getInteger(const ArgumentValue& value, T defaultValue, int base)
        {
            auto s = value.value();
            if (!s)
                return defaultValue;
            auto n = parseInteger<T>(std::string(*s), base);
            if (!n)
                value.error();
            return *n;
        }

        template <typename T>
        T getFloatingPoint(const ArgumentValue& value, T defaultValue)
        {
            auto s = value.value();
            if (!s)
                return defaultValue;
            auto n = parseFloatingPoint<T>(std::string(*s));
            if (!n)
                value.error();
            return *n;
        }
    }

    ArgumentValue::ArgumentValue(std::optional<std::string_view> value,
                                 std::shared_ptr<ParsedArgumentsImpl> args,
                                 ValueId valueId,
                                 ArgumentId argumentId)
        : m_Value(value),
          m_Args(std::move(args)),
          m_ValueId(valueId),
          m_ArgumentId(argumentId)
    {}

    ArgumentValue::ArgumentValue(const ArgumentValue&) = default;

    ArgumentValue::ArgumentValue(ArgumentValue&& ) noexcept = default;

    ArgumentValue::~ArgumentValue() = default;

    ArgumentValue&
    ArgumentValue::operator=(const ArgumentValue&) = default;

    ArgumentValue&
    ArgumentValue::operator=(ArgumentValue&&) noexcept = default;

    std::unique_ptr<IArgumentView>
    ArgumentValue::argument() const
    {
        return m_Args->getArgumentView(m_ArgumentId);
    }

    bool ArgumentValue::hasValue() const
    {
        return m_Value.has_value();
    }

    std::optional<std::string_view> ArgumentValue::value() const
    {
        return m_Value;
    }

    bool ArgumentValue::asBool(bool defaultValue) const
    {
        if (!m_Value)
            return defaultValue;
        return m_Value != "0" && m_Value != "false";
    }

    int ArgumentValue::asInt(int defaultValue, int base) const
    {
        return getInteger<int>(*this, defaultValue, base);
    }

    unsigned ArgumentValue::asUInt(unsigned defaultValue, int base) const
    {
        return getInteger<unsigned>(*this, defaultValue, base);
    }

    long ArgumentValue::asLong(long defaultValue, int base) const
    {
        return getInteger<long>(*this, defaultValue, base);
    }

    long long ArgumentValue::asLLong(long long defaultValue, int base) const
    {
        return getInteger<long long>(*this, defaultValue, base);
    }

    unsigned long
    ArgumentValue::asULong(unsigned long defaultValue, int base) const
    {
        return getInteger<unsigned long>(*this, defaultValue, base);
    }

    unsigned long long
    ArgumentValue::asULLong(unsigned long long defaultValue, int base) const
    {
        return getInteger<unsigned long long>(*this, defaultValue, base);
    }

    float ArgumentValue::asFloat(float defaultValue) const
    {
        return getFloatingPoint<float>(*this, defaultValue);
    }

    double ArgumentValue::asDouble(double defaultValue) const
    {
        return getFloatingPoint<double>(*this, defaultValue);
    }

    std::string ArgumentValue::asString(const std::string& defaultValue) const
    {
        return m_Value ? std::string(*m_Value) : defaultValue;
    }

    ArgumentValues
    ArgumentValue::split(char separator,
                         size_t minParts, size_t maxParts) const
    {
        if (!m_Value)
            return ArgumentValues({}, m_Args, m_ValueId);
        auto parts = splitString(*m_Value, separator, maxParts - 1);
        if (parts.size() < minParts)
        {
            error("Invalid value: \"" + std::string(*m_Value)
                  + "\". Must have at least " + std::to_string(minParts)
                  + " values separated by \"" + separator + "\".");
            return ArgumentValues({}, m_Args, m_ValueId);
        }
        std::vector<std::pair<std::string_view, ArgumentId>> values;
        values.reserve(parts.size());
        for (auto& part : parts)
            values.emplace_back(part, m_ArgumentId);
        return {move(values), m_Args, m_ValueId};
    }

    void ArgumentValue::error(const std::string& message) const
    {
        m_Args->error(message, m_ArgumentId);
    }

    void ArgumentValue::error() const
    {
        error("Invalid value: " + std::string(*m_Value) + ".");
    }
}
