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
         *  command line arguments that produced these values.
         */
        std::vector<std::unique_ptr<IArgumentView>> arguments() const;

        /**
         * Display @a message as if it was an error produced within Argos
         * itself, including a reference to the argument or option this value
         * comes from and the usage section from the help text. If autoExit is
         * true the program will exit after displaying the message, an
         * exception is thrown if it's not.
         */
        [[noreturn]]
        void error(const std::string& message) const;

        /**
         * @brief Returns true if there are no values.
         */
        bool empty() const;

        /**
         * @brief Returns the number of values.
         */
        size_t size() const;

        /**
         * @brief Returns a vector with all the values.
         */
        std::vector<ArgumentValue> values() const;

        /**
         * @brief Returns a vector with all the raw string_view values.
         */
        std::vector<std::string_view> rawValues() const;

        /**
         * @brief Returns the value with the given index.
         *
         * If index is to large, an error message is written to stderr, the
         * program also automatically exits if autoExit is true.
         */
        ArgumentValue value(size_t index) const;

        /**
         * @brief Returns a vector where all values have been converted
         *  to ints.
         * @param defaultValue This vector is returned if there are no values.
         * @param base
         * @return
         */
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
