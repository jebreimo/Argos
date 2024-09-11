//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "CommandData.hpp"

#include <algorithm>
#include "Argos/Option.hpp"
#include "ArgosThrow.hpp"
#include "ParserData.hpp"
#include "StringUtilities.hpp"

namespace argos
{
    CommandData::CommandData() = default;

    CommandData::CommandData(const CommandData& rhs)
        : name(rhs.name),
          texts(rhs.texts),
          current_section(rhs.current_section),
          require_command(rhs.require_command),
          section(rhs.section)
    {
        arguments.reserve(rhs.arguments.size());
        for (const auto& a: rhs.arguments)
            arguments.push_back(std::make_unique<ArgumentData>(*a));
        options.reserve(rhs.options.size());
        for (const auto& o: rhs.options)
            options.push_back(std::make_unique<OptionData>(*o));
    }

    CommandData::CommandData(CommandData&& rhs) noexcept
        : arguments(std::move(rhs.arguments)),
          options(std::move(rhs.options)),
          commands(std::move(rhs.commands)),
          name(std::move(rhs.name)),
          texts(std::move(rhs.texts)),
          current_section(std::move(rhs.current_section)),
          require_command(rhs.require_command),
          section(std::move(rhs.section))
    {
    }

    CommandData::~CommandData() = default;

    CommandData& CommandData::operator=(const CommandData& rhs)
    {
        if (&rhs == this)
            return *this;
        name = rhs.name;
        texts = rhs.texts;
        current_section = rhs.current_section;
        require_command = rhs.require_command;
        section = rhs.section;

        arguments.clear();
        arguments.reserve(rhs.arguments.size());
        for (const auto& a: rhs.arguments)
            arguments.push_back(std::make_unique<ArgumentData>(*a));

        options.clear();
        options.reserve(rhs.options.size());
        for (const auto& o: rhs.options)
            options.push_back(std::make_unique<OptionData>(*o));

        commands.clear();
        commands.reserve(rhs.commands.size());
        for (const auto& c: rhs.commands)
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
        require_command = rhs.require_command;
        section = std::move(rhs.section);
        return *this;
    }

    namespace
    {
        void update_require_command(CommandData& cmd)
        {
            if (cmd.require_command.value_or(false) && cmd.commands.empty())
                ARGOS_THROW("require_command is true, but no commands have been added.");

            if (!cmd.require_command)
            {
                cmd.require_command = !cmd.commands.empty()
                                      && cmd.arguments.empty();
            }
        }

        bool has_help_option(const CommandData& cmd)
        {
            for (const auto& o: cmd.options)
            {
                if (o->type == OptionType::HELP)
                    return true;
            }
            return false;
        }

        void add_help_option(CommandData& cmd, const ParserSettings& settings)
        {
            if (!settings.generate_help_option)
                return;
            if (has_help_option(cmd))
                return;
            std::vector<std::string> flags;
            switch (settings.option_style)
            {
            case OptionStyle::STANDARD:
                if (!has_flag(cmd, "-h", settings))
                    flags.emplace_back("-h");
                if (!has_flag(cmd, "--help", settings))
                    flags.emplace_back("--help");
                break;
            case OptionStyle::SLASH:
                if (!has_flag(cmd, "/?", settings))
                    flags.emplace_back("/?");
                break;
            case OptionStyle::DASH:
                if (!has_flag(cmd, "-h", settings))
                    flags.emplace_back("-h");
                else if (!has_flag(cmd, "-help", settings))
                    flags.emplace_back("-help");
                break;
            }

            if (flags.empty())
                return;

            auto opt = Option().flags(std::move(flags)).type(OptionType::HELP)
                .help("Display the help text.")
                .constant("1").release();
            opt->argument_id = ArgumentId(cmd.options.size()
                                          + cmd.arguments.size() + 1);
            opt->section = cmd.current_section;
            cmd.options.push_back(std::move(opt));
        }

        void add_missing_options(CommandData& cmd,
                                 const ParserSettings& settings)
        {
            add_help_option(cmd, settings);
        }
    }

    bool has_flag(const CommandData& cmd,
                  std::string_view flag,
                  const ParserSettings& settings)
    {
        for (auto& o: cmd.options)
        {
            for (auto& f: o->flags)
            {
                if (are_equal(f, flag, settings.case_insensitive))
                    return true;
            }
        }
        return false;
    }

    void finish_initialization(CommandData& cmd, const ParserData& data)
    {
        update_require_command(cmd);
        add_help_option(cmd, data.parser_settings);
        for (auto& c: cmd.commands)
            finish_initialization(*c, data);
    }
}
