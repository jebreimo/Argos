//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-18.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <optional>
#include <string>
#include <string_view>
#include <span>

namespace argos
{
    class IOptionIterator
    {
    public:
        virtual ~IOptionIterator() = default;

        virtual std::optional<std::string> next() = 0;

        virtual std::optional<std::string> next_value() = 0;

        [[nodiscard]] virtual std::string_view current() const = 0;

        [[nodiscard]] virtual std::span<std::string_view>
        remaining_arguments() const = 0;

        [[nodiscard]] virtual IOptionIterator* clone() const = 0;
    };
}
