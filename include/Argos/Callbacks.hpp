//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <functional>
#include "ArgumentView.hpp"
#include "OptionView.hpp"
#include "ParsedArgumentsBuilder.hpp"

namespace Argos
{
    using ArgumentCallback = std::function<bool(ArgumentView,
                                                std::string_view,
                                                ParsedArgumentsBuilder)>;

    using OptionCallback = std::function<bool(OptionView,
                                              std::string_view,
                                              ParsedArgumentsBuilder)>;
}
