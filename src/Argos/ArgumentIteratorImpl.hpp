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
    enum class IteratorResultCode
    {
        ARGUMENT,
        OPTION,
        DONE,
        UNKNOWN,
        ERROR
    };

    using IteratorResult = std::tuple<
            IteratorResultCode,
            const void*,
            std::string_view>;

    class ArgumentIteratorImpl
    {
    public:
        ArgumentIteratorImpl(std::vector<std::string_view> args,
                             std::shared_ptr<ParserData> data);

        IteratorResult next();

        static std::shared_ptr<ParsedArgumentsImpl>
        parse(std::vector<std::string_view> args,
              const std::shared_ptr<ParserData>& data);

        const std::shared_ptr<ParsedArgumentsImpl>& parsedArguments() const;
    private:
        enum class OptionResult
        {
            NORMAL,
            LAST_ARGUMENT,
            STOP,
            HELP,
            ERROR
        };

        std::pair<OptionResult, std::string_view>
        processOption(const OptionData& option, const std::string& flag);

        IteratorResult doNext();

        void copyRemainingArgumentsToParserResult();

        size_t countArguments() const;

        bool checkArgumentAndOptionCounts();

        void error(const std::string& message = {});

        std::shared_ptr<ParserData> m_Data;
        std::vector<std::pair<std::string_view, const OptionData*>> m_Options;
        std::shared_ptr<ParsedArgumentsImpl> m_ParsedArgs;
        std::unique_ptr<IOptionIterator> m_Iterator;
        ArgumentCounter m_ArgumentCounter;
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
