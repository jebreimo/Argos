//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ArgumentIteratorImpl.hpp"

#include <algorithm>
#include <cassert>
#include "ArgosThrow.hpp"
#include "HelpText.hpp"
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
            sort(index.begin(), index.end(), [&](auto& a, auto& b)
            {
                return isLess(a.first, b.first, caseInsensitive);
            });
            auto it = adjacent_find(index.begin(), index.end(),
                                    [&](auto& a, auto& b)
            {
                return areEqual(a.first, b.first, caseInsensitive);
            });
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

        const OptionData* findOptionImpl(const OptionTable& options,
                                         std::string_view arg,
                                         bool allowAbbreviations,
                                         bool caseInsensitive)
        {
            auto it = std::lower_bound(
                options.begin(), options.end(),
                OptionTable::value_type(arg, nullptr),
                [&](auto& a, auto& b)
                {return isLess(a.first, b.first, caseInsensitive);});
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
            case OptionStyle::SLASH:
                return std::make_unique<OptionIterator>(move(args), '/');
            case OptionStyle::DASH:
                return std::make_unique<OptionIterator>(move(args), '-');
            default:
                return std::make_unique<StandardOptionIterator>(move(args));
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

    std::shared_ptr<ParsedArgumentsImpl>
    ArgumentIteratorImpl::parse(std::vector<std::string_view> args,
                                const std::shared_ptr<ParserData>& data)
    {
        ArgumentIteratorImpl iterator(move(args), data);
        while (true)
        {
            auto code = std::get<0>(iterator.next());
            if (code == IteratorResultCode::ERROR
                || code == IteratorResultCode::DONE)
            {
                break;
            }
        }
        return iterator.m_ParsedArgs;
    }

    IteratorResult ArgumentIteratorImpl::next()
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
            if (checkArgumentAndOptionCounts())
                return {IteratorResultCode::DONE, nullptr, {}};
            else
                return {IteratorResultCode::ERROR, nullptr, {}};
        }

        if (m_State == State::ARGUMENTS_AND_OPTIONS
            && isOption(*arg, m_Data->parserSettings.optionStyle))
        {
            return processOption(*arg);
        }
        else
        {
            return processArgument(*arg);
        }
    }

    const std::shared_ptr<ParsedArgumentsImpl>& ArgumentIteratorImpl::parsedArguments() const
    {
        return m_ParsedArgs;
    }

    std::pair<ArgumentIteratorImpl::OptionResult, std::string_view>
    ArgumentIteratorImpl::processOption(const OptionData& opt,
                                        const std::string& flag)
    {
        std::string_view arg;
        switch (opt.operation)
        {
        case OptionOperation::ASSIGN:
            if (!opt.constant.empty())
            {
                m_ParsedArgs->assignValue(opt.valueId, opt.constant,
                                          opt.argumentId);
            }
            else if (auto value = m_Iterator->nextValue())
            {
                arg = m_ParsedArgs->assignValue(opt.valueId, *value,
                                                opt.argumentId);
            }
            else
            {
                error(flag + ": no value given.");
                return {OptionResult::ERROR, {}};
            }
            break;
        case OptionOperation::APPEND:
            if (!opt.constant.empty())
            {
                m_ParsedArgs->appendValue(opt.valueId, opt.constant,
                                          opt.argumentId);
            }
            else if (auto value = m_Iterator->nextValue())
            {
                arg = m_ParsedArgs->appendValue(opt.valueId, *value,
                                                opt.argumentId);
            }
            else
            {
                error(flag + ": no value given.");
                return {OptionResult::ERROR, {}};
            }
            break;
        case OptionOperation::CLEAR:
            m_ParsedArgs->clearValue(opt.valueId);
            break;
        case OptionOperation::NONE:
            break;
        }

        if (opt.callback
            && !opt.callback(OptionView(&opt), arg,
                             ParsedArgumentsBuilder(m_ParsedArgs)))
        {
            error();
            return {OptionResult::ERROR, {}};
        }

        if (m_Data->parserSettings.optionCallback
            && !m_Data->parserSettings.optionCallback(
                    OptionView(&opt), arg,
                    ParsedArgumentsBuilder(m_ParsedArgs)))
        {
            error();
            return {OptionResult::ERROR, {}};
        }
        switch (opt.type)
        {
        case OptionType::NORMAL:
            return {OptionResult::NORMAL, arg};
        case OptionType::HELP:
            writeHelpText(*m_Data);
            m_State = State::DONE;
            m_ParsedArgs->setBreakingOption(&opt);
            return {OptionResult::HELP, arg};
        case OptionType::STOP:
            m_State = State::DONE;
            m_ParsedArgs->setBreakingOption(&opt);
            return {OptionResult::STOP, arg};
        case OptionType::LAST_ARGUMENT:
            m_State = State::DONE;
            return {OptionResult::LAST_ARGUMENT, arg};
        case OptionType::LAST_OPTION:
            m_State = State::ARGUMENTS_ONLY;
            return {OptionResult::NORMAL, arg};
        }
        return {};
    }

    IteratorResult
    ArgumentIteratorImpl::processOption(const std::string& flag)
    {
        auto option = findOption(
                m_Options, flag,
                m_Data->parserSettings.allowAbbreviatedOptions,
                m_Data->parserSettings.caseInsensitive);
        if (option)
        {
            auto optRes = processOption(*option, flag);
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
                if (!checkArgumentAndOptionCounts())
                    return {IteratorResultCode::ERROR, nullptr, {}};
                [[fallthrough]];
            case OptionResult::STOP:
                copyRemainingArgumentsToParserResult();
                [[fallthrough]];
            default:
                return {IteratorResultCode::OPTION, option, optRes.second};
            }
        }
        if (!m_Data->parserSettings.ignoreUndefinedOptions
            || !startsWith(m_Iterator->current(), flag))
        {
            error("Unknown option: " + std::string(m_Iterator->current()));
            return {IteratorResultCode::ERROR, nullptr, {}};
        }
        else
        {
            m_ParsedArgs->addUnprocessedArgument(
                std::string(m_Iterator->current()));
            return {IteratorResultCode::UNKNOWN, nullptr, m_Iterator->current()};
        }
    }

    IteratorResult
    ArgumentIteratorImpl::processArgument(const std::string& name)
    {
        if (auto argument = m_ArgumentCounter.nextArgument())
        {
            auto s = m_ParsedArgs->appendValue(argument->valueId, name,
                                               argument->argumentId);
            if (argument->callback
                && !argument->callback(ArgumentView(argument), s,
                                       ParsedArgumentsBuilder(m_ParsedArgs)))
            {
                error();
                return {IteratorResultCode::ERROR, nullptr, {}};
            }
            if (m_Data->parserSettings.argumentCallback
                && !m_Data->parserSettings.argumentCallback(
                        ArgumentView(argument), s,
                        ParsedArgumentsBuilder(m_ParsedArgs)))
            {
                error();
                return {IteratorResultCode::ERROR, nullptr, {}};
            }
            return {IteratorResultCode::ARGUMENT, argument, s};
        }
        else if (m_Data->parserSettings.ignoreUndefinedArguments)
        {
            m_ParsedArgs->addUnprocessedArgument(name);
        }
        else
        {
            error("Too many arguments, starting with \"" + name + "\"");
            return {IteratorResultCode::ERROR, nullptr, {}};
        }
        return {IteratorResultCode::UNKNOWN, nullptr, m_Iterator->current()};
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

    bool ArgumentIteratorImpl::checkArgumentAndOptionCounts()
    {
        for (auto& o : m_Data->options)
        {
            if (!o->optional && !m_ParsedArgs->has(o->valueId))
            {
                auto flags = o->flags.front();
                for (unsigned i = 1; i < o->flags.size(); ++i)
                    flags += ", " + o->flags[i];
                error("Mandatory option \"" + flags + "\" is missing.");
                return false;
            }
        }
        if (m_ArgumentCounter.isComplete())
        {
            m_State = State::DONE;
            m_ParsedArgs->setResultCode(ParserResultCode::SUCCESS);
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
            writeErrorMessage(*m_Data, message);
        if (m_Data->parserSettings.autoExit)
            exit(1);
        copyRemainingArgumentsToParserResult();
        m_ParsedArgs->setResultCode(ParserResultCode::ERROR);
        m_State = State::ERROR;
    }
}
