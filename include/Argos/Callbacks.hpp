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

/**
 * @file
 * @brief Defines ArgumentCallback and OptionCallback.
 */

namespace Argos
{
    /**
     * @brief A callback that is called each time given arguments appear
     *      on the command line.
     *
     * The three parameters are:
     * - ArgumentView: the argument that was encountered (particularly
     *   useful if the same function has been registered with multiple
     *   arguments).
     * - std::string_view: the raw value of the argument. Note that this
     *   value can also be retrieved via the ParsedArgumentsBuilder.
     * - ParsedArgumentsBuilder: this object can be used to read or modify
     *   the values of arguments and options.
     */
    using ArgumentCallback = std::function<bool(ArgumentView,
                                                std::string_view,
                                                ParsedArgumentsBuilder)>;

    /**
     * @brief A callback that is called each time given options appear
     *      on the command line.
     *
     * The three parameters are:
     * - OptionView: the option that was encountered (particularly
     *   useful if the same function has been registered with multiple
     *   options).
     * - std::string_view: the raw value of the option if the option actually
     *   has one. Note that this value can also be retrieved via the
     *   ParsedArgumentsBuilder.
     * - ParsedArgumentsBuilder: this object can be used to read or modify
     *   the values of arguments and options.
     */
    using OptionCallback = std::function<bool(OptionView,
                                              std::string_view,
                                              ParsedArgumentsBuilder)>;
}
