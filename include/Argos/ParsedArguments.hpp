//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>

namespace Argos
{
    class ParsedArgumentsImpl;

    class ParsedArguments
    {
    public:
        ParsedArguments();

        ParsedArguments(std::unique_ptr<ParsedArgumentsImpl> impl);

        ParsedArguments(const ParsedArguments&) = delete;

        ParsedArguments(ParsedArguments&&) noexcept;

        ~ParsedArguments();

        ParsedArguments& operator=(const ParsedArguments&) = delete;

        ParsedArguments& operator=(ParsedArguments&&) noexcept;

    private:
        std::unique_ptr<ParsedArgumentsImpl> m_Impl;
    };
}
