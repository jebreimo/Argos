//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace argos
{
    class StandardOptionIterator
    {
    public:
        StandardOptionIterator();

        explicit StandardOptionIterator(std::vector<std::string_view> args);

        StandardOptionIterator(const StandardOptionIterator& rhs);

        std::optional<std::string> next();

        std::optional<std::string> next_value();

        [[nodiscard]] std::string_view current() const;

        [[nodiscard]] std::span<std::string> remaining_arguments();

        void insert(std::vector<std::string> args);
    private:
        void split_concatenated_flags();

        std::vector<std::string> m_all_args;
        std::span<std::string> m_args;
        size_t m_pos = 0;
    };
}
