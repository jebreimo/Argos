//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include <vector>
#include "ArgumentData.hpp"

namespace argos
{
    class ArgumentCounter
    {
    public:
        ArgumentCounter();

        explicit ArgumentCounter(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments);

        ArgumentCounter(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments,
                size_t argument_count);

        const ArgumentData* next_argument();

        size_t count() const;

        bool is_complete() const;

        static std::pair<size_t, size_t> get_min_max_count(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments);

        static bool requires_argument_count(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments);
    private:
        using Counter = std::pair<size_t, const ArgumentData*>;
        std::vector<Counter> m_counters;
        size_t m_index = 0;
        size_t m_first_optional = 0;
        size_t m_counter = 0;
    };
}
