//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "IOptionIterator.hpp"

namespace argos
{
    class StandardOptionIterator : public IOptionIterator
    {
    public:
        StandardOptionIterator();

        explicit StandardOptionIterator(std::vector<std::string_view> args);

        StandardOptionIterator(const StandardOptionIterator& rhs);

        std::optional<std::string> next() final;

        std::optional<std::string> next_value() final;

        std::string_view current() const final;

        std::vector<std::string_view> remaining_arguments() const final;

        IOptionIterator* clone() const final;
    private:
        std::vector<std::string_view> m_args;
        std::vector<std::string_view>::const_iterator m_args_it;
        size_t m_pos = 0;
    };
}
