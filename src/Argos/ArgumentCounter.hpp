//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <vector>
#include "ArgumentData.hpp"

namespace Argos
{
    class ArgumentCounter
    {
    public:
        explicit ArgumentCounter(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments);

        ArgumentCounter(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments,
                size_t argumentCount);

        const ArgumentData* nextArgument();

        bool isComplete() const;

        static std::pair<size_t, size_t> getMinMaxCount(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments);

        static bool requiresArgumentCount(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments);
    private:
        using Counter = std::pair<size_t, const ArgumentData*>;
        std::vector<Counter> m_Counters;
        std::vector<Counter>::iterator m_Iterator;
    };
}
