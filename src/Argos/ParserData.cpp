//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-11.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParserData.hpp"
#include "Argos/Option.hpp"

#include <iostream>

namespace argos
{
    namespace
    {
        void add_version_option(ParserData& data)
        {
            if (data.version.empty())
                return;
            auto& cmd = data.command;
            std::string flag;
            switch (data.parser_settings.option_style)
            {
            case OptionStyle::STANDARD:
                if (!has_flag(cmd, "--version", data.parser_settings))
                    flag = "--version";
                break;
            case OptionStyle::SLASH:
                if (!has_flag(cmd, "/VERSION", data.parser_settings))
                    flag = "/VERSION";
                break;
            case OptionStyle::DASH:
                if (!has_flag(cmd, "-version", data.parser_settings))
                    flag = "-version";
                break;
            }

            if (flag.empty())
                return;

            auto stream = data.help_settings.output_stream
                              ? data.help_settings.output_stream
                              : &std::cout;
            auto opt = Option().flag(flag).type(OptionType::STOP)
                .help("Display the program version.")
                .constant("1")
                .callback([v = data.version, stream]
                (auto, auto, auto pa)
                    {
                        *stream << pa.program_name() << " " << v << "\n";
                        return true;
                    })
                .release();
            opt->argument_id = ArgumentId(cmd.options.size()
                                          + cmd.arguments.size() + 1);
            opt->section = cmd.current_section;
            cmd.options.push_back(std::move(opt));
        }
    }

    void finish_initialization(ParserData& data)
    {
        add_version_option(data);
        finish_initialization(data.command, data);
    }

    bool find_command_path(const std::vector<std::unique_ptr<CommandData>>& commands,
                           const CommandData* command,
                           std::vector<const CommandData*> path)
    {
        for (auto& cmd : commands)
        {
            path.push_back(cmd.get());
            if (cmd.get() == command
                || find_command_path(cmd->commands, command, path))
            {
                return true;
            }
            path.pop_back();
        }
        return false;
    }

    std::vector<const CommandData*>
    get_command_path(const ParserData* parser_data, const CommandData* cmd)
    {
        // Use depth-first search to find the path to the command. While
        // inefficient, it is unlikely that the command hierarchy will be
        // deep enough for this to be a problem.
        std::vector<const CommandData*> path;
        path.push_back(&parser_data->command);
        if (&parser_data->command == cmd
            || find_command_path(parser_data->command.commands, cmd, path))
        {
            return path;
        }
        return {};
    }
}
