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
#include "OptionView.hpp"

namespace Argos
{
    class ParsedArgumentsImpl;

    class ParsedArguments
    {
    public:
        ParsedArguments();

        ParsedArguments(std::shared_ptr<ParsedArgumentsImpl> impl);

        ParsedArguments(const ParsedArguments&) = delete;

        ParsedArguments(ParsedArguments&&) noexcept;

        ~ParsedArguments();

        ParsedArguments& operator=(const ParsedArguments&) = delete;

        ParsedArguments& operator=(ParsedArguments&&) noexcept;

        bool has(const std::string& name) const;

        ArgumentValue value(const std::string& name) const;

        ArgumentValues values(const std::string& name) const;

        ParserResultCode resultCode() const;

        OptionView stopOption() const;

        const std::vector<std::string>& unprocessedArguments() const;
    private:
        std::shared_ptr<ParsedArgumentsImpl> m_Impl;
    };
}
