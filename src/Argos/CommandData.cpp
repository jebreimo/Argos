//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-05.
//
// This file is distributed under the Zero-Clause BSD License.
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
    constexpr char DEFAULT_HELP_TEXT[] = "Display this help text.";

    CommandData::CommandData() = default;

    CommandData::CommandData(const CommandData& rhs)
        : current_section(rhs.current_section),
          name(rhs.name),
          full_name(rhs.full_name),
          texts(rhs.texts),
          require_command(rhs.require_command),
          multi_command(rhs.multi_command),
          section(rhs.section),
          id(rhs.id),
          argument_id(rhs.argument_id)
    {
        arguments.reserve(rhs.arguments.size());
        for (const auto& a : rhs.arguments)
            arguments.push_back(std::make_unique<ArgumentData>(*a));
        options.reserve(rhs.options.size());
        for (const auto& o : rhs.options)
            options.push_back(std::make_unique<OptionData>(*o));
        commands.reserve(rhs.commands.size());
        for (const auto& c : rhs.commands)
            commands.push_back(std::make_unique<CommandData>(*c));
    }

    CommandData::CommandData(CommandData&& rhs) noexcept
        : arguments(std::move(rhs.arguments)),
          options(std::move(rhs.options)),
          commands(std::move(rhs.commands)),
          current_section(std::move(rhs.current_section)),
          name(std::move(rhs.name)),
          full_name(std::move(rhs.full_name)),
          texts(std::move(rhs.texts)),
          require_command(rhs.require_command),
          multi_command(rhs.multi_command),
          section(std::move(rhs.section)),
          id(rhs.id),
          argument_id(rhs.argument_id)
    {
    }

    CommandData::~CommandData() = default;

    CommandData& CommandData::operator=(const CommandData& rhs)
    {
        if (&rhs == this)
            return *this;
        current_section = rhs.current_section;
        name = rhs.name;
        full_name = rhs.full_name;
        texts = rhs.texts;
        require_command = rhs.require_command;
        multi_command = rhs.multi_command;
        section = rhs.section;
        id = rhs.id;
        argument_id = rhs.argument_id;

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

        current_section = std::move(rhs.current_section);
        name = std::move(rhs.name);
        full_name = std::move(rhs.full_name);
        texts = std::move(rhs.texts);
        arguments = std::move(rhs.arguments);
        options = std::move(rhs.options);
        commands = std::move(rhs.commands);
        require_command = rhs.require_command;
        multi_command = rhs.multi_command;
        section = std::move(rhs.section);
        id = rhs.id;
        argument_id = rhs.argument_id;
        return *this;
    }

    void CommandData::add(std::unique_ptr<ArgumentData> arg)
    {
        if (!arg)
            ARGOS_THROW("Argument is empty (it has probably already been added).");
        if (arg->name.empty())
            ARGOS_THROW("Argument must have a name.");
        if (arg->section.empty())
            arg->section = current_section;
        arguments.emplace_back(std::move(arg));
    }

    void CommandData::add(std::unique_ptr<OptionData> opt)
    {
        if (!opt)
            ARGOS_THROW("Option is empty (it has probably already been added).");
        if (opt->section.empty())
            opt->section = current_section;
        options.push_back(std::move(opt));
    }

    void CommandData::add(std::unique_ptr<CommandData> cmd)
    {
        if (!cmd)
            ARGOS_THROW("Command is empty (it has probably already been added).");
        if (cmd->name.empty())
            ARGOS_THROW("Command must have a name.");
        if (cmd->section.empty())
            cmd->section = current_section;
        commands.push_back(std::move(cmd));
    }

    void CommandData::copy_from(const CommandData& cmd)
    {
        for (const auto& a : cmd.arguments)
            arguments.push_back(std::make_unique<ArgumentData>(*a));
        for (const auto& o : cmd.options)
            options.push_back(std::make_unique<OptionData>(*o));
        for (const auto& c : cmd.commands)
            commands.push_back(std::make_unique<CommandData>(*c));
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

    const CommandData* CommandData::find_command(std::string_view name,
                                                 bool case_insensitive) const
    {
        for (const auto& c : commands)
        {
            if (are_equal(c->name, name, case_insensitive))
                return c.get();
        }
        return nullptr;
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
                .help(DEFAULT_HELP_TEXT)
                .constant("1").release();
            opt->section = cmd.current_section;
            cmd.options.push_back(std::move(opt));
        }

        std::pair<ValueId, ArgumentId>
        set_internal_ids(const CommandData& cmd,
                         ValueId value_id_offset,
                         ArgumentId argument_id_offset)
        {
            struct InternalIdMaker
            {
                std::map<std::string_view, ValueId> explicit_ids;
                int id;

                explicit InternalIdMaker(ValueId start_id)
                    : id(start_id)
                {
                }

                std::optional<ValueId> find_value_id(std::string_view name)
                {
                    const auto it = explicit_ids.find(name);
                    if (it == explicit_ids.end())
                        return {};
                    return it->second;
                }

                ValueId get_value_id(std::string_view name)
                {
                    if (const auto id = find_value_id(name))
                        return *id;
                    id++;
                    explicit_ids.emplace(name, ValueId(id));
                    return ValueId(id);
                }

                ValueId get_value_id(const std::vector<std::string>& names)
                {
                    for (const auto& name : names)
                    {
                        if (const auto id = find_value_id(name))
                            return *id;
                    }
                    id++;
                    for (const auto& name : names)
                        explicit_ids.emplace(name, ValueId(id));
                    return ValueId(id);
                }
            };

            int argument_id = argument_id_offset;
            InternalIdMaker id_maker(value_id_offset);
            for (const auto& a : cmd.arguments)
            {
                a->argument_id = ArgumentId(++argument_id);
                if (!a->alias.empty())
                {
                    a->value_id = id_maker.get_value_id(a->alias);
                    id_maker.explicit_ids.emplace(a->name, a->value_id);
                }
                else
                {
                    a->value_id = id_maker.get_value_id(a->name);
                }
            }
            for (const auto& o : cmd.options)
            {
                o->argument_id = ArgumentId(++argument_id);
                if (o->operation == OptionOperation::NONE)
                    continue;
                if (!o->alias.empty())
                {
                    o->value_id = id_maker.get_value_id(o->alias);
                    for (auto& f : o->flags)
                        id_maker.explicit_ids.emplace(f, o->value_id);
                }
                else
                {
                    o->value_id = id_maker.get_value_id(o->flags);
                }
            }

            return {ValueId(id_maker.id), ArgumentId(argument_id)};
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
                               ValueId start_id,
                               ArgumentId argument_id)
    {
        if (cmd.full_name.empty())
            cmd.full_name = cmd.name;
        for (auto& o : cmd.options)
            validate_and_update(*o, data.parser_settings.option_style);
        update_require_command(cmd);
        add_help_option(cmd, data.parser_settings);

        argument_id = ArgumentId(argument_id + 1);
        cmd.argument_id = argument_id;
        std::tie(start_id, argument_id) = set_internal_ids(cmd, start_id, argument_id);

        cmd.build_option_index(data.parser_settings.case_insensitive);
        for (auto& c : cmd.commands)
        {
            c->full_name = cmd.name + ' ' + c->name;
            finish_initialization(*c, data, start_id, argument_id);
        }
    }
}
