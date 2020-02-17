//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include "Argos/ParseFloatingPoint.hpp"

#include <cerrno>
#include <cstdlib>
#include <string>
#include "StringUtilities.hpp"

namespace Argos
{
    namespace
    {
        template <typename T, typename Func>
        std::optional<T> parseFloatingPointImpl(std::string str, Func func)
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

            if (!removeUnderscoresFromNumber(str))
                return false;
            return parseFloatingPointImpl<T>(std::move(str), func);
        }
    }

    template <>
    std::optional<float> parseFloatingPoint(const std::string_view& str)
    {
        return parseFloatingPointImpl<float>(std::string(str), strtof);
    }

    template <>
    std::optional<double> parseFloatingPoint(const std::string_view& str)
    {
        return parseFloatingPointImpl<double>(std::string(str), strtod);
    }

    template <>
    std::optional<long double> parseFloatingPoint(const std::string_view& str)
    {
        return parseFloatingPointImpl<long double>(std::string(str), strtold);
    }
}
