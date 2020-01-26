//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "StandardOptionIterator.hpp"
#include "Argos/ArgosException.hpp"

namespace Argos
{
    StandardOptionIterator::StandardOptionIterator()
        : m_ArgsIt(m_Args.end()),
          m_Pos(0)
    {}

    StandardOptionIterator::StandardOptionIterator(std::vector<std::string_view> args)
        : m_Args(move(args)),
          m_ArgsIt(m_Args.begin()),
          m_Pos(0)
    {}

    std::optional<std::string> StandardOptionIterator::next()
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

    std::optional<std::string> StandardOptionIterator::nextValue()
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

    std::string_view StandardOptionIterator::current() const
    {
        if (m_ArgsIt == m_Args.end())
            ARGOS_THROW("There is no current argument.");
        return *m_ArgsIt;
    }

    std::vector<std::string_view> StandardOptionIterator::remainingArguments() const
    {
        auto it = m_Pos == 0 ? m_ArgsIt : std::next(m_ArgsIt);
        return std::vector<std::string_view>(it, m_Args.end());
    }
}
