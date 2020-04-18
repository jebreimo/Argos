//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string>
#include "Enums.hpp"

/**
 * @file
 * @brief Defines the IArgumentView interface class.
 */

namespace Argos
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
        virtual const std::string& text() const = 0;

        /**
         * @brief Returns the argument's or option's section name.
         */
        virtual const std::string& section() const = 0;

        /**
         * @brief Returns the argument's or option's value name.
         */
        virtual const std::string& value() const = 0;

        /**
         * @brief Returns the argument's or option's visibility in
         *      the help text and error messages.
         */
        virtual Visibility visibility() const = 0;

        /**
         * @brief Returns the argument's or option's custom id.
         */
        virtual int id() const = 0;

        /**
         * @brief Returns the numeric id of the value the argument or option
         *  assigns or appends to.
         *
         * This value is created internally in Argos and must not be
         * confused with the customizable value returned by id().
         * If different options or arguments have the same value name, they
         * will also have the same value id.
         *
         * @return options with operation OptionOperation::NONE have
         *   a value of 0, all other options and arguments have a value
         *   greater than 0.
         */
        virtual ValueId valueId() const = 0;

        /**
         * @brief Returns the argument's or option's argumentId().
         *
         * This id is assigned and used internally to uniquely identify
         * each argument and option.
         */
        virtual ArgumentId argumentId() const = 0;
    };
}
