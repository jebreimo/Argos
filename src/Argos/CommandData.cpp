//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "CommandData.hpp"

namespace argos
{
    CommandData::CommandData() = default;

    CommandData::CommandData(const CommandData& rhs)
        : name(rhs.name),
          texts(rhs.texts),
          current_section(rhs.current_section)
    {
        arguments.reserve(rhs.arguments.size());
        for (const auto& a : rhs.arguments)
            arguments.push_back(std::make_unique<ArgumentData>(*a));
        options.reserve(rhs.options.size());
        for (const auto& o : rhs.options)
            options.push_back(std::make_unique<OptionData>(*o));
    }

    CommandData::CommandData(CommandData&& rhs) noexcept
        : arguments(std::move(rhs.arguments)),
          options(std::move(rhs.options)),
          commands(std::move(rhs.commands)),
          name(std::move(rhs.name)),
          texts(std::move(rhs.texts)),
          current_section(std::move(rhs.current_section))
    {}

    CommandData::~CommandData() = default;

    CommandData& CommandData::operator=(const CommandData& rhs)
    {
        if (&rhs == this)
            return *this;
        name = rhs.name;
        texts = rhs.texts;
        current_section = rhs.current_section;

        arguments.clear();
        arguments.reserve(rhs.arguments.size());
        for (const auto& a : rhs.arguments)
            arguments.push_back(std::make_unique<ArgumentData>(*a));

        options.clear();
        options.reserve(rhs.options.size());
        for (const auto& o : rhs.options)
            options.push_back(std::make_unique<OptionData>(*o));

        commands.clear();
        commands.reserve(rhs.commands.size());
        for (const auto& c : rhs.commands)
            commands.push_back(std::make_unique<CommandData>(*c));

        return *this;
    }

    CommandData& CommandData::operator=(CommandData&& rhs) noexcept
    {
        if (&rhs == this)
            return *this;

        name = std::move(rhs.name);
        texts = std::move(rhs.texts);
        current_section = std::move(rhs.current_section);
        arguments = std::move(rhs.arguments);
        options = std::move(rhs.options);
        commands = std::move(rhs.commands);
        return *this;
    }
}
