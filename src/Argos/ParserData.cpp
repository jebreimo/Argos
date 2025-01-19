//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-11.
//
// This file is distributed under the Zero-Clause BSD License.
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
                (auto& a)
                    {
                        *stream << a.builder.program_name() << " " << v << "\n";
                        return true;
                    })
                .release();
            opt->section = cmd.current_section;
            cmd.options.push_back(std::move(opt));
        }
    }

    void finish_initialization(ParserData& data)
    {
        add_version_option(data);
        finish_initialization(data.command, data);
    }
}
