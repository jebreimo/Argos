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
        size_t findFirstOptional(const std::vector<std::unique_ptr<ArgumentData>>& arguments)
        {
            size_t result = 0;
            for (auto i = 0; i < arguments.size(); ++i)
            {
                if (arguments[i]->minCount > 0)
                    result = i + 1;
            }
            return result;
        }

        void makeArgumentCounters(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments,
                std::vector<std::pair<size_t, const ArgumentData*>>& counters,
                size_t& firstOptional)
        {
            firstOptional = findFirstOptional(arguments);
            for (size_t i = 0; i < arguments.size(); ++i)
            {
                auto& a = arguments[i];
                if (i + 1 == firstOptional && a->minCount != a->maxCount)
                {
                    counters.emplace_back(a->minCount, a.get());
                    counters.emplace_back(a->maxCount - a->minCount, a.get());
                }
                else
                {
                    counters.emplace_back(a->maxCount, a.get());
                }
            }
        }

        std::vector<std::pair<size_t, const ArgumentData*>>
        makeArgumentCounters(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments,
                size_t n)
        {
            auto minmax = ArgumentCounter::getMinMaxCount(arguments);
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

    ArgumentCounter::ArgumentCounter()
        : m_Counters()
    {}

    ArgumentCounter::ArgumentCounter(const std::vector<std::unique_ptr<ArgumentData>>& arguments)
    {
        makeArgumentCounters(arguments, m_Counters, m_FirstOptional);
    }

    ArgumentCounter::ArgumentCounter(const std::vector<std::unique_ptr<ArgumentData>>& arguments,
                                     size_t argumentCount)
        : m_Counters(makeArgumentCounters(arguments, argumentCount)),
          m_FirstOptional(m_Counters.size())
    {}

    const ArgumentData* ArgumentCounter::nextArgument()
    {
        while (m_Index != m_Counters.size() && m_Counters[m_Index].first == 0)
            ++m_Index;

        if (m_Index == m_Counters.size())
            return nullptr;

        --m_Counters[m_Index].first;
        return m_Counters[m_Index].second;
    }

    bool ArgumentCounter::isComplete() const
    {
        return m_Index >= m_FirstOptional
               || (m_Index + 1 == m_FirstOptional
                   && m_Counters[m_Index].first == 0);
    }

    std::pair<size_t, size_t>
    ArgumentCounter::getMinMaxCount(const std::vector<std::unique_ptr<ArgumentData>>& arguments)
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

    bool ArgumentCounter::requiresArgumentCount(
            const std::vector<std::unique_ptr<ArgumentData>>& arguments)
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
