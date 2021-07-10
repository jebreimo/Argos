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
     * @a ParsedArguments and @a ArgumentValues returns instances of
     * @a ArgumentValue.
     */
    class ArgumentValue
    {
    public:
        /**
         * @private
         */
        ArgumentValue();

        /**
         * @private
         */
        ArgumentValue(std::optional<std::string_view> value,
                      std::shared_ptr<ParsedArgumentsImpl> args,
                      ValueId valueId,
                      ArgumentId argumentId);

        /**
         * @private
         */
        ArgumentValue(const ArgumentValue&);

        /**
         * @private
         */
        ArgumentValue(ArgumentValue&&) noexcept;

        /**
         * @private
         */
        ~ArgumentValue();

        /**
         * @private
         */
        ArgumentValue& operator=(const ArgumentValue&);

        /**
         * @private
         */
        ArgumentValue& operator=(ArgumentValue&&) noexcept;

        /**
         * @brief Returns true if this argument or option was given on the
         *      command line.
         */
        [[nodiscard]]
        explicit operator bool() const;

        /**
         * @brief Returns the @a IArgumentView instance which identifies
         *      the argument or option that is the source of this value.
         */
        [[nodiscard]] std::unique_ptr<IArgumentView> argument() const;

        /**
         * @brief Returns the value as it was found on the command line.
         *
         * The optional is empty if the value was not given. The optional
         * will never be empty if the current ArgumentValue instance
         * was returned by ArgumentValues::value(n).
         */
        [[nodiscard]] std::optional<std::string_view> value() const;

        /**
         * @brief Returns defaultValue if the value was not given on the
         *      command line, otherwise it returns true unless the given value
         *      is "0" or "false".
         */
        [[nodiscard]] bool asBool(bool defaultValue = false) const;

        /**
         * @brief Converts the value from the command line to int and returns it.
         *
         * Returns defaultValue if the value was not given on the command line.
         *
         * If the given value can not be converted to int, an error message
         * is displayed and the program either exits (autoExit is true) or
         * the function throws an exception (autoExit is false). The actual
         * conversion is performed by @a strtol and @a base is passed unmodified
         * to that function.
         *
         * @throw ArgosException if @a autoExit is false and the given value
         *      can not be converted to int.
         */
        [[nodiscard]] int asInt(int defaultValue = 0, int base = 10) const;

        /**
         * @brief Converts the value from the command line to unsigned int
         *      and returns it.
         *
         * Returns defaultValue if the value was not given on the command line.
         *
         * If the given value can not be converted to unsigned int, an error
         * message is displayed and the program either exits (autoExit is true)
         * or the function throws an exception (autoExit is false). The actual
         * conversion is performed by @a strtoul and @a base is passed
         * unmodified to that function.
         *
         * @throw ArgosException if @a autoExit is false and the given value
         *      can not be converted to unsigned int.
         */
        [[nodiscard]]
        unsigned asUInt(unsigned defaultValue = 0, int base = 10) const;

        /**
         * @brief Converts the value from the command line to long and
         *      returns it.
         *
         * Returns defaultValue if the value was not given on the command line.
         *
         * If the given value can not be converted to long, an error
         * message is displayed and the program either exits (autoExit is true)
         * or the function throws an exception (autoExit is false). The actual
         * conversion is performed by @a strtol and @a base is passed
         * unmodified to that function.
         *
         * @throw ArgosException if @a autoExit is false and the given value
         *      can not be converted to long.
         */
        [[nodiscard]] long asLong(long defaultValue = 0, int base = 10) const;

        /**
         * @brief Converts the value from the command line to long long and
         *      returns it.
         *
         * Returns defaultValue if the value was not given on the command line.
         *
         * If the given value can not be converted to long long, an error
         * message is displayed and the program either exits (autoExit is true)
         * or the function throws an exception (autoExit is false). The actual
         * conversion is performed by @a strtoll and @a base is passed
         * unmodified to that function.
         *
         * @throw ArgosException if @a autoExit is false and the given value
         *      can not be converted to long long.
         */
        [[nodiscard]]
        long long asLLong(long long defaultValue = 0, int base = 10) const;

        /**
         * @brief Converts the value from the command line to unsigned long and
         *      returns it.
         *
         * Returns defaultValue if the value was not given on the command line.
         *
         * If the given value can not be converted to unsigned long, an error
         * message is displayed and the program either exits (autoExit is true)
         * or the function throws an exception (autoExit is false). The actual
         * conversion is performed by @a strtoul and @a base is passed
         * unmodified to that function.
         *
         * @throw ArgosException if @a autoExit is false and the given value
         *      can not be converted to unsigned long.
         */
        [[nodiscard]] unsigned long
        asULong(unsigned long defaultValue = 0, int base = 10) const;

        /**
         * @brief Converts the value from the command line to unsigned long
         *      long and returns it.
         *
         * Returns defaultValue if the value was not given on the command line.
         *
         * If the given value can not be converted to unsigned long long,
         * an error message is displayed and the program either exits (autoExit
         * is true) or the function throws an exception (autoExit is false).
         * The actual conversion is performed by @a strtoul and @a base is
         * passed unmodified to that function.
         *
         * @throw ArgosException if @a autoExit is false and the given value
         *      can not be converted to unsigned long.
         */
        [[nodiscard]] unsigned long long
        asULLong(unsigned long long defaultValue = 0, int base = 10) const;

        /**
         * @brief Converts the value from the command line to float and
         *      returns it.
         *
         * Returns defaultValue if the value was not given on the command line.
         *
         * If the given value can not be converted to float, an error message
         * is displayed and the program either exits (autoExit is true) or the
         * function throws an exception (autoExit is false).
         *
         * @throw ArgosException if @a autoExit is false and the given value
         *      can not be converted to float.
         */
        [[nodiscard]] float asFloat(float defaultValue = 0) const;

        /**
         * @brief Converts the value from the command line to double and
         *      returns it.
         *
         * Returns defaultValue if the value was not given on the command line.
         *
         * If the given value can not be converted to double, an error message
         * is displayed and the program either exits (autoExit is true) or the
         * function throws an exception (autoExit is false).
         *
         * @throw ArgosException if @a autoExit is false and the given value
         *      can not be converted to double.
         */
        [[nodiscard]] double asDouble(double defaultValue = 0) const;

        /**
         * @brief Returns the value from the command line as a string.
         *
         * Returns defaultValue if the value was not given on the command line.
         */
        [[nodiscard]] std::string
        asString(const std::string& defaultValue = {}) const;

        /**
         * @brief Splits the string from the command line on @a separator and
         *      returns the resulting parts.
         *
         * An error message is displayed if the result has less than
         * @a minParts parts (i.e. number of separators is less than
         * minParts - 1). The result will never consist of more than
         * @a maxParts parts, even if there are more occurrences of
         * @a separator in the value, it just means that the final part will
         * contain one or more separators. The only exception is if
         * @a maxParts is zero (the default), which means there is no upper
         * limit to the number of parts.
         *
         * @throw ArgosException if @a autoExit is false and the result
         *      has less than @a minParts parts.
         */
        [[nodiscard]] ArgumentValues
        split(char separator, size_t minParts = 0, size_t maxParts = 0) const;

        /**
         * Display @a message as if it was an error produced within Argos
         * itself, including a reference to the argument or option this value
         * comes from and the usage section from the help text. If autoExit is
         * true the program will exit after displaying the message.
         */
        void error(const std::string& message) const;

        /**
         * Calls error(message) with a message that says the given value
         * is invalid.
         */
        void error() const;
    private:
        std::optional<std::string_view> m_Value;
        std::shared_ptr<ParsedArgumentsImpl> m_Args;
        ValueId m_ValueId;
        ArgumentId m_ArgumentId;
    };
}
