//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ArgumentParser.hpp"

#include <cassert>
#include "ArgosException.hpp"
#include "HelpTextWriter.hpp"
#include "StringUtilities.hpp"

namespace Argos
{
    namespace
    {
        std::vector<std::string_view> makeStringViewVector(
                int count, char** strings, bool ignoreFirst)
        {
            std::vector<std::string_view> result;
            for (int i = ignoreFirst ? 1 : 0; i < count; ++i)
                result.emplace_back(strings[i]);
            return result;
        }

        std::vector<std::string_view> makeStringViewVector(
                const std::vector<std::string>& strings)
        {
            std::vector<std::string_view> result;
            for (auto& s : strings)
                result.emplace_back(s);
            return result;
        }

        using OptionTable = std::vector<std::pair<std::string_view, const Option*>>;

        OptionTable makeOptionIndex(const std::vector<Option>& options,
                                    bool caseInsensitive)
        {
            OptionTable index;
            for (auto& option : options)
            {
                for (auto& flag : option.flags)
                    index.emplace_back(flag, &option);
            }
            if (caseInsensitive)
            {
                std::sort(index.begin(), index.end(), [](auto& a, auto& b)
                {
                    return isLessCI(a.first, b.first);
                });
            }
            else
            {
                std::sort(index.begin(), index.end(), [](auto& a, auto& b)
                {
                    return a.first < b.first;
                });
            }
            return index;
        }

        OptionTable::const_iterator findOptionCS(const OptionTable& options,
                                                 std::string_view arg)
        {
            OptionTable::value_type key = {arg, nullptr};
            return std::lower_bound(
                    options.begin(), options.end(),
                    key,
                    [&](auto& a, auto& b) {return a.first < b.first;});
        }

        OptionTable::const_iterator findOptionCI(const OptionTable& options,
                                                 std::string_view arg)
        {
            OptionTable::value_type key = {arg, nullptr};
            return std::lower_bound(
                    options.begin(), options.end(),
                    key,
                    [&](auto& a, auto& b) {return isLessCI(a.first, b.first);});
        }

        const Option* findOption(const OptionTable& options,
                                 std::string_view arg,
                                 bool allowAbbreviations,
                                 bool caseInsensitive)
        {
            OptionTable::value_type key = {arg, nullptr};
            auto it = caseInsensitive ? findOptionCI(options, arg)
                                      : findOptionCS(options, arg);
            bool equalChar = false;
            if (it == options.end())
            {
                if (arg.size() > 2 && arg.back() == '=')
                    arg = arg.substr(arg.size() - 1);
                it = caseInsensitive ? findOptionCI(options, arg)
                                     : findOptionCS(options, arg);
                equalChar = true;
            }
            if (it == options.end())
                return nullptr;
            if (equalChar && it->second->argument.empty())
                return nullptr;
            if (it->first == arg)
                return it->second;
            if (caseInsensitive && areEqualCI(it->first, arg))
                return it->second;
            if (!allowAbbreviations)
                return nullptr;
            if (!startsWith(it->first, arg, caseInsensitive))
                return nullptr;
            auto nxt = next(it);
            if (nxt != options.end()
                && startsWith(nxt->first, arg, caseInsensitive))
                return nullptr;
            return it->second;
        }

        bool isOption(const std::string& s, OptionStyle style)
        {
            if (s.size() < 2)
                return false;

            if (style == OptionStyle::WINDOWS)
                return s[0] == '/';
            else
                return s[0] == '-';
        }
    }

    ArgumentParser::ArgumentParser(int argc, char** argv, std::shared_ptr<ArgumentData> data)
            : m_Data(move(data)),
              m_Options(makeOptionIndex(m_Data->options, m_Data->caseInsensitive)),
              m_ParserResult(m_Data),
              m_ArgumentIterator(makeStringViewVector(argc, argv, true))
    {
        if (!ArgumentCounter::requiresArgumentCount(m_Data->arguments))
            m_ArgumentCounter.emplace(m_Data->arguments);
    }

    ArgumentParser::ArgumentParser(const std::vector<std::string>& args,
                                   std::shared_ptr<ArgumentData> data)
            : m_Data(move(data)),
              m_Options(makeOptionIndex(m_Data->options, m_Data->caseInsensitive)),
              m_ParserResult(m_Data),
              m_ArgumentIterator(makeStringViewVector(args))
    {
        if (!ArgumentCounter::requiresArgumentCount(m_Data->arguments))
            m_ArgumentCounter.emplace(m_Data->arguments);
    }

    std::optional<int> ArgumentParser::next()
    {
        if (!m_ArgumentCounter)
        {
            m_ArgumentCounter = ArgumentCounter(m_Data->arguments,
                                                countArguments());
        }
        return nextImpl();
    }

    int ArgumentParser::processOption(const Option& option, const std::string& flag)
    {
        switch (option.operation)
        {
        case ArgumentOperation::ASSIGN:
            if (!option.value.empty())
            {
                m_ParserResult.assignValue(option.valueId_, option.value);
            }
            else if (auto value = m_ArgumentIterator.nextValue(); value)
            {
                m_ParserResult.assignValue(option.valueId_, *value);
            }
            else
            {
                m_State = State::ERROR;
                HelpTextWriter(m_Data).writeErrorMessage(
                        option, flag + ": no value given.");
                return 2;
            }
            break;
        case ArgumentOperation::APPEND:
            if (!option.value.empty())
            {
                m_ParserResult.appendValue(option.valueId_, option.value);
            }
            else if (auto value = m_ArgumentIterator.nextValue(); value)
            {
                m_ParserResult.appendValue(option.valueId_, *value);
            }
            else
            {
                m_State = State::ERROR;
                HelpTextWriter(m_Data).writeErrorMessage(
                        option, flag + ": no value given.");
                return 2;
            }
            break;
        case ArgumentOperation::CLEAR:
            m_ParserResult.clearValue(option.valueId_);
            break;
        case ArgumentOperation::NONE:
            break;
        }

        switch (option.optionType)
        {
        case OptionType::NORMAL:
            return 0;
        case OptionType::HELP:
            HelpTextWriter(m_Data).writeHelpText();
            m_State = State::DONE;
            return 1;
        case OptionType::BREAK:
            m_State = State::DONE;
            return 0;
        case OptionType::FINAL:
            m_State = State::ARGUMENTS_ONLY;
            return 0;
        }
    }

    std::optional<int> ArgumentParser::nextImpl()
    {
        if (m_State == State::ERROR)
            ARGOS_THROW("next() called after error.");
        if (m_State == State::DONE)
            return {};

        auto arg = m_State == State::ARGUMENTS_AND_OPTIONS
                   ? m_ArgumentIterator.next()
                   : m_ArgumentIterator.nextValue();
        if (!arg)
            return {};

        if (m_State == State::ARGUMENTS_AND_OPTIONS)
        {
            auto option = findOption(m_Options, *arg,
                                     m_Data->allowAbbreviatedOptions,
                                     m_Data->caseInsensitive);
            if (option)
            {
                switch (processOption(*option, *arg))
                {
                case 1:
                    if (m_Data->autoExit)
                        exit(0);
                    copyRemainingArgumentsToParserResult();
                    return option->id;
                case 2:
                    if (m_Data->autoExit)
                        exit(1);
                    copyRemainingArgumentsToParserResult();
                    return {};
                default:
                    return option->id;
                }
            }
            else if (isOption(*arg, m_Data->optionStyle))
            {
                if (m_Data->ignoreUndefinedArguments && *arg == m_ArgumentIterator.current())
                {
                    m_ParserResult.addUnprocessedArgument(*arg);
                }
                else
                {
                    HelpTextWriter(m_Data).writeErrorMessage(
                            "Invalid option: " + std::string(m_ArgumentIterator.current()));
                    m_State = State::ERROR;
                    if (m_Data->autoExit)
                        exit(1);
                    copyRemainingArgumentsToParserResult();
                    return {};
                }
            }
        }
        else if (m_ArgumentCounter)
        {
            if (auto argument = m_ArgumentCounter->nextArgument())
            {
                m_ParserResult.addArgument(*arg);
                m_ParserResult.assignValue(argument->valueId_, *arg);
                return argument->valueId_;
            }
            else if (m_Data->ignoreUndefinedArguments)
            {
                m_ParserResult.addUnprocessedArgument(*arg);
                return {};
            }
            else
            {
                HelpTextWriter(m_Data).writeErrorMessage(
                        "Too many arguments, starting with " + *arg);
                m_State = State::ERROR;
                if (m_Data->autoExit)
                    exit(1);
                copyRemainingArgumentsToParserResult();
                return {};
            }
        }
        else
        {
            m_ParserResult.addArgument(*arg);
            return 0;
        }
        return {};
    }

    void ArgumentParser::copyRemainingArgumentsToParserResult()
    {
        for (auto str : m_ArgumentIterator.remainingArguments())
            m_ParserResult.addUnprocessedArgument(std::string(str));
    }

    size_t ArgumentParser::countArguments() const
    {
        size_t result = 0;
        ArgumentIterator it = m_ArgumentIterator;
        bool argumentsOnly = false;
        for (auto arg = it.next(); arg && !argumentsOnly; arg = it.next())
        {
            auto option = findOption(m_Options, *arg,
                                     m_Data->allowAbbreviatedOptions,
                                     m_Data->caseInsensitive);
            if (option)
            {
                if (!option->argument.empty())
                    it.nextValue();
                switch (option->optionType)
                {
                case OptionType::HELP:
                case OptionType::BREAK:
                    return result;
                case OptionType::FINAL:
                    argumentsOnly = true;
                    break;
                default:
                    break;
                }
            }
            else if (!isOption(*arg, m_Data->optionStyle))
            {
                ++result;
            }
        }

        for (auto arg = it.next(); arg; arg = it.next())
            ++result;
        return result;
    }
}
