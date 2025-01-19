//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-22.
//
// This file is distributed under the Zero-Clause BSD License.
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

namespace argos
{
    /**
     * @brief The parameter type for argument and option callbacks.
     * @tparam ViewT Either ArgumentView or OptionView.
     */
    template <typename ViewT>
    struct CallbackArguments
    {
        /**
         * The argument or option that was encountered.
         * Is an instance of either ArgumentView or OptionView.
         */
        ViewT view;

        /**
         * The value of the argument or the option's argument, if any.
         */
        std::string_view value;

        /**
         * Gives access to the arguments and options processed so far. Can be
         * used to get or set the values of arguments and options.
         */
        ParsedArgumentsBuilder builder;

        /**
         * Add new arguments to the command line that is being parsed. These
         * arguments are inserted immediately after the current argument
         * and can be anything, including options and
         * commands.
         */
        std::vector<std::string> new_arguments;

        CallbackArguments(const ViewT& view,
                          std::string_view value,
                          std::shared_ptr<ParsedArgumentsImpl> impl)
            : view(view),
              value(value),
              builder(std::move(impl))
        {
        }
    };

    /**
     * @brief The parameter type for argument callbacks.
     */
    using ArgumentCallbackArguments = CallbackArguments<ArgumentView>;

    /**
     * @brief A callback that is called each time given arguments appear
     *      on the command line.
     */
    using ArgumentCallback = std::function<void(ArgumentCallbackArguments&)>;

    /**
     * @brief The parameter type for option callbacks.
     */
    using OptionCallbackArguments = CallbackArguments<OptionView>;

    /**
     * @brief A callback that is called each time given options appear
     *      on the command line.
     */
    using OptionCallback = std::function<void(OptionCallbackArguments&)>;

    /**
     * @brief A callback that returns a part of the help text.
     */
    using TextCallback = std::function<std::string()>;
}
