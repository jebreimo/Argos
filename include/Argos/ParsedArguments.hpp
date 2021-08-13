//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include <ostream>
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
     *
     * Instances of this class is returned by ArgumentParser::parse and
     * ArgumentIterator::parsedArguments.
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
         */
        [[nodiscard]] bool has(const IArgumentView& arg) const;

        /**
         * @brief Returns the value of the argument with the given name.
         *
         * @throw ArgosException if @a name doesn't match the name of any
         *  argument or option.
         */
        [[nodiscard]] ArgumentValue value(const std::string& name) const;

        /**
         * @brief Returns the value of the given argument.
         */
        [[nodiscard]] ArgumentValue value(const IArgumentView& arg) const;

        /**
         * @brief Returns the values of the argument with the given name.
         *
         * @throw ArgosException if @a name doesn't match the name of any
         *  argument or option.
         */
        [[nodiscard]] ArgumentValues values(const std::string& name) const;

        /**
         * @brief Returns the value of the given argument.
         */
        [[nodiscard]] ArgumentValues values(const IArgumentView& arg) const;

        [[nodiscard]]
        /**
         * @brief Returns all argument definitions that were registered with
         *  ArgumentParser.
         *
         * Intended for testing and debugging, for instance to list all
         * defined arguments along with their given values.
         */
        std::vector<std::unique_ptr<ArgumentView>> allArguments() const;

        /**
         * @brief Returns all option definitions that were registered with
         *  ArgumentParser.
         *
         * Intended for testing and debugging, for instance to list all
         * defined options along with their given values.
         */
        [[nodiscard]]
        std::vector<std::unique_ptr<OptionView>> allOptions() const;

        /**
         * @brief Returns the parser result code.
         */
        [[nodiscard]] ParserResultCode resultCode() const;

        /**
         * @brief If the parser stopped early because it encountered an option
         *  of type, this function returns that option.
         */
        [[nodiscard]] OptionView stopOption() const;

        /**
         * @brief Returns the command line arguments that were ignored by the
         *  argument parser.
         *
         * This function will always return an empty vector unless at least
         * one of the following is true:
         *
         * - ArgumentParser::ignoreUndefinedArguments is true.
         * - ArgumentParser::ignoreUndefinedOptions is true.
         * - ArgumentParser::autoExit is false and there are options with type
         *   set to OptionType::STOP.
         */
        [[nodiscard]]
        const std::vector<std::string>& unprocessedArguments() const;

        /**
         * @brief Removes parsed arguments and options from @a argv and
         *  decrements @a argc correspondingly.
         *
         * The first value in @a argv is assumed to be the program name and
         * is ignored, the remainder should be identical to the command line
         * given to ArgumentParser::parse or ArgumentParser::makeIterator.
         *
         * @note No memory is freed, the function only rearranges the pointers
         *  @a in argv.
         */
        void filterParsedArguments(int& argc, char**& argv);

        /**
         * @brief Print @a msg along with a brief help text and exit.
         *
         * @throw ArgosException if ArgumentParser::autoExit is false.
         */
        [[noreturn]]
        void error(const std::string& msg);
    private:
        std::shared_ptr<ParsedArgumentsImpl> m_Impl;
    };

    /**
     * @brief Write a list of all arguments and options along with theirs
     *  values to @a stream.
     *
     * This function is intended for testing and debugging.
     */
    void print(std::ostream& stream, const ParsedArguments& args);
}
