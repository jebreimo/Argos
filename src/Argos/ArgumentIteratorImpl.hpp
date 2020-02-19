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

    using IteratorResult = std::pair<IteratorResultCode, const void*>;

    class ArgumentIteratorImpl
    {
    public:
        ArgumentIteratorImpl(int argc, char* argv[],
                             std::shared_ptr<ParserData> data);

        ArgumentIteratorImpl(const std::vector<std::string>& args,
                             std::shared_ptr<ParserData> data);

        IteratorResult next();

        static std::unique_ptr<ParsedArgumentsImpl>
        parse(int argc, char* argv[], const std::shared_ptr<ParserData>& data);
    private:
        int processOption(const OptionData& option, const std::string& flag);

        IteratorResult doNext();

        void copyRemainingArgumentsToParserResult();

        size_t countArguments() const;

        void postProcessArguments();

        std::shared_ptr<ParserData> m_Data;
        std::vector<std::pair<std::string_view, const OptionData*>> m_Options;
        std::unique_ptr<ParsedArgumentsImpl> m_ParsedArgs;
        std::unique_ptr<IOptionIterator> m_Iterator;
        std::optional<ArgumentCounter> m_ArgumentCounter;
        enum class State
        {
            ARGUMENTS_AND_OPTIONS,
            ARGUMENTS_ONLY,
            DONE,
            ERROR
        };
        State m_State = State::ARGUMENTS_AND_OPTIONS;
        std::vector<std::string> m_InernalArgs;
    };
}
