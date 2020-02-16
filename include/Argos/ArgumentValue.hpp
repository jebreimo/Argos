//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-31.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "IArgumentView.hpp"

namespace Argos
{
    class ParsedArgumentsImpl;

    class ArgumentValue
    {
    public:
        ArgumentValue(std::optional<std::string_view> value,
                      std::shared_ptr<ParsedArgumentsImpl> args,
                      int valueId);

        ArgumentValue(const ArgumentValue&);

        ArgumentValue(ArgumentValue&&) noexcept;

        ~ArgumentValue();

        ArgumentValue& operator=(const ArgumentValue&);

        ArgumentValue& operator=(ArgumentValue&&) noexcept;

        std::unique_ptr<IArgumentView> argument() const;

        bool hasValue() const;

        std::optional<std::string_view> value() const;

        bool boolValue(bool defaultValue = false) const;

        int8_t int8Value(int8_t defaultValue = 0) const;

        int16_t int16Value(int16_t defaultValue = 0) const;

        int32_t int32Value(int32_t defaultValue = 0) const;

        int64_t int64Value(int64_t defaultValue = 0) const;

        uint8_t uint8Value(uint8_t defaultValue = 0) const;

        uint16_t uint16Value(uint16_t defaultValue = 0) const;

        uint32_t uint32Value(uint32_t defaultValue = 0) const;

        uint64_t uint64Value(uint64_t defaultValue = 0) const;

        float floatValue(float defaultValue = 0) const;

        double doubleValue(double defaultValue = 0) const;

        long double longDoubleValue(long double defaultValue = 0) const;

        std::string stringValue(const std::string& defaultValue = {}) const;

        void error(const std::string& message) const;
    private:
        std::optional<std::string_view> m_Value;
        std::shared_ptr<ParsedArgumentsImpl> m_Args;
        int m_ValueId;
    };

    class ArgumentValues
    {
    public:
        ArgumentValues(std::vector<std::string_view> value,
                       std::shared_ptr<ParsedArgumentsImpl> args,
                       int valueId);

        ArgumentValues(const ArgumentValues&);

        ArgumentValues(ArgumentValues&&) noexcept;

        ~ArgumentValues();

        ArgumentValues& operator=(const ArgumentValues&);

        ArgumentValues& operator=(ArgumentValues&&) noexcept;

        std::unique_ptr<IArgumentView> argument() const;

        void error(const std::string& message) const;
    private:
        std::vector<std::string_view> m_Values;
        std::shared_ptr<ParsedArgumentsImpl> m_Args;
        int m_ValueId;
    };
}
