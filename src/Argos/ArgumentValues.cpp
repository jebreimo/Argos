//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentValues.hpp"

#include "Argos/ArgumentValue.hpp"
#include "ParseValue.hpp"
#include "ParsedArgumentsImpl.hpp"
#include "StringUtilities.hpp"

namespace Argos
{
    namespace
    {
        ArgumentId getArgumentId(
                const std::vector<std::pair<std::string_view, ArgumentId>>& values)
        {
            if (values.empty())
                return {};
            for (auto it = next(values.begin()); it != values.end(); ++it)
            {
                if (it->second != prev(it)->second)
                    return {};
            }
            return values.front().second;
        }
    }

    ArgumentValues::ArgumentValues(
            std::vector<std::pair<std::string_view, ArgumentId>> values,
            std::shared_ptr<ParsedArgumentsImpl> args,
            ValueId valueId)
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
        m_Args->error(message, getArgumentId(m_Values));
    }

    bool ArgumentValues::empty() const
    {
        return m_Values.empty();
    }

    size_t ArgumentValues::size() const
    {
        return m_Values.size();
    }

    std::vector<ArgumentValue> ArgumentValues::values() const
    {
        std::vector<ArgumentValue> result;
        for (auto& v : m_Values)
            result.emplace_back(v.first, m_Args, m_ValueId, v.second);
        return result;
    }

    std::vector<std::string_view> ArgumentValues::rawValues() const
    {
        std::vector<std::string_view> result;
        for (auto& s : m_Values)
            result.push_back(s.first);
        return result;
    }

    ArgumentValue ArgumentValues::value(size_t index) const
    {
        if (m_Values.empty())
            return {{}, m_Args, m_ValueId, {}};

        auto& v = m_Values.at(index);
        return {v.first, m_Args, m_ValueId, v.second};
    }

    std::vector<int8_t>
    ArgumentValues::asInt8s(const std::vector<int8_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<int16_t>
    ArgumentValues::asInt16s(const std::vector<int16_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<int32_t>
    ArgumentValues::asInt32s(const std::vector<int32_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<int64_t>
    ArgumentValues::asInt64s(const std::vector<int64_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<uint8_t>
    ArgumentValues::asUint8s(const std::vector<uint8_t>& defaultValue) const
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

    std::vector<float>
    ArgumentValues::asFloats(const std::vector<float>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<double>
    ArgumentValues::asDoubles(const std::vector<double>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<long double> ArgumentValues::asLongDoubles(
            const std::vector<long double>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<std::string> ArgumentValues::asStrings(
            const std::vector<std::string>& defaultValue) const
    {
        if (m_Values.empty())
            return defaultValue;

        std::vector<std::string> result;
        result.reserve(m_Values.size());
        for (auto& v : m_Values)
            result.emplace_back(v.first);
        return result;
    }

    ArgumentValues
    ArgumentValues::split(char separator,
                          size_t minParts, size_t maxParts) const
    {
        std::vector<std::pair<std::string_view, ArgumentId>> values;
        for (auto value : m_Values)
        {
            auto parts = splitString(value.first, separator, maxParts - 1);
            if (parts.size() < minParts)
            {
                error("Invalid value: \"" + std::string(value.first)
                      + "\". Must be at least " + std::to_string(minParts)
                      + " values separated by \"" + separator + "\".");
                return {{}, m_Args, m_ValueId};
            }
            for (auto& part : parts)
                values.emplace_back(part, value.second);
        }
        return {move(values), m_Args, m_ValueId};
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
            auto value = parseValue<T>(v.first);
            if (!value)
                error("Invalid value: " + std::string(v.first) + ".");
            result.push_back(*value);
        }
        return result;
    }
}
