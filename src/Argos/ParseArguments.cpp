//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParseArguments.hpp"

#include <iostream>
#include "Argos/Option.hpp"
#include "ArgumentIteratorImpl.hpp"
#include "StringUtilities.hpp"

namespace argos
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
        for (const auto& a: data.arguments)
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
        for (const auto& o: data.options)
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

    inline bool has_help_option(const ParserData& data)
    {
        return std::any_of(data.options.begin(), data.options.end(),
                           [](const auto& o)
                           {
                               return o->type == OptionType::HELP;
                           });
    }

    inline bool has_flag(const ParserData& data, std::string_view flag)
    {
        bool ci = data.parser_settings.case_insensitive;
        return any_of(data.options.begin(), data.options.end(),
                      [&](const auto& o)
                      {
                          return any_of(o->flags.begin(), o->flags.end(),
                                        [&](const auto& f)
                                        {
                                            return are_equal(f, flag, ci);
                                        });
                      });
    }

    void add_help_option(ParserData& data)
    {
        if (!data.parser_settings.generate_help_option)
            return;
        if (has_help_option(data))
            return;
        std::vector<std::string> flags;
        switch (data.parser_settings.option_style)
        {
        case OptionStyle::STANDARD:
            if (!has_flag(data, "-h"))
                flags.emplace_back("-h");
            if (!has_flag(data, "--help"))
                flags.emplace_back("--help");
            break;
        case OptionStyle::SLASH:
            if (!has_flag(data, "/?"))
                flags.emplace_back("/?");
            break;
        case OptionStyle::DASH:
            if (!has_flag(data, "-h"))
                flags.emplace_back("-h");
            else if (!has_flag(data, "-help"))
                flags.emplace_back("-help");
            break;
        }

        if (flags.empty())
            return;

        auto opt = Option().flags(std::move(flags)).type(OptionType::HELP)
            .help("Display the help text.")
            .constant("1").release();
        opt->argument_id = ArgumentId(data.options.size()
                                      + data.arguments.size() + 1);
        opt->section = data.current_section;
        data.options.push_back(std::move(opt));
    }

    void add_version_option(ParserData& data)
    {
        if (data.help_settings.version.empty())
            return;
        std::string flag;
        switch (data.parser_settings.option_style)
        {
        case OptionStyle::STANDARD:
            if (!has_flag(data, "--version"))
                flag = "--version";
            break;
        case OptionStyle::SLASH:
            if (!has_flag(data, "/VERSION"))
                flag = "/VERSION";
            break;
        case OptionStyle::DASH:
            if (!has_flag(data, "-version"))
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
            .callback([v = data.help_settings.version, stream]
            (auto, auto, auto pa)
                {
                    *stream << pa.program_name() << " " << v << "\n";
                    return true;
                })
            .release();
        opt->argument_id = ArgumentId(data.options.size()
                                      + data.arguments.size() + 1);
        opt->section = data.current_section;
        data.options.push_back(std::move(opt));
    }

    void add_missing_options(ParserData& data)
    {
        add_help_option(data);
        add_version_option(data);
    }

    ParsedArguments parse_arguments(std::vector<std::string_view> args,
                                    const std::shared_ptr<ParserData>& data)
    {
        add_missing_options(*data);
        set_value_ids(*data);
        return ParsedArguments(
            ArgumentIteratorImpl::parse(std::move(args), data));
    }

    ArgumentIterator
    make_argument_iterator(std::vector<std::string_view> args,
                       const std::shared_ptr<ParserData>& data)
    {
        add_missing_options(*data);
        set_value_ids(*data);
        return {std::move(args), data};
    }
}
