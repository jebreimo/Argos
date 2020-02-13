//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>
#include <string>

namespace Argos
{
    namespace
    {
        template <typename T, typename Func>
        std::optional<T> parseFloatingPointImpl(const std::string& str, Func func)
        {
            char* endp = nullptr;
            errno = 0;
            auto value = func(str.c_str(), &endp);
            if (endp == str.c_str() + str.size())
            {
                if (errno == 0)
                    return value;
                return {};
            }

            std::string tmp;
            bool underscore = true;
            for (auto c : str)
            {
                if (c != '_')
                {
                    tmp.push_back(c);
                    underscore = false;
                }
                else if (!underscore)
                {
                    underscore = true;
                }
                else
                {
                    return {};
                }
            }
            return parseFloatingPointImpl<T>(tmp, func);
        }
    }

    template <typename T>
    std::optional<T> parseFloatingPoint(const std::string& str);

    template <>
    std::optional<float> parseFloatingPoint(const std::string& str)
    {
        return parseFloatingPointImpl<float>(str, strtof);
    }

    template <>
    std::optional<double> parseFloatingPoint(const std::string& str)
    {
        return parseFloatingPointImpl<double>(str, strtod);
    }

    template <>
    std::optional<long double> parseFloatingPoint(const std::string& str)
    {
        return parseFloatingPointImpl<long double>(str, strtold);
    }
}
