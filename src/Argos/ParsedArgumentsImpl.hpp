//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <map>
#include <Argos/IArgumentView.hpp>
#include "ParserData.hpp"

namespace Argos
{
    class ArgumentIteratorImpl;

    class ParsedArgumentsImpl
    {
    public:
        ParsedArgumentsImpl(std::shared_ptr<ParserData> data);

        bool has(int valueId) const;

        const std::vector<std::string>& arguments() const;

        void addArgument(const std::string& arg);

        const std::vector<std::string>& unprocessedArguments() const;

        void addUnprocessedArgument(const std::string& arg);

        std::string_view assignValue(int valueId, const std::string& value);

        std::string_view appendValue(int valueId, const std::string& value);

        void clearValue(int valueId);

        int getValueId(std::string_view valueName) const;

        std::optional<std::string_view> getValue(int valueId) const;

        std::vector<std::string_view> getValues(int valueId) const;

        std::vector<std::unique_ptr<IArgumentView>>
        getArgumentViews(int valueId) const;

        const std::shared_ptr<ParserData>& parserData() const;

        ParserResultCode resultCode() const;

        void setResultCode(ParserResultCode resultCode);

        const OptionData* breakingOption() const;

        void setBreakingOption(const OptionData* option);

        void error(const std::string& message);

        void error(const std::string& message, int valueId);
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
