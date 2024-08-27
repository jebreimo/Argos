//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-18.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "OptionIterator.hpp"
#include "ArgosThrow.hpp"

namespace argos
{
    OptionIterator::OptionIterator()
        : m_args_it(m_args.begin())
    {}

    OptionIterator::OptionIterator(std::vector<std::string_view> args, char prefix)
        : m_args(std::move(args)),
          m_args_it(m_args.begin()),
          m_prefix(prefix)
    {}

    OptionIterator::OptionIterator(const OptionIterator& rhs)
        : m_args(rhs.m_args),
          m_args_it(m_args.begin() + std::distance(rhs.m_args.begin(), rhs.m_args_it)),
          m_pos(rhs.m_pos),
          m_prefix(rhs.m_prefix)
    {}

    std::optional<std::string> OptionIterator::next()
    {
        if (m_pos != 0)
        {
            m_pos = 0;
            ++m_args_it;
        }

        if (m_args_it == m_args.end())
            return {};

        if (m_args_it->size() <= 2 || (*m_args_it)[0] != m_prefix)
        {
            m_pos = std::string_view::npos;
            return std::string(*m_args_it);
        }

        const auto eq = m_args_it->find('=');
        if (eq == std::string_view::npos)
        {
            m_pos = std::string_view::npos;
            return std::string(*m_args_it);
        }

        m_pos = eq + 1;
        return std::string(m_args_it->substr(0, m_pos));
    }

    std::optional<std::string> OptionIterator::next_value()
    {
        if (m_args_it == m_args.end())
            return {};

        if (m_pos != std::string_view::npos)
        {
            const auto result = m_args_it->substr(m_pos);
            m_pos = std::string_view::npos;
            return std::string(result);
        }

        if (++m_args_it == m_args.end())
        {
            m_pos = 0;
            return {};
        }

        m_pos = m_args_it->size();
        return std::string(*m_args_it);
    }

    std::string_view OptionIterator::current() const
    {
        if (m_args_it == m_args.end())
            ARGOS_THROW("There is no current argument.");
        return *m_args_it;
    }

    std::vector<std::string_view> OptionIterator::remaining_arguments() const
    {
        auto it = m_pos == 0 ? m_args_it : std::next(m_args_it);
        return {it, m_args.end()};
    }

    OptionIterator* OptionIterator::clone() const
    {
        return new OptionIterator(*this);
    }
}
