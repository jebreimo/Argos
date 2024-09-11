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

        inline bool has_help_option(const CommandData& cmd)
        {
            for (const auto& o: cmd.options)
            {
                if (o->type == OptionType::HELP)
                    return true;
            }
            return false;
        }

        inline bool has_flag(const CommandData& data,
                             std::string_view flag,
                             bool case_insensitive)
        {
            for (auto& o: data.options)
            {
                for (auto& f: o->flags)
                {
                    if (are_equal(f, flag, case_insensitive))
                        return true;
                }
            }
            return false;
        }

        void add_help_option(CommandData& cmd, const ParserData& data)
        {
            if (!data.parser_settings.generate_help_option)
                return;
            if (has_help_option(cmd))
                return;
            std::vector<std::string> flags;
            switch (data.parser_settings.option_style)
            {
            case OptionStyle::STANDARD:
                if (!has_flag(cmd, "-h", data.parser_settings.case_insensitive))
                    flags.emplace_back("-h");
                if (!has_flag(cmd, "--help", data.parser_settings.case_insensitive))
                    flags.emplace_back("--help");
                break;
            case OptionStyle::SLASH:
                if (!has_flag(cmd, "/?", data.parser_settings.case_insensitive))
                    flags.emplace_back("/?");
                break;
            case OptionStyle::DASH:
                if (!has_flag(cmd, "-h", data.parser_settings.case_insensitive))
                    flags.emplace_back("-h");
                else if (!has_flag(cmd, "-help", data.parser_settings.case_insensitive))
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
            opt->section = data.command.current_section;
            cmd.options.push_back(std::move(opt));
        }
    }

    void CommandData::complete_definition(const ParserData& data)
    {
        update_require_command(*this);
        add_help_option(*this, data);
        for (auto& c: commands)
            c->complete_definition(data);
    }
}
