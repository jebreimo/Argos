//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-06.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <list>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace Argos
{
    class WordSplitter
    {
    public:
        void addWord(std::string wordRule);

        std::tuple<std::string_view, char, std::string_view>
        split(std::string_view word, size_t startPos, size_t maxLength,
              bool mustSplit) const;
    private:
        std::tuple<std::string_view, char, std::string_view>
        defaultRule(std::string_view word, size_t startPos,
                    size_t maxLength) const;

        using Split = std::pair<unsigned, char>;
        std::map<std::string_view, std::vector<Split>> m_Splits;
        std::list<std::string> m_Strings;
    };
}
