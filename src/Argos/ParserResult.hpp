//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <map>
#include "RawArg.hpp"

namespace Argos
{
    class ParserResult
    {
    public:
        //ParserResult(std::shared_ptr<ArgParserImpl> argParser);

        RawArg get(const std::string& name) const;
    private:
        std::map<int, std::string_view> m_Values;
        //std::shared_ptr<ArgParserImpl> m_ArgParser;
    };
}
