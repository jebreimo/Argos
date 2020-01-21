//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ArgumentIterator.hpp"
#include "ArgosException.hpp"

namespace Argos
{
    ArgumentIterator::ArgumentIterator()
        : m_ArgsIt(m_Args.end()),
          m_Pos(0)
    {}

    ArgumentIterator::ArgumentIterator(std::vector<std::string_view> args)
        : m_Args(move(args)),
          m_ArgsIt(m_Args.begin()),
          m_Pos(0)
    {}

    void ArgumentIterator::setArguments(std::vector<std::string_view> args)
    {
        m_Args = move(args);
        m_ArgsIt = m_Args.begin();
        m_Pos = 0;
    }

    std::optional<std::string> ArgumentIterator::next()
    {
        if (m_Pos == std::string_view::npos)
        {
            m_Pos = 0;
            ++m_ArgsIt;
        }
        else if (m_Pos != 0)
        {
            if (m_Pos < m_ArgsIt->size() && (*m_ArgsIt)[1] != '-')
                return std::string{'-', (*m_ArgsIt)[m_Pos++]};

            ++m_ArgsIt;
            m_Pos = 0;
        }

        if (m_ArgsIt == m_Args.end())
            return {};

        if (m_ArgsIt->size() <= 2 || (*m_ArgsIt)[0] != '-')
        {
            m_Pos = std::string_view::npos;
            return std::string(*m_ArgsIt);
        }

        if ((*m_ArgsIt)[1] != '-')
        {
            m_Pos = 2;
            return std::string(m_ArgsIt->substr(0, 2));
        }

        auto eq = m_ArgsIt->find('=');
        if (eq == std::string_view::npos)
        {
            m_Pos = std::string_view::npos;
            return std::string(*m_ArgsIt);
        }

        m_Pos = eq + 1;
        return std::string(m_ArgsIt->substr(0, m_Pos));
    }

    std::optional<std::string> ArgumentIterator::nextValue()
    {
        if (m_ArgsIt == m_Args.end())
            return {};

        if (m_Pos != std::string_view::npos)
        {
            auto result = m_ArgsIt->substr(m_Pos);
            m_Pos = std::string_view::npos;
            return std::string(result);
        }

        if (++m_ArgsIt == m_Args.end())
        {
            m_Pos = 0;
            return {};
        }

        m_Pos = m_ArgsIt->size();
        return std::string(*m_ArgsIt);
    }

    std::string_view ArgumentIterator::current() const
    {
        if (m_ArgsIt == m_Args.end())
            ARGOS_THROW("There is no current argument.");
        return *m_ArgsIt;
    }

    bool ArgumentIterator::hasRemainder() const
    {
        return m_ArgsIt != m_Args.end() && m_Pos <= m_ArgsIt->size();
    }

    std::string_view ArgumentIterator::remainder() const
    {
        if (m_ArgsIt == m_Args.end())
            ARGOS_THROW("There is no current argument.");
        return m_ArgsIt->substr(m_Pos);
    }

    void ArgumentIterator::skipRemainder()
    {
        if (m_ArgsIt != m_Args.end() && m_Pos != std::string_view::npos)
            m_Pos = std::string_view::npos;
    }
}
