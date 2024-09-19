//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ArgumentCounter.hpp"

namespace argos
{
    namespace
    {
        size_t find_first_optional(const CommandData& command)
        {
            auto& arguments = command.arguments;
            size_t result = 0;
            for (size_t i = 0; i < arguments.size(); ++i)
            {
                if (arguments[i]->min_count > 0)
                    result = i + 1;
            }
            return result;
        }

        void make_argument_counters(
            const CommandData& command,
            std::vector<std::pair<size_t, const ArgumentData*>>& counters,
            size_t& first_optional)
        {
            first_optional = find_first_optional(command);
            for (size_t i = 0; i < command.arguments.size(); ++i)
            {
                auto& a = command.arguments[i];
                if (i + 1 == first_optional && a->min_count != a->max_count)
                {
                    counters.emplace_back(a->min_count, a.get());
                    counters.emplace_back(a->max_count - a->min_count, a.get());
                }
                else
                {
                    counters.emplace_back(a->max_count, a.get());
                }
            }
        }

        std::vector<std::pair<size_t, const ArgumentData*>>
        make_argument_counters(const CommandData& command,
                               size_t n)
        {
            const auto [lo, hi] = ArgumentCounter::get_min_max_count(command);
            if (n < lo)
                n = 0;
            else if (n > hi)
                n = hi - lo;
            else
                n -= lo;

            std::vector<std::pair<size_t, const ArgumentData*>> result;
            for (auto& arg : command.arguments)
            {
                if (n == 0 || arg->min_count == arg->max_count)
                {
                    result.emplace_back(arg->min_count, arg.get());
                }
                else if (arg->min_count + n <= arg->max_count)
                {
                    result.emplace_back(arg->min_count + n, arg.get());
                    n = 0;
                }
                else
                {
                    result.emplace_back(arg->max_count, arg.get());
                    n -= arg->max_count - arg->min_count;
                }
            }
            return result;
        }
    }

    ArgumentCounter::ArgumentCounter()
        : m_counters()
    {
    }

    ArgumentCounter::ArgumentCounter(const CommandData& command)
    {
        make_argument_counters(command, m_counters, m_first_optional);
    }

    ArgumentCounter::ArgumentCounter(const CommandData& command,
                                     size_t argument_count)
        : m_counters(make_argument_counters(command, argument_count)),
          m_first_optional(m_counters.size())
    {
    }

    const ArgumentData* ArgumentCounter::next_argument()
    {
        while (m_index != m_counters.size() && m_counters[m_index].first == 0)
            ++m_index;

        if (m_index == m_counters.size())
            return nullptr;

        ++m_counter;
        --m_counters[m_index].first;
        return m_counters[m_index].second;
    }

    size_t ArgumentCounter::count() const
    {
        return m_counter;
    }

    bool ArgumentCounter::is_complete() const
    {
        return m_index >= m_first_optional
               || (m_index + 1 == m_first_optional
                   && m_counters[m_index].first == 0);
    }

    std::pair<size_t, size_t>
    ArgumentCounter::get_min_max_count(const CommandData& command)
    {
        size_t lo = 0, hi = 0;
        for (auto& arg : command.arguments)
        {
            lo += arg->min_count;
            if (hi != SIZE_MAX)
            {
                if (arg->max_count > SIZE_MAX - hi)
                    hi = SIZE_MAX;
                else
                    hi += arg->max_count;
            }
        }
        return {lo, hi};
    }

    bool ArgumentCounter::requires_argument_count(const CommandData& command)
    {
        bool deterministic = true;
        for (auto& arg : command.arguments)
        {
            if (!deterministic)
                return true;
            if (arg->min_count != arg->max_count)
                deterministic = false;
        }
        return false;
    }
}
