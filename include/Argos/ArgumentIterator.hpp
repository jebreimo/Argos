//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include "IArgument.hpp"

namespace Argos
{
    class ArgumentIteratorImpl;
    class ParserData;

    class ArgumentIterator
    {
    public:
        ArgumentIterator(int argc, char* argv[],
                         std::shared_ptr<ParserData> parserData);

        ArgumentIterator(const ArgumentIterator&) = delete;

        ArgumentIterator(ArgumentIterator&&) noexcept;

        ~ArgumentIterator();

        ArgumentIterator& operator=(const ArgumentIterator&) = delete;

        ArgumentIterator& operator=(ArgumentIterator&&) noexcept;

        std::pair<std::unique_ptr<const IArgument>, std::string_view> next();
    private:
        std::unique_ptr<ArgumentIteratorImpl> m_Impl;
    };
}
