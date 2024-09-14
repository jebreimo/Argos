//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-14.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace argos
{
    bool are_equal_ci(std::string_view str1, std::string_view str2);

    bool are_equal(std::string_view str1, std::string_view str2,
                   bool case_insensitive);

    bool starts_with(std::string_view str, std::string_view prefix);

    bool starts_with_ci(std::string_view str, std::string_view prefix);

    bool starts_with(std::string_view str, std::string_view prefix,
                     bool case_insensitive);

    bool is_less_ci(std::string_view str1, std::string_view str2);

    bool is_less(std::string_view str1, std::string_view str2,
                 bool case_insensitive);

    std::vector<std::string_view>
    split_string(std::string_view s, char delimiter, size_t max_split);

    std::string_view get_base_name(std::string_view str);

    size_t count_code_points(std::string_view str);

    size_t find_nth_code_point(std::string_view str, size_t n);

    char to_lower(char c);

    void to_lower(std::string& word);

    std::string to_lower(std::string_view word);

    bool is_lower(std::string_view word);

    inline void pop_front(std::span<std::string_view>& span)
    {
        span = span.subspan(1);
    }
}
