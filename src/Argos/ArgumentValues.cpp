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

namespace argos
{
    namespace
    {
        ArgumentId get_argument_id(
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

        void error(const ArgumentValues& values, std::string_view value)
        {
            values.error("Invalid value: " + std::string(value) + ".");
        }

        template <typename T>
        std::vector<T> parse_floats(const ArgumentValues& values,
                                    const std::vector<T>& default_value)
        {
            if (values.empty())
                return default_value;

            std::vector<T> result;
            result.reserve(values.size());
            for (auto& v : values.raw_values())
            {
                auto value = parse_floating_point<T>(std::string(v));
                if (!value)
                    error(values, v);
                else
                    result.push_back(*value);
            }
            return result;
        }

        template <typename T>
        std::vector<T> parse_integers(const ArgumentValues& values,
                                      const std::vector<T>& default_value,
                                      int base)
        {
            if (values.empty())
                return default_value;

            std::vector<T> result;
            result.reserve(values.size());
            for (auto& v : values.raw_values())
            {
                auto value = parse_integer<T>(std::string(v), base);
                if (!value)
                    error(values, v);
                else
                    result.push_back(*value);
            }
            return result;
        }
    }

    ArgumentValues::ArgumentValues(
            std::vector<std::pair<std::string_view, ArgumentId>> values,
            std::shared_ptr<ParsedArgumentsImpl> args,
            ValueId value_id)
        : m_values(std::move(values)),
          m_args(std::move(args)),
          m_value_id(value_id)
    {}

    ArgumentValues::ArgumentValues(const ArgumentValues&) = default;

    ArgumentValues::ArgumentValues(ArgumentValues&&) noexcept = default;

    ArgumentValues::~ArgumentValues() = default;

    ArgumentValues&
    ArgumentValues::operator=(const ArgumentValues&) = default;

    ArgumentValues&
    ArgumentValues::operator=(ArgumentValues&&) noexcept = default;

    ArgumentValues::operator bool() const
    {
        return !m_values.empty();
    }

    ArgumentValue ArgumentValues::operator[](size_t index) const
    {
        return value(index);
    }

    std::vector<std::unique_ptr<IArgumentView>>
    ArgumentValues::arguments() const
    {
        return m_args->get_argument_views(m_value_id);
    }

    void ArgumentValues::error(const std::string& message) const
    {
        m_args->error(message, get_argument_id(m_values));
    }

    bool ArgumentValues::empty() const
    {
        return m_values.empty();
    }

    size_t ArgumentValues::size() const
    {
        return m_values.size();
    }

    std::vector<ArgumentValue> ArgumentValues::values() const
    {
        std::vector<ArgumentValue> result;
        result.reserve(m_values.size());
        for (const auto& [value, arg_id] : m_values)
            result.emplace_back(value, m_args, m_value_id, arg_id);
        return result;
    }

    std::vector<std::string_view> ArgumentValues::raw_values() const
    {
        std::vector<std::string_view> result;
        result.reserve(m_values.size());
        for (const auto& [value, _] : m_values)
            result.push_back(value);
        return result;
    }

    ArgumentValue ArgumentValues::value(size_t index) const
    {
        if (m_values.empty())
            return {{}, m_args, m_value_id, {}};

        const auto& [value, arg_id] = m_values.at(index);
        return {value, m_args, m_value_id, arg_id};
    }

    std::vector<int>
    ArgumentValues::as_ints(const std::vector<int>& default_value,
                            int base) const
    {
        return parse_integers<int>(*this, default_value, base);
    }

    std::vector<unsigned>
    ArgumentValues::as_uints(const std::vector<unsigned>& default_value,
                             int base) const
    {
        return parse_integers<unsigned>(*this, default_value, base);
    }

    std::vector<long>
    ArgumentValues::as_longs(const std::vector<long>& default_value,
                             int base) const
    {
        return parse_integers<long>(*this, default_value, base);
    }

    std::vector<long long>
    ArgumentValues::as_llongs(const std::vector<long long>& default_value,
                              int base) const
    {
        return parse_integers<long long>(*this, default_value, base);
    }

    std::vector<unsigned long>
    ArgumentValues::as_ulongs(const std::vector<unsigned long>& default_value,
                              int base) const
    {
        return parse_integers<unsigned long>(*this, default_value, base);
    }

    std::vector<unsigned long long> ArgumentValues::as_ullongs(
        const std::vector<unsigned long long>& default_value,
        int base) const
    {
        return parse_integers<unsigned long long>(*this, default_value, base);
    }

    std::vector<float>
    ArgumentValues::as_floats(const std::vector<float>& default_value) const
    {
        return parse_floats(*this, default_value);
    }

    std::vector<double>
    ArgumentValues::as_doubles(const std::vector<double>& default_value) const
    {
        return parse_floats(*this, default_value);
    }

    std::vector<std::string> ArgumentValues::as_strings(
            const std::vector<std::string>& default_value) const
    {
        if (m_values.empty())
            return default_value;

        std::vector<std::string> result;
        result.reserve(m_values.size());
        for (const auto& [value, _] : m_values)
            result.emplace_back(value);
        return result;
    }

    ArgumentValues
    ArgumentValues::split(char separator,
                          size_t min_parts, size_t max_parts) const
    {
        std::vector<std::pair<std::string_view, ArgumentId>> values;
        for (auto [value, arg_id] : m_values)
        {
            auto parts = split_string(value, separator, max_parts - 1);
            if (parts.size() < min_parts)
            {
                error("Invalid value: \"" + std::string(value)
                      + "\". Must be at least " + std::to_string(min_parts)
                      + " values separated by \"" + separator + "\".");
            }
            for (auto& part : parts)
                values.emplace_back(part, arg_id);
        }
        return {std::move(values), m_args, m_value_id};
    }

    ArgumentValues
    ArgumentValues::split_n(char separator, size_t num_parts) const
    {
        return split(separator, num_parts, num_parts);
    }

    ArgumentValueIterator ArgumentValues::begin() const
    {
        return {m_values.begin(), m_args, m_value_id};
    }

    ArgumentValueIterator ArgumentValues::end() const
    {
        return {m_values.end(), m_args, m_value_id};
    }
}
