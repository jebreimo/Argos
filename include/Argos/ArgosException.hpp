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

namespace Argos
{
    class ArgosException : public std::runtime_error
    {
    public:
        ArgosException() noexcept
                : std::runtime_error("Unspecified error.")
        {}

        explicit ArgosException(const std::string& message) noexcept
                : std::runtime_error(message)
        {}

        ArgosException(const std::string& message,
                       const std::string& fileName,
                       int lineno,
                       const std::string& funcName)
                : std::runtime_error(message)
        {
            if (!funcName.empty())
                m_Message += funcName + "() in ";
            m_Message += fileName + ":" + std::to_string(lineno) + ": " + message;
        }

        const char* what() const noexcept override
        {
            if (!m_Message.empty())
                return m_Message.c_str();
            return std::runtime_error::what();
        }

    private:
        std::string m_Message;
    };
}

#define ARGOS_THROW(msg) \
        throw ::Argos::ArgosException((msg), __FILE__, __LINE__, __FUNCTION__)
