//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
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
        constexpr T maxPrecedingDigitsNegative()
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
        constexpr T maxPrecedingDigitsPositive()
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
                    && (value < maxPrecedingDigitsPositive<IntT, Base>()
                        || (value == maxPrecedingDigitsPositive<IntT, Base>()
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
                        && (value < maxPrecedingDigitsNegative<IntT, Base>()
                            || (value == maxPrecedingDigitsNegative<IntT, Base>()
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

    template <typename IntT>
    std::optional<IntT> parseInteger(std::string_view str)
    {
        if (str.empty())
            return {};

        bool positive = true;
        size_t i = 0;
        if (str[0] == '-')
        {
            positive = false;
            if (str.size() == 1)
                return {};
            i = 1;
        }
        else if (str[0] == '+')
        {
            if (str.size() == 1)
                return {};
            i = 1;
        }

        if (str[i] == '0')
        {
            if (++i == str.size())
                return {0};
            auto str2 = str.substr(i + 1);
            switch (uint8_t(str[i]) | 0x20u)
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
            case '_':
                if (str2.empty())
                    return {};
                [[fallthrough]];
            default:
                return positive ? parsePositiveIntegerImpl<IntT, 10>(str)
                                : parseNegativeIntegerImpl<IntT, 10>(str);
            }
        }
        if ('0' <= str[i] && str[i] <= '9')
        {
            auto str2 = str.substr(i);
            return positive ? parsePositiveIntegerImpl<IntT, 10>(str2)
                            : parseNegativeIntegerImpl<IntT, 10>(str2);
        }
        if (str == "false")
            return {0};
        if (str == "null")
            return {0};
        if (str == "true")
            return {1};
        return {};
    }
}
