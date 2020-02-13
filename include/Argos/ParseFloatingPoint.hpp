//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <optional>
#include <string>

namespace Argos
{
    template <typename T>
    std::optional<T> parseFloatingPoint(const std::string& str);

    template <>
    std::optional<float> parseFloatingPoint(const std::string& str);

    template <>
    std::optional<double> parseFloatingPoint(const std::string& str);

    template <>
    std::optional<long double> parseFloatingPoint(const std::string& str);
}
