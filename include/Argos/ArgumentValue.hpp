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

/**
 * @file
 * @brief Defines the ArgumentValue class.
 */

namespace Argos
{
    class ParsedArgumentsImpl;
    class ArgumentValues;

    /**
     * @brief Wrapper class for the value of an argument or option.
     *
     * ParsedArguments returns instances of ArgumentValue.
     */
    class ArgumentValue
    {
    public:
        /**
         * @private
         */
        ArgumentValue(std::optional<std::string_view> value,
                      std::shared_ptr<ParsedArgumentsImpl> args,
                      ValueId valueId,
                      ArgumentId argumentId);

        ArgumentValue(const ArgumentValue&);

        ArgumentValue(ArgumentValue&&) noexcept;

        ~ArgumentValue();

        ArgumentValue& operator=(const ArgumentValue&);

        ArgumentValue& operator=(ArgumentValue&&) noexcept;

        std::unique_ptr<IArgumentView> argument() const;

        bool hasValue() const;

        std::optional<std::string_view> value() const;

        bool asBool(bool defaultValue = false) const;

        int asInt(int defaultValue = 0, int base = 10) const;

        unsigned asUInt(unsigned defaultValue = 0, int base = 10) const;

        long asLong(long defaultValue = 0, int base = 10) const;

        long long asLLong(long long defaultValue = 0, int base = 10) const;

        unsigned long
        asULong(unsigned long defaultValue = 0, int base = 10) const;

        unsigned long long
        asULLong(unsigned long long defaultValue = 0, int base = 10) const;

        float asFloat(float defaultValue = 0) const;

        double asDouble(double defaultValue = 0) const;

        std::string asString(const std::string& defaultValue = {}) const;

        ArgumentValues
        split(char separator, size_t minParts = 0, size_t maxParts = 0) const;

        void error(const std::string& message) const;

        void error() const;
    private:
        std::optional<std::string_view> m_Value;
        std::shared_ptr<ParsedArgumentsImpl> m_Args;
        ValueId m_ValueId;
        ArgumentId m_ArgumentId;
    };
}
