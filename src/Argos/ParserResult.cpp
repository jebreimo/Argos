//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParserResult.hpp"

namespace Argos
{
    ParserResult::ParserResult(std::shared_ptr<ArgumentData> data)
        : m_Data(move(data))
    {}

    const std::vector<std::string>& ParserResult::arguments() const
    {
        return m_Arguments;
    }

    void ParserResult::addArgument(const std::string& arg)
    {
        m_Arguments.push_back(arg);
    }

    void ParserResult::addUnprocessedArgument(const std::string& arg)
    {
        m_UnprocessedArguments.push_back(arg);
    }

    void ParserResult::assignValue(int valueId, const std::string& value)
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

    void ParserResult::appendValue(int valueId, const std::string& value)
    {
        m_Values.emplace(valueId, value);
    }

    void ParserResult::clearValue(int valueId)
    {
        m_Values.erase(valueId);
    }
}
