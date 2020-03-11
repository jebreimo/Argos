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

namespace Argos
{
    class ParsedArgumentsImpl;

    class ParsedArgumentsBuilder
    {
    public:
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

        ArgumentValue value(const std::string& name);

        ArgumentValue value(const IArgumentView& arg);

        ArgumentValues values(const std::string& name);

        ArgumentValues values(const IArgumentView& arg);

        bool has(const std::string& name);

        bool has(const IArgumentView& arg);

        void error(const std::string& errorMessage);

        void error(const std::string& errorMessage,
                   const IArgumentView& arg);
    private:
        std::shared_ptr<ParsedArgumentsImpl> m_Impl;
    };
}
