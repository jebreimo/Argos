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
#include <string>
#include <type_traits>

namespace Argos
{
    template <typename T>
    std::optional<T> parseInteger(const std::string& str, int base);

    template <>
    std::optional<int> parseInteger<int>(const std::string& str, int base);

    template <>
    std::optional<long> parseInteger<long>(const std::string& str, int base);

    template <>
    std::optional<long long> parseInteger<long long>(const std::string& str, int base);

    template <>
    std::optional<unsigned long> parseInteger<unsigned long>(const std::string& str, int base);

    template <>
    std::optional<unsigned long long> parseInteger<unsigned long long>(const std::string& str, int base);

    template <typename T>
    std::optional<T> parseFloatingPoint(const std::string& str);

    template <>
    std::optional<float> parseFloatingPoint<float>(const std::string& str);

    template <>
    std::optional<double> parseFloatingPoint<double>(const std::string& str);
}
