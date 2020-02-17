//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cerrno>
#include <cinttypes>
#include <optional>
#include <string_view>
#include <type_traits>

namespace Argos
{
    namespace
    {
        template <typename IntT>
        IntT fromDigit(char c)
        {
            if ('0' <= c && c <= '9')
                return IntT(c - '0');
            auto u = uint8_t(c);
            u &= 0xDFu;
            if ('A' <= u && u <= 'Z')
                return IntT(10 + u - 'A');
            return std::numeric_limits<IntT>::max();
        }

        template <typename T, T Base>
        constexpr T maxPrecedingValueNegative()
        {
            if constexpr (std::is_signed<T>::value)
            {
                using U = typename std::make_unsigned<T>::type;
                return T((U(1) << (sizeof(T) * 8 - 1)) / U(Base));
            }
            else
            {
                return T(0);
            }
        }

        template <typename T, T Base>
        constexpr T maxFinalDigitNegative()
        {
            if constexpr (std::is_signed<T>::value)
            {
                using U = typename std::make_unsigned<T>::type;
                return T((U(1) << (sizeof(T) * 8 - 1)) % U(Base));
            }
            else
            {
                return T(0);
            }
        }

        template <typename T, T Base>
        constexpr T maxPrecedingValuePositive()
        {
            if constexpr (std::is_signed<T>::value)
            {
                using U = typename std::make_unsigned<T>::type;
                return T(U(~(U(1) << (sizeof(T) * 8 - 1))) / U(Base));
            }
            else
            {
                return T(~T(0)) / Base;
            }
        }

        template <typename T, T Base>
        constexpr T maxFinalDigitPositive()
        {
            if constexpr (std::is_signed<T>::value)
            {
                using U = typename std::make_unsigned<T>::type;
                return T(U(~(U(1) << (sizeof(T) * 8 - 1))) % U(Base));
            }
            else
            {
                return T(~T(0)) % Base;
            }
        }

        template <typename IntT, IntT Base>
        std::optional<IntT> parsePositiveIntegerImpl(std::string_view str)
        {
            if (str.empty())
                return {};
            IntT value = fromDigit<IntT>(str[0]);
            if (value >= Base)
                return {};
            for (size_t i = 1; i < str.size(); ++i)
            {
                auto digit = fromDigit<IntT>(str[i]);
                if (digit < Base
                    && (value < maxPrecedingValuePositive<IntT, Base>()
                        || (value == maxPrecedingValuePositive<IntT, Base>()
                            && digit <= maxFinalDigitPositive<IntT, Base>())))
                {
                    value *= Base;
                    value += digit;
                }
                else if (str[i] != '_' || i == str.size() - 1
                         || str[i - 1] == '_')
                {
                    return {};
                }
            }
            return value;
        }

        template <typename IntT, IntT Base>
        std::optional<IntT> parseNegativeIntegerImpl(std::string_view str)
        {
            if constexpr (std::is_signed<IntT>::value)
            {
                if (str.empty())
                    return {};
                IntT value = fromDigit<IntT>(str[0]);
                if (value >= Base)
                    return {};
                for (size_t i = 1; i < str.size(); ++i)
                {
                    auto digit = fromDigit<IntT>(str[i]);
                    if (digit < Base
                        && (value < maxPrecedingValueNegative<IntT, Base>()
                            || (value == maxPrecedingValueNegative<IntT, Base>()
                                && digit <= maxFinalDigitNegative<IntT, Base>())))
                    {
                        value *= Base;
                        value += digit;
                    }
                    else if (str[i] != '_' || i == str.size() - 1
                             || str[i - 1] == '_')
                    {
                        return {};
                    }
                }
                return -value;
            }
            else
            {
                if (str.empty())
                    return {};
                for (char c : str)
                {
                    auto digit = fromDigit<IntT>(c);
                    if (digit > 0)
                        return {};
                }
                return 0;
            }
        }
    }

    template <typename IntT, typename std::enable_if<std::is_integral<IntT>::value, int>::type = 0>
    std::optional<IntT> parseValue(std::string_view num)
    {
        static_assert(std::is_integral<IntT>());

        if (num.empty())
            return {};

        auto str = num;
        bool positive = true;
        if (str[0] == '-')
        {
            positive = false;
            str = str.substr(1);
        }
        else if (str[0] == '+')
        {
            str = str.substr(1);
        }

        if (str.empty())
            return {};

        if (str[0] == '0' && str.size() >= 3)
        {
            auto str2 = str.substr(2);
            switch (uint8_t(str[1]) | 0x20u)
            {
            case 'b':
                return positive ? parsePositiveIntegerImpl<IntT, 2>(str2)
                                : parseNegativeIntegerImpl<IntT, 2>(str2);
            case 'o':
                return positive ? parsePositiveIntegerImpl<IntT, 8>(str2)
                                : parseNegativeIntegerImpl<IntT, 8>(str2);
            case 'x':
                return positive ? parsePositiveIntegerImpl<IntT, 16>(str2)
                                : parseNegativeIntegerImpl<IntT, 16>(str2);
            default:
                break;
            }
        }
        if ('0' <= str[0] && str[0] <= '9')
        {
            return positive ? parsePositiveIntegerImpl<IntT, 10>(str)
                            : parseNegativeIntegerImpl<IntT, 10>(str);
        }
        if (num == "false")
            return 0;
        if (num == "true")
            return 1;
        return {};
    }

    template <typename T, typename std::enable_if<std::is_floating_point<T>::value, int>::type = 0>
    std::optional<T> parseValue(std::string_view str);

    template <>
    std::optional<float> parseValue<float>(std::string_view str);

    template <>
    std::optional<double> parseValue<double>(std::string_view str);

    template <>
    std::optional<long double> parseValue<long double>(std::string_view str);
}
