//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <string>
#include <variant>
#include "ArgumentCounter.hpp"
#include "ParserData.hpp"
#include "StandardOptionIterator.hpp"
#include "OptionData.hpp"
#include "OptionIteratorWrapper.hpp"
#include "ParsedArgumentsImpl.hpp"

namespace argos
{
    enum class IteratorResultCode
    {
        ARGUMENT,
        OPTION,
        DONE,
        UNKNOWN,
        ERROR
    };

    using IteratorResultData = std::variant<
        std::monostate,
        const ArgumentData*,
        const OptionData*>;

    using IteratorResult = std::tuple<
        IteratorResultCode,
        IteratorResultData,
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

        [[nodiscard]] const std::shared_ptr<ParsedArgumentsImpl>&
        parsed_arguments() const;

    private:
        enum class OptionResult
        {
            NORMAL,
            LAST_ARGUMENT,
            STOP,
            EXIT,
            ERROR
        };

        std::pair<OptionResult, std::string_view>
        process_option(const OptionData& opt, const std::string& flag);

        IteratorResult process_option(const std::string& flag);

        IteratorResult process_argument(const std::string& name);

        IteratorResult process_command(const std::string& name);

        void copy_remaining_arguments_to_parser_result();

        [[nodiscard]] size_t count_arguments() const;

        bool check_argument_and_option_counts();

        void error(const std::string& message = {});

        struct Something
        {
            const CommandData* command;
            std::shared_ptr<ParsedArgumentsImpl> parsed_args;
            ArgumentCounter argument_counter;

            Something(const CommandData* cmd,
                      std::span<std::string_view> args,
                      std::shared_ptr<ParserData> data);
        };

        std::vector<Something> m_somethings;

        std::shared_ptr<ParserData> m_data;
        const CommandData* m_command = nullptr;
        std::shared_ptr<ParsedArgumentsImpl> m_parsed_args;
        // std::unique_ptr<IOptionIterator> m_iterator;
        OptionIteratorWrapper m_iterator;
        ArgumentCounter m_argument_counter;

        enum class State
        {
            ARGUMENTS_AND_OPTIONS,
            ARGUMENTS_ONLY,
            DONE,
            ERROR
        };

        State m_state = State::ARGUMENTS_AND_OPTIONS;
    };
}
