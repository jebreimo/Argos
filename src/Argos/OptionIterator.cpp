//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-18.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "OptionIterator.hpp"
#include "ArgosThrow.hpp"
#include "StringUtilities.hpp"

namespace argos
{
    OptionIterator::OptionIterator()
        : m_args(m_all_args)
    {
    }

    OptionIterator::OptionIterator(std::vector<std::string_view> args, char prefix)
        : m_all_args(std::move(args)),
          m_args(m_all_args),
          m_prefix(prefix)
    {
    }

    OptionIterator::OptionIterator(const OptionIterator& rhs)
        : m_all_args(rhs.m_all_args),
          m_args(m_all_args.begin() + rhs.m_all_args.size() - rhs.m_args.size(), m_all_args.end()),
          m_pos(rhs.m_pos),
          m_prefix(rhs.m_prefix)
    {
    }

    std::optional<std::string> OptionIterator::next()
    {
        if (m_pos != 0)
        {
            m_pos = 0;
            pop_front(m_args);
        }

        if (m_args.empty())
            return {};

        if (m_args[0].size() <= 2 || m_args[0][0] != m_prefix)
        {
            m_pos = std::string_view::npos;
            return std::string(m_args[0]);
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

    std::optional<std::string> OptionIterator::next_value()
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

        m_pos = m_args[0].size();
        return std::string(m_args[0]);
    }

    std::string_view OptionIterator::current() const
    {
        if (m_args.empty())
            ARGOS_THROW("There is no current argument.");
        return m_args[0];
    }

    std::span<std::string_view> OptionIterator::remaining_arguments() const
    {
        return m_pos == 0 ? m_args : m_args.subspan(1);
    }

    OptionIterator* OptionIterator::clone() const
    {
        return new OptionIterator(*this);
    }
}
