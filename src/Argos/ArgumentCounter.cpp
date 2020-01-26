//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <optional>
#include "ArgumentCounter.hpp"

namespace Argos
{
    namespace
    {
        std::vector<std::pair<size_t, const ArgumentData*>>
        makeArgumentCounters(const std::vector<std::shared_ptr<ArgumentData>>& arguments)
        {
            std::vector<std::pair<size_t, const ArgumentData*>> result;
            result.reserve(arguments.size());
            for (auto& arg : arguments)
                result.emplace_back(arg->maxCount, arg.get());
            return result;
        }

        std::pair<size_t, size_t>
        countArguments(const std::vector<std::shared_ptr<ArgumentData>>& arguments)
        {
            size_t lo = 0, hi = 0;
            for (auto& arg : arguments)
            {
                lo += arg->minCount;
                if (hi != SIZE_MAX)
                {
                    if (arg->maxCount > SIZE_MAX - hi)
                        hi = SIZE_MAX;
                    else
                        hi += arg->maxCount;
                }
            }
            return {lo, hi};
        }

        std::vector<std::pair<size_t, const ArgumentData*>>
        makeArgumentCounters(const std::vector<std::shared_ptr<ArgumentData>>& arguments,
                             size_t n)
        {
            auto minmax = countArguments(arguments);
            if (n < minmax.first)
                n = 0;
            else if (n > minmax.second)
                n = minmax.second - minmax.first;
            else
                n -= minmax.first;

            std::vector<std::pair<size_t, const ArgumentData*>> result;
            for (auto& arg : arguments)
            {
                if (n == 0 || arg->minCount == arg->maxCount)
                {
                    result.emplace_back(arg->minCount, arg.get());
                }
                else if (arg->minCount + n <= arg->maxCount)
                {
                    result.emplace_back(arg->minCount + n, arg.get());
                    n = 0;
                }
                else
                {
                    result.emplace_back(arg->maxCount, arg.get());
                    n -= arg->maxCount - arg->minCount;
                }
            }
            return result;
        }
    }
    ArgumentCounter::ArgumentCounter(const std::vector<std::shared_ptr<ArgumentData>>& arguments)
        : m_Counters(makeArgumentCounters(arguments)),
          m_Iterator(m_Counters.begin())
    {}

    ArgumentCounter::ArgumentCounter(const std::vector<std::shared_ptr<ArgumentData>>& arguments,
                                     size_t argumentCount)
        : m_Counters(makeArgumentCounters(arguments, argumentCount)),
          m_Iterator(m_Counters.begin())
    {}

    const ArgumentData* ArgumentCounter::nextArgument()
    {
        while (m_Iterator != m_Counters.end() && m_Iterator->first == 0)
            ++m_Iterator;

        if (m_Iterator == m_Counters.end())
            return nullptr;

        --m_Iterator->first;
        return m_Iterator->second;
    }

    bool ArgumentCounter::isComplete() const
    {
        for (auto it = m_Iterator; it != m_Counters.end(); ++it)
        {
            if (it->first != 0)
                return false;
        }
        return true;
    }

    bool ArgumentCounter::requiresArgumentCount(
            const std::vector<std::shared_ptr<ArgumentData>>& arguments)
    {
        bool deterministic = true;
        for (auto& arg : arguments)
        {
            if (!deterministic)
                return true;
            if (arg->minCount != arg->maxCount)
                deterministic = false;
        }
        return false;
    }
}
