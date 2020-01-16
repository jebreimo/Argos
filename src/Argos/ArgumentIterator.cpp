//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ArgumentIterator.hpp"

namespace Argos
{
    ArgumentIterator::ArgumentIterator(std::vector<std::string_view> args)
        : m_Args(move(args)),
          m_ArgsIt(m_Args.begin())
    {}

    std::optional<std::string> ArgumentIterator::next()
    {
        if (m_PrecededByEqual)
        {
            m_PrecededByEqual = false;
            m_Remainder = {};
        }

        if (!m_Remainder.empty())
        {
            auto flag = m_Remainder.front();
            m_Remainder = m_Remainder.substr(1);
            return std::string{'-', flag};
        }

        if (m_ArgsIt == m_Args.end())
            return {};

        if (m_ArgsIt->size() <= 2 || (*m_ArgsIt)[0] != '-')
            return std::string(*m_ArgsIt++);

        if ((*m_ArgsIt)[1] != '-')
        {
            m_Remainder = m_ArgsIt->substr(2);
            return std::string((m_ArgsIt++)->substr(0, 2));
        }

        auto eq = m_ArgsIt->find('=');
        if (eq == std::string_view::npos)
            return std::string(*m_ArgsIt++);

        ++eq;
        m_PrecededByEqual = true;
        m_Remainder = m_ArgsIt->substr(eq);
        return std::string((m_ArgsIt++)->substr(0, eq));
    }

    std::optional<std::string> ArgumentIterator::nextValue()
    {
        if (!m_Remainder.empty() || m_PrecededByEqual)
        {
            auto result = m_Remainder;
            m_Remainder = {};
            m_PrecededByEqual = false;
            return std::string(result);
        }

        if (m_ArgsIt != m_Args.end())
            return std::string(*m_ArgsIt++);
        return {};
    }

    bool ArgumentIterator::hasRemainder() const
    {
        return m_PrecededByEqual;
    }
}
