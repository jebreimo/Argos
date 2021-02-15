//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "ArgumentValue.hpp"
#include "ArgumentValues.hpp"

/**
 * @file
 * @brief Defines the ParsedArgumentsBuilder class.
 */

namespace Argos
{
    class ParsedArgumentsImpl;

    /**
     * @brief An interface to ParsedArguments that lets argument and option
     *      callbacks query and modify the parsed argument values
     */
    class ParsedArgumentsBuilder
    {
    public:
        /**
         * @private
         */
        explicit ParsedArgumentsBuilder(
                std::shared_ptr<ParsedArgumentsImpl> impl);

        ParsedArgumentsBuilder& append(const std::string& name,
                                       const std::string& value);

        ParsedArgumentsBuilder& append(const IArgumentView& arg,
                                       const std::string& value);

        ParsedArgumentsBuilder& assign(const std::string& name,
                                       const std::string& value);

        ParsedArgumentsBuilder& assign(const IArgumentView& arg,
                                       const std::string& value);

        ParsedArgumentsBuilder& clear(const std::string& name);

        ParsedArgumentsBuilder& clear(const IArgumentView& arg);

        [[nodiscard]] ArgumentValue value(const std::string& name) const;

        [[nodiscard]] ArgumentValue value(const IArgumentView& arg) const;

        [[nodiscard]] ArgumentValues values(const std::string& name) const;

        [[nodiscard]] ArgumentValues values(const IArgumentView& arg) const;

        [[nodiscard]] bool has(const std::string& name) const;

        [[nodiscard]] bool has(const IArgumentView& arg) const;

        void error(const std::string& errorMessage);

        void error(const std::string& errorMessage, const IArgumentView& arg);

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
        [[nodiscard]] const std::string& programName() const;
    private:
        std::shared_ptr<ParsedArgumentsImpl> m_Impl;
    };
}
