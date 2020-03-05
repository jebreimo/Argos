//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentIterator.hpp"

#include "Argos/ArgosException.hpp"
#include "ArgumentIteratorImpl.hpp"

namespace Argos
{
    ArgumentIterator::ArgumentIterator(std::vector<std::string_view> args,
                                       std::shared_ptr<ParserData> parserData)
        : m_Impl(std::make_unique<ArgumentIteratorImpl>(move(args),
                                                        move(parserData)))
    {}

    ArgumentIterator::ArgumentIterator(ArgumentIterator&& rhs) noexcept
        : m_Impl(move(rhs.m_Impl))
    {}

    ArgumentIterator::~ArgumentIterator() = default;

    ArgumentIterator& ArgumentIterator::operator=(ArgumentIterator&& rhs) noexcept
    {
        m_Impl = move(rhs.m_Impl);
        return *this;
    }

    bool ArgumentIterator::next(std::unique_ptr<IArgumentView>& arg,
                                std::string_view& value)
    {
        auto res = m_Impl->next();
        switch (std::get<0>(res))
        {
        case IteratorResultCode::ARGUMENT:
            arg = std::make_unique<ArgumentView>(
                    static_cast<const ArgumentData*>(std::get<1>(res)));
            value = std::get<2>(res);
            return true;
        case IteratorResultCode::OPTION:
            arg = std::make_unique<OptionView>(
                    static_cast<const OptionData*>(std::get<1>(res)));
            value = std::get<2>(res);
            return true;
        case IteratorResultCode::DONE:
        case IteratorResultCode::UNKNOWN:
        case IteratorResultCode::ERROR:
            arg = {};
            value = {};
            return false;
        }
    }

    ParsedArguments ArgumentIterator::parsedArguments() const
    {
        return ParsedArguments(impl().parsedArguments());
    }

    ArgumentIteratorImpl& ArgumentIterator::impl()
    {
        if (!m_Impl)
            ARGOS_THROW("This ArgumentIterator has been moved from.");
        return *m_Impl;
    }

    const ArgumentIteratorImpl& ArgumentIterator::impl() const
    {
        if (!m_Impl)
            ARGOS_THROW("This ArgumentIterator has been moved from.");
        return *m_Impl;
    }
}
