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
    class ArgumentIteratorImpl;

    class ParsedArgumentsImpl
    {
    public:
        ParsedArgumentsImpl(std::shared_ptr<ParserData> data);

        bool has(const std::string& name) const;

        bool has(int valueId) const;

        RawArg get(const std::string& name) const;

        const std::vector<std::string>& arguments() const;

        void addArgument(const std::string& arg);

        void addUnprocessedArgument(const std::string& arg);

        void assignValue(int valueId, const std::string& value);

        void appendValue(int valueId, const std::string& value);

        void clearValue(int valueId);

        ParserResultCode resultCode() const;

        void setResultCode(ParserResultCode resultCode);

        const OptionData* specialOption() const;

        void setSpecialOption(const OptionData* option);
    private:
        std::multimap<int, std::string> m_Values;
        std::vector<std::pair<std::string_view, int>> m_ValueIds;
        std::vector<std::string> m_Arguments;
        std::vector<std::string> m_UnprocessedArguments;
        std::shared_ptr<ParserData> m_Data;
        ParserResultCode m_ResultCode = ParserResultCode::NONE;
        const OptionData* m_SpecialOption = nullptr;
    };
}
