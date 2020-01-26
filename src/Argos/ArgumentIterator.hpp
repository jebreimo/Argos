//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace Argos
{
    class ArgumentIterator
    {
    public:
        ArgumentIterator();

        explicit ArgumentIterator(std::vector<std::string_view> args);

        std::optional<std::string> next();

        std::optional<std::string> nextValue();

        std::string_view current() const;

        std::vector<std::string_view> remainingArguments() const;
    private:
        std::vector<std::string_view> m_Args;
        std::vector<std::string_view>::const_iterator m_ArgsIt;
        size_t m_Pos;
    };
}
