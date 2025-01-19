//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "StandardOptionIterator.hpp"

#include "ArgosThrow.hpp"
#include "StringUtilities.hpp"

namespace argos
{
    StandardOptionIterator::StandardOptionIterator()
    {
    }

    StandardOptionIterator::StandardOptionIterator(std::vector<std::string_view> args)
        : m_all_args(args.begin(), args.end()),
          m_args(m_all_args)
    {
    }

    StandardOptionIterator::StandardOptionIterator(const StandardOptionIterator& rhs)
        : m_all_args(rhs.m_all_args),
          m_args(m_all_args.begin() + rhs.m_all_args.size() - rhs.m_args.size(), m_all_args.end()),
          m_pos(rhs.m_pos)
    {
    }

    std::optional<std::string> StandardOptionIterator::next()
    {
        if (m_pos != 0)
        {
            // m_pos is always greater than one if we get here.
            if (m_pos < m_args[0].size() && m_args[0][1] != '-')
            {
                const auto c = m_args[0][m_pos++];
                if (m_pos == m_args[0].size())
                    m_pos = std::string::npos;
                return std::string{'-', c};
            }
            pop_front(m_args);
            m_pos = 0;
        }

        if (m_args.empty())
            return {};

        if (m_args[0].size() <= 2 || m_args[0][0] != '-')
        {
            m_pos = std::string::npos;
            return m_args[0];
        }

        if (m_args[0][1] != '-')
        {
            m_pos = 2;
            return m_args[0].substr(0, 2);
        }

        m_pos = m_args[0].find('=');

        if (m_pos == std::string::npos)
            return m_args[0];

        m_pos++;
        return m_args[0].substr(0, m_pos);
    }

    std::optional<std::string> StandardOptionIterator::next_value()
    {
        if (m_args.empty())
            return {};

        if (m_pos != std::string::npos)
        {
            const auto pos = m_pos;
            m_pos = std::string::npos;
            return m_args[0].substr(pos);
        }

        pop_front(m_args);
        if (m_args.empty())
        {
            m_pos = 0;
            return {};
        }

        return m_args[0];
    }

    std::string_view StandardOptionIterator::current() const
    {
        if (m_args.empty())
            ARGOS_THROW("There is no current argument.");
        return m_args[0];
    }

    std::span<std::string> StandardOptionIterator::remaining_arguments()
    {
        split_concatenated_flags();
        return m_pos == 0 ? m_args : m_args.subspan(1);
    }

    void StandardOptionIterator::insert(std::vector<std::string> args)
    {
        split_concatenated_flags();
        auto args_index = m_all_args.size() - m_args.size();
        auto insert_index = args_index;
        if (m_pos != 0)
            insert_index++;
        m_all_args.insert(m_all_args.begin() + insert_index,
                          args.begin(), args.end());
        m_args = std::span(m_all_args.begin() + args_index, m_all_args.end());
    }

    void StandardOptionIterator::split_concatenated_flags()
    {
        if (m_pos == 0 || m_pos >= m_args[0].size() || m_args[0][1] == '-')
            return;

        auto index = m_all_args.size() - m_args.size();
        m_all_args.insert(m_all_args.begin() + index + 1,
                          "-" + m_args[0].substr(m_pos));
        m_all_args[index].resize(m_pos);
        m_args = std::span(m_all_args.begin() + index, m_all_args.end());
    }
}
