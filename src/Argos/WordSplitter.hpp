//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-06.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <list>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace argos
{
    class WordSplitter
    {
    public:
        void add_word(std::string word_rule);

        void add_words(std::vector<std::string> word_rules);

        [[nodiscard]] std::tuple<std::string_view, char, std::string_view>
        split(std::string_view word, size_t start_index, size_t max_length,
              bool must_split) const;
    private:
        static std::tuple<std::string_view, char, std::string_view>
        default_rule(std::string_view word, size_t max_length) ;

        struct Split
        {
            unsigned index;
            char separator;
        };
        std::map<std::string_view, std::vector<Split>> m_splits;
        std::list<std::string> m_strings;
    };
}
