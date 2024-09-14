//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "StandardOptionIterator.hpp"

#include "ArgosThrow.hpp"
#include "StringUtilities.hpp"

namespace argos
{
    StandardOptionIterator::StandardOptionIterator()
        : m_args()
    {}

    StandardOptionIterator::StandardOptionIterator(std::vector<std::string_view> args)
        : m_all_args(std::move(args)),
          m_args(m_all_args)
    {}

    StandardOptionIterator::StandardOptionIterator(const StandardOptionIterator& rhs)
        : m_all_args(rhs.m_all_args),
          m_args(m_all_args.begin() + rhs.m_all_args.size() - rhs.m_args.size(), m_all_args.end()),
          m_pos(rhs.m_pos)
    {}

    std::optional<std::string> StandardOptionIterator::next()
    {
        if (m_pos == std::string_view::npos)
        {
            m_pos = 0;
            pop_front(m_args);
        }
        else if (m_pos != 0)
        {
            if (m_pos < m_args[0].size() && m_args[0][1] != '-')
            {
                const auto c = m_args[0][m_pos++];
                if (m_pos == m_args[0].size())
                    m_pos = std::string_view::npos;
                return std::string{'-', c};
            }
            pop_front(m_args);
            m_pos = 0;
        }

        if (m_args.empty())
            return {};

        if (m_args[0].size() <= 2 || m_args[0][0] != '-')
        {
            m_pos = std::string_view::npos;
            return std::string(m_args[0]);
        }

        if (m_args[0][1] != '-')
        {
            m_pos = 2;
            return std::string(m_args[0].substr(0, 2));
        }

        const auto eq = m_args[0].find('=');
        if (eq == std::string_view::npos)
        {
            m_pos = std::string_view::npos;
            return std::string(m_args[0]);
        }

        m_pos = eq + 1;
        return std::string(m_args[0].substr(0, m_pos));
    }

    std::optional<std::string> StandardOptionIterator::next_value()
    {
        if (m_args.empty())
            return {};

        if (m_pos != std::string_view::npos)
        {
            const auto result = m_args[0].substr(m_pos);
            m_pos = std::string_view::npos;
            return std::string(result);
        }

        pop_front(m_args);
        if (m_args.empty())
        {
            m_pos = 0;
            return {};
        }

        return std::string(m_args[0]);
    }

    std::string_view StandardOptionIterator::current() const
    {
        if (m_args.empty())
            ARGOS_THROW("There is no current argument.");
        return m_args[0];
    }

    std::span<std::string_view> StandardOptionIterator::remaining_arguments() const
    {
        return m_pos == 0 ? m_args : m_args.subspan(1);
    }

    IOptionIterator* StandardOptionIterator::clone() const
    {
        return new StandardOptionIterator(*this);
    }
}
