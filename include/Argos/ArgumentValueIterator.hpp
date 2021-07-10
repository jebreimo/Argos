//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-07-06.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iterator>
#include <memory>
#include <string_view>
#include <vector>
#include "Enums.hpp"

/**
 * @file
 * Defines ArgumentValueIterator, an input iterator suitable for range-based
 * for loops over the values in instances of ArgumentValues.
 */

namespace Argos
{
    class ArgumentValue;
    class ParsedArgumentsImpl;

    /**
     * @brief Iterator for the values in an instance of ArgumentValues.
     */
    class ArgumentValueIterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using pointer = void;
        using reference = ArgumentValue;
        using value_type = ArgumentValue;
        using difference_type = void;

        using It = typename std::vector<std::pair<std::string_view, ArgumentId>>::const_iterator;

        ArgumentValueIterator();

        ArgumentValueIterator(const It& internalIterator,
                              std::shared_ptr<ParsedArgumentsImpl> args,
                              ValueId valueId);

        ArgumentValueIterator& operator++();

        ArgumentValueIterator operator++(int);

        ArgumentValue operator*() const;

        It internalIterator() const;
    private:
        It m_Iterator = {};
        std::shared_ptr<ParsedArgumentsImpl> m_Args;
        ValueId m_ValueId = {};
    };

    bool operator==(const ArgumentValueIterator& a,
                    const ArgumentValueIterator& b);

    bool operator!=(const ArgumentValueIterator& a,
                    const ArgumentValueIterator& b);
}
