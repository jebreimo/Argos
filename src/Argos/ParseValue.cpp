//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include "ParseValue.hpp"

#include <cerrno>
#include <climits>
#include <cstdlib>
#include "StringUtilities.hpp"

namespace argos
{
    namespace
    {
        template <typename T>
        T str_to_int(const char* str, char** endp, int base);

        template <>
        long str_to_int<long>(const char* str, char** endp, int base)
        {
            return strtol(str, endp, base);
        }

        template <>
        long long str_to_int<long long>(const char* str, char** endp, int base)
        {
            return strtoll(str, endp, base);
        }

        template <>
        unsigned long
        str_to_int<unsigned long>(const char* str, char** endp, int base)
        {
            return strtoul(str, endp, base);
        }

        template <>
        unsigned long long
        str_to_int<unsigned long long>(const char* str, char** endp, int base)
        {
            return strtoull(str, endp, base);
        }

        template <typename T>
        std::optional<T> parse_integer_impl(const std::string& str, int base)
        {
            if (str.empty())
                return {};
            char* endp = nullptr;
            errno = 0;
            auto value = str_to_int<T>(str.c_str(), &endp, base);
            if (endp == str.c_str() + str.size() && errno == 0)
                return value;
            return {};
        }
    }

    template <>
    std::optional<int> parse_integer<int>(const std::string& str, int base)
    {
        const auto n = parse_integer_impl<long>(str, base);
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
    std::optional<unsigned>
    parse_integer<unsigned>(const std::string& str, int base)
    {
        auto n = parse_integer_impl<unsigned long>(str, base);
        if (!n)
            return {};

        if constexpr (sizeof(unsigned) != sizeof(unsigned long))
        {
            if (UINT_MAX < *n)
                return {};
        }
        return static_cast<unsigned>(*n);
    }

    template <>
    std::optional<long> parse_integer<long>(const std::string& str, int base)
    {
        return parse_integer_impl<long>(str, base);
    }

    template <>
    std::optional<long long>
    parse_integer<long long>(const std::string& str, int base)
    {
        return parse_integer_impl<long long>(str, base);
    }

    template <>
    std::optional<unsigned long>
    parse_integer<unsigned long>(const std::string& str, int base)
    {
        return parse_integer_impl<unsigned long>(str, base);
    }

    template <>
    std::optional<unsigned long long>
    parse_integer<unsigned long long>(const std::string& str, int base)
    {
        return parse_integer_impl<unsigned long long>(str, base);
    }

    namespace
    {
        template <typename T>
        T str_to_float(const char* str, char** endp);

        template <>
        float str_to_float<float>(const char* str, char** endp)
        {
            return strtof(str, endp);
        }

        template <>
        double str_to_float<double>(const char* str, char** endp)
        {
            return strtod(str, endp);
        }

        template <typename T>
        std::optional<T> parse_floating_point_impl(const std::string& str)
        {
            if (str.empty())
                return {};
            char* endp = nullptr;
            errno = 0;
            auto value = str_to_float<T>(str.c_str(), &endp);
            if (endp == str.c_str() + str.size() && errno == 0)
                return value;
            return {};
        }
    }

    template <>
    std::optional<float> parse_floating_point<float>(const std::string& str)
    {
        return parse_floating_point_impl<float>(str);
    }

    template <>
    std::optional<double> parse_floating_point<double>(const std::string& str)
    {
        return parse_floating_point_impl<double>(str);
    }
}
