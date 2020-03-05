//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-06.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgosException.hpp"
#include "WordSplitter.hpp"

namespace Argos
{
    namespace
    {
        bool isVowel(char c)
        {
            constexpr auto VOWELS = "aeiouyAEIOUY";
            return memchr(VOWELS, c, 6) != nullptr;
        }
    }

    void WordSplitter::addWord(std::string wordRule)
    {
        size_t offset = 0;
        std::vector<Split> splits;
        for (auto i = wordRule.find_first_of(' '); i != std::string::npos;
             i = wordRule.find_first_of(' ', i + 1))
        {
            if (i == 0 || wordRule[i - 1] == ' ')
                ARGOS_THROW("Invalid split rule: '" + wordRule + "'");
            auto sep = wordRule[i - 1] == '-' ? '\0' : '-';
            splits.emplace_back(unsigned(i - offset), sep);
            ++offset;
        }
        wordRule.erase(remove(wordRule.begin(), wordRule.end(), ' '),
                       wordRule.end());
        m_Strings.push_back(move(wordRule));
        m_Splits.insert({std::string_view(m_Strings.back()), move(splits)});
    }

    void WordSplitter::addWord(const std::string& word, const std::string& parts)
    {

    }

    std::tuple<std::string_view, char, std::string_view>
    WordSplitter::split(std::string_view word, size_t startPos,
                        size_t maxLength, bool mustSplit) const
    {
        if (word.size() - startPos <= maxLength)
            return {word.substr(startPos), '\0', {}};
        auto it = m_Splits.find(word);
        if (it != m_Splits.end())
        {
            Split prev = {};
            for (auto split : it->second)
            {
                auto length = split.first - startPos + (split.second ? 1 : 0);
                if (length > maxLength)
                {
                    if (prev.first > startPos + 1)
                        return {word.substr(startPos, prev.first - startPos),
                                prev.second,
                                word.substr(prev.first)};
                    break;
                }
                prev = split;
            }
        }
        if (mustSplit)
            return defaultRule(word.substr(startPos), startPos, maxLength);
        return {{}, '\0', word};
    }

    std::tuple<std::string_view, char, std::string_view>
    WordSplitter::defaultRule(std::string_view word, size_t startPos,
                              size_t maxLength) const
    {
        if (word.size() - startPos <= maxLength)
            return {word.substr(startPos), '\0', {}};
        if (maxLength <= 2)
            return {{}, '\0', word};
        auto index = startPos + maxLength - 1;
        auto minPos = startPos + (maxLength + 2) / 3;
        while (index-- > minPos)
        {
            if (isalnum(word[index - 1]) != isalnum(word[index]))
                return {word.substr(startPos, index - startPos),
                        '\0',
                        word.substr(index)};
            if (isnumber(word[index - 1]) != isnumber(word[index]))
                break;
            if (isalpha(word[index]) && !isVowel(word[index])
                && word[index] != word[index - 1]
                && word[index] != word[index + 1])
                break;
        }
        return {word.substr(startPos, index - startPos),
                '-',
                word.substr(index)};
    }
}
