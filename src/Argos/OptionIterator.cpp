//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-18.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "OptionIterator.hpp"
#include "Argos/ArgosException.hpp"

namespace Argos
{
    OptionIterator::OptionIterator()
        : m_ArgsIt(m_Args.begin())
    {}

    OptionIterator::OptionIterator(std::vector<std::string_view> args, char prefix)
        : m_Args(move(args)),
          m_ArgsIt(m_Args.begin()),
          m_Prefix(prefix)
    {}

    OptionIterator::OptionIterator(const OptionIterator& rhs)
        : m_Args(rhs.m_Args),
          m_ArgsIt(m_Args.begin() + std::distance(rhs.m_Args.begin(), rhs.m_ArgsIt)),
          m_Pos(rhs.m_Pos),
          m_Prefix(rhs.m_Prefix)
    {}

    std::optional<std::string> OptionIterator::next()
    {
        if (m_Pos != 0)
        {
            m_Pos = 0;
            ++m_ArgsIt;
        }

        if (m_ArgsIt == m_Args.end())
            return {};

        if (m_ArgsIt->size() <= 2 || (*m_ArgsIt)[0] != m_Prefix)
        {
            m_Pos = std::string_view::npos;
            return std::string(*m_ArgsIt);
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

    std::optional<std::string> OptionIterator::nextValue()
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

    std::string_view OptionIterator::current() const
    {
        if (m_ArgsIt == m_Args.end())
            ARGOS_THROW("There is no current argument.");
        return *m_ArgsIt;
    }

    std::vector<std::string_view> OptionIterator::remainingArguments() const
    {
        auto it = m_Pos == 0 ? m_ArgsIt : std::next(m_ArgsIt);
        return std::vector<std::string_view>(it, m_Args.end());
    }

    OptionIterator* OptionIterator::clone() const
    {
        return new OptionIterator(*this);
    }
}
