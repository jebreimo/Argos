//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParserResultImpl.hpp"

namespace Argos
{
    ParserResultImpl::ParserResultImpl(std::shared_ptr<ParserData> data)
        : m_Data(move(data))
    {}

    const std::vector<std::string>& ParserResultImpl::arguments() const
    {
        return m_Arguments;
    }

    void ParserResultImpl::addArgument(const std::string& arg)
    {
        m_Arguments.push_back(arg);
    }

    void ParserResultImpl::addUnprocessedArgument(const std::string& arg)
    {
        m_UnprocessedArguments.push_back(arg);
    }

    void ParserResultImpl::assignValue(int valueId, const std::string& value)
    {
        auto it = m_Values.lower_bound(valueId);
        if (it == m_Values.end() || it->first != valueId)
        {
            m_Values.emplace(valueId, value);
            return;
        }

        it->second = value;
        ++it;
        while (it != m_Values.end() && it->first == valueId)
            m_Values.erase(it++);
    }

    void ParserResultImpl::appendValue(int valueId, const std::string& value)
    {
        m_Values.emplace(valueId, value);
    }

    void ParserResultImpl::clearValue(int valueId)
    {
        m_Values.erase(valueId);
    }
}
