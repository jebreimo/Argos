//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <optional>
#include <string_view>

namespace Argos
{
    class ArgumentIteratorImpl;

    class RawArg
    {
    public:
        RawArg(std::shared_ptr<ArgumentIteratorImpl> parser, std::string_view value);

        bool hasValue() const;
    private:
        std::shared_ptr<ArgumentIteratorImpl> m_Parser;
        std::optional<std::string_view> m_Value;
    };
}
