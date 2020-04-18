//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "IArgumentView.hpp"
#include <vector>

/**
 * @file
 * @brief Defines the OptionView class.
 */

namespace Argos
{
    struct OptionData;

    /**
     * @brief Provides read-only access to an option definition.
     */
    class OptionView : public IArgumentView
    {
    public:
        /**
         * @private
         * @brief For internal use only.
         *
         * Client code can only receive objects, not construct them.
         */
        explicit OptionView(const OptionData* data);

        /**
         * @brief Returns the option's or option's help text.
         */
        const std::string& text() const final;

        /**
         * @brief Returns the option's section name.
         */
        const std::string& section() const final;

        /**
         * @brief Returns the option's value name.
         */
        const std::string& value() const final;

        /**
         * @brief Returns the option's visibility in
         *      the help text and error messages.
         */
        Visibility visibility() const final;

        /**
         * @brief Returns the option's custom id.
         */
        int id() const final;

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
        ValueId valueId() const final;

        /**
         * @brief Returns the option's argumentId().
         *
         * This id is assigned and used internally to uniquely identify
         * each argument and option.
         */
        ArgumentId argumentId() const final;

        OptionOperation operation() const;

        const std::vector<std::string>& flags() const;

        const std::string& argument() const;

        const std::string& constant() const;

        OptionType type() const;

        bool optional() const;
    private:
        const OptionData* m_Option;
    };
}
