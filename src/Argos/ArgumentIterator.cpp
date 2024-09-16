//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentIterator.hpp"

#include "ArgosThrow.hpp"
#include "ArgumentIteratorImpl.hpp"

namespace argos
{
    ArgumentIterator::ArgumentIterator(std::vector<std::string_view> args,
                                       std::shared_ptr<ParserData> parser_data)
        : m_impl(std::make_unique<ArgumentIteratorImpl>(std::move(args),
                                                        std::move(parser_data)))
    {}

    ArgumentIterator::ArgumentIterator(ArgumentIterator&& rhs) noexcept
        : m_impl(std::move(rhs.m_impl))
    {}

    ArgumentIterator::~ArgumentIterator() = default;

    ArgumentIterator&
    ArgumentIterator::operator=(ArgumentIterator&& rhs) noexcept
    {
        m_impl = std::move(rhs.m_impl);
        return *this;
    }

    bool ArgumentIterator::next(std::unique_ptr<IArgumentView>& arg,
                                std::string_view& value)
    {
        const auto res = impl().next();
        switch (std::get<0>(res))
        {
        case IteratorResultCode::ARGUMENT:
            arg = std::make_unique<ArgumentView>(
                    std::get<const ArgumentData*>(std::get<1>(res)));
            value = std::get<2>(res);
            return true;
        case IteratorResultCode::OPTION:
            arg = std::make_unique<OptionView>(
                    std::get<const OptionData*>(std::get<1>(res)));
            value = std::get<2>(res);
            return true;
        case IteratorResultCode::UNKNOWN:
            arg = {};
            value = std::get<2>(res);
            return true;
        case IteratorResultCode::DONE:
        case IteratorResultCode::ERROR:
            break;
        }
        arg = {};
        value = {};
        return false;
    }

    ParsedArguments ArgumentIterator::parsed_arguments() const
    {
        return ParsedArguments(impl().parsed_arguments());
    }

    ArgumentIteratorImpl& ArgumentIterator::impl()
    {
        if (!m_impl)
            ARGOS_THROW("This ArgumentIterator has been moved from.");
        return *m_impl;
    }

    const ArgumentIteratorImpl& ArgumentIterator::impl() const
    {
        if (!m_impl)
            ARGOS_THROW("This ArgumentIterator has been moved from.");
        return *m_impl;
    }
}
