//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-31.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentValue.hpp"

#include "Argos/ArgumentValues.hpp"
#include "ArgosThrow.hpp"
#include "ParseValue.hpp"
#include "ParsedArgumentsImpl.hpp"
#include "StringUtilities.hpp"

namespace argos
{
    namespace
    {
        template <typename T>
        T get_integer(const ArgumentValue& value, T default_value, int base)
        {
            const auto s = value.value();
            if (!s)
                return default_value;
            auto n = parse_integer<T>(std::string(*s), base);
            if (!n)
                value.error();
            return *n;
        }

        template <typename T>
        T get_floating_point(const ArgumentValue& value, T default_value)
        {
            const auto s = value.value();
            if (!s)
                return default_value;
            auto n = parse_floating_point<T>(std::string(*s));
            if (!n)
                value.error();
            return *n;
        }
    }

    ArgumentValue::ArgumentValue()
        : m_value_id(),
          m_argument_id()
    {}

    ArgumentValue::ArgumentValue(const std::optional<std::string_view>& value,
                                 std::shared_ptr<ParsedArgumentsImpl> args,
                                 ValueId value_id,
                                 ArgumentId argument_id)
        : m_value(value),
          m_args(std::move(args)),
          m_value_id(value_id),
          m_argument_id(argument_id)
    {}

    ArgumentValue::ArgumentValue(const ArgumentValue&) = default;

    ArgumentValue::ArgumentValue(ArgumentValue&& ) noexcept = default;

    ArgumentValue::~ArgumentValue() = default;

    ArgumentValue&
    ArgumentValue::operator=(const ArgumentValue&) = default;

    ArgumentValue&
    ArgumentValue::operator=(ArgumentValue&&) noexcept = default;

    ArgumentValue::operator bool() const
    {
        return m_value.has_value();
    }

    std::unique_ptr<IArgumentView>
    ArgumentValue::argument() const
    {
        if (!m_args)
            ARGOS_THROW("ArgumentValue has not been initialized.");
        return m_args->get_argument_view(m_argument_id);
    }

    std::optional<std::string_view> ArgumentValue::value() const
    {
        return m_value;
    }

    bool ArgumentValue::as_bool(bool default_value) const
    {
        if (!m_value)
            return default_value;
        return !m_value->empty() && m_value != "0" && m_value != "false";
    }

    int ArgumentValue::as_int(int default_value, int base) const
    {
        return get_integer<int>(*this, default_value, base);
    }

    unsigned ArgumentValue::as_uint(unsigned default_value, int base) const
    {
        return get_integer<unsigned>(*this, default_value, base);
    }

    long ArgumentValue::as_long(long default_value, int base) const
    {
        return get_integer<long>(*this, default_value, base);
    }

    long long ArgumentValue::as_llong(long long default_value, int base) const
    {
        return get_integer<long long>(*this, default_value, base);
    }

    unsigned long
    ArgumentValue::as_ulong(unsigned long default_value, int base) const
    {
        return get_integer<unsigned long>(*this, default_value, base);
    }

    unsigned long long
    ArgumentValue::as_ullong(unsigned long long default_value, int base) const
    {
        return get_integer<unsigned long long>(*this, default_value, base);
    }

    float ArgumentValue::as_float(float default_value) const
    {
        return get_floating_point<float>(*this, default_value);
    }

    double ArgumentValue::as_double(double default_value) const
    {
        return get_floating_point<double>(*this, default_value);
    }

    std::string ArgumentValue::as_string(const std::string& default_value) const
    {
        return m_value ? std::string(*m_value) : default_value;
    }

    ArgumentValues
    ArgumentValue::split(char separator,
                         size_t min_parts, size_t max_parts) const
    {
        if (!m_args)
            ARGOS_THROW("ArgumentValue has not been initialized.");
        if (!m_value)
            return {{}, m_args, m_value_id};
        auto parts = split_string(*m_value, separator, max_parts - 1);
        if (parts.size() < min_parts)
        {
            error("Invalid value: \"" + std::string(*m_value)
                  + "\". Must have at least " + std::to_string(min_parts)
                  + " values separated by \"" + separator + "\".");
        }
        std::vector<std::pair<std::string_view, ArgumentId>> values;
        values.reserve(parts.size());
        for (auto& part : parts)
            values.emplace_back(part, m_argument_id);
        return {std::move(values), m_args, m_value_id};
    }

    ArgumentValues
    ArgumentValue::split_n(char separator, size_t num_parts) const
    {
        return split(separator, num_parts, num_parts);
    }

    void ArgumentValue::error(const std::string& message) const
    {
        if (!m_args)
            ARGOS_THROW("ArgumentValue has not been initialized.");
        m_args->error(message, m_argument_id);
    }

    void ArgumentValue::error() const
    {
        if (!m_value)
            ARGOS_THROW("ArgumentValue has no value.");
        error("Invalid value: " + std::string(*m_value) + ".");
    }
}
