//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
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
        using OptionTable = std::vector<std::pair<std::string_view, const OptionData*>>;

        OptionTable make_option_index(
            const std::vector<std::unique_ptr<OptionData>>& options,
            bool case_insensitive)
        {
            OptionTable index;
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
                return index;

            if (it->first == next(it)->first)
            {
                ARGOS_THROW("Multiple definitions of flag "
                            + std::string(it->first));
            }

            ARGOS_THROW("Conflicting flags: " + std::string(it->first)
                        + " and " + std::string(next(it)->first));
        }

        const OptionData* find_option_impl(const OptionTable& options,
                                           std::string_view arg,
                                           bool allow_abbreviations,
                                           bool case_insensitive)
        {
            const auto it = std::lower_bound(
                options.begin(), options.end(),
                OptionTable::value_type(arg, nullptr),
                [&](auto& a, auto& b)
                {
                    return is_less(a.first, b.first, case_insensitive);
                });
            if (it == options.end())
                return nullptr;
            if (it->first == arg)
                return it->second;
            if (case_insensitive && are_equal_ci(it->first, arg))
                return it->second;
            if (!allow_abbreviations)
                return nullptr;
            if (!starts_with(it->first, arg, case_insensitive))
                return nullptr;
            const auto nxt = next(it);
            if (nxt != options.end()
                && starts_with(nxt->first, arg, case_insensitive))
                return nullptr;
            return it->second;
        }

        const OptionData* find_option(const OptionTable& options,
                                      std::string_view arg,
                                      bool allow_abbreviations,
                                      bool case_insensitive)
        {
            auto opt = find_option_impl(options, arg, allow_abbreviations,
                                        case_insensitive);
            if (opt == nullptr && arg.size() > 2 && arg.back() == '=')
            {
                arg = arg.substr(0, arg.size() - 1);
                opt = find_option_impl(options, arg, allow_abbreviations,
                                       case_insensitive);
                if (opt && opt->argument.empty())
                    opt = nullptr;
            }
            return opt;
        }

        bool is_option(const std::string& s, OptionStyle style)
        {
            if (s.size() < 2)
                return false;

            return s[0] == (style == OptionStyle::SLASH ? '/' : '-');
        }

        std::unique_ptr<IOptionIterator>
        make_option_iterator(OptionStyle style,
                             std::vector<std::string_view> args)
        {
            switch (style)
            {
            case OptionStyle::SLASH:
                return std::make_unique<OptionIterator>(std::move(args), '/');
            case OptionStyle::DASH:
                return std::make_unique<OptionIterator>(std::move(args), '-');
            default:
                return std::make_unique<StandardOptionIterator>(std::move(args));
            }
        }
    }

    ArgumentIteratorImpl::ArgumentIteratorImpl(std::vector<std::string_view> args,
                                               std::shared_ptr<ParserData> data)
        : m_data(std::move(data)),
          m_options(make_option_index(m_data->command.options,
                                      m_data->parser_settings.case_insensitive)),
          m_parsed_args(std::make_shared<ParsedArgumentsImpl>(m_data)),
          m_iterator(make_option_iterator(m_data->parser_settings.option_style,
                                          std::move(args)))
    {
        for (const auto& option : m_data->command.options)
        {
            if (!option->initial_value.empty())
            {
                m_parsed_args->append_value(option->value_id,
                                            option->initial_value,
                                            option->argument_id);
            }
        }

        if (!ArgumentCounter::requires_argument_count(m_data->command.arguments))
            m_argument_counter = ArgumentCounter(m_data->command.arguments);
        else
            m_argument_counter = ArgumentCounter(m_data->command.arguments,
                                                 count_arguments());
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
        return iterator.m_parsed_args;
    }

    IteratorResult ArgumentIteratorImpl::next()
    {
        if (m_state == State::ERROR)
            ARGOS_THROW("next() called after error.");
        if (m_state == State::DONE)
            return {IteratorResultCode::DONE, nullptr, {}};

        const auto arg = m_state == State::ARGUMENTS_AND_OPTIONS
                             ? m_iterator->next()
                             : m_iterator->next_value();
        if (!arg)
        {
            if (check_argument_and_option_counts())
                return {IteratorResultCode::DONE, nullptr, {}};
            else
                return {IteratorResultCode::ERROR, nullptr, {}};
        }

        if (m_state == State::ARGUMENTS_AND_OPTIONS
            && is_option(*arg, m_data->parser_settings.option_style))
        {
            return process_option(*arg);
        }
        else
        {
            return process_argument(*arg);
        }
    }

    const std::shared_ptr<ParsedArgumentsImpl>&
    ArgumentIteratorImpl::parsed_arguments() const
    {
        return m_parsed_args;
    }

    std::pair<ArgumentIteratorImpl::OptionResult, std::string_view>
    ArgumentIteratorImpl::process_option(const OptionData& opt,
                                         const std::string& flag)
    {
        std::string_view arg;
        switch (opt.operation)
        {
        case OptionOperation::ASSIGN:
            if (!opt.constant.empty())
            {
                m_parsed_args->assign_value(opt.value_id, opt.constant,
                                            opt.argument_id);
            }
            else if (const auto value = m_iterator->next_value())
            {
                arg = m_parsed_args->assign_value(opt.value_id, *value,
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
                m_parsed_args->append_value(opt.value_id, opt.constant,
                                            opt.argument_id);
            }
            else if (const auto value = m_iterator->next_value())
            {
                arg = m_parsed_args->append_value(opt.value_id, *value,
                                                  opt.argument_id);
            }
            else
            {
                error(flag + ": no value given.");
                return {OptionResult::ERROR, {}};
            }
            break;
        case OptionOperation::CLEAR:
            m_parsed_args->clear_value(opt.value_id);
            break;
        case OptionOperation::NONE:
            break;
        }

        if (opt.callback)
        {
            opt.callback(OptionView(&opt), arg,
                         ParsedArgumentsBuilder(m_parsed_args));
        }
        if (m_data->parser_settings.option_callback)
        {
            m_data->parser_settings.option_callback(
                OptionView(&opt), arg,
                ParsedArgumentsBuilder(m_parsed_args));
        }

        switch (opt.type)
        {
        case OptionType::NORMAL:
            return {OptionResult::NORMAL, arg};
        case OptionType::HELP:
            write_help_text(*m_data);
            [[fallthrough]];
        case OptionType::EXIT:
            m_state = State::DONE;
            m_parsed_args->set_breaking_option(&opt);
            return {OptionResult::EXIT, arg};
        case OptionType::STOP:
            m_state = State::DONE;
            m_parsed_args->set_breaking_option(&opt);
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
        auto option = find_option(
            m_options, flag,
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
                    return {IteratorResultCode::ERROR, nullptr, {}};
                [[fallthrough]];
            case OptionResult::STOP:
                copy_remaining_arguments_to_parser_result();
                [[fallthrough]];
            default:
                return {IteratorResultCode::OPTION, option, arg};
            }
        }
        if (!m_data->parser_settings.ignore_undefined_options
            || !starts_with(m_iterator->current(), flag))
        {
            error("Unknown option: " + std::string(m_iterator->current()));
            return {IteratorResultCode::ERROR, nullptr, {}};
        }
        else
        {
            m_parsed_args->add_unprocessed_argument(
                std::string(m_iterator->current()));
            return {IteratorResultCode::UNKNOWN, nullptr, m_iterator->current()};
        }
    }

    IteratorResult
    ArgumentIteratorImpl::process_argument(const std::string& name)
    {
        if (auto argument = m_argument_counter.next_argument())
        {
            auto s = m_parsed_args->append_value(argument->value_id, name,
                                                 argument->argument_id);
            if (argument->callback)
            {
                argument->callback(ArgumentView(argument), s,
                                   ParsedArgumentsBuilder(m_parsed_args));
            }
            if (m_data->parser_settings.argument_callback)
            {
                m_data->parser_settings.argument_callback(
                    ArgumentView(argument), s,
                    ParsedArgumentsBuilder(m_parsed_args));
            }
            return {IteratorResultCode::ARGUMENT, argument, s};
        }
        else if (m_data->parser_settings.ignore_undefined_arguments)
        {
            m_parsed_args->add_unprocessed_argument(name);
        }
        else
        {
            error("Too many arguments, starting with \"" + name + "\".");
            return {IteratorResultCode::ERROR, nullptr, {}};
        }
        return {IteratorResultCode::UNKNOWN, nullptr, m_iterator->current()};
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void ArgumentIteratorImpl::copy_remaining_arguments_to_parser_result()
    {
        for (auto str : m_iterator->remaining_arguments())
            m_parsed_args->add_unprocessed_argument(std::string(str));
    }

    size_t ArgumentIteratorImpl::count_arguments() const
    {
        size_t result = 0;
        const std::unique_ptr<IOptionIterator> it(m_iterator->clone());
        bool arguments_only = false;
        for (auto arg = it->next(); arg && !arguments_only; arg = it->next())
        {
            const auto option = find_option(
                m_options, *arg,
                m_data->parser_settings.allow_abbreviated_options,
                m_data->parser_settings.case_insensitive
                );
            if (option)
            {
                if (!option->argument.empty())
                    it->next_value();
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
            }
            else if (!is_option(*arg, m_data->parser_settings.option_style))
            {
                ++result;
            }
        }

        for (auto arg = it->next(); arg; arg = it->next())
            ++result;
        return result;
    }

    bool ArgumentIteratorImpl::check_argument_and_option_counts()
    {
        for (const auto& o : m_data->command.options)
        {
            if (!o->optional && !m_parsed_args->has(o->value_id))
            {
                auto flags = o->flags.front();
                for (unsigned i = 1; i < o->flags.size(); ++i)
                    flags += ", " + o->flags[i];
                error("Mandatory option \"" + flags + "\" is missing.");
                return false;
            }
        }
        if (m_argument_counter.is_complete())
        {
            m_state = State::DONE;
            m_parsed_args->set_result_code(ParserResultCode::SUCCESS);
            return true;
        }
        else
        {
            auto [lo, hi] = ArgumentCounter::get_min_max_count(m_data->command.arguments);
            error((lo == hi
                       ? "Too few arguments. Expected "
                       : "Too few arguments. Expected at least ")
                  + std::to_string(lo) + ", received "
                  + std::to_string(m_argument_counter.count()) + ".");
            return false;
        }
    }

    void ArgumentIteratorImpl::error(const std::string& message)
    {
        if (!message.empty())
            write_error_message(*m_data, message);
        if (m_data->parser_settings.auto_exit)
            exit(m_data->parser_settings.error_exit_code);
        copy_remaining_arguments_to_parser_result();
        m_parsed_args->set_result_code(ParserResultCode::FAILURE);
        m_state = State::ERROR;
    }
}
