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
        int getDigit(char c)
        {
            return int(uint8_t(c) ^ 0x30u);
        }
    }

    template <typename T>
    std::optional<T> parseFloatingPoint(std::string_view str)
    {
        if (str.empty())
            return {};

        size_t i = 0;
        // Get the sign of the number
        bool negative = false;
        if (str[0] == '-')
        {
            negative = true;
            if (++i == str.size())
                return {};
        }
        else if (str[0] == '+')
        {
            if (++i == str.size())
                return {};
        }

        // Get the integer value
        auto value = T(getDigit(str[i]));
        if (value > 9)
        {
            if (str == "Infinity" || str == "+Infinity")
                return std::numeric_limits<T>::infinity();
            if (str == "-Infinity")
                return -std::numeric_limits<T>::infinity();
            if (str == "NaN")
                return std::numeric_limits<T>::quiet_NaN();
            return {};
        }

        bool underscore = false;
        for (++i; i < str.size(); ++i)
        {
            auto digit = getDigit(str[i]);
            if (digit <= 9)
            {
                value *= 10;
                value += digit;
                underscore = false;
            }
            else if (str[i] != '_' || underscore)
            {
                break;
            }
            else
            {
                underscore = true;
            }
        }

        if (underscore)
            return {};

        if (i == str.size())
            return !negative ? value : -value;

        // Get the fraction
        underscore = true; // Makes underscore after point illegal.
        if (str[i] == '.')
        {
            int decimals = 0;
            T fraction = {};
            for (++i; i < str.size(); ++i)
            {
                auto digit = getDigit(str[i]);
                if (digit <= 9)
                {
                    fraction *= 10;
                    fraction += digit;
                    underscore = false;
                    ++decimals;
                }
                else if (str[i] != '_' || underscore)
                {
                    break;
                }
                else
                {
                    underscore = true;
                }
            }
            value += fraction * pow(T(10), -decimals);
        }

        // Get the exponent
        int exponent = 0;
        if (i != str.size())
        {
            if ((str[i] & 0xDF) != 'E')
                return {};

            if (++i == str.size())
                return {};

            bool negativeExponent = false;
            if (str[i] == '-')
            {
                negativeExponent = true;
                if (++i == str.size())
                    return {};
            }
            else if (str[i] == '+')
            {
                if (++i == str.size())
                    return {};
            }

            exponent += getDigit(str[i]);
            if (exponent > 9)
                return {};

            for (++i; i != str.size(); ++i)
            {
                auto digit = getDigit(str[i]);
                if (digit <= 9)
                {
                    exponent *= 10;
                    exponent += digit;
                    underscore = false;
                }
                else if (str[i] != '_' || underscore)
                {
                    return {};
                }
                else
                {
                    underscore = true;
                }
                if (exponent > std::numeric_limits<T>::max_exponent10)
                    return {};
            }
            if (negativeExponent)
                exponent = -exponent;
        }

        if (exponent)
            value *= pow(T(10), exponent);

        // Add the sign
        if (negative)
            value = -value;

        return value;
    }

    template <typename T>
    std::optional<T> parseFloatingPoint2(const std::string& str);

    template <typename T, typename Func>
    std::optional<T> parseFloatingPoint2Impl(const std::string& str, Func func)
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
        return parseFloatingPoint2Impl<T>(tmp, func);
    }

    template <>
    std::optional<float> parseFloatingPoint2(const std::string& str)
    {
        return parseFloatingPoint2Impl<float>(str, strtof);
    }

    template <>
    std::optional<double> parseFloatingPoint2(const std::string& str)
    {
        return parseFloatingPoint2Impl<double>(str, strtod);
    }

    template <>
    std::optional<long double> parseFloatingPoint2(const std::string& str)
    {
        return parseFloatingPoint2Impl<long double>(str, strtold);
    }
}
