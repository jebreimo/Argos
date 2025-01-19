//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-22.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include <vector>
#include "CommandData.hpp"

namespace argos
{
    class ArgumentCounter
    {
    public:
        ArgumentCounter();

        explicit ArgumentCounter(const CommandData& command);

        ArgumentCounter(const CommandData& command,
                        size_t argument_count,
                        size_t initial_count = 0);

        const ArgumentData* next_argument();

        [[nodiscard]] size_t count() const;

        [[nodiscard]] bool is_complete() const;

        static std::pair<size_t, size_t>
        get_min_max_count(const CommandData& command);

        static bool requires_argument_count(const CommandData& command);

    private:
        using Counter = std::pair<size_t, const ArgumentData*>;
        std::vector<Counter> m_counters;
        size_t m_index = 0;
        size_t m_first_optional = 0;
        size_t m_counter = 0;
    };
}
