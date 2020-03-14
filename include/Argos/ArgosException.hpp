//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-14.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <stdexcept>
#include <string>

/**
 * @file
 * @brief Defines the ArgosException class.
 */

/**
 * @brief The namespace for all Argos classes and functions.
 */
namespace Argos
{
    /**
     * @brief The exception class used throughout Argos.
     */
    class ArgosException : public std::runtime_error
    {
    public:
        ArgosException() noexcept
            : std::runtime_error("Unspecified error.")
        {}

        /**
         * @brief Passes @a message on to the base class.
         */
        explicit ArgosException(const std::string& message) noexcept
            : std::runtime_error(message)
        {}

        /**
         * @brief Prefixes @a message with source file, line number
         * and function name.
         * @param message The error message.
         * @param fileName Typically the value of __FILE__ where the
         *      exception was thrown.
         * @param lineno Typically the value of __LINE__ where the
         *      exception was thrown.
         * @param funcName Typically the value of __func__ where the
         *      exception was thrown.
         */
        ArgosException(const std::string& message,
                       const std::string& fileName,
                       int lineno,
                       const std::string& funcName)
            : std::runtime_error(funcName + "() in " + fileName + ":"
                                 + std::to_string(lineno) + ": " + message)
        {}
    };
}
