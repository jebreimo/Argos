//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-19.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <variant>
#include "OptionIterator.hpp"
#include "StandardOptionIterator.hpp"

namespace argos
{
    struct OptionIteratorWrapper
    {
        std::optional<std::string> next()
        {
            if (std::holds_alternative<OptionIterator>(iterator))
                return std::get<OptionIterator>(iterator).next();
            else
                return std::get<StandardOptionIterator>(iterator).next();
        }

        std::optional<std::string> next_value()
        {
            if (std::holds_alternative<OptionIterator>(iterator))
                return std::get<OptionIterator>(iterator).next_value();
            else
                return std::get<StandardOptionIterator>(iterator).next_value();
        }

        [[nodiscard]] std::string_view current() const
        {
            if (std::holds_alternative<OptionIterator>(iterator))
                return std::get<OptionIterator>(iterator).current();
            else
                return std::get<StandardOptionIterator>(iterator).current();
        }

        [[nodiscard]] std::span<std::string_view> remaining_arguments() const
        {
            if (std::holds_alternative<OptionIterator>(iterator))
                return std::get<OptionIterator>(iterator).remaining_arguments();
            else
                return std::get<StandardOptionIterator>(iterator).remaining_arguments();
        }

        std::variant<OptionIterator, StandardOptionIterator> iterator;
    };
}
