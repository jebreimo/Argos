//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include "ParsedArguments.hpp"

namespace Argos
{
    class ArgumentIteratorImpl;
    struct ParserData;

    class ArgumentIterator
    {
    public:
        ArgumentIterator(std::vector<std::string_view> args,
                         std::shared_ptr<ParserData> parserData);

        ArgumentIterator(const ArgumentIterator&) = delete;

        ArgumentIterator(ArgumentIterator&&) noexcept;

        ~ArgumentIterator();

        ArgumentIterator& operator=(const ArgumentIterator&) = delete;

        ArgumentIterator& operator=(ArgumentIterator&&) noexcept;

        bool next(std::unique_ptr<IArgumentView>& arg,
                  std::string_view& value);

        ParsedArguments parsedArguments() const;
    private:
        ArgumentIteratorImpl& impl();

        const ArgumentIteratorImpl& impl() const;

        std::unique_ptr<ArgumentIteratorImpl> m_Impl;
    };
}
