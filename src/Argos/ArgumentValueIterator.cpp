//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-07-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentValueIterator.hpp"

#include "Argos/ArgumentValue.hpp"

namespace Argos
{
    ArgumentValueIterator::ArgumentValueIterator() = default;

    ArgumentValueIterator::ArgumentValueIterator(
        const ArgumentValueIterator::It& internalIterator,
        std::shared_ptr<ParsedArgumentsImpl> args,
        ValueId valueId)
    : m_Iterator(internalIterator),
      m_Args(std::move(args)),
      m_ValueId(valueId)
    {}

    ArgumentValueIterator& ArgumentValueIterator::operator++()
    {
        ++m_Iterator;
        return *this;
    }

    ArgumentValueIterator ArgumentValueIterator::operator++(int)
    {
        auto it = *this;
        ++m_Iterator;
        return it;
    }

    ArgumentValue ArgumentValueIterator::operator*() const
    {
        return ArgumentValue(m_Iterator->first, m_Args,
                             m_ValueId, m_Iterator->second);
    }

    ArgumentValueIterator::It ArgumentValueIterator::internalIterator() const
    {
        return Argos::ArgumentValueIterator::It();
    }

    bool operator==(const ArgumentValueIterator& a, const ArgumentValueIterator& b)
    {
        return a.internalIterator() == b.internalIterator();
    }

    bool operator!=(const ArgumentValueIterator& a, const ArgumentValueIterator& b)
    {
        return a.internalIterator() != b.internalIterator();
    }
}
