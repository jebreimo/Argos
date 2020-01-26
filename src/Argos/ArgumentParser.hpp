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
#include "ArgumentData.hpp"
#include "ArgumentIterator.hpp"
#include "Option.hpp"
#include "ParserResult.hpp"

namespace Argos
{
    class ArgumentParser
    {
    public:
        ArgumentParser(int argc, char* argv[],
                       std::shared_ptr<ArgumentData> data);

        ArgumentParser(const std::vector<std::string>& args,
                       std::shared_ptr<ArgumentData> data);

        std::optional<int> next();
    private:
        int processOption(const Option& option, const std::string& flag);

        std::optional<int> nextImpl();

        void copyRemainingArgumentsToParserResult();

        size_t countArguments() const;

        std::shared_ptr<ArgumentData> m_Data;
        std::vector<std::pair<std::string_view, const Option*>> m_Options;
        ParserResult m_ParserResult;
        ArgumentIterator m_ArgumentIterator;
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
