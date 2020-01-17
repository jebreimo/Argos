//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-14.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string_view>

namespace Argos
{
    // Copied from C++17 in Detail by Bartłomiej Filipek
    template <class... Ts>
    struct overload : Ts ...
    {
        using Ts::operator()...;
    };

    template <class... Ts> overload(Ts...) -> overload<Ts...>;

    bool startsWith(std::string_view str, std::string_view prefix);

    bool startsWithCI(std::string_view str, std::string_view prefix);
}
