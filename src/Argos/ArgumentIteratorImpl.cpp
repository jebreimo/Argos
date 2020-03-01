//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ArgumentIteratorImpl.hpp"

#include <cassert>
#include "Argos/ArgosException.hpp"
#include "HelpWriter.hpp"
#include "StringUtilities.hpp"
#include "OptionIterator.hpp"

namespace Argos
{
    namespace
    {
        using OptionTable = std::vector<std::pair<std::string_view, const OptionData*>>;

        OptionTable makeOptionIndex(
                const std::vector<std::unique_ptr<OptionData>>& options,
                bool caseInsensitive)
        {
            OptionTable index;
            for (auto& option : options)
            {
                for (auto& flag : option->flags)
                    index.emplace_back(flag, option.get());
            }
            OptionTable::iterator it;
            if (caseInsensitive)
            {
                sort(index.begin(), index.end(), [](auto& a, auto& b)
                {
                    return isLessCI(a.first, b.first);
                });
                it = adjacent_find(index.begin(), index.end(),
                                   [](auto& a, auto& b)
                {
                    return areEqualCI(a.first, b.first);
                });
            }
            else
            {
                sort(index.begin(), index.end(), [](auto& a, auto& b)
                {
                    return a.first < b.first;
                });
                it = adjacent_find(index.begin(), index.end(),
                                   [](auto& a, auto& b)
                {
                    return a.first == b.first;
                });
            }
            if (it == index.end())
                return index;

            if (it->first == next(it)->first)
            {
                ARGOS_THROW("Multiple definitions of flag "
                            + std::string(it->first));
            }
            else
            {
                ARGOS_THROW("Conflicting flags: " + std::string(it->first)
                            + " and " + std::string(next(it)->first));
            }
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

        const OptionData* findOptionImpl(const OptionTable& options,
                                         std::string_view arg,
                                         bool allowAbbreviations,
                                         bool caseInsensitive)
        {
            auto it = caseInsensitive ? findOptionCI(options, arg)
                                      : findOptionCS(options, arg);
            if (it == options.end())
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

        const OptionData* findOption(const OptionTable& options,
                                     std::string_view arg,
                                     bool allowAbbreviations,
                                     bool caseInsensitive)
        {
            auto opt = findOptionImpl(options, arg, allowAbbreviations,
                                      caseInsensitive);
            if (opt == nullptr && arg.size() > 2 && arg.back() == '=')
            {
                arg = arg.substr(0, arg.size() - 1);
                opt = findOptionImpl(options, arg, allowAbbreviations,
                                     caseInsensitive);
                if (opt && opt->argument.empty())
                    opt = nullptr;
            }
            return opt;
        }

        bool isOption(const std::string& s, OptionStyle style)
        {
            if (s.size() < 2)
                return false;

            return s[0] == (style == OptionStyle::SLASH ? '/' : '-');
        }

        std::unique_ptr<IOptionIterator> makeOptionIterator(
                OptionStyle style,
                std::vector<std::string_view> args)
        {
            switch (style)
            {
            case OptionStyle::STANDARD:
                return std::make_unique<StandardOptionIterator>(move(args));
            case OptionStyle::SLASH:
                return std::make_unique<OptionIterator>(move(args), '/');
            case OptionStyle::DASH:
                return std::make_unique<OptionIterator>(move(args), '-');
            }
        }
    }

    ArgumentIteratorImpl::ArgumentIteratorImpl(std::vector<std::string_view> args,
                                               std::shared_ptr<ParserData> data)
        : m_Data(move(data)),
          m_Options(makeOptionIndex(m_Data->options, m_Data->parserSettings.caseInsensitive)),
          m_ParsedArgs(std::make_shared<ParsedArgumentsImpl>(m_Data)),
          m_Iterator(makeOptionIterator(m_Data->parserSettings.optionStyle, move(args)))
    {
        if (!ArgumentCounter::requiresArgumentCount(m_Data->arguments))
            m_ArgumentCounter = ArgumentCounter(m_Data->arguments);
        else
            m_ArgumentCounter = ArgumentCounter(m_Data->arguments,
                                                countArguments());
    }

    IteratorResult ArgumentIteratorImpl::next()
    {
        auto result = doNext();
        while (std::get<0>(result) == IteratorResultCode::UNKNOWN)
            result = doNext();
        return result;
    }

    std::shared_ptr<ParsedArgumentsImpl>
    ArgumentIteratorImpl::parse(std::vector<std::string_view> args,
                                const std::shared_ptr<ParserData>& data)
    {
        ArgumentIteratorImpl iterator(move(args), data);
        while (true)
        {
            auto code = std::get<0>(iterator.doNext());
            if (code == IteratorResultCode::ERROR
                || code == IteratorResultCode::DONE)
            {
                break;
            }
        }
        return iterator.m_ParsedArgs;
    }

    const std::shared_ptr<ParsedArgumentsImpl>& ArgumentIteratorImpl::parsedArguments() const
    {
        return m_ParsedArgs;
    }

    std::pair<ArgumentIteratorImpl::OptionResult, std::string_view>
    ArgumentIteratorImpl::processOption(const OptionData& option, const std::string& flag)
    {
        std::string_view arg;
        switch (option.operation)
        {
        case OptionOperation::ASSIGN:
            if (!option.value.empty())
            {
                m_ParsedArgs->assignValue(option.valueId_, option.value);
            }
            else if (auto value = m_Iterator->nextValue(); value)
            {
                arg = m_ParsedArgs->assignValue(option.valueId_, *value);
            }
            else
            {
                error(flag + ": no value given.");
                return {OptionResult::ERROR, {}};
            }
            break;
        case OptionOperation::APPEND:
            if (!option.value.empty())
            {
                m_ParsedArgs->appendValue(option.valueId_, option.value);
            }
            else if (auto value = m_Iterator->nextValue(); value)
            {
                arg = m_ParsedArgs->appendValue(option.valueId_, *value);
            }
            else
            {
                error(flag + ": no value given.");
                return {OptionResult::ERROR, {}};
            }
            break;
        case OptionOperation::CLEAR:
            m_ParsedArgs->clearValue(option.valueId_);
            break;
        case OptionOperation::NONE:
            break;
        }

        if (option.callback
            && !option.callback(OptionView(&option), arg,
                                ParsedArgumentsBuilder(m_ParsedArgs.get())))
        {
            error();
            return {OptionResult::ERROR, {}};
        }

        switch (option.type)
        {
        case OptionType::NORMAL:
            return {OptionResult::NORMAL, arg};
        case OptionType::HELP:
            HelpWriter().writeHelpText(*m_Data);
            m_State = State::DONE;
            m_ParsedArgs->setBreakingOption(&option);
            return {OptionResult::HELP, arg};
        case OptionType::STOP:
            m_State = State::DONE;
            m_ParsedArgs->setBreakingOption(&option);
            return {OptionResult::STOP, arg};
        case OptionType::LAST_ARGUMENT:
            m_State = State::DONE;
            return {OptionResult::LAST_ARGUMENT, arg};
        case OptionType::LAST_OPTION:
            m_State = State::ARGUMENTS_ONLY;
            return {OptionResult::NORMAL, arg};
        }
    }

    IteratorResult ArgumentIteratorImpl::doNext()
    {
        if (m_State == State::ERROR)
            ARGOS_THROW("next() called after error.");
        if (m_State == State::DONE)
            return {IteratorResultCode::DONE, nullptr, {}};

        auto arg = m_State == State::ARGUMENTS_AND_OPTIONS
                   ? m_Iterator->next()
                   : m_Iterator->nextValue();
        if (!arg)
        {
            if (checkArgumentCounter())
                return {IteratorResultCode::DONE, nullptr, {}};
            else
                return {IteratorResultCode::ERROR, nullptr, {}};
        }

        if (m_State == State::ARGUMENTS_AND_OPTIONS
            && isOption(*arg, m_Data->parserSettings.optionStyle))
        {
            auto option = findOption(m_Options, *arg,
                                     m_Data->parserSettings.allowAbbreviatedOptions,
                                     m_Data->parserSettings.caseInsensitive);
            if (option)
            {
                auto optRes = processOption(*option, *arg);
                switch (optRes.first)
                {
                case OptionResult::HELP:
                    if (m_Data->parserSettings.autoExit)
                        exit(0);
                    copyRemainingArgumentsToParserResult();
                    return {IteratorResultCode::OPTION, option, optRes.second};
                case OptionResult::ERROR:
                    return {IteratorResultCode::ERROR, option, {}};
                case OptionResult::LAST_ARGUMENT:
                    if (!checkArgumentCounter())
                        return {IteratorResultCode::ERROR, nullptr, {}};
                    [[fallthrough]];
                case OptionResult::STOP:
                    copyRemainingArgumentsToParserResult();
                    [[fallthrough]];
                default:
                    return {IteratorResultCode::OPTION, option, optRes.second};
                }
            }
            else
            {
                if (m_Data->parserSettings.ignoreUndefinedOptions
                    && startsWith(m_Iterator->current(), *arg))
                {
                    m_ParsedArgs->addUnprocessedArgument(*arg);
                }
                else
                {
                    error("Invalid option: " + std::string(m_Iterator->current()));
                    return {IteratorResultCode::ERROR, nullptr, {}};
                }
            }
        }
        else if (auto argument = m_ArgumentCounter.nextArgument())
        {
            auto s = m_ParsedArgs->appendValue(argument->valueId_, *arg);
            if (argument->callback
                && !argument->callback(ArgumentView(argument), s,
                                       ParsedArgumentsBuilder(m_ParsedArgs.get())))
            {
                error();
                return {IteratorResultCode::ERROR, nullptr, {}};
            }
            return {IteratorResultCode::ARGUMENT, argument, s};
        }
        else if (m_Data->parserSettings.ignoreUndefinedArguments)
        {
            m_ParsedArgs->addUnprocessedArgument(*arg);
        }
        else
        {
            error("Too many arguments, starting with \"" + *arg + "\"");
            return {IteratorResultCode::ERROR, nullptr, {}};
        }
        return {IteratorResultCode::UNKNOWN, nullptr, {}};
    }

    void ArgumentIteratorImpl::copyRemainingArgumentsToParserResult()
    {
        for (auto str : m_Iterator->remainingArguments())
            m_ParsedArgs->addUnprocessedArgument(std::string(str));
    }

    size_t ArgumentIteratorImpl::countArguments() const
    {
        size_t result = 0;
        std::unique_ptr<IOptionIterator> it(m_Iterator->clone());
        bool argumentsOnly = false;
        for (auto arg = it->next(); arg && !argumentsOnly; arg = it->next())
        {
            auto option = findOption(m_Options, *arg,
                                     m_Data->parserSettings.allowAbbreviatedOptions,
                                     m_Data->parserSettings.caseInsensitive);
            if (option)
            {
                if (!option->argument.empty())
                    it->nextValue();
                switch (option->type)
                {
                case OptionType::HELP:
                case OptionType::LAST_ARGUMENT:
                case OptionType::STOP:
                    return result;
                case OptionType::LAST_OPTION:
                    argumentsOnly = true;
                    break;
                default:
                    break;
                }
            }
            else if (!isOption(*arg, m_Data->parserSettings.optionStyle))
            {
                ++result;
            }
        }

        for (auto arg = it->next(); arg; arg = it->next())
            ++result;
        return result;
    }

    bool ArgumentIteratorImpl::checkArgumentCounter()
    {
        if (m_ArgumentCounter.isComplete())
        {
            m_State = State::DONE;
            m_ParsedArgs->setResultCode(ParserResultCode::NORMAL);
            return true;
        }
        else
        {
            auto ns = ArgumentCounter::getMinMaxCount(m_Data->arguments);
            error((ns.first == ns.second
                   ? "Too few arguments. Expected "
                   : "Too few arguments. Expected at least ")
                  + std::to_string(ns.first) + ", received "
                  + std::to_string(m_ArgumentCounter.count()) + ".");
            return false;
        }
    }

    void ArgumentIteratorImpl::error(const std::string& message)
    {
        if (!message.empty())
            HelpWriter().writeErrorMessage(*m_Data, message);
        if (m_Data->parserSettings.autoExit)
            exit(1);
        copyRemainingArgumentsToParserResult();
        m_ParsedArgs->setResultCode(ParserResultCode::ERROR);
        m_State = State::ERROR;
    }
}
