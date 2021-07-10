//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include "ArgumentValue.hpp"
#include "ArgumentValues.hpp"
#include "ArgumentView.hpp"
#include "OptionView.hpp"

/**
 * @file
 * @brief Defines the ParsedArguments class.
 */

namespace Argos
{
    class ParsedArgumentsImpl;

    /**
     * @brief The result of the ArgumentParser. Gives access to all argument
     *      and option values.
     */
    class ParsedArguments
    {
    public:
        ParsedArguments();

        /**
         * @private
         */
        explicit ParsedArguments(std::shared_ptr<ParsedArgumentsImpl> impl);

        /**
         * @private
         */
        ParsedArguments(const ParsedArguments&) = delete;

        /**
         * @private
         */
        ParsedArguments(ParsedArguments&&) noexcept;

        /**
         * @private
         */
        ~ParsedArguments();

        /**
         * @private
         */
        ParsedArguments& operator=(const ParsedArguments&) = delete;

        /**
         * @private
         */
        ParsedArguments& operator=(ParsedArguments&&) noexcept;

        /**
         * @brief Returns true if the argument or option named @a name
         *  was given on command line.
         *
         * @throw ArgosException if @a name doesn't match the name of any
         *  argument or option.
         */
        [[nodiscard]] bool has(const std::string& name) const;

        /**
         * @brief Returns true if the given argument instance was given a
         *  value on command line.
         *
         * @throw ArgosException if @a name doesn't match the name of any
         *  argument or option.
         */
        [[nodiscard]] bool has(const IArgumentView& arg) const;

        [[nodiscard]] ArgumentValue value(const std::string& name) const;

        [[nodiscard]] ArgumentValue value(const IArgumentView& arg) const;

        [[nodiscard]] ArgumentValues values(const std::string& name) const;

        [[nodiscard]] ArgumentValues values(const IArgumentView& arg) const;

        [[nodiscard]]
        std::vector<std::unique_ptr<ArgumentView>> allArguments() const;

        [[nodiscard]]
        std::vector<std::unique_ptr<OptionView>> allOptions() const;

        [[nodiscard]] ParserResultCode resultCode() const;

        [[nodiscard]] OptionView stopOption() const;

        [[nodiscard]]
        const std::vector<std::string>& unprocessedArguments() const;

        void filterParsedArguments(int& argc, char**& argv);

        void error(const std::string& msg);
    private:
        std::shared_ptr<ParsedArgumentsImpl> m_Impl;
    };
}
