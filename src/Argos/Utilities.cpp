//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include "Utilities.hpp"

#include <algorithm>

namespace Argos
{
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
        return std::equal(prefix.begin(), prefix.end(), str.begin(),
                          [](auto a, auto b)
                          {
                              if (a == b)
                                  return true;
                              if ((a ^ b) != 32)
                                  return false;
                              return 'A' <= (a & 0xDF) && (a & 0xDF) <= 'Z';
                          });
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
}
