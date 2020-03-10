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

        explicit Argument(const std::string& name);

        Argument(const Argument&);

        Argument(Argument&&) noexcept;

        ~Argument();

        Argument& operator=(const Argument&);

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
        Argument& valueName(const std::string& id);

        /**
         * @brief Set a callback that will be called when this argument is
         *      encountered.
         * @param callback A function pointer or callable object.
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

        Argument& id(int id);

        Argument& name(const std::string& name);

        Argument& optional(bool optional);

        Argument& count(unsigned n);

        Argument& count(unsigned minCount, unsigned maxCount);

        std::unique_ptr<ArgumentData> release();
    private:
        std::unique_ptr<ArgumentData> m_Argument;
    };
}
