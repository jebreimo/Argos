//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-14.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

// Copied from C++17 in Detail by Bartłomiej Filipek
namespace Argos
{
    template <class... Ts>
    struct overload : Ts ...
    {
        using Ts::operator()...;
    };

    template <class... Ts> overload(Ts...) -> overload<Ts...>;
}
