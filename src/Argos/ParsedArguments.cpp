//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ParsedArguments.hpp"

#include <algorithm>
#include <iostream>
#include "ArgosThrow.hpp"
#include "ParsedArgumentsImpl.hpp"

namespace argos
{
    ParsedArguments::ParsedArguments() = default;

    ParsedArguments::ParsedArguments(std::shared_ptr<ParsedArgumentsImpl> impl)
        : m_impl(std::move(impl))
    {}

    ParsedArguments::ParsedArguments(ParsedArguments&& rhs) noexcept
        : m_impl(std::move(rhs.m_impl))
    {}

    ParsedArguments::~ParsedArguments() = default;

    ParsedArguments& ParsedArguments::operator=(ParsedArguments&& rhs) noexcept
    {
        m_impl = std::move(rhs.m_impl);
        return *this;
    }

    bool ParsedArguments::has(const std::string& name) const
    {
        return m_impl->has(m_impl->get_value_id(name));
    }

    bool ParsedArguments::has(const IArgumentView& arg) const
    {
        return m_impl->has(arg.value_id());
    }

    ArgumentValue ParsedArguments::value(const std::string& name) const
    {
        auto id = m_impl->get_value_id(name);
        auto value = m_impl->get_value(id);
        if (value)
            return {value->first, m_impl, id, value->second};
        else
            return {{}, m_impl, id, {}};
    }

    ArgumentValue ParsedArguments::value(const IArgumentView& arg) const
    {
        auto value = m_impl->get_value(arg.value_id());
        if (value)
            return {value->first, m_impl, arg.value_id(), arg.argument_id()};
        else
            return {{}, m_impl, arg.value_id(), arg.argument_id()};
    }

    ArgumentValues ParsedArguments::values(const std::string& name) const
    {
        auto id = m_impl->get_value_id(name);
        auto values = m_impl->get_values(id);
        return {values, m_impl, id};
    }

    ArgumentValues ParsedArguments::values(const IArgumentView& arg) const
    {
        auto values = m_impl->get_values(arg.value_id());
        return {values, m_impl, arg.value_id()};
    }

    std::vector<std::unique_ptr<ArgumentView>>
    ParsedArguments::all_arguments() const
    {
        std::vector<std::unique_ptr<ArgumentView>> result;
        for (auto& a : m_impl->parser_data()->arguments)
            result.emplace_back(std::make_unique<ArgumentView>(a.get()));
        return result;
    }

    std::vector<std::unique_ptr<OptionView>>
    ParsedArguments::all_options() const
    {
        std::vector<std::unique_ptr<OptionView>> result;
        for (auto& o : m_impl->parser_data()->options)
            result.emplace_back(std::make_unique<OptionView>(o.get()));
        return result;
    }

    ParserResultCode ParsedArguments::result_code() const
    {
        return m_impl->result_code();
    }

    OptionView ParsedArguments::stop_option() const
    {
        const auto* option = m_impl->stop_option();
        if (!option)
            ARGOS_THROW("There is no special option.");
        return OptionView(option);
    }

    const std::vector<std::string>&
    ParsedArguments::unprocessed_arguments() const
    {
        return m_impl->unprocessed_arguments();
    }

    void ParsedArguments::filter_parsed_arguments(int& argc, char**& argv)
    {
        if (argc <= 1)
            return;
        const auto& unprocessed = m_impl->unprocessed_arguments();
        auto it = unprocessed.begin();
        int out = 1;
        if (!unprocessed.empty())
        {
            for (int in = 1; in < argc; ++in)
            {
                if (argv[in] == *it)
                {
                    argv[out++] = argv[in];
                    if (++it == unprocessed.end())
                        break;
                }
            }
        }
        argc = out;
    }

    void ParsedArguments::error(const std::string& msg)
    {
        m_impl->error(msg);
    }

    namespace
    {
        std::string get_name(const IArgumentView& arg)
        {
            if (const auto* a = dynamic_cast<const ArgumentView*>(&arg))
                return a->name();

            if (const auto* o = dynamic_cast<const OptionView*>(&arg))
            {
                std::string s;
                for (const auto& f : o->flags())
                {
                    if (!s.empty())
                        s += ", ";
                    s += f;
                }
                return s;
            }
            return {};
        }

        void print_argument(std::ostream& stream,
                            const std::string& label,
                            const ArgumentValues& values)
        {
            stream << label << ":";
            for (const auto value : values)
                stream << " \"" << value.as_string() << "\"";
            stream << "\n";
        }
    }

    void print(const ParsedArguments& args)
    {
        print(args, std::cout);
    }

    void print(const ParsedArguments& args, std::ostream& stream)
    {
        std::vector<const IArgumentView*> argViews;
        auto a = args.all_arguments();
        std::transform(a.begin(), a.end(), back_inserter(argViews),
                       [](auto& av) {return av.get();});
        auto o = args.all_options();
        std::transform(o.begin(), o.end(), back_inserter(argViews),
                       [](auto& ov) {return ov.get();});

        stable_sort(argViews.begin(), argViews.end(),
                    [](auto& a, auto& b) {return a->value_id() < b->value_id();});

        std::vector<std::pair<const IArgumentView*, std::string>> labels;
        for (const auto* arg : argViews)
        {
            if (!labels.empty() && labels.back().first->value_id() == arg->value_id())
                labels.back().second += ", " + get_name(*arg);
            else
                labels.emplace_back(arg, get_name(*arg));
        }

        for (const auto&[arg, label] : labels)
            print_argument(stream, label, args.values(*arg));

        if (!args.unprocessed_arguments().empty())
        {
            stream << "Unprocessed arguments:";
            for (auto& arg : args.unprocessed_arguments())
                stream << " \"" << arg << "\"";
        }
    }
}
