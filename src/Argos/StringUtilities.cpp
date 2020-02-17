//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include "StringUtilities.hpp"

#include <algorithm>
#include <string>

namespace Argos
{
    bool areEqualCharsCI(char a, char b)
    {
        if (a == b)
            return true;
        auto ua = uint8_t(a), ub = uint8_t(b);
        if ((ua ^ ub) != 32)
            return false;
        return 'A' <= (ua & 0xDFu) && (ua & 0xDFu) <= 'Z';
    }

    bool areEqualCI(std::string_view str1, std::string_view str2)
    {
        if (str1.size() != str2.size())
            return false;
        return std::equal(str2.begin(), str2.end(), str1.begin(), areEqualCharsCI);
    }

    bool areEqual(std::string_view str1, std::string_view str2,
                  bool caseInsensitive)
    {
        if (caseInsensitive)
            return areEqualCI(str1, str2);
        return str1 == str2;
    }

    bool startsWith(std::string_view str, std::string_view prefix)
    {
        if (str.size() < prefix.size())
            return false;
        return std::equal(prefix.begin(), prefix.end(), str.begin());
    }

    bool startsWithCI(std::string_view str, std::string_view prefix)
    {
        if (str.size() < prefix.size())
            return false;
        return std::equal(prefix.begin(), prefix.end(), str.begin(), areEqualCharsCI);
    }

    bool startsWith(std::string_view str, std::string_view prefix, bool caseInsensitive)
    {
        if (caseInsensitive)
            return startsWithCI(str, prefix);
        return startsWith(str, prefix);
    }

    int compareCI(int c1, int c2)
    {
        if (c1 == c2)
            return 0;
        auto ic1 = int(uint8_t(c1) & 0xDFu);
        if (ic1 < 'A' || 'Z' < ic1)
            return c1 - c2;
        auto ic2 = int(uint8_t(c2) & 0xDFu);
        return ic1 - ic2;
    }

    bool isLessCI(std::string_view str1, std::string_view str2)
    {
        auto size = std::min(str1.size(), str2.size());
        for (size_t i = 0; i < size; ++i)
        {
            if (auto cmp = compareCI(str1[i], str2[i]); cmp != 0)
                return cmp < 0;
        }
        return str1.size() < str2.size();
    }

    bool isLess(std::string_view str1, std::string_view str2, bool caseInsensitive)
    {
        if (caseInsensitive)
            return isLessCI(str1, str2);
        return str1 < str2;
    }

    bool removeUnderscoresFromNumber(std::string& num)
    {
        auto inIx = num.find('_');
        if (inIx == 0 || inIx == std::string::npos)
            return false;
        if (inIx == 1 && !isalnum(num[0]))
            return false;

        auto outIx = inIx;
        bool underscore = true;
        for (++inIx; inIx < num.size(); ++inIx)
        {
            if (num[inIx] != '_')
            {
                num[outIx++] = num[inIx];
                underscore = false;
            }
            else if (!underscore)
            {
                underscore = true;
            }
            else
            {
                return false;
            }
        }
        num.erase(outIx);
        return !underscore;
    }
}
