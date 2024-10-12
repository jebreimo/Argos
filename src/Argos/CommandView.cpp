//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-21.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/CommandView.hpp"

#include "ArgosThrow.hpp"
#include "CommandData.hpp"

namespace argos
{
    CommandView::CommandView(const CommandData* command)
        : m_command(command)
    {}

    std::string CommandView::help() const
    {
        auto it = m_command->texts.find(TextId::ABOUT);
        return it != m_command->texts.end() ? get_text(it->second) : "";
    }

    const std::string& CommandView::section() const
    {
        return m_command->section;
    }

    const std::string& CommandView::alias() const
    {
        return m_command->name;
    }

    Visibility CommandView::visibility() const
    {
        return m_command->visibility;
    }

    int CommandView::id() const
    {
        return m_command->id;
    }

    ValueId CommandView::value_id() const
    {
        return {};
    }

    ArgumentId CommandView::argument_id() const
    {
        return m_command->argument_id;
    }

    std::string CommandView::name() const
    {
        return m_command->name;
    }

    std::vector<ArgumentView> CommandView::arguments() const
    {
        std::vector<ArgumentView> result;
        for (const auto& arg : m_command->arguments)
            result.emplace_back(arg.get());
        return result;
    }

    std::vector<OptionView> CommandView::options() const
    {
        std::vector<OptionView> result;
        for (const auto& opt : m_command->options)
            result.emplace_back(opt.get());
        return result;
    }

    std::vector<CommandView> CommandView::subcommands() const
    {
        std::vector<CommandView> result;
        for (const auto& cmd : m_command->commands)
            result.emplace_back(cmd.get());
        return result;
    }

    bool CommandView::require_subcommand() const
    {
        // Instances of CommandView are only created after require_subcommand
        // has been automatically set, the or-value should therefore never
        // be returned, and it doesn't matter that it might not be correct.
        return m_command->require_subcommand.value_or(false);
    }
}
