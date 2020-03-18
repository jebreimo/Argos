//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include "ParseValue.hpp"

#include <cerrno>
#include <cstdlib>
#include "StringUtilities.hpp"

namespace Argos
{
    namespace
    {
        template <typename T>
        T strToInt(const char* str, char** endp, int base);

        template <>
        long strToInt<long>(const char* str, char** endp, int base)
        {
            return strtol(str, endp, base);
        }

        template <>
        long long strToInt<long long>(const char* str, char** endp, int base)
        {
            return strtoll(str, endp, base);
        }

        template <>
        unsigned long
        strToInt<unsigned long>(const char* str, char** endp, int base)
        {
            return strtoul(str, endp, base);
        }

        template <>
        unsigned long long
        strToInt<unsigned long long>(const char* str, char** endp, int base)
        {
            return strtoull(str, endp, base);
        }

        template <typename T>
        std::optional<T> parseIntegerImpl(const std::string& str, int base)
        {
            char* endp = nullptr;
            errno = 0;
            auto value = strToInt<T>(str.c_str(), &endp, base);
            if (endp == str.c_str() + str.size() && errno == 0)
                return value;
            return {};
        }
    }

    template <>
    std::optional<int> parseInteger<int>(const std::string& str, int base)
    {
        auto n = parseIntegerImpl<long>(str, base);
        if (!n)
            return {};

        if constexpr (sizeof(int) != sizeof(long))
        {
            if (*n < INT_MIN || INT_MAX < *n)
                return {};
        }
        return static_cast<int>(*n);
    }

    template <>
    std::optional<long> parseInteger<long>(const std::string& str, int base)
    {
        return parseIntegerImpl<long>(str, base);
    }

    template <>
    std::optional<long long>
    parseInteger<long long>(const std::string& str, int base)
    {
        return parseIntegerImpl<long long>(str, base);
    }

    template <>
    std::optional<unsigned long>
    parseInteger<unsigned long>(const std::string& str, int base)
    {
        return parseIntegerImpl<unsigned long>(str, base);
    }

    template <>
    std::optional<unsigned long long>
    parseInteger<unsigned long long>(const std::string& str, int base)
    {
        return parseIntegerImpl<unsigned long long>(str, base);
    }

    namespace
    {
        template <typename T>
        T strToFloat(const char* str, char** endp);

        template <>
        float strToFloat<float>(const char* str, char** endp)
        {
            return strtof(str, endp);
        }

        template <>
        double strToFloat<double>(const char* str, char** endp)
        {
            return strtof(str, endp);
        }

        template <typename T>
        std::optional<T> parseFloatingPointImpl(const std::string& str)
        {
            char* endp = nullptr;
            errno = 0;
            auto value = strToFloat<T>(str.c_str(), &endp);
            if (endp == str.c_str() + str.size() && errno == 0)
                return value;
            return {};
        }
    }

    template <>
    std::optional<float> parseFloatingPoint<float>(const std::string& str)
    {
        return parseFloatingPointImpl<float>(str);
    }

    template <>
    std::optional<double> parseFloatingPoint<double>(const std::string& str)
    {
        return parseFloatingPointImpl<double>(str);
    }
}
