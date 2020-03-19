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
     * @brief Class for defining command line arguments.
     *
     * The minimum requirement for a command line argument is that it has a
     * name. Once the argument has been defined it must be *added* to the
     * ArgumentParser with ArgumentParser::add.
     */
    class Option
    {
    public:
        Option();

        explicit Option(std::initializer_list<std::string> flags);

        Option(const Option&);

        Option(Option&&) noexcept;

        ~Option();

        Option& operator=(const Option&);

        Option& operator=(Option&&) noexcept;

        /**
         * @brief Set the option's help text.
         * @param text The text will be automatically divided into multiple
         *      lines if it doesn't fit fit inside the terminal window.
         *      Text formatting using newlines, spaces and tabs is possible.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
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
         *      method calls.
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
         * value.
         * @param id An alternative name that can be used to retrieve the
         *      option's value.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Option& valueName(const std::string& id);

        /**
         * @brief Set a callback that will be called when this option is
         *      encountered.
         * @param callback A function pointer or callable object.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Option& callback(OptionCallback callback);

        /**
         * @brief Set restrictions for where this option is displayed in the
         *      auto-generated help text.
         * @param visibility
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Option& visibility(Visibility visibility);

        /**
         * @brief Set a custom id that can be used in callback functions etc.
         *      to quickly distinguish between different options.
         *
         * The id purely is intended for client code, Argos itself ignores
         * this value.
         *
         * @param id Can be any integer value.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Option& id(int id);

        Option& operation(OptionOperation operation);

        Option& flag(const std::string& f);

        Option& flags(std::vector<std::string> f);

        Option& argument(const std::string& name);

        Option& value(const std::string& value);

        Option& value(bool value);

        Option& value(int value);

        Option& value(double value);

        Option& type(OptionType type);

        Option& mandatory(bool mandatory);

        const OptionData& data() const;

        /**
         * @private
         * @brief Used internally in Argos.
         *
         * The object is no longer usable after this function has
         * been called.
         * @return Pointer to the option implementation.
         */
        std::unique_ptr<OptionData> release();
    private:
        void checkOption() const;

        std::unique_ptr<OptionData> m_Option;
    };
}
