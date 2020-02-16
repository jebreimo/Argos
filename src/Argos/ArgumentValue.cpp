//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-31.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentValue.hpp"

#include "ParserData.hpp"
#include "HelpWriter.hpp"

namespace Argos
{
    ArgumentValue::ArgumentValue(std::optional<std::string_view> value,
                                 std::shared_ptr<ParserData> data,
                                 int valueId)
        : m_Value(value),
          m_Data(std::move(data)),
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
        // TODO: use ParsedArgumentsImpl::error().
        HelpWriter(m_Data).writeErrorMessage(message);
        if (m_Data->parserSettings.autoExit)
            exit(1);
    }
}
