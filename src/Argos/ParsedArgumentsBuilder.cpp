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
    ParsedArgumentsBuilder::ParsedArgumentsBuilder(ParsedArgumentsImpl* impl)
        : m_Impl(impl)
    {}

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::append(const std::string& name,
                                   const std::string& value)
    {
        m_Impl->appendValue(m_Impl->getValueId(name), value);
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::append(const IArgumentView& arg,
                                   const std::string& value)
    {
        m_Impl->appendValue(arg.id(), value);
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::assign(const std::string& name,
                                   const std::string& value)
    {
        m_Impl->assignValue(m_Impl->getValueId(name), value);
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::assign(const IArgumentView& arg,
                                   const std::string& value)
    {
        m_Impl->assignValue(arg.id(), value);
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
        m_Impl->clearValue(arg.id());
        return *this;
    }

    std::optional<std::string_view>
    ParsedArgumentsBuilder::value(const std::string& name)
    {
        return m_Impl->getValue(m_Impl->getValueId(name));
    }

    std::optional<std::string_view>
    ParsedArgumentsBuilder::value(const IArgumentView& arg)
    {
        return m_Impl->getValue(arg.id());
    }

    std::vector<std::string_view>
    ParsedArgumentsBuilder::values(const std::string& name)
    {
        return m_Impl->getValues(m_Impl->getValueId(name));
    }

    std::vector<std::string_view>
    ParsedArgumentsBuilder::values(const IArgumentView& arg)
    {
        return m_Impl->getValues(arg.id());
    }

    bool ParsedArgumentsBuilder::has(const std::string& name)
    {
        return m_Impl->has(m_Impl->getValueId(name));
    }

    bool ParsedArgumentsBuilder::has(const IArgumentView& arg)
    {
        return m_Impl->has(arg.id());
    }

    void ParsedArgumentsBuilder::error(const std::string& errorMessage)
    {
        m_Impl->error(errorMessage);
    }
}
