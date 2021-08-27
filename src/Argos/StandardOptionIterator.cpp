//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "StandardOptionIterator.hpp"
#include "ArgosThrow.hpp"

namespace argos
{
    StandardOptionIterator::StandardOptionIterator()
        : m_args_it(m_args.end()),
          m_pos(0)
    {}

    StandardOptionIterator::StandardOptionIterator(std::vector<std::string_view> args)
        : m_args(move(args)),
          m_args_it(m_args.begin()),
          m_pos(0)
    {}

    StandardOptionIterator::StandardOptionIterator(const StandardOptionIterator& rhs)
        : m_args(rhs.m_args),
          m_args_it(m_args.begin() + std::distance(rhs.m_args.begin(), rhs.m_args_it)),
          m_pos(rhs.m_pos)
    {}

    std::optional<std::string> StandardOptionIterator::next()
    {
        if (m_pos == std::string_view::npos)
        {
            m_pos = 0;
            ++m_args_it;
        }
        else if (m_pos != 0)
        {
            if (m_pos < m_args_it->size() && (*m_args_it)[1] != '-')
            {
                auto c = (*m_args_it)[m_pos++];
                if (m_pos == m_args_it->size())
                    m_pos = std::string_view::npos;
                return std::string{'-', c};
            }
            ++m_args_it;
            m_pos = 0;
        }

        if (m_args_it == m_args.end())
            return {};

        if (m_args_it->size() <= 2 || (*m_args_it)[0] != '-')
        {
            m_pos = std::string_view::npos;
            return std::string(*m_args_it);
        }

        if ((*m_args_it)[1] != '-')
        {
            m_pos = 2;
            return std::string(m_args_it->substr(0, 2));
        }

        auto eq = m_args_it->find('=');
        if (eq == std::string_view::npos)
        {
            m_pos = std::string_view::npos;
            return std::string(*m_args_it);
        }

        m_pos = eq + 1;
        return std::string(m_args_it->substr(0, m_pos));
    }

    std::optional<std::string> StandardOptionIterator::next_value()
    {
        if (m_args_it == m_args.end())
            return {};

        if (m_pos != std::string_view::npos)
        {
            auto result = m_args_it->substr(m_pos);
            m_pos = std::string_view::npos;
            return std::string(result);
        }

        if (++m_args_it == m_args.end())
        {
            m_pos = 0;
            return {};
        }

        return std::string(*m_args_it);
    }

    std::string_view StandardOptionIterator::current() const
    {
        if (m_args_it == m_args.end())
            ARGOS_THROW("There is no current argument.");
        return *m_args_it;
    }

    std::vector<std::string_view> StandardOptionIterator::remaining_arguments() const
    {
        auto it = m_pos == 0 ? m_args_it : std::next(m_args_it);
        return std::vector<std::string_view>(it, m_args.end());
    }

    IOptionIterator* StandardOptionIterator::clone() const
    {
        return new StandardOptionIterator(*this);
    }
}
