//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <map>
#include "ParserData.hpp"
#include "RawArg.hpp"

namespace Argos
{
    class ArgumentIterator;

    class ParserResultImpl
    {
    public:
        ParserResultImpl(std::shared_ptr<ParserData> data);

        RawArg get(const std::string& name) const;

        const std::vector<std::string>& arguments() const;
    private:
        friend ArgumentIterator;

        void addArgument(const std::string& arg);

        void addUnprocessedArgument(const std::string& arg);

        void assignValue(int valueId, const std::string& value);

        void appendValue(int valueId, const std::string& value);

        void clearValue(int valueId);

        std::multimap<int, std::string> m_Values;
        std::vector<std::string> m_Arguments;
        std::vector<std::string> m_UnprocessedArguments;
        std::shared_ptr<ParserData> m_Data;
    };
}
