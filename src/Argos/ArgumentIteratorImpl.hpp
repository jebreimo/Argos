//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <string>
#include "ArgumentCounter.hpp"
#include "ParserData.hpp"
#include "StandardOptionIterator.hpp"
#include "OptionData.hpp"
#include "ParsedArgumentsImpl.hpp"

namespace Argos
{
    class ArgumentIteratorImpl
    {
    public:
        ArgumentIteratorImpl(int argc, char* argv[],
                             std::shared_ptr<ParserData> data);

        ArgumentIteratorImpl(const std::vector<std::string>& args,
                             std::shared_ptr<ParserData> data);

        std::optional<int> next();

        std::unique_ptr<ParsedArgumentsImpl> releaseResult();

        void parseAll();
    private:
        int processOption(const OptionData& option, const std::string& flag);

        std::optional<int> nextImpl();

        void copyRemainingArgumentsToParserResult();

        size_t countArguments() const;

        std::shared_ptr<ParserData> m_Data;
        std::vector<std::pair<std::string_view, const OptionData*>> m_Options;
        ParsedArgumentsImpl m_ParserResult;
        StandardOptionIterator m_ArgumentIterator;
        std::optional<ArgumentCounter> m_ArgumentCounter;
        enum class State
        {
            ARGUMENTS_AND_OPTIONS,
            ARGUMENTS_ONLY,
            DONE,
            ERROR
        };
        State m_State = State::ARGUMENTS_AND_OPTIONS;
    };
}
