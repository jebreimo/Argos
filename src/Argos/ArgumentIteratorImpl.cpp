//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ArgumentIteratorImpl.hpp"

#include <algorithm>
#include "ArgosThrow.hpp"
#include "HelpText.hpp"
#include "StringUtilities.hpp"
#include "OptionIterator.hpp"

namespace argos
{
    namespace
    {
        bool is_option(const std::string& s, OptionStyle style)
        {
            if (s.size() < 2)
                return false;

            return s[0] == (style == OptionStyle::SLASH ? '/' : '-');
        }

        std::variant<OptionIterator, StandardOptionIterator>
        make_option_iterator(OptionStyle style,
                             std::vector<std::string_view> args)
        {
            switch (style)
            {
            case OptionStyle::SLASH:
                return OptionIterator(std::move(args), '/');
            case OptionStyle::DASH:
                return OptionIterator(std::move(args), '-');
            default:
                return StandardOptionIterator(std::move(args));
            }
        }
    }

    ArgumentIteratorImpl::ArgumentIteratorImpl(std::vector<std::string_view> args,
                                               std::shared_ptr<ParserData> data)
        : m_data(std::move(data)),
          m_command(&m_data->command),
          m_parsed_args{std::make_shared<ParsedArgumentsImpl>(m_command, m_data)},
          m_iterator{
              make_option_iterator(m_data->parser_settings.option_style,
                                   std::move(args))
          }
    {
        auto& parsed_args = *parsed_arguments();
        for (const auto& option : m_command->options)
        {
            if (!option->initial_value.empty())
            {
                parsed_args.append_value(option->value_id,
                                         option->initial_value,
                                         option->argument_id);
            }
        }

        if (!ArgumentCounter::requires_argument_count(*m_command))
        {
            m_argument_counter = ArgumentCounter(*m_command);
        }
        else
        {
            m_argument_counter = ArgumentCounter(*m_command,
                                                 count_arguments());
        }
    }

    std::shared_ptr<ParsedArgumentsImpl>
    ArgumentIteratorImpl::parse(std::vector<std::string_view> args,
                                const std::shared_ptr<ParserData>& data)
    {
        ArgumentIteratorImpl iterator(std::move(args), data);
        while (true)
        {
            const auto code = std::get<0>(iterator.next());
            if (code == IteratorResultCode::ERROR
                || code == IteratorResultCode::DONE)
            {
                break;
            }
        }
        return iterator.toplevel_parsed_arguments();
    }

    IteratorResult ArgumentIteratorImpl::next()
    {
        if (m_state == State::ERROR)
            ARGOS_THROW("next() called after error.");
        if (m_state == State::DONE)
            return {IteratorResultCode::DONE, {}, {}};

        const auto arg = m_state == State::ARGUMENTS_AND_OPTIONS
                             ? m_iterator.next()
                             : m_iterator.next_value();
        if (!arg)
        {
            if (check_argument_and_option_counts())
                return {IteratorResultCode::DONE, {}, {}};
            else
                return {IteratorResultCode::ERROR, {}, {}};
        }

        if (m_state == State::ARGUMENTS_AND_OPTIONS
            && is_option(*arg, m_data->parser_settings.option_style))
        {
            return process_option(*arg);
        }
        else if (m_command->commands.empty())
        {
            return process_argument(*arg);
        }
        else if (auto cmd = m_command->find_command(
            *arg, m_data->parser_settings.case_insensitive))
        {
            return process_command(cmd);
        }
        else
        {
            error("Unknown command: " + std::string(*arg));
            return {IteratorResultCode::ERROR, {}, {}};
        }
    }

    const std::shared_ptr<ParsedArgumentsImpl>&
    ArgumentIteratorImpl::parsed_arguments() const
    {
        return m_parsed_args.back();
    }

    const std::shared_ptr<ParsedArgumentsImpl>&
    ArgumentIteratorImpl::toplevel_parsed_arguments() const
    {
        return m_parsed_args.front();
    }

    std::pair<ArgumentIteratorImpl::OptionResult, std::string_view>
    ArgumentIteratorImpl::process_option(const OptionData& opt,
                                         const std::string& flag)
    {
        const auto& parsed_args = parsed_arguments();
        std::string_view arg;
        switch (opt.operation)
        {
        case OptionOperation::ASSIGN:
            if (!opt.constant.empty())
            {
                parsed_args->assign_value(opt.value_id, opt.constant,
                                          opt.argument_id);
            }
            else if (const auto value = m_iterator.next_value())
            {
                arg = parsed_args->assign_value(opt.value_id, *value,
                                                opt.argument_id);
            }
            else
            {
                error(flag + ": no value given.");
                return {OptionResult::ERROR, {}};
            }
            break;
        case OptionOperation::APPEND:
            if (!opt.constant.empty())
            {
                parsed_args->append_value(opt.value_id, opt.constant,
                                          opt.argument_id);
            }
            else if (const auto value = m_iterator.next_value())
            {
                arg = parsed_args->append_value(opt.value_id, *value,
                                                opt.argument_id);
            }
            else
            {
                error(flag + ": no value given.");
                return {OptionResult::ERROR, {}};
            }
            break;
        case OptionOperation::CLEAR:
            parsed_args->clear_value(opt.value_id);
            break;
        case OptionOperation::NONE:
            break;
        }

        if (opt.callback)
        {
            opt.callback(OptionView(&opt), arg,
                         ParsedArgumentsBuilder(parsed_args));
        }
        if (m_data->parser_settings.option_callback)
        {
            m_data->parser_settings.option_callback(
                OptionView(&opt), arg,
                ParsedArgumentsBuilder(parsed_args));
        }

        switch (opt.type)
        {
        case OptionType::NORMAL:
            return {OptionResult::NORMAL, arg};
        case OptionType::HELP:
            write_help_text(*m_data, *m_command);
            [[fallthrough]];
        case OptionType::EXIT:
            m_state = State::DONE;
            parsed_args->set_breaking_option(&opt);
            return {OptionResult::EXIT, arg};
        case OptionType::STOP:
            m_state = State::DONE;
            parsed_args->set_breaking_option(&opt);
            return {OptionResult::STOP, arg};
        case OptionType::LAST_ARGUMENT:
            m_state = State::DONE;
            return {OptionResult::LAST_ARGUMENT, arg};
        case OptionType::LAST_OPTION:
            m_state = State::ARGUMENTS_ONLY;
            return {OptionResult::NORMAL, arg};
        }
        return {};
    }

    IteratorResult
    ArgumentIteratorImpl::process_option(const std::string& flag)
    {
        auto option = m_command->find_option(
            flag,
            m_data->parser_settings.allow_abbreviated_options,
            m_data->parser_settings.case_insensitive);
        if (option)
        {
            auto [res, arg] = process_option(*option, flag);
            switch (res)
            {
            case OptionResult::EXIT:
                if (m_data->parser_settings.auto_exit)
                    exit(m_data->parser_settings.normal_exit_code);
                copy_remaining_arguments_to_parser_result();
                return {IteratorResultCode::OPTION, option, arg};
            case OptionResult::ERROR:
                return {IteratorResultCode::ERROR, option, {}};
            case OptionResult::LAST_ARGUMENT:
                if (!check_argument_and_option_counts())
                    return {IteratorResultCode::ERROR, {}, {}};

                if (auto index = find_first_multi_command_parent())
                {
                    reactivate_multi_command_parent(*index);
                    return {IteratorResultCode::OPTION, option, arg};
                }

                [[fallthrough]];
            case OptionResult::STOP:
                copy_remaining_arguments_to_parser_result();
                [[fallthrough]];
            default:
                return {IteratorResultCode::OPTION, option, arg};
            }
        }
        if (!m_data->parser_settings.ignore_undefined_options
            || !starts_with(m_iterator.current(), flag))
        {
            error("Unknown option: " + std::string(m_iterator.current()));
            return {IteratorResultCode::ERROR, {}, {}};
        }
        else
        {
            parsed_arguments()->add_unprocessed_argument(
                std::string(m_iterator.current()));
            return {IteratorResultCode::UNKNOWN, {}, m_iterator.current()};
        }
    }

    IteratorResult
    ArgumentIteratorImpl::process_argument(const std::string& value)
    {
        const auto& parsed_args = parsed_arguments();
        if (auto [next_cmd, i] = find_sibling_command(value); next_cmd)
        {
            reactivate_multi_command_parent(i);
            return process_command(next_cmd);
        }
        if (auto argument = m_argument_counter.next_argument())
        {
            auto s = parsed_args->append_value(argument->value_id, value,
                                               argument->argument_id);
            if (argument->callback)
            {
                argument->callback(ArgumentView(argument), s,
                                   ParsedArgumentsBuilder(parsed_args));
            }

            if (m_data->parser_settings.argument_callback)
            {
                m_data->parser_settings.argument_callback(
                    ArgumentView(argument), s,
                    ParsedArgumentsBuilder(parsed_args));
            }
            return {IteratorResultCode::ARGUMENT, argument, s};
        }
        if (m_data->parser_settings.ignore_undefined_arguments)
        {
            parsed_args->add_unprocessed_argument(value);
            return {IteratorResultCode::UNKNOWN, {}, m_iterator.current()};
        }

        error("Too many arguments, starting from \"" + value + "\".");
        return {IteratorResultCode::ERROR, {}, {}};
    }

    IteratorResult
    ArgumentIteratorImpl::process_command(const CommandData* command)
    {
        m_parsed_args.push_back(parsed_arguments()->add_subcommand(command));
        m_command = command;
        if (!ArgumentCounter::requires_argument_count(*m_command))
        {
            m_argument_counter = ArgumentCounter(*m_command);
        }
        else
        {
            m_argument_counter = ArgumentCounter(*m_command,
                                                 count_arguments());
        }
        m_state = State::ARGUMENTS_AND_OPTIONS;
        return {IteratorResultCode::COMMAND, command, m_iterator.current()};
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void ArgumentIteratorImpl::copy_remaining_arguments_to_parser_result()
    {
        auto& parsed_args = *parsed_arguments();
        for (auto str : m_iterator.remaining_arguments())
            parsed_args.add_unprocessed_argument(std::string(str));
    }

    size_t ArgumentIteratorImpl::count_arguments() const
    {
        size_t result = 0;
        auto it = m_iterator;
        auto [min_count, _] = ArgumentCounter::get_min_max_count(*m_command);
        for (auto arg = it.next(); arg; arg = it.next())
        {
            const OptionData* option = m_command->find_option(
                *arg,
                m_data->parser_settings.allow_abbreviated_options,
                m_data->parser_settings.case_insensitive);

            if (option)
            {
                if (!option->argument.empty())
                    it.next_value();

                bool arguments_only = false;

                switch (option->type)
                {
                case OptionType::HELP:
                case OptionType::LAST_ARGUMENT:
                case OptionType::STOP:
                    return result;
                case OptionType::LAST_OPTION:
                    arguments_only = true;
                    break;
                default:
                    break;
                }

                if (arguments_only)
                    break;
            }
            else if (!is_option(*arg, m_data->parser_settings.option_style))
            {
                if (m_command->find_command(*arg, m_data->parser_settings.case_insensitive))
                    return result;

                if (result >= min_count && find_sibling_command(*arg).first)
                    return result;

                ++result;
            }
        }

        for (auto arg = it.next_value(); arg; arg = it.next_value())
        {
            if (m_command->find_command(*arg, m_data->parser_settings.case_insensitive))
                return result;

            if (result >= min_count && find_sibling_command(*arg).first)
                return result;

            ++result;
        }
        return result;
    }

    bool ArgumentIteratorImpl::check_argument_and_option_counts()
    {
        auto& parsed_args = *parsed_arguments();
        for (const auto& o : m_command->options)
        {
            if (!o->optional && !parsed_args.has(o->value_id))
            {
                auto flags = o->flags.front();
                for (unsigned i = 1; i < o->flags.size(); ++i)
                    flags += ", " + o->flags[i];
                error("Mandatory option is missing: " + flags);
                return false;
            }
        }
        if (*m_command->require_subcommand && parsed_args.subcommands().empty())
        {
            error("No command was given.");
            return false;
        }
        if (m_argument_counter.is_complete())
        {
            m_state = State::DONE;
            for (const auto& parsed_args : m_parsed_args)
                parsed_args->set_result_code(ParserResultCode::SUCCESS);
            return true;
        }
        else
        {
            auto [lo, hi] = ArgumentCounter::get_min_max_count(*m_command);
            error((lo == hi
                       ? "Too few arguments. Expected "
                       : "Too few arguments. Expected at least ")
                  + std::to_string(lo) + ", received "
                  + std::to_string(m_argument_counter.count()) + ".");
            return false;
        }
    }

    std::optional<size_t>
    ArgumentIteratorImpl::find_first_multi_command_parent() const
    {
        auto size = m_parsed_args.size();
        if (size <= 1)
            return {};

        for (size_t i = size - 1; i-- > 0;)
        {
            const auto& parent = *m_parsed_args[i]->command();
            if (parent.multi_command)
                return i;
        }

        return {};
    }

    std::pair<const CommandData*, size_t>
    ArgumentIteratorImpl::find_sibling_command(std::string_view name) const
    {
        if (!m_argument_counter.is_complete())
            return {nullptr, 0};

        if (auto index = find_first_multi_command_parent())
        {
            return {
                m_parsed_args[*index]->command()->find_command(
                    name, m_data->parser_settings.case_insensitive),
                *index
            };
        }

        return {nullptr, 0};
    }

    void ArgumentIteratorImpl::reactivate_multi_command_parent(size_t index)
    {
        m_parsed_args.resize(index + 1);
        m_command = m_parsed_args.back()->command();
        m_argument_counter = {};
        m_state = State::ARGUMENTS_ONLY;
    }

    void ArgumentIteratorImpl::error(const std::string& message)
    {
        if (!message.empty())
            write_error_message(*m_data, *m_command, message);

        if (m_data->parser_settings.auto_exit)
            exit(m_data->parser_settings.error_exit_code);

        copy_remaining_arguments_to_parser_result();
        for (const auto& parsed_args : m_parsed_args)
            parsed_args->set_result_code(ParserResultCode::FAILURE);
        m_state = State::ERROR;
    }
}
