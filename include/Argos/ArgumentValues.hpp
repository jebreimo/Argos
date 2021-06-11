//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include <string>
#include <vector>
#include "IArgumentView.hpp"

/**
 * @file
 * @brief Defines the ArgumentValues class.
 */

namespace Argos
{
    class ArgumentValue;
    class ParsedArgumentsImpl;

    /**
     * @brief Wrapper class for the values of a multi-value argument
     *      or option.
     *
     * ParsedArguments returns instances of ArgumentValues.
     */
    class ArgumentValues
    {
    public:
        /**
         * @private
         */
        ArgumentValues(std::vector<std::pair<std::string_view, ArgumentId>> values,
                       std::shared_ptr<ParsedArgumentsImpl> args,
                       ValueId valueId);

        /**
         * @private
         */
        ArgumentValues(const ArgumentValues&);

        /**
         * @private
         */
        ArgumentValues(ArgumentValues&&) noexcept;

        /**
         * @private
         */
        ~ArgumentValues();

        /**
         * @private
         */
        ArgumentValues& operator=(const ArgumentValues&);

        /**
         * @private
         */
        ArgumentValues& operator=(ArgumentValues&&) noexcept;

        /**
         * @brief Returns instances of IArgumentView that identifies the
         *  origins of the values.
         */
        std::vector<std::unique_ptr<IArgumentView>> arguments() const;

        void error(const std::string& message) const;

        bool empty() const;

        size_t size() const;

        std::vector<ArgumentValue> values() const;

        std::vector<std::string_view> rawValues() const;

        ArgumentValue value(size_t index) const;

        std::vector<int>
        asInts(const std::vector<int>& defaultValue = {},
               int base = 10) const;

        std::vector<unsigned>
        asUInts(const std::vector<unsigned>& defaultValue = {},
               int base = 10) const;

        std::vector<long>
        asLongs(const std::vector<long>& defaultValue = {},
                int base = 10) const;

        std::vector<unsigned long>
        asULongs(const std::vector<unsigned long>& defaultValue = {},
                 int base = 10) const;

        std::vector<long long>
        asLLongs(const std::vector<long long>& defaultValue = {},
                 int base = 10) const;

        std::vector<unsigned long long>
        asULLongs(const std::vector<unsigned long long>& defaultValue = {},
                  int base = 10) const;

        std::vector<float> asFloats(
                const std::vector<float>& defaultValue = {}) const;

        std::vector<double> asDoubles(
                const std::vector<double>& defaultValue = {}) const;

        std::vector<std::string> asStrings(
                const std::vector<std::string>& defaultValue = {}) const;

        ArgumentValues
        split(char separator, size_t minParts = 0, size_t maxParts = 0) const;
    private:
        std::vector<std::pair<std::string_view, ArgumentId>> m_Values;
        std::shared_ptr<ParsedArgumentsImpl> m_Args;
        ValueId m_ValueId;
    };
}
