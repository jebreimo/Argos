//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentValues.hpp"

#include "Argos/ParseValue.hpp"
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

    std::vector<int8_t> ArgumentValues::asInt8s(const std::vector<int8_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<int16_t> ArgumentValues::asInt16s(const std::vector<int16_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<int32_t> ArgumentValues::asInt32s(
            const std::vector<int32_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<int64_t> ArgumentValues::asInt64s(const std::vector<int64_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<uint8_t> ArgumentValues::asUint8s(const std::vector<uint8_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<uint16_t>
    ArgumentValues::asUint16s(const std::vector<uint16_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<uint32_t>
    ArgumentValues::asUint32s(const std::vector<uint32_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<uint64_t>
    ArgumentValues::asUint64s(const std::vector<uint64_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<float> ArgumentValues::asFloats(const std::vector<float>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<double> ArgumentValues::asDoubles(const std::vector<double>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<long double>
    ArgumentValues::asLongDoubles(const std::vector<long double>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<std::string>
    ArgumentValues::asStrings(const std::vector<std::string>& defaultValue) const
    {
        if (m_Values.empty())
            return defaultValue;

        std::vector<std::string> result;
        result.reserve(m_Values.size());
        for (auto& v : m_Values)
            result.emplace_back(v);
        return result;
    }

    template <typename T>
    std::vector<T> ArgumentValues::getValues(
            const std::vector<T>& defaultValue) const
    {
        if (m_Values.empty())
            return defaultValue;

        std::vector<T> result;
        result.reserve(m_Values.size());
        for (auto& v : m_Values)
        {
            auto value = parseValue<T>(v);
            if (!value)
            {
                m_Args->error("Invalid value: " + std::string(v) + ".",
                              m_ValueId);
            }
            result.push_back(*value);
        }
        return result;
    }
}
