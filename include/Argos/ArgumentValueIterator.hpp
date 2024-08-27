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

namespace argos
{
    class ArgumentValue;
    class ParsedArgumentsImpl;

    /**
     * @brief Iterator for the values in an instance of ArgumentValues.
     *
     * Direct use of this iterator should be avoided, it is intended to
     * be used in range-based for loops. For algorithms etc. it is recommended
     * to use the vector returned by ArgumentValues::values.
     */
    class ArgumentValueIterator
    {
    public:
        /**
         * @private
         */
        using It = std::vector<
            std::pair<std::string_view, ArgumentId>
            >::const_iterator;

        /**
         * @brief Construct an empty iterator.
         *
         * The iterator has to be assigned the result of ArgumentValues::begin
         * or ArgumentValues::end before it can be used.
         */
        ArgumentValueIterator();

        /**
         * @private
         * Only called from ArgumentValues
         */
        ArgumentValueIterator(const It& internal_iterator,
                              std::shared_ptr<ParsedArgumentsImpl> args,
                              ValueId value_id);

        /**
         * @brief Prefix increment operator.
         */
        ArgumentValueIterator& operator++();

        /**
         * @brief Postfix increment operator.
         */
        ArgumentValueIterator operator++(int) &;

        /**
         * @brief Returns the current value.
         *
         * @note The returned value is not a reference.
         */
        ArgumentValue operator*() const;

        /**
         * @private
         */
        [[nodiscard]] It internal_iterator() const;
    private:
        It m_iterator = {};
        std::shared_ptr<ParsedArgumentsImpl> m_args;
        ValueId m_value_id = {};
    };

    /**
     * @brief Returns true @a a and @a b point to the same argument.
     */
    bool operator==(const ArgumentValueIterator& a,
                    const ArgumentValueIterator& b);

    /**
     * @brief Returns false unless @a a and @a b point to the same argument.
     */
    bool operator!=(const ArgumentValueIterator& a,
                    const ArgumentValueIterator& b);
}
