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
        require_command = rhs.require_command;
        section = std::move(rhs.section);
        return *this;
    }

    void CommandData::build_option_index(bool case_insensitive)
    {
        std::vector<std::pair<std::string_view, const OptionData*>> index;
        for (auto& option : options)
        {
            for (auto& flag : option->flags)
                index.emplace_back(flag, option.get());
        }

        sort(index.begin(), index.end(), [&](const auto& a, const auto& b)
        {
            return is_less(a.first, b.first, case_insensitive);
        });

        const auto it = adjacent_find(
            index.begin(), index.end(),
            [&](const auto& a, const auto& b)
            {
                return are_equal(a.first, b.first, case_insensitive);
            });

        if (it == index.end())
        {
            option_index = std::move(index);
            return;
        }

        if (it->first == next(it)->first)
        {
            ARGOS_THROW("Multiple definitions of flag "
                + std::string(it->first));
        }

        ARGOS_THROW("Conflicting flags: " + std::string(it->first)
            + " and " + std::string(next(it)->first));
    }

    const OptionData* CommandData::find_option(std::string_view flag,
                                               bool allow_abbreviations,
                                               bool case_insensitive) const
    {
        auto opt = find_option_impl(flag, allow_abbreviations,
                                    case_insensitive);
        if (opt == nullptr && flag.size() > 2 && flag.back() == '=')
        {
            flag = flag.substr(0, flag.size() - 1);
            opt = find_option_impl(flag, allow_abbreviations,
                                   case_insensitive);
            if (opt && opt->argument.empty())
                opt = nullptr;
        }
        return opt;
    }

    const OptionData* CommandData::find_option_impl(std::string_view flag,
                                                    bool allow_abbreviations,
                                                    bool case_insensitive) const
    {
        const auto it = std::lower_bound(
            option_index.begin(), option_index.end(),
            std::pair(flag, nullptr),
            [&](auto& a, auto& b)
            {
                return is_less(a.first, b.first, case_insensitive);
            });
        if (it == option_index.end())
            return nullptr;
        if (it->first == flag)
            return it->second;
        if (case_insensitive && are_equal_ci(it->first, flag))
            return it->second;
        if (!allow_abbreviations)
            return nullptr;
        if (!starts_with(it->first, flag, case_insensitive))
            return nullptr;
        const auto nxt = next(it);
        if (nxt != option_index.end()
            && starts_with(nxt->first, flag, case_insensitive))
            return nullptr;
        return it->second;
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
            for (const auto& o : cmd.options)
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

        ValueId set_value_ids(const CommandData& cmd,
                              ValueId start_id = ValueId(0))
        {
            struct InternalIdMaker
            {
                std::map<std::string_view, ValueId> explicit_ids;
                ValueId n;

                explicit InternalIdMaker(ValueId start_id)
                    : n(start_id)
                {
                }

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
                    for (const auto& name : names)
                    {
                        if (const auto id = find_value_id(name))
                            return *id;
                    }
                    n = ValueId(n + 1);
                    for (const auto& name : names)
                        explicit_ids.emplace(name, n);
                    return n;
                }
            };

            InternalIdMaker id_maker(start_id);
            for (const auto& a : cmd.arguments)
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
            for (const auto& o : cmd.options)
            {
                if (o->operation == OptionOperation::NONE)
                    continue;
                if (!o->alias.empty())
                {
                    o->value_id = id_maker.make_value_id(o->alias);
                    for (auto& f : o->flags)
                        id_maker.explicit_ids.emplace(f, o->value_id);
                }
                else
                {
                    o->value_id = id_maker.make_value_id(o->flags);
                }
            }

            return id_maker.n;
        }
    }

    bool has_flag(const CommandData& cmd,
                  std::string_view flag,
                  const ParserSettings& settings)
    {
        for (auto& o : cmd.options)
        {
            for (auto& f : o->flags)
            {
                if (are_equal(f, flag, settings.case_insensitive))
                    return true;
            }
        }
        return false;
    }

    void finish_initialization(CommandData& cmd,
                               const ParserData& data,
                               ValueId start_id)
    {
        update_require_command(cmd);
        add_help_option(cmd, data.parser_settings);
        start_id = set_value_ids(cmd, start_id);
        cmd.build_option_index(data.parser_settings.case_insensitive);
        for (auto& c : cmd.commands)
            finish_initialization(*c, data, start_id);
    }
}
