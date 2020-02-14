//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-31.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "ArgumentValueRef.hpp"
#include "ParseFloatingPoint.hpp"
#include "ParseInteger.hpp"
#include "ArgosException.hpp"

namespace Argos
{
    template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    std::optional<T> parseSingleValue(std::string s)
    {
        return parseInteger<T>(s);
    }

    template <typename T, typename std::enable_if<std::is_floating_point<T>::value, int>::type = 0>
    std::optional<T> parseSingleValue(std::string s)
    {
        return parseFloatingPoint<T>(std::string(s));
    }

    template <typename T>
    std::pair<T, ArgumentValueRef>
    parseValue(std::pair<std::string, ArgumentValueRef> value)
    {
        auto v = parseSingleValue<T>(value.first);
        if (!v)
        {
            // TODO: use ArgumentValueRef's error function.
            auto msg = "Invalid value: '" + std::string(value.first) + "'.";
            value.second.error(msg);
            ARGOS_THROW(msg);
        }
        return {std::move(*v), std::move(value.second)};
    }
}
