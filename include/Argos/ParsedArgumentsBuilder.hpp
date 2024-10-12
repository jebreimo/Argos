//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-29.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "ArgumentValue.hpp"
#include "ArgumentValues.hpp"

/**
 * @file
 * @brief Defines the ParsedArgumentsBuilder class.
 */

namespace argos
{
    class ParsedArgumentsImpl;

    /**
     * @brief An interface to ParsedArguments that lets argument and option
     *      callbacks query and modify the parsed argument values
     *
     * Eaxmple:
     * ~~~{.cpp}
     * ArgumentParser()
     *     .add(Option{"--foo"}...)
     *     .add(Option{"--bar"}...)
     *     .add(Option{"--baz"}.argument("NUMBER")...)
     *     .add(Option{"--all"}
     *         .text("Enables --foo and --bar, and sets --baz to 11.")
     *         .callback([](auto, auto, ParsedArguments pa)
     *             {
     *                 pa.assign("--foo", "true").assign("--bar", "true")
     *                   .assign("--baz", "11");
     *                 return true;
     *             }))
     *         ...
     * ~~~
     */
    class ParsedArgumentsBuilder
    {
    public:
        /**
         * @private
         */
        explicit ParsedArgumentsBuilder(
                std::shared_ptr<ParsedArgumentsImpl> impl);

        /**
         * @brief Add @a value to the named argument or option.
         * @param name The name (flag, alias etc.) of an argument or option.
         * @param value Even if the values will be read as integers or floats,
         *  they must be given as strings. Boolean values must be given as
         *  "0" or "false" for *false* and "1" or "true" for *true*.
         * @throw ArgosException if the name doesn't match any of names,
         *  flags or aliases of the arguments and options added
         *  to ArgumentParser.
         */
        ParsedArgumentsBuilder& append(const std::string& name,
                                       const std::string& value);

        /**
         * @brief Add @a value to the given argument or option.
         * @param arg An argument or option.
         * @param value Even if the values will be read as integers or floats,
         *  they must be given as strings. Boolean values must be given as
         *  "0" for *false* and "1" for *true*.
         */
        ParsedArgumentsBuilder& append(const IArgumentView& arg,
                                       const std::string& value);

        /**
         * @brief Set the value of the named argument or option.
         *
         * Any previous value or values are replaced by @a value.
         *
         * @param name The name (flag, alias etc.) of an argument or option.
         * @param value Even if the values will be read as integers or floats,
         *  they must be given as strings. Boolean values must be given as
         *  "0" for *false* and "1" for *true*.
         * @throw ArgosException if the name doesn't match any of names,
         *  flags or aliases of the arguments and options added
         *  to ArgumentParser.
         */
        ParsedArgumentsBuilder& assign(const std::string& name,
                                       const std::string& value);

        /**
         * @brief Set the value of the given argument or option.
         *
         * Any previous value or values are replaced by @a value.
         *
         * @param arg An argument or option.
         * @param value Even if the values will be read as integers or floats,
         *  they must be given as strings. Boolean values must be given as
         *  "0" for *false* and "1" for *true*.
         */
        ParsedArgumentsBuilder& assign(const IArgumentView& arg,
                                       const std::string& value);

        /**
         * @brief Removes the value or values of the named argument or option.
         *
         * After it's been cleared, the argument or value will be treated as
         * if it hasn't been given any value at all, i.e. the default value
         * will be returned when its value is retrieved with
         * ArgumentValue::as_string et al.
         *
         * @param name The name (flag, alias etc.) of an argument or option.
         * @throw ArgosException if the name doesn't match any of names,
         *  flags or aliases of the arguments and options added
         *  to ArgumentParser.
         */
        ParsedArgumentsBuilder& clear(const std::string& name);

        /**
         * @brief Removes the value or values of the named argument or option.
         *
         * After it's been cleared, the argument or value will be treated as
         * if it hasn't been given any value at all, i.e. the default value
         * will be returned when its value is retrieved with
         * ArgumentValue::as_string et al.
         */
        ParsedArgumentsBuilder& clear(const IArgumentView& arg);

        /**
         * @brief Returns the value of the named argument or option.
         * @throw ArgosException if the argument or option has multiple
         *  values, or the name doesn't match any of names, flags or aliases
         *  of the arguments and options added to ArgumentParser.
         */
        [[nodiscard]] ArgumentValue value(const std::string& name) const;

        /**
         * @brief Returns the value of the given argument or option.
         * @throw ArgosException if the argument or option has multiple
         *  values.
         */
        [[nodiscard]] ArgumentValue value(const IArgumentView& arg) const;

        /**
         * @brief Returns the value of the named argument or option.
         * @throw ArgosException if the name doesn't match any of names,
         *  flags or aliases of the arguments and options added
         *  to ArgumentParser.
         */
        [[nodiscard]] ArgumentValues values(const std::string& name) const;

        /**
         * @brief Returns the values of the given argument or option.
         */
        [[nodiscard]] ArgumentValues values(const IArgumentView& arg) const;

        /**
         * @brief Returns true if the named argument or option has been
         *  assigned a value.
         */
        [[nodiscard]] bool has(const std::string& name) const;

        /**
         * @brief Returns true if the @a arg has been assigned a value.
         */
        [[nodiscard]] bool has(const IArgumentView& arg) const;

        /**
         * @brief Print @a msg along with a brief help text and exit.
         *
         * @throw ArgosException if ArgumentParser::auto_exit is false.
         */
        [[noreturn]] void error(const std::string& msg) const;

        /**
         * @brief Print @a msg prefixed by the argument's name or option's
         *  flags along with a brief help text and exit.
         *
         * @throw ArgosException if ArgumentParser::auto_exit is false.
         */
        [[noreturn]]
        void error(const std::string& msg, const IArgumentView& arg) const;

        /**
         * @brief Returns the stream that was assigned to the
         *  ArgumentParser.
         *
         * Returns a reference to the default stream (std::cout) if none
         * has been assigned.
         */
        [[nodiscard]] std::ostream& stream() const;

        /**
         * @brief Returns the program name that was assigned to the
         *  ArgumentParser.
         */
        [[nodiscard]] const std::string& program_name() const;
    private:
        std::shared_ptr<ParsedArgumentsImpl> m_impl;
    };
}
