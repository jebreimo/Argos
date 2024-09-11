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
        void set_value_ids(const ParserData& data)
        {
            struct InternalIdMaker
            {
                std::map<std::string_view, ValueId> explicit_ids;
                ValueId n = ValueId(0);

                std::optional<ValueId> find_value_id(std::string_view name)
                {
                    const auto it = explicit_ids.find(name);
                    if (it == explicit_ids.end())
                        return {};
                    return it->second;
                }

                ValueId make_value_id(std::string_view name)
                {
                    if (const auto id = find_value_id(name))
                        return *id;
                    n = ValueId(n + 1);
                    explicit_ids.emplace(name, n);
                    return n;
                }

                ValueId make_value_id(const std::vector<std::string>& names)
                {
                    for (const auto& name: names)
                    {
                        if (const auto id = find_value_id(name))
                            return *id;
                    }
                    n = ValueId(n + 1);
                    for (const auto& name: names)
                        explicit_ids.emplace(name, n);
                    return n;
                }
            };

            InternalIdMaker id_maker;
            for (const auto& a: data.command.arguments)
            {
                if (!a->value.empty())
                {
                    a->value_id = id_maker.make_value_id(a->value);
                    id_maker.explicit_ids.emplace(a->name, a->value_id);
                }
                else
                {
                    a->value_id = id_maker.make_value_id(a->name);
                }
            }
            for (const auto& o: data.command.options)
            {
                if (o->operation == OptionOperation::NONE)
                    continue;
                if (!o->alias.empty())
                {
                    o->value_id = id_maker.make_value_id(o->alias);
                    for (auto& f: o->flags)
                        id_maker.explicit_ids.emplace(f, o->value_id);
                }
                else
                {
                    o->value_id = id_maker.make_value_id(o->flags);
                }
            }
        }

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
        set_value_ids(data);
    }
}
