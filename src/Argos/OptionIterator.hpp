//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-18.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "IOptionIterator.hpp"

namespace argos
{
    class OptionIterator : public IOptionIterator
    {
    public:
        OptionIterator();

        explicit OptionIterator(std::vector<std::string_view> args,
                                char prefix);

        OptionIterator(const OptionIterator& rhs);

        std::optional<std::string> next() final;

        std::optional<std::string> next_value() final;

        [[nodiscard]] std::string_view current() const final;

        [[nodiscard]] std::vector<std::string_view> remaining_arguments() const final;

        [[nodiscard]] OptionIterator* clone() const final;
    private:
        std::vector<std::string_view> m_args;
        std::vector<std::string_view>::const_iterator m_args_it;
        size_t m_pos = 0;
        char m_prefix = '-';
    };
}
