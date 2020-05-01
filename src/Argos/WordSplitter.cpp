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

namespace Argos
{
    namespace
    {
        bool isUtf8Continuation(char c)
        {
            return (uint8_t(c) & 0xC0u) == 0x80;
        }
    }

    void WordSplitter::addWord(std::string wordRule)
    {
        size_t offset = 0;
        std::vector<Split> splits;
        for (auto pos = wordRule.find_first_of(' '); pos != std::string::npos;
             pos = wordRule.find_first_of(' ', pos + 1))
        {
            if (pos == 0 || wordRule[pos - 1] == ' ')
                ARGOS_THROW("Invalid split rule: '" + wordRule + "'");
            auto sep = wordRule[pos - 1] == '-' ? '\0' : '-';
            splits.push_back({unsigned(pos - offset), sep});
            ++offset;
        }
        splits.push_back({unsigned(wordRule.size() - offset), '\0'});
        wordRule.erase(remove(wordRule.begin(), wordRule.end(), ' '),
                       wordRule.end());
        m_Strings.push_back(move(wordRule));
        m_Splits.insert({std::string_view(m_Strings.back()), move(splits)});
    }

    std::tuple<std::string_view, char, std::string_view>
    WordSplitter::split(std::string_view word, size_t startIndex,
                        size_t maxLength, bool mustSplit) const
    {
        auto it = m_Splits.find(word);
        if (it != m_Splits.end())
        {
            Split prev = {unsigned(startIndex), '\0'};
            size_t length = 0;
            for (auto split : it->second)
            {
                if (split.index < startIndex + 1)
                    continue;
                length += countCodePoints(word.substr(prev.index, split.index - prev.index));
                if (length + (split.separator ? 1 : 0) > maxLength)
                    break;
                prev = split;
            }
            if (prev.index > startIndex + 1)
                return {word.substr(startIndex, prev.index - startIndex),
                        prev.separator,
                        word.substr(prev.index)};
        }
        if (mustSplit)
            return defaultRule(word.substr(startIndex), maxLength);
        return {{}, '\0', word};
    }

    std::tuple<std::string_view, char, std::string_view>
    WordSplitter::defaultRule(std::string_view word, size_t maxLength) const
    {
        if (maxLength <= 2)
            return {{}, '\0', word};
        auto maxPos = findNthCodePoint(word, maxLength);
        if (maxPos == std::string_view::npos)
            return {word, '\0', {}};
        auto ignoreUtf8 = maxPos == maxLength;
        --maxPos;
        while (!ignoreUtf8 && isUtf8Continuation(word[maxPos]))
            --maxPos;

        auto minPos = (maxLength + 2) / 3;
        auto index = maxPos;
        for (auto count = maxLength - 1; count-- > minPos;)
        {
            --index;
            while (!ignoreUtf8 && isUtf8Continuation(word[index]))
                --index;
            if (uint8_t(word[index - 1]) >= 127 || uint8_t(word[index]) >= 127)
                continue;
            if ((isalnum(word[index - 1]) == 0) != (isalnum(word[index]) == 0))
                return {word.substr(0, index), '\0', word.substr(index)};
            if ((isdigit(word[index - 1]) == 0) != (isdigit(word[index]) == 0))
                return {word.substr(0, index), '-', word.substr(index)};
        }
        return {word.substr(0, maxPos),
                '-',
                word.substr(maxPos)};
    }
}
