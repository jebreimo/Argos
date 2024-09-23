//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include <iosfwd>
#include "ArgumentValue.hpp"
#include "ArgumentValues.hpp"
#include "ArgumentView.hpp"
#include "CommandView.hpp"
#include "OptionView.hpp"

/**
 * @file
 * @brief Defines the ParsedArguments class.
 */

namespace argos
{
    class ParsedArgumentsImpl;

    /**
     * @brief The result of the ArgumentParser. Gives access to all argument
     *      and option values.
     *
     * Instances of this class is returned by ArgumentParser::parse and
     * ArgumentIterator::parsed_arguments.
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

        [[nodiscard]] CommandView command() const;

        [[nodiscard]] std::string_view command_name() const;

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

        [[nodiscard]] std::vector<ParsedArguments> subcommands() const;

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

        /**
         * @brief Returns all argument definitions that were registered with
         *  ArgumentParser.
         *
         * Intended for testing and debugging, for instance to list all
         * defined arguments along with their given values.
        *
         * @warning The returned instances are only guaranteed to remain
         *  valid as long the ParsedArguments instance is valid.
         */
        [[nodiscard]]
        std::vector<std::unique_ptr<ArgumentView>> all_arguments() const;

        /**
         * @brief Returns all option definitions that were registered with
         *  ArgumentParser.
         *
         * Intended for testing and debugging, for instance to list all
         * defined options along with their given values.
         *
         * @warning The returned instances are only guaranteed to remain
         *  valid as long the ParsedArguments instance is valid.
         */
        [[nodiscard]]
        std::vector<std::unique_ptr<OptionView>> all_options() const;

        [[nodiscard]]
        std::vector<std::unique_ptr<CommandView>> all_subcommands() const;

        /**
         * @brief Returns the parser result code.
         */
        [[nodiscard]] ParserResultCode result_code() const;

        /**
         * @brief If the parser stopped early because it encountered an option
         *  of type 'STOP', this function returns that option.
         */
        [[nodiscard]] OptionView stop_option() const;

        /**
         * @brief Returns the command line arguments that were ignored by the
         *  argument parser.
         *
         * This function will always return an empty vector unless at least
         * one of the following is true:
         *
         * - ArgumentParser::ignore_undefined_arguments is true.
         * - ArgumentParser::ignore_undefined_options is true.
         * - ArgumentParser::auto_exit is false and there are options with
         *   type set to OptionType::STOP.
         */
        [[nodiscard]]
        const std::vector<std::string>& unprocessed_arguments() const;

        /**
         * @brief Removes parsed arguments and options from @a argv and
         *  decrements @a argc correspondingly.
         *
         * The first value in @a argv is assumed to be the program name and
         * is ignored, the remainder should be identical to the command line
         * given to ArgumentParser::parse or ArgumentParser::make_iterator.
         *
         * @note No memory is freed, the function only rearranges the pointers
         *  @a in argv.
         */
        void filter_parsed_arguments(int& argc, char**& argv) const;

        /**
         * @brief Print @a msg along with a brief help text and exit.
         *
         * @throw ArgosException if ArgumentParser::auto_exit is false.
         */
        [[noreturn]]
        void error(const std::string& msg) const;
    private:
        std::shared_ptr<ParsedArgumentsImpl> m_impl;
    };

    /**
     * @brief Write a list of all arguments and options along with their
     *  values to stdout.
     *
     * This function is intended for testing and debugging.
     */
    void print(const ParsedArguments& args);

    /**
     * @brief Write a list of all arguments and options along with their
     *  values to @a stream.
     *
     * This function is intended for testing and debugging.
     */
    void print(const ParsedArguments& parsed_args, std::ostream& stream);
}
