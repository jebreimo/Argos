//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <map>
#include "ArgumentData.hpp"
#include "RawArg.hpp"

namespace Argos
{
    class ArgumentParser;

    class ParserResult
    {
    public:
        ParserResult(std::shared_ptr<ArgumentData> data);

        RawArg get(const std::string& name) const;

        const std::vector<std::string>& arguments() const;
    private:
        friend ArgumentParser;

        void addArgument(const std::string& arg);

        void addUnprocessedArgument(const std::string& arg);

        std::map<int, std::string_view> m_Values;
        std::vector<std::string> m_Arguments;
        std::vector<std::string> m_UnprocessedArguments;
        std::shared_ptr<ArgumentData> m_Data;
    };
}
