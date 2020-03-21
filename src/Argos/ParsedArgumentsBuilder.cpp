//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ParsedArgumentsBuilder.hpp"

#include "ParsedArgumentsImpl.hpp"

namespace Argos
{
    ParsedArgumentsBuilder::ParsedArgumentsBuilder(
            std::shared_ptr<ParsedArgumentsImpl> impl)
        : m_Impl(move(impl))
    {}

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::append(const std::string& name,
                                   const std::string& value)
    {
        m_Impl->appendValue(m_Impl->getValueId(name), value, {});
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::append(const IArgumentView& arg,
                                   const std::string& value)
    {
        m_Impl->appendValue(arg.valueId(), value, arg.argumentId());
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::assign(const std::string& name,
                                   const std::string& value)
    {
        m_Impl->assignValue(m_Impl->getValueId(name), value, {});
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::assign(const IArgumentView& arg,
                                   const std::string& value)
    {
        m_Impl->assignValue(arg.valueId(), value, arg.argumentId());
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::clear(const std::string& name)
    {
        m_Impl->clearValue(m_Impl->getValueId(name));
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::clear(const IArgumentView& arg)
    {
        m_Impl->clearValue(arg.valueId());
        return *this;
    }

    ArgumentValue ParsedArgumentsBuilder::value(const std::string& name)
    {
        auto id = m_Impl->getValueId(name);
        auto value = m_Impl->getValue(id);
        if (value)
            return {value->first, m_Impl, id, value->second};
        else
            return {{}, m_Impl, id, {}};
    }

    ArgumentValue
    ParsedArgumentsBuilder::value(const IArgumentView& arg)
    {
        auto value = m_Impl->getValue(arg.valueId());
        if (value)
            return {value->first, m_Impl, arg.valueId(), arg.argumentId()};
        else
            return {{}, m_Impl, arg.valueId(), arg.argumentId()};
    }

    ArgumentValues
    ParsedArgumentsBuilder::values(const std::string& name)
    {
        auto id = m_Impl->getValueId(name);
        auto values = m_Impl->getValues(id);
        return {move(values), m_Impl, id};
    }

    ArgumentValues
    ParsedArgumentsBuilder::values(const IArgumentView& arg)
    {
        auto values = m_Impl->getValues(arg.valueId());
        return {move(values), m_Impl, arg.valueId()};
    }

    bool ParsedArgumentsBuilder::has(const std::string& name)
    {
        return m_Impl->has(m_Impl->getValueId(name));
    }

    bool ParsedArgumentsBuilder::has(const IArgumentView& arg)
    {
        return m_Impl->has(arg.valueId());
    }

    void ParsedArgumentsBuilder::error(const std::string& errorMessage)
    {
         m_Impl->error(errorMessage);
    }

    void ParsedArgumentsBuilder::error(const std::string& errorMessage,
                                       const IArgumentView& arg)
    {
        m_Impl->error(errorMessage, arg.argumentId());
    }
}
