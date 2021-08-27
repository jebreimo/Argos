//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-06.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "WordSplitter.hpp"

#include <algorithm>
#include "ArgosThrow.hpp"
#include "StringUtilities.hpp"

namespace argos
{
    namespace
    {
        bool is_utf8_continuation(char c)
        {
            return (uint8_t(c) & 0xC0u) == 0x80;
        }
    }

    void WordSplitter::add_word(std::string word_rule)
    {
        size_t offset = 0;
        std::vector<Split> splits;
        for (auto pos = word_rule.find_first_of(' '); pos != std::string::npos;
             pos = word_rule.find_first_of(' ', pos + 1))
        {
            if (pos == 0 || word_rule[pos - 1] == ' ')
                ARGOS_THROW("Invalid split rule: '" + word_rule + "'");
            auto sep = word_rule[pos - 1] == '-' ? '\0' : '-';
            splits.push_back({unsigned(pos - offset), sep});
            ++offset;
        }
        splits.push_back({unsigned(word_rule.size() - offset), '\0'});
        word_rule.erase(remove(word_rule.begin(), word_rule.end(), ' '),
                        word_rule.end());
        m_strings.push_back(move(word_rule));
        m_splits.insert({std::string_view(m_strings.back()), move(splits)});
    }

    std::tuple<std::string_view, char, std::string_view>
    WordSplitter::split(std::string_view word, size_t start_index,
                        size_t max_length, bool must_split) const
    {
        auto it = m_splits.find(word);
        if (it != m_splits.end())
        {
            Split prev = {unsigned(start_index), '\0'};
            size_t length = 0;
            for (auto split : it->second)
            {
                if (split.index < start_index + 1)
                    continue;
                length += count_code_points(word.substr(prev.index, split.index - prev.index));
                if (length + (split.separator ? 1 : 0) > max_length)
                    break;
                prev = split;
            }
            if (prev.index > start_index + 1)
                return {word.substr(start_index, prev.index - start_index),
                        prev.separator,
                        word.substr(prev.index)};
        }
        if (must_split)
            return default_rule(word.substr(start_index), max_length);
        return {{}, '\0', word};
    }

    std::tuple<std::string_view, char, std::string_view>
    WordSplitter::default_rule(std::string_view word, size_t max_length) const
    {
        if (max_length <= 2)
            return {{}, '\0', word};
        auto max_pos = find_nth_code_point(word, max_length);
        if (max_pos == std::string_view::npos)
            return {word, '\0', {}};
        auto ignore_utf8 = max_pos == max_length;
        --max_pos;
        if (!ignore_utf8)
        {
            while (is_utf8_continuation(word[max_pos]))
                --max_pos;
        }

        auto min_pos = (max_length + 2) / 3;
        auto index = max_pos;
        for (auto count = max_length - 1; count-- > min_pos;)
        {
            --index;
            if (!ignore_utf8)
            {
                while (is_utf8_continuation(word[index]))
                    --index;
            }
            if (uint8_t(word[index - 1]) >= 127 || uint8_t(word[index]) >= 127)
                continue;
            if ((isalnum(word[index - 1]) == 0) != (isalnum(word[index]) == 0))
                return {word.substr(0, index), '\0', word.substr(index)};
            if ((isdigit(word[index - 1]) == 0) != (isdigit(word[index]) == 0))
                return {word.substr(0, index), '-', word.substr(index)};
        }
        return {word.substr(0, max_pos),
                '-',
                word.substr(max_pos)};
    }
}
