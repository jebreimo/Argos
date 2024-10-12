//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "IArgumentView.hpp"
#include <vector>

/**
 * @file
 * @brief Defines the OptionView class.
 */

namespace argos
{
    struct OptionData;

    /**
     * @brief Provides read-only access to an option definition.
     */
    class OptionView final : public IArgumentView
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
        [[nodiscard]] std::string help() const final;

        /**
         * @brief Returns the option's section name.
         */
        [[nodiscard]] const std::string& section() const final;

        /**
         * @brief Returns the option's alias.
         */
        [[nodiscard]] const std::string& alias() const final;

        /**
         * @brief Returns the option's visibility in
         *      the help text and error messages.
         */
        [[nodiscard]] Visibility visibility() const final;

        /**
         * @brief Returns the option's custom id.
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
         * @brief Returns the option's argument id.
         *
         * This id is assigned and used internally to uniquely identify
         * each argument and option.
         */
        [[nodiscard]] ArgumentId argument_id() const final;

        /**
         * @brief Returns the option's operation.
         */
        [[nodiscard]] OptionOperation operation() const;

        /**
         * @brief Returns the option's flags.
         */
        [[nodiscard]] const std::vector<std::string>& flags() const;

        /**
         * @brief Returns the option's argument.
         */
        [[nodiscard]] const std::string& argument() const;

        /**
         * @brief Returns the option's initial value.
         */
        [[nodiscard]] const std::string& initial_value() const;

        /**
         * @brief Returns the option's constant.
         *
         * @note The constant is stored as a string internally, even if the
         *  option was assigned an integer or boolean value.
         */
        [[nodiscard]] const std::string& constant() const;

        /**
         * @brief Returns the option's type.
         */
        [[nodiscard]] OptionType type() const;

        /**
         * @brief Returns false if the option is mandatory.
         */
        [[nodiscard]] bool optional() const;
    private:
        const OptionData* m_option;
    };
}
