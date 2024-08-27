//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParsedArgumentsImpl.hpp"

#include <algorithm>
#include <cassert>
#include "Argos/ArgumentView.hpp"
#include "Argos/OptionView.hpp"
#include "ArgosThrow.hpp"
#include "HelpText.hpp"

namespace argos
{
    namespace
    {
        template <typename It, typename Value, typename IsLess>
        It lower_bound(It begin, It end, Value&& v, IsLess is_less)
        {
            while (begin != end)
            {
                auto offset = std::distance(begin, end) / 2;
                auto mid = begin + offset;
                if (is_less(*mid, v))
                    begin = mid + 1;
                else
                    end = mid;
            }
            return begin;
        }
    }

    ParsedArgumentsImpl::ParsedArgumentsImpl(std::shared_ptr<ParserData> data)
        : m_data(std::move(data))
    {
        assert(m_data);
        for (auto& a : m_data->arguments)
        {
            m_ids.emplace_back(a->name, a->value_id, a->argument_id);
            if (!a->value.empty())
                m_ids.emplace_back(a->value, a->value_id, a->argument_id);
        }
        for (auto& o : m_data->options)
        {
            if (o->operation == OptionOperation::NONE)
                continue;

            for (auto& f : o->flags)
                m_ids.emplace_back(f, o->value_id, o->argument_id);
            if (!o->alias.empty())
                m_ids.emplace_back(o->alias, o->value_id, o->argument_id);
        }
        if (!m_ids.empty())
        {
            using std::get;
            sort(m_ids.begin(), m_ids.end());
            for (auto it = next(m_ids.begin()); it != m_ids.end(); ++it)
            {
                auto p = prev(it);
                if (get<0>(*it) == get<0>(*p) && get<2>(*it) != get<2>(*p))
                    get<2>(*it) = get<2>(*p) = {};
            }
            m_ids.erase(unique(m_ids.begin(), m_ids.end()), m_ids.end());
        }
    }

    bool ParsedArgumentsImpl::has(ValueId value_id) const
    {
        return m_values.find(value_id) != m_values.end();
    }

    const std::vector<std::string>&
    ParsedArgumentsImpl::unprocessed_arguments() const
    {
        return m_unprocessed_arguments;
    }

    void ParsedArgumentsImpl::add_unprocessed_argument(const std::string& arg)
    {
        m_unprocessed_arguments.push_back(arg);
    }

    std::string_view
    ParsedArgumentsImpl::assign_value(ValueId value_id,
                                      const std::string& value,
                                      ArgumentId argument_id)
    {
        auto it = m_values.lower_bound(value_id);
        if (it == m_values.end() || it->first != value_id)
            return append_value(value_id, value, argument_id);

        it->second = {value, argument_id};
        auto nxt = next(it);
        while (nxt != m_values.end() && nxt->first == value_id)
            m_values.erase(nxt++);
        return it->second.first;
    }

    std::string_view
    ParsedArgumentsImpl::append_value(ValueId value_id,
                                      const std::string& value,
                                      ArgumentId argument_id)
    {
        return m_values.insert({value_id, {value, argument_id}})->second.first;
    }

    void ParsedArgumentsImpl::clear_value(ValueId value_id)
    {
        m_values.erase(value_id);
    }

    ValueId
    ParsedArgumentsImpl::get_value_id(std::string_view value_name) const
    {
        using std::get;
        auto it = lower_bound(m_ids.begin(), m_ids.end(), value_name,
                              [](auto& p, auto& s) {return get<0>(p) < s;});
        if (it == m_ids.end() || get<0>(*it) != value_name)
            ARGOS_THROW("Unknown value: " + std::string(value_name));
        return get<1>(*it);
    }

    std::optional<std::pair<std::string_view, ArgumentId>>
    ParsedArgumentsImpl::get_value(ValueId value_id) const
    {
        auto it = m_values.lower_bound(value_id);
        if (it == m_values.end() || it->first != value_id)
            return {};
        auto nx = next(it);
        if (nx != m_values.end() && nx->first == value_id)
            ARGOS_THROW("Attempt to read multiple values as a single value.");
        return it->second;
    }

    std::vector<std::pair<std::string_view, ArgumentId>>
    ParsedArgumentsImpl::get_values(ValueId value_id) const
    {
        std::vector<std::pair<std::string_view, ArgumentId>> result;
        for (auto it = m_values.lower_bound(value_id);
             it != m_values.end() && it->first == value_id; ++it)
        {
            result.emplace_back(it->second);
        }
        return result;
    }

    std::vector<std::unique_ptr<IArgumentView>>
    ParsedArgumentsImpl::get_argument_views(ValueId value_id) const
    {
        std::vector<std::unique_ptr<IArgumentView>> result;
        for (auto& a : m_data->arguments)
        {
            if (a->value_id == value_id)
                result.emplace_back(std::make_unique<ArgumentView>(a.get()));
        }
        for (auto& o : m_data->options)
        {
            if (o->value_id == value_id)
                result.emplace_back(std::make_unique<OptionView>(o.get()));
        }
        return result;
    }

    std::unique_ptr<IArgumentView>
    ParsedArgumentsImpl::get_argument_view(ArgumentId argument_id) const
    {
        for (auto& a : m_data->arguments)
        {
            if (a->argument_id == argument_id)
                return std::make_unique<ArgumentView>(a.get());
        }
        for (auto& o : m_data->options)
        {
            if (o->argument_id == argument_id)
                return std::make_unique<OptionView>(o.get());
        }
        return {};
    }

    const std::shared_ptr<ParserData>& ParsedArgumentsImpl::parser_data() const
    {
        return m_data;
    }

    ParserResultCode ParsedArgumentsImpl::result_code() const
    {
        return m_result_code;
    }

    void ParsedArgumentsImpl::set_result_code(ParserResultCode result_code)
    {
        m_result_code = result_code;
    }

    const OptionData* ParsedArgumentsImpl::stop_option() const
    {
        return m_stop_option;
    }

    void ParsedArgumentsImpl::set_breaking_option(const OptionData* option)
    {
        m_result_code = ParserResultCode::STOP;
        m_stop_option = option;
    }

    void ParsedArgumentsImpl::error(const std::string& message) const
    {
        write_error_message(*m_data, message);
        if (m_data->parser_settings.auto_exit)
            exit(m_data->parser_settings.error_exit_code);
        else
            ARGOS_THROW("Error while parsing arguments.");
    }

    void ParsedArgumentsImpl::error(const std::string& message,
                                    ArgumentId argument_id)
    {
        write_error_message(*m_data, message, argument_id);
        if (m_data->parser_settings.auto_exit)
            exit(m_data->parser_settings.error_exit_code);
        else
            ARGOS_THROW("Error while parsing arguments.");
    }
}
