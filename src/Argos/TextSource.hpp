//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-10-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <variant>
#include "Argos/Callbacks.hpp"

namespace argos
{
    using TextSource = std::variant <std::string, TextCallback>;

    inline std::string get_text(const TextSource& source)
    {
        struct Visitor
        {
            std::string operator()(const std::string& s) const
            {
                return s;
            }

            std::string operator()(const std::function<std::string()>& f) const
            {
                return f();
            }
        };
        return std::visit(Visitor(), source);
    }
}
