//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ParsedArgumentsBuilder.hpp"

#include <iostream>
#include "ParsedArgumentsImpl.hpp"

namespace argos
{
    ParsedArgumentsBuilder::ParsedArgumentsBuilder(
            std::shared_ptr<ParsedArgumentsImpl> impl)
        : m_impl(std::move(impl))
    {}

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::append(const std::string& name,
                                   const std::string& value)
    {
        m_impl->append_value(m_impl->get_value_id(name), value, {});
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::append(const IArgumentView& arg,
                                   const std::string& value)
    {
        m_impl->append_value(arg.value_id(), value, arg.argument_id());
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::assign(const std::string& name,
                                   const std::string& value)
    {
        m_impl->assign_value(m_impl->get_value_id(name), value, {});
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::assign(const IArgumentView& arg,
                                   const std::string& value)
    {
        m_impl->assign_value(arg.value_id(), value, arg.argument_id());
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::clear(const std::string& name)
    {
        m_impl->clear_value(m_impl->get_value_id(name));
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::clear(const IArgumentView& arg)
    {
        m_impl->clear_value(arg.value_id());
        return *this;
    }

    ArgumentValue ParsedArgumentsBuilder::value(const std::string& name) const
    {
        auto id = m_impl->get_value_id(name);
        if (auto value = m_impl->get_value(id))
            return {value->first, m_impl, id, value->second};
        else
            return {{}, m_impl, id, {}};
    }

    ArgumentValue
    ParsedArgumentsBuilder::value(const IArgumentView& arg) const
    {
        if (auto value = m_impl->get_value(arg.value_id()))
            return {value->first, m_impl, arg.value_id(), arg.argument_id()};
        else
            return {{}, m_impl, arg.value_id(), arg.argument_id()};
    }

    ArgumentValues
    ParsedArgumentsBuilder::values(const std::string& name) const
    {
        auto id = m_impl->get_value_id(name);
        auto values = m_impl->get_values(id);
        return {std::move(values), m_impl, id};
    }

    ArgumentValues
    ParsedArgumentsBuilder::values(const IArgumentView& arg) const
    {
        auto values = m_impl->get_values(arg.value_id());
        return {std::move(values), m_impl, arg.value_id()};
    }

    bool ParsedArgumentsBuilder::has(const std::string& name) const
    {
        return m_impl->has(m_impl->get_value_id(name));
    }

    bool ParsedArgumentsBuilder::has(const IArgumentView& arg) const
    {
        return m_impl->has(arg.value_id());
    }

    void ParsedArgumentsBuilder::error(const std::string& msg) const
    {
         m_impl->error(msg);
    }

    void ParsedArgumentsBuilder::error(const std::string& msg,
                                       const IArgumentView& arg) const
    {
        m_impl->error(msg, arg.argument_id());
    }

    std::ostream& ParsedArgumentsBuilder::stream() const
    {
        const auto custom_stream = m_impl->parser_data()->help_settings.output_stream;
        return custom_stream ? *custom_stream : std::cout;
    }

    const std::string& ParsedArgumentsBuilder::program_name() const
    {
      return m_impl->parser_data()->command.name;
    }
}
