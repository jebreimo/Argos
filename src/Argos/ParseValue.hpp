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

namespace argos
{
    template <typename T>
    std::optional<T> parse_integer(const std::string& str, int base) = delete;

    template <>
    std::optional<int> parse_integer<int>(const std::string& str, int base);

    template <>
    std::optional<unsigned>
    parse_integer<unsigned>(const std::string& str, int base);

    template <>
    std::optional<long> parse_integer<long>(const std::string& str, int base);

    template <>
    std::optional<long long>
    parse_integer<long long>(const std::string& str, int base);

    template <>
    std::optional<unsigned long>
    parse_integer<unsigned long>(const std::string& str, int base);

    template <>
    std::optional<unsigned long long>
    parse_integer<unsigned long long>(const std::string& str, int base);

    template <typename T>
    std::optional<T> parse_floating_point(const std::string& str);

    template <>
    std::optional<float> parse_floating_point<float>(const std::string& str);

    template <>
    std::optional<double> parse_floating_point<double>(const std::string& str);
}
