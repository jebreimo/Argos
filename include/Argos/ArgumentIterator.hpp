//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include "ParsedArguments.hpp"

/**
 * @file
 * @brief Defines the ArgumentIterator class.
 */
namespace Argos
{
    class ArgumentIteratorImpl;
    struct ParserData;

    /**
     * @brief Iterator class created by ArgumentParser that lets client code
     *      process one argument or option at a time.
     */
    class ArgumentIterator
    {
    public:
        /**
         * @brief Constructs a new instance of ArgumentIterator.
         *
         * Client code must use ArgumentParser::makeIterator().
         */
        ArgumentIterator(std::vector<std::string_view> args,
                         std::shared_ptr<ParserData> parserData);

        ArgumentIterator(const ArgumentIterator&) = delete;

        ArgumentIterator(ArgumentIterator&&) noexcept;

        ~ArgumentIterator();

        ArgumentIterator& operator=(const ArgumentIterator&) = delete;

        ArgumentIterator& operator=(ArgumentIterator&&) noexcept;

        /**
         * @brief Process the next argument or option and return the result.
         *
         * ParsedArguments has been updated when the function returns.
         *
         * @param arg The definition (ArgumentView or OptionView) of the
         *      processed argument or option. If ignoreUndefinedArguments
         *      or ignoreUndefinedOptions is true, this pointer can be empty.
         * @param value If @a arg is an argument then this is the argument's
         *      value. If @a arg is an option that take an argument then this
         *      is the option's value. If @a arg is empty (i.e. this is an
         *      undefined argument or option) then this is the unrecognized
         *      argument value or option flag. Otherwise @a value is empty.
         * @return true If an argument or option was processed successfully,
         *      false if there were no more arguments or an error was
         *      encountered.
         */
        bool next(std::unique_ptr<IArgumentView>& arg,
                  std::string_view& value);

        /**
         * @brief Gives access to all the arguments and options processed
         *      so far.
         *
         * The returned object is "live" in the sense that it is updated
         * behind the scenes each time next() is called. It is therefore
         * sufficient to only call this function once and keep a copy of
         * the returned object.
         */
        [[nodiscard]]
        ParsedArguments parsedArguments() const;
    private:
        ArgumentIteratorImpl& impl();

        const ArgumentIteratorImpl& impl() const;

        std::unique_ptr<ArgumentIteratorImpl> m_Impl;
    };
}
