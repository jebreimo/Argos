//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-07-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentValueIterator.hpp"

#include "Argos/ArgumentValue.hpp"

namespace argos
{
    ArgumentValueIterator::ArgumentValueIterator() = default;

    ArgumentValueIterator::ArgumentValueIterator(
        const ArgumentValueIterator::It& internal_iterator,
        std::shared_ptr<ParsedArgumentsImpl> args,
        ValueId value_id)
    : m_iterator(internal_iterator),
      m_args(std::move(args)),
      m_value_id(value_id)
    {}

    ArgumentValueIterator& ArgumentValueIterator::operator++()
    {
        ++m_iterator;
        return *this;
    }

    ArgumentValueIterator ArgumentValueIterator::operator++(int)
    {
        auto it = *this;
        ++m_iterator;
        return it;
    }

    ArgumentValue ArgumentValueIterator::operator*() const
    {
        return ArgumentValue(m_iterator->first, m_args,
                             m_value_id, m_iterator->second);
    }

    ArgumentValueIterator::It ArgumentValueIterator::internal_iterator() const
    {
        return m_iterator;
    }

    bool operator==(const ArgumentValueIterator& a, const ArgumentValueIterator& b)
    {
        return a.internal_iterator() == b.internal_iterator();
    }

    bool operator!=(const ArgumentValueIterator& a, const ArgumentValueIterator& b)
    {
        return a.internal_iterator() != b.internal_iterator();
    }
}
