//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-17.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include <string>
#include <vector>
#include "IArgumentView.hpp"
#include "ArgumentValueIterator.hpp"

/**
 * @file
 * @brief Defines the ArgumentValues class.
 */

namespace argos
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
                       ValueId value_id);

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
         * @brief Returns true if there is at least one value.
         */
        [[nodiscard]]
        explicit operator bool() const;

        /**
         * @brief Returns the value with the given index.
         *
         * Calling this operator is identical to calling value(index).
         *
         * If @a index is to great, an error message is written to stderr, the
         * program also automatically exits if auto_exit is true.
         *
         * @throw ArgosException if @a index is too great and auto_exit
         *  is false.
         */
        ArgumentValue operator[](size_t index) const;

        /**
         * @brief Returns instances of IArgumentView that identifies the
         *  command line arguments that produced these values.
         */
        [[nodiscard]] std::vector<std::unique_ptr<IArgumentView>> arguments() const;

        /**
         * Display @a message as if it was an error produced within Argos
         * itself, including a reference to the argument or option this value
         * comes from and the usage section from the help text. If auto_exit is
         * true the program will exit after displaying the message, an
         * exception is thrown if it's not.
         */
        [[noreturn]]
        void error(const std::string& message) const;

        /**
         * @brief Returns true if there are no values.
         */
        [[nodiscard]] bool empty() const;

        /**
         * @brief Returns the number of values.
         */
        [[nodiscard]] size_t size() const;

        /**
         * @brief Returns a vector with all the values.
         */
        [[nodiscard]] std::vector<ArgumentValue> values() const;

        /**
         * @brief Returns a vector with all the raw string_view values.
         */
        [[nodiscard]] std::vector<std::string_view> raw_values() const;

        /**
         * @brief Returns the value with the given index.
         *
         * If @a index is to great, an error message is written to stderr, the
         * program also automatically exits if auto_exit is true.
         *
         * @throw ArgosException if @a index is too great and auto_exit
         *  is false.
         */
        [[nodiscard]] ArgumentValue value(size_t index) const;

        /**
         * @brief Returns a vector where every argument value has been
         *  converted to int.
         *
         * If any of the argument values can't be converted, an error
         * message is written to stderr, the program also automatically exits
         * if auto_exit is true.
         *
         * @param default_value This vector is returned if there are no values.
         * @param base See the documentation for std::strtoint for details.
         * @throw ArgosException if the conversion fails for any value and
         *  auto_exit is false.
         */
        [[nodiscard]] std::vector<int>
        as_ints(const std::vector<int>& default_value = {},
                int base = 10) const;

        /**
         * @brief Returns a vector where every argument value has been
         *  converted to unsigned.
         *
         * If any of the argument values can't be converted, an error
         * message is written to stderr, the program also automatically exits
         * if auto_exit is true.
         *
         * @param default_value This vector is returned if there are no values.
         * @param base See the documentation for std::strtoint for details.
         * @throw ArgosException if the conversion fails for any value and
         *  auto_exit is false.
         */
        [[nodiscard]] std::vector<unsigned>
        as_uints(const std::vector<unsigned>& default_value = {},
                 int base = 10) const;

        /**
         * @brief Returns a vector where every argument value has been
         *  converted to long.
         *
         * If any of the argument values can't be converted, an error
         * message is written to stderr, the program also automatically exits
         * if auto_exit is true.
         *
         * @param default_value This vector is returned if there are no values.
         * @param base See the documentation for std::strtoint for details.
         * @throw ArgosException if the conversion fails for any value and
         *  auto_exit is false.
         */
        [[nodiscard]] std::vector<long>
        as_longs(const std::vector<long>& default_value = {},
                 int base = 10) const;

        /**
         * @brief Returns a vector where every argument value has been
         *  converted to unsigned long.
         *
         * If any of the argument values can't be converted, an error
         * message is written to stderr, the program also automatically exits
         * if auto_exit is true.
         *
         * @param default_value This vector is returned if there are no values.
         * @param base See the documentation for std::strtoint for details.
         * @throw ArgosException if the conversion fails for any value and
         *  auto_exit is false.
         */
        [[nodiscard]] std::vector<unsigned long>
        as_ulongs(const std::vector<unsigned long>& default_value = {},
                  int base = 10) const;

        /**
         * @brief Returns a vector where every argument value has been
         *  converted to long long.
         *
         * If any of the argument values can't be converted, an error
         * message is written to stderr, the program also automatically exits
         * if auto_exit is true.
         *
         * @param default_value This vector is returned if there are no values.
         * @param base See the documentation for std::strtoint for details.
         * @throw ArgosException if the conversion fails for any value and
         *  auto_exit is false.
         */
        [[nodiscard]] std::vector<long long>
        as_llongs(const std::vector<long long>& default_value = {},
                  int base = 10) const;

        /**
        * @brief Returns a vector where every argument value has been
        *  converted to unsigned long long.
        *
        * If any of the argument values can't be converted, an error
        * message is written to stderr, the program also automatically exits
        * if auto_exit is true.
        *
        * @param default_value This vector is returned if there are no values.
        * @param base See the documentation for std::strtoint for details.
        * @throw ArgosException if the conversion fails for any value and
        *  auto_exit is false.
        */
        [[nodiscard]] std::vector<unsigned long long>
        as_ullongs(const std::vector<unsigned long long>& default_value = {},
                   int base = 10) const;

        /**
         * @brief Returns a vector where every argument value has been
         *  converted to float.
         *
         * If any of the argument values can't be converted, an error
         * message is written to stderr, the program also automatically exits
         * if auto_exit is true.
         *
         * @param default_value This vector is returned if there are no values.
         * @throw ArgosException if the conversion fails for any value and
         *  auto_exit is false.
         */
        [[nodiscard]] std::vector<float>
        as_floats(const std::vector<float>& default_value = {}) const;

        /**
         * @brief Returns a vector where every argument value has been
         *  converted to double.
         *
         * If any of the argument values can't be converted, an error
         * message is written to stderr, the program also automatically exits
         * if auto_exit is true.
         *
         * @param default_value This vector is returned if there are no values.
         * @throw ArgosException if the conversion fails for any value and
         *  auto_exit is false.
         */
        [[nodiscard]] std::vector<double>
        as_doubles(const std::vector<double>& default_value = {}) const;

        /**
         * @brief Returns a vector with the argument values.
         *
         * @param default_value This vector is returned if there are no values.
         * @throw ArgosException if the conversion fails for any value and
         *  auto_exit is false.
         */
        [[nodiscard]] std::vector<std::string>
        as_strings(const std::vector<std::string>& default_value = {}) const;

        /**
         * @brief Splits each value on @a separator and returns the parts in
         *  a single list.
         * @param separator The separator.
         * @param min_parts The minimum number of parts each value must
         *  consist of.
         * @param max_parts The maximum number of parts any value can
         *  consist of. The final part will retain all excessive separators.
         * @throw ArgosException if any value consists of less than
         *  @a min_parts parts.
         */
        [[nodiscard]] ArgumentValues
        split(char separator, size_t min_parts = 0, size_t max_parts = 0) const;

        /**
         * @brief Splits each value on @a separator into exactly @a num_parts
         *  parts and returns the result.
         *
         * @note This function is equivalent to calling
         *  `split(separator, num_parts, num_parts)`.
         *
         * @param separator The separator.
         * @param num_parts The number of parts each value must
         *  consist of.
         * @throw ArgosException if any value consists of less than
         *  @a num_parts parts.
         */
        [[nodiscard]] ArgumentValues
        split_n(char separator, size_t num_parts) const;

        /**
         * @brief Returns an iterator pointing to the first value.
         */
        [[nodiscard]] ArgumentValueIterator begin() const;

        /**
         * @brief Returns an iterator pointing to the end of the last value.
         */
        [[nodiscard]] ArgumentValueIterator end() const;
    private:
        std::vector<std::pair<std::string_view, ArgumentId>> m_values;
        std::shared_ptr<ParsedArgumentsImpl> m_args;
        ValueId m_value_id;
    };
}
