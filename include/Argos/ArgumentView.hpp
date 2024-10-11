//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include "IArgumentView.hpp"

/**
 * @file
 * @brief Defines the ArgumentView class.
 */

namespace argos
{
    struct ArgumentData;

    /**
     * @brief Provides read-only access to an argument definition.
     */
    class ArgumentView final : public IArgumentView
    {
    public:
        /**
         * @private
         * @brief For internal use only.
         *
         * Client code can only receive objects, not construct them.
         */
        explicit ArgumentView(const ArgumentData* data);

        /**
         * @brief Returns the argument's or option's help text.
         */
        [[nodiscard]] std::string help() const final;

        /**
         * @brief Returns the argument's section name.
         */
        [[nodiscard]] const std::string& section() const final;

        /**
         * @brief Returns the argument's value name.
         */
        [[nodiscard]] const std::string& value() const final;

        /**
         * @brief Returns the argument's visibility in
         *      the help text and error messages.
         */
        [[nodiscard]] Visibility visibility() const final;

        /**
         * @brief Returns the argument's custom id.
         */
        [[nodiscard]] int id() const final;

        /**
         * @brief Returns the numeric id of the value the argument assigns
         *      or appends to.
         *
         * This value is created internally in Argos and must not be
         * confused with the customizable value returned by id().
         * If different options or arguments have the same value name, they
         * will also have the same value id.
         *
         * @return options with operation OptionOperation::NONE have
         *      a value of 0, all other options and arguments have a value
         *      greater than 0.
         */
        [[nodiscard]] ValueId value_id() const final;

        /**
         * @brief Returns the argument's argument id.
         *
         * This id is assigned and used internally to uniquely identify
         * each argument and option.
         */
        [[nodiscard]] ArgumentId argument_id() const final;

        /**
         * @brief Returns the argument's name.
         */
        [[nodiscard]] const std::string& name() const;

        /**
         * @brief Returns true if the argument is optional (i.e. its minimum
         *  count is 0).
         */
        [[nodiscard]] bool optional() const;

        /**
         * @brief Returns the argument's minimum and maximum counts.
         *
         * Normal arguments have both set to 1.
         */
        [[nodiscard]] std::pair<unsigned, unsigned> count() const;
    private:
        const ArgumentData* m_argument;
    };
}
