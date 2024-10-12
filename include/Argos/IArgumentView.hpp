//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string>
#include "Enums.hpp"

/**
 * @file
 * @brief Defines the IArgumentView interface class.
 */

namespace argos
{
    /**
     * @brief Interface class with the functions ArgumentView and OptionView
     *      have in common.
     */
    class IArgumentView
    {
    public:
        virtual ~IArgumentView() = default;

        /**
         * @brief Returns the argument's or option's help text.
         */
        [[nodiscard]] virtual std::string help() const = 0;

        /**
         * @brief Returns the argument's or option's section name.
         */
        [[nodiscard]] virtual const std::string& section() const = 0;

        /**
         * @brief Returns the argument's or option's value name.
         */
        [[nodiscard]] virtual const std::string& alias() const = 0;

        /**
         * @brief Returns the argument's or option's visibility in
         *      the help text and error messages.
         */
        [[nodiscard]] virtual Visibility visibility() const = 0;

        /**
         * @brief Returns the argument's or option's custom id.
         */
        [[nodiscard]] virtual int id() const = 0;

        /**
         * @brief Returns the numeric id of the value the argument or option
         *  assigns or appends to.
         *
         * This value is created internally in Argos and must not be
         * confused with the customizable value returned by id().
         * If different options or arguments are aliases for each other,
         * they will also have the same value id.
         *
         * @return all options with operation OptionOperation::NONE have
         *   a value of 0, all other options and arguments have a value
         *   greater than 0.
         */
        [[nodiscard]] virtual ValueId value_id() const = 0;

        /**
         * @brief Returns the argument's or option's argument_id().
         *
         * This id is assigned and used internally to uniquely identify
         * each argument and option.
         */
        [[nodiscard]] virtual ArgumentId argument_id() const = 0;
    };
}
