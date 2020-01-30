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

    ParsedArgumentsBuilder& ParsedArgumentsBuilder::append(int valueId, const std::string& value)
    {
        m_Impl->appendValue(valueId, value);
        return *this;
    }

    ParsedArgumentsBuilder& ParsedArgumentsBuilder::assign(int valueId, const std::string& value)
    {
        m_Impl->assignValue(valueId, value);
        return *this;
    }

    ParsedArgumentsBuilder& ParsedArgumentsBuilder::clear(int valueId)
    {
        m_Impl->clearValue(valueId);
        return *this;
    }

    bool ParsedArgumentsBuilder::has(int valueId)
    {
        return m_Impl->has(valueId);
    }
}
