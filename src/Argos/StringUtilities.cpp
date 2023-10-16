//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "StringUtilities.hpp"

#include <algorithm>

namespace argos
{
    bool are_equal_chars_ci(char a, char b)
    {
        if (a == b)
            return true;
        auto ua = uint8_t(a), ub = uint8_t(b);
        if ((ua ^ ub) != 32)
            return false;
        return 'A' <= (ua & 0xDFu) && (ua & 0xDFu) <= 'Z';
    }

    bool are_equal_ci(std::string_view str1, std::string_view str2)
    {
        if (str1.size() != str2.size())
            return false;
        return std::equal(str2.begin(), str2.end(), str1.begin(), are_equal_chars_ci);
    }

    bool are_equal(std::string_view str1, std::string_view str2,
                   bool case_insensitive)
    {
        if (case_insensitive)
            return are_equal_ci(str1, str2);
        return str1 == str2;
    }

    bool starts_with(std::string_view str, std::string_view prefix)
    {
        if (str.size() < prefix.size())
            return false;
        return std::equal(prefix.begin(), prefix.end(), str.begin());
    }

    bool starts_with_ci(std::string_view str, std::string_view prefix)
    {
        if (str.size() < prefix.size())
            return false;
        return std::equal(prefix.begin(), prefix.end(), str.begin(),
                          are_equal_chars_ci);
    }

    bool starts_with(std::string_view str, std::string_view prefix,
                     bool case_insensitive)
    {
        if (case_insensitive)
            return starts_with_ci(str, prefix);
        return starts_with(str, prefix);
    }

    int compare_ci(int c1, int c2)
    {
        if (c1 == c2)
            return 0;
        auto ic1 = int(uint8_t(c1) & 0xDFu);
        if (ic1 < 'A' || 'Z' < ic1)
            return c1 - c2;
        auto ic2 = int(uint8_t(c2) & 0xDFu);
        return ic1 - ic2;
    }

    bool is_less_ci(std::string_view str1, std::string_view str2)
    {
        auto size = std::min(str1.size(), str2.size());
        for (size_t i = 0; i < size; ++i)
        {
            if (auto cmp = compare_ci(str1[i], str2[i]); cmp != 0)
                return cmp < 0;
        }
        return str1.size() < str2.size();
    }

    bool is_less(std::string_view str1, std::string_view str2,
                 bool case_insensitive)
    {
        if (case_insensitive)
            return is_less_ci(str1, str2);
        return str1 < str2;
    }

    std::vector<std::string_view>
    split_string(std::string_view s, char delimiter, size_t max_split)
    {
        if (max_split == 0)
            return {s};

        std::vector<std::string_view> result;
        size_t pos = 0;
        while (true)
        {
            auto next_pos = s.find(delimiter, pos);
            result.push_back(s.substr(pos, next_pos - pos));
            if (next_pos == std::string_view::npos)
                break;
            pos = next_pos + 1;
            if (result.size() == max_split)
            {
                result.push_back(s.substr(pos));
                break;
            }
        }
        return result;
    }

    std::string_view get_base_name(std::string_view str)
    {
        auto pos = str.find_last_of("/\\");
        return pos == std::string_view::npos ? str : str.substr(pos + 1);
    }

    constexpr size_t get_code_point_length(char c) noexcept
    {
        auto u = unsigned(static_cast<uint8_t>(c));
        if (u < 0x80)
            return 1;
        if (u > 0xF7)
            return 0;
        switch (unsigned(u >> 4u) & 7u)
        {
        case 7: return 4;
        case 6: return 3;
        case 5:
        case 4: return 2;
        default: break;
        }
        return 0;
    }

    size_t count_code_points(std::string_view str)
    {
        size_t count = 0;
        size_t char_len = 0;
        for (auto c : str)
        {
            if (char_len == 0)
            {
                char_len = get_code_point_length(c);
                if (char_len == 0)
                    return str.size();
                ++count;
                --char_len;
            }
            else if ((unsigned(static_cast<uint8_t>(c)) & 0xC0u) == 0x80u)
            {
                --char_len;
            }
            else
            {
                return str.size();
            }
        }
        return count;
    }

    size_t find_nth_code_point(std::string_view str, size_t n)
    {
        if (n >= str.size())
            return std::string_view::npos;
        size_t count = 0;
        size_t char_len = 0;
        for (size_t i = 0; i < str.size(); ++i)
        {
            if (char_len == 0)
            {
                if (count == n)
                    return i;
                char_len = get_code_point_length(str[i]);
                if (char_len == 0)
                    return n;
                ++count;
                --char_len;
            }
            else if ((static_cast<uint8_t>(str[i]) & 0xC0u) == 0x80u)
            {
                --char_len;
            }
            else
            {
                return n;
            }
        }
        return char_len ? n : std::string_view::npos;
    }
}
