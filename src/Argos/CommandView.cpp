//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-21.
//
// This file is distributed under the BSD License.
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

    const std::string& CommandView::value() const
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
}
