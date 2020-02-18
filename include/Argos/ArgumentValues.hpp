//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include <string>
#include <vector>
#include "IArgumentView.hpp"

namespace Argos
{
    class ParsedArgumentsImpl;

    class ArgumentValues
    {
    public:
        ArgumentValues(std::vector<std::string_view> values,
                       std::shared_ptr<ParsedArgumentsImpl> args,
                       int valueId);

        ArgumentValues(const ArgumentValues&);

        ArgumentValues(ArgumentValues&&) noexcept;

        ~ArgumentValues();

        ArgumentValues& operator=(const ArgumentValues&);

        ArgumentValues& operator=(ArgumentValues&&) noexcept;

        std::vector<std::unique_ptr<IArgumentView>> arguments() const;

        void error(const std::string& message) const;

        bool empty() const;

        size_t size() const;

        const std::vector<std::string_view>& values() const;
    private:
        std::vector<std::string_view> m_Values;
        std::shared_ptr<ParsedArgumentsImpl> m_Args;
        int m_ValueId;
    };
}
