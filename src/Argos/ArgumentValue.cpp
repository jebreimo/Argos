//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-31.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentValue.hpp"

#include "ParsedArgumentsImpl.hpp"
#include "HelpWriter.hpp"

namespace Argos
{
    ArgumentValue::ArgumentValue(std::optional<std::string_view> value,
                                 std::shared_ptr<ParsedArgumentsImpl> args,
                                 int valueId)
        : m_Value(value),
          m_Args(std::move(args)),
          m_ValueId(valueId)
    {}

    ArgumentValue::ArgumentValue(const ArgumentValue&) = default;

    ArgumentValue::ArgumentValue(ArgumentValue&& ) noexcept = default;

    ArgumentValue::~ArgumentValue() = default;

    ArgumentValue& ArgumentValue::operator=(const ArgumentValue&) = default;

    ArgumentValue& ArgumentValue::operator=(ArgumentValue&&) noexcept = default;

    std::unique_ptr<IArgumentView> ArgumentValue::argument() const
    {
        return std::unique_ptr<IArgumentView>();
    }

    bool ArgumentValue::hasValue() const
    {
        return m_Value.has_value();
    }

    std::optional<std::string_view> ArgumentValue::value() const
    {
        return m_Value;
    }

    std::string ArgumentValue::stringValue(const std::string& defaultValue) const
    {
        return m_Value ? std::string(*m_Value) : defaultValue;
    }

    void ArgumentValue::error(const std::string& message) const
    {
        m_Args->error(message, m_ValueId);
    }
}
