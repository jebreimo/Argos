//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <map>
#include "Argos/IArgumentView.hpp"
#include "ParserData.hpp"

namespace Argos
{
    class ArgumentIteratorImpl;

    class ParsedArgumentsImpl
    {
    public:
        ParsedArgumentsImpl(std::shared_ptr<ParserData> data);

        bool has(ValueId valueId) const;

        const std::vector<std::string>& unprocessedArguments() const;

        void addUnprocessedArgument(const std::string& arg);

        std::string_view assignValue(ValueId valueId, const std::string& value, ArgumentId argumentId);

        std::string_view appendValue(ValueId valueId, const std::string& value, ArgumentId argumentId);

        void clearValue(ValueId valueId);

        ValueId getValueId(std::string_view valueName) const;

        std::optional<std::pair<std::string_view, ArgumentId>> getValue(ValueId valueId) const;

        std::vector<std::pair<std::string_view, ArgumentId>> getValues(ValueId valueId) const;

        std::vector<std::unique_ptr<IArgumentView>>
        getArgumentViews(ValueId valueId) const;

        std::unique_ptr<IArgumentView>
        getArgumentView(ArgumentId argumentId) const;

        const std::shared_ptr<ParserData>& parserData() const;

        ParserResultCode resultCode() const;

        void setResultCode(ParserResultCode resultCode);

        const OptionData* breakingOption() const;

        void setBreakingOption(const OptionData* option);

        void error(const std::string& message);

        void error(const std::string& message, ArgumentId argumentId);
    private:
        std::multimap<ValueId, std::pair<std::string, ArgumentId>> m_Values;
        std::vector<std::tuple<std::string_view, ValueId, ArgumentId>> m_Ids;
        std::vector<std::string> m_UnprocessedArguments;
        std::shared_ptr<ParserData> m_Data;
        ParserResultCode m_ResultCode = ParserResultCode::NONE;
        const OptionData* m_SpecialOption = nullptr;
    };
}
