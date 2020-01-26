//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Argument.hpp"
#include "ArgumentIteratorImpl.hpp"
#include "Option.hpp"

namespace Argos
{
    class ParserBuilder
    {
    public:
        Argument addArgument(const std::string& name);

        Option addOption(const std::string& flag);

        ArgumentIteratorImpl makeParser() const;
    private:
        std::vector<ArgumentData> m_Arguments;
        std::vector<OptionData> m_Options;
    };
}
