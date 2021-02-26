//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <memory>
#include <string>
#include "Callbacks.hpp"

/**
 * @file
 * @brief Defines the Argument class.
 */

namespace Argos
{
    struct ArgumentData;

    /**
     * @brief Class for defining command line arguments.
     *
     * The minimum requirement for a command line argument is that it has a
     * name. Once the argument has been defined it must be *added* to the
     * ArgumentParser with ArgumentParser::add.
     */
    class Argument
    {
    public:
        /**
         * @brief Creates an unnamed argument.
         */
        Argument();

        /**
         * @brief Creates an argument with name @a name.
         * @param name The name that will be displayed in the help text as
         *      well as the name used when retrieving the argument's value from
         *      ParsedArguments.
         */
        explicit Argument(const std::string& name);

        /**
         * @brief Creates a complete copy of the given argument.
         */
        Argument(const Argument&);

        /**
         * @brief Moves the innards of the given argument to the new object.
         *
         * Attempts to use the old object will result in an exception.
         */
        Argument(Argument&&) noexcept;

        ~Argument();

        /**
         * @brief Copies everything in the given argument.
         */
        Argument& operator=(const Argument&);

        /**
         * @brief Moves the innards of the given argument to the current
         * object.
         *
         * Attempts to use the old object will result in an exception.
         */
        Argument& operator=(Argument&&) noexcept;

        /**
         * @brief Set the argument's help text.
         * @param text The text will be automatically divided into multiple
         *      lines if it doesn't fit fit inside the terminal window.
         *      Text formatting using newlines, spaces and tabs is possible.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& text(const std::string& text);

        /**
         * @brief Specifies under which heading the argument will appear
         *      in the help text.
         *
         * The default heading for arguments is "ARGUMENTS".
         * @param name All arguments and options with the same section name
         *      will be listed under the same heading.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& section(const std::string& name);

        /**
         * @brief Set an alternative name for the value this argument
         *      assigns to.
         *
         * The value or values of the argument can be retrieved from
         * ParsedArgument using the displayed name, but sometimes this
         * is inconvenient, for instance if the same argument has different
         * names in different languages.
         * @param id An alternative name that can be used to retrieve the
         *      argument's value.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& value(const std::string& id);

        /**
         * @brief Set a callback that will be called when this argument is
         *      encountered.
         * @param callback A function pointer or callable object accepting the
         *      parameters (ArgumentView, string_view, ParsedArgumentsBuilder).
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& callback(ArgumentCallback callback);

        /**
         * @brief Set restrictions for where this argument is displayed in the
         *      auto-generated help text.
         * @param visibility
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& visibility(Visibility visibility);

        /**
         * @brief Set a custom id that can be used in callback functions etc.
         *      to quickly distinguish between different arguments.
         *
         * The id purely is intended for client code, Argos itself ignores
         * this value.
         *
         * @param id Can be any integer value.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& id(int id);

        /**
         * @brief Set the name of the argument.
         *
         * An alternative to supplying the name to the constructor.
         *
         * @param name The name that will be displayed in the help text as
         *      well as the name used when retrieving the argument's value from
         *      ParsedArguments.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& name(const std::string& name);

        /**
         * @brief Make this argument optional (or mandatory).
         *
         * All arguments are mandatory by default.
         *
         * This function is a convenience function that affects the argument's
         * minimum count.
         * @param optional
         *      @arg true The argument's minimum count is set to 0.
         *      @arg false The argument's minimum count is set to 1
         *          if it currently is 0.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& optional(bool optional);

        /**
         * @brief Set the number of times this argument must appear on the
         *      command line.
         * @param n This is both the minimum and the maximum count.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& count(unsigned n);

        /**
         * @brief Set the number of times this argument must appear on the
         *      command line.
         * @param minCount Can be any value.
         * @param maxCount Must be greater than or equal to @a minCount.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& count(unsigned minCount, unsigned maxCount);

        /**
         * @private
         * @brief Used internally in Argos.
         *
         * The object is no longer usable after this function has
         * been called.
         * @return Pointer to the argument implementation.
         */
        std::unique_ptr<ArgumentData> release();
    private:
        void checkArgument() const;

        std::unique_ptr<ArgumentData> m_Argument;
    };
}
