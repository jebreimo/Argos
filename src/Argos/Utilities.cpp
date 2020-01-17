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
}
