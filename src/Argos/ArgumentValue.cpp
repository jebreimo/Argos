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
        return getValue(defaultValue ? 1 : 0);
    }

    int8_t ArgumentValue::asInt8(int8_t defaultValue) const
    {
        return getValue(defaultValue);
    }

    int16_t ArgumentValue::asInt16(int16_t defaultValue) const
    {
        return getValue(defaultValue);
    }

    int32_t ArgumentValue::asInt32(int32_t defaultValue) const
    {
        return getValue(defaultValue);
    }

    int64_t ArgumentValue::asInt64(int64_t defaultValue) const
    {
        return getValue(defaultValue);
    }

    uint8_t ArgumentValue::asUint8(uint8_t defaultValue) const
    {
        return getValue(defaultValue);
    }

    uint16_t ArgumentValue::asUint16(uint16_t defaultValue) const
    {
        return getValue(defaultValue);
    }

    uint32_t ArgumentValue::asUint32(uint32_t defaultValue) const
    {
        return getValue(defaultValue);
    }

    uint64_t ArgumentValue::asUint64(uint64_t defaultValue) const
    {
        return getValue(defaultValue);
    }

    float ArgumentValue::asFloat(float defaultValue) const
    {
        return getValue(defaultValue);
    }

    double ArgumentValue::asDouble(double defaultValue) const
    {
        return getValue(defaultValue);
    }

    long double ArgumentValue::asLongDouble(long double defaultValue) const
    {
        return getValue(defaultValue);
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
                  + "\". Must be at least " + std::to_string(minParts)
                  + " values separated by \"" + separator + "\".");
            return ArgumentValues({}, m_Args, m_ValueId);
        }
        std::vector<std::pair<std::string_view, ArgumentId>> values;
        for (auto& part : parts)
            values.emplace_back(part, m_ArgumentId);
        return {move(values), m_Args, m_ValueId};
    }

    void ArgumentValue::error(const std::string& message) const
    {
        m_Args->error(message, m_ArgumentId);
    }

    template <typename T>
    T ArgumentValue::getValue(const T& defaultValue) const
    {
        if (!m_Value)
            return defaultValue;
        auto v = parseValue<T>(*m_Value);
        if (!v)
            error("Invalid value: " + std::string(*m_Value) + ".");
        return *v;
    }
}
