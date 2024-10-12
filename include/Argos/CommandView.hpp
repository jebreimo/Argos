//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-21.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "ArgumentView.hpp"
#include "OptionView.hpp"

namespace argos
{
    struct CommandData;

    /**
     * @brief Provides read-only access to a command definition.
     */
    class CommandView : public IArgumentView
    {
    public:
        /**
         * @private
         * @brief For internal use only.
         *
         * Client code can only receive objects, not construct them.
         */
        explicit CommandView(const CommandData* command);

        /**
         * @brief Returns the command's help text.
         *
         * This is the text that is displayed in the parent command's list
         * of sub-commands. It will also be used as the about text if the
         * command doesn't have an explicit about text.
         */
        [[nodiscard]] std::string help() const override;

        /**
         * @brief Returns the command's section name.
         */
        [[nodiscard]] const std::string& section() const override;

        /**
         * @brief Returns the command's value name.
         */
        [[nodiscard]] const std::string& alias() const override;

        [[nodiscard]] Visibility visibility() const override;

        [[nodiscard]] int id() const override;

        [[nodiscard]] ValueId value_id() const override;

        [[nodiscard]] ArgumentId argument_id() const override;

        /**
         * @brief Returns the command's name.
         */
        [[nodiscard]] std::string name() const;

        /**
         * @brief Returns the command's arguments.
         */
        [[nodiscard]] std::vector<ArgumentView> arguments() const;

        /**
         * @brief Returns the command's options.
         */
        [[nodiscard]] std::vector<OptionView> options() const;

        /**
         * @brief Returns the command's sub-commands.
         */
        [[nodiscard]] std::vector<CommandView> subcommands() const;
    private:
        const CommandData* m_command;
    };
}
