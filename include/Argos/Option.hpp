//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>
#include "Callbacks.hpp"

/**
 * @file
 * @brief Defines the Option class.
 */

namespace Argos
{
    struct OptionData;

    /**
     * @brief Class for defining command line options.
     *
     * The minimum requirement for a command line option is that it has at
     * least one flag. Once the option has been defined it must be *added* to
     * the ArgumentParser with ArgumentParser::add.
     */
    class Option
    {
    public:
        Option();

        explicit Option(std::initializer_list<std::string> flags);

        /**
         * @private
         */
        Option(const Option&);

        /**
         * @private
         */
        Option(Option&&) noexcept;

        /**
         * @private
         */
        ~Option();

        /**
         * @private
         */
        Option& operator=(const Option&);

        /**
         * @private
         */
        Option& operator=(Option&&) noexcept;

        /**
         * @brief Set the option's help text.
         * @param text The text will be automatically divided into multiple
         *      lines if it doesn't fit fit inside the terminal window.
         *      Text formatting using newlines, spaces and tabs is possible.
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& text(const std::string& text);

        /**
         * @brief Specifies under which heading the option will appear
         *      in the help text.
         *
         * The default heading for options is "OPTIONS".
         * @param name All arguments and options with the same section name
         *      will be listed under the same heading.
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& section(const std::string& name);

        /**
         * @brief Set an alternative name for the value this option
         *      assigns to.
         *
         * The value or values of the option can be retrieved from
         * ParsedArgument using one of its flags, but sometimes this
         * is inconvenient, for instance if the same option has different
         * names in different languages, or multiple options share the same
         * value. In the latter case, for instance if there are two options
         * --verbose and --quiet that negates each other, one of them, but not
         * both, should have the other option as an alias (e.g. --verbose has
         * an alias("--quiet") and
         * @param id An alternative name that can be used to retrieve the
         *      option's value.
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& alias(const std::string& id);

        /**
         * @brief Set a callback that will be called when this option is
         *      encountered.
         * @param callback A function pointer or callable object accepting the
         *      parameters (OptionView, string_view, ParsedArgumentsBuilder).
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& callback(OptionCallback callback);

        /**
         * @brief Set restrictions for where this option is displayed in the
         *      auto-generated help text.
         * @param visibility
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& visibility(Visibility visibility);

        /**
         * @brief Set a custom id that can be used in callback functions etc.
         *      to quickly distinguish between different options.
         *
         * The id purely is intended for client code, Argos itself ignores
         * this value, but makes it available through IArgumentView.
         *
         * @param id Can be any integer value.
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& id(int id);

        /**
         * @brief Specify which operation the option performs. The default
         *  operations is ASSIGN.
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& operation(OptionOperation operation);

        /**
         * @brief Set the flag of a single-flag option.
         * @param f a flag with one or two leading dashes or a slash depending
         *  on the option style (ArgumentParser::optionStyle).
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& flag(const std::string& f);

        /**
         * @brief Set the flag of a multi-flag option.
         * @param f flags with one or two leading dashes or a slash depending
         *  on the option style (ArgumentParser::optionStyle).
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& flags(std::vector<std::string> f);

        /**
         * @brief Set the name of the option's argument (a value given on
         *  the command line).
         *
         * The argument is displayed in the help text and error messages.
         * Setting the argument is how one informs the ArgumentParser that
         * this option takes an argument from the command line rather than
         * assign a constant.
         *
         * @param name a string that will appear in the help text, typically
         *  something like FILE or NUMBER.
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& argument(const std::string& name);

        /**
         * @brief Sets a value that will be assigned to the option before
         *  arguments are parsed.
         *
         * @note This is not the recommended way to specify a default value,
         *  prefer instead to use the defaultValue argument to ArgumentValue's
         *  asString() et al. ParsedArguments can not distinguish between
         *  values assigned through initialValue() and actual command line
         *  arguments. APPEND-options will not overwrite the initial value,
         *  but add its values after it.
         *
         *  The initialValue can be useful if for instance one wants to use
         *  ArgumentValue's split function on a default value read from a
         *  file or an environment variable, or want to prepend
         *  a value to a list built by options of operation "APPEND".
         *
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& initialValue(const std::string& value);

        Option& constant(const char* value);

        Option& constant(const std::string& value);

        Option& constant(bool value);

        Option& constant(int value);

        Option& constant(long long value);

        Option& type(OptionType type);

        /**
         * @brief Set whether this option is optional or mandatory.
         *
         * @param optional
         *
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& optional(bool optional);

        const OptionData& data() const;

        /**
         * @private
         * @brief Used internally in Argos.
         *
         * The object is no longer usable after this function has
         * been called.
         *
         * @return Pointer to the option implementation.
         */
        std::unique_ptr<OptionData> release();
    private:
        void checkOption() const;

        std::unique_ptr<OptionData> m_Option;
    };
}
