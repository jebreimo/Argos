//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ArgumentParser.hpp"
#include "ArgumentIterator.hpp"
#include "ArgosException.hpp"
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

        std::optional<std::vector<std::pair<size_t, const Argument*>>>
        makeArgumentCounters(const std::vector<Argument>& arguments)
        {
            std::vector<std::pair<size_t, const Argument*>> result;
            for (auto& arg : arguments)
            {
                if (!result.empty() && result.back().first == SIZE_MAX)
                    return {};
                if (arg.minCount == arg.maxCount)
                    result.emplace_back(arg.minCount, &arg);
                else
                    result.emplace_back(SIZE_MAX, &arg);
            }
            return move(result);
        }
    }

    ArgumentParser::ArgumentParser(int argc, char** argv, std::shared_ptr<ArgumentData> data)
            : m_Data(move(data)),
              m_Arguments(makeArgumentCounters(m_Data->arguments)),
              m_Options(makeOptionIndex(m_Data->options, m_Data->caseInsensitive)),
              m_ArgumentIterator(makeStringViewVector(argc, argv, true))
    {}

    ArgumentParser::ArgumentParser(const std::vector<std::string>& args,
                                   std::shared_ptr<ArgumentData> data)
            : m_Data(move(data)),
              m_Arguments(makeArgumentCounters(m_Data->arguments)),
              m_Options(makeOptionIndex(m_Data->options, m_Data->caseInsensitive)),
              m_ArgumentIterator(makeStringViewVector(args))
    {}

    std::optional<int> ArgumentParser::next()
    {
        auto arg = m_ArgumentIterator.next();
        if (!arg)
            return {};

        auto option = findOption(m_Options, *arg,
                                 m_Data->allowAbbreviatedOptions,
                                 m_Data->caseInsensitive);
        return {};
    }

    //std::tuple<ArgumentOperation, OptionType, int, const std::string*>
    //getArgumentParams(const ArgumentVariant & variant)
    //{
    //    std::tuple<ArgumentOperation, OptionType, int, const std::string*> result;
    //    std::visit(overload {
    //            [&](const Argument* a)
    //            {
    //                result = {a->operation,
    //                          OptionType::NORMAL,
    //                          a->m_InternalId,
    //                          nullptr};
    //            },
    //            [&](const Option* o)
    //            {
    //                result = {o->operation,
    //                          o->optionType,
    //                          o->m_InternalId,
    //                          o->argument.empty() ? &o->value : nullptr};
    //            }
    //    }, variant);
    //    return result;
    //}

    void assign(std::multimap<int, std::string>& map,
                int key, const std::string& value)
    {
        auto it = map.lower_bound(key);
        if (it == map.end() || it->first != key)
        {
            map.emplace(key, value);
            return;
        }

        it->second = value;
        ++it;
        while (it != map.end() && it->first == key)
            map.erase(it++);
    }

    //std::vector<std::pair<std::string_view, const Option*>>::const_iterator findOption(
    //        const std::map<std::string, const Option*>& map,
    //        const std::string& option,
    //        bool )
    //
    //{
    //}

    ParserResult parseArguments(int argc, char** argv,
                                const std::shared_ptr<ArgumentData>& data)
    {
        if (argc == 0)
            ARGOS_THROW("argc is 0");
        if (!data)
            ARGOS_THROW("data is null");
        if (data->programName.empty())
            data->programName = argv[0];

        std::map<std::string, const Option*> optionMap;
        for (auto& option : data->options)
        {
            for (auto& flag : option.flags)
                optionMap.emplace(flag, &option);
        }

        std::vector<std::string_view> args;
        for (int i = 1; i < argc; ++i)
            args.emplace_back(argv[i]);
        ArgumentIterator iterator(args);
        while (true)
        {
            auto arg = iterator.next();
            if (!arg)
                break;

            auto it = optionMap.find(*arg);
            if (it == optionMap.end() && data->allowAbbreviatedOptions)
            {
                it = optionMap.lower_bound(*arg);
                //if (it != optionMap.end() && !startsWith(it->first, *arg))
                //    it = optionM
                //    || (next(it) != optionMap.end()
                //        && startsWith(next(it)->first, *arg)))

            }
            if (it != optionMap.end())
            {
                auto& option = *it->second;
                switch (option.operation)
                {
                case ArgumentOperation::ASSIGN:
                    if (!option.value.empty())
                    {
                        assign(data->values, option.m_InternalId,
                               option.value);
                    }
                    else if (auto value = iterator.nextValue(); value)
                    {
                        assign(data->values, option.m_InternalId, *value);
                    }
                    else
                    {
                        // TODO: Display error message about missing argument.
                    }
                    break;
                case ArgumentOperation::APPEND:
                    if (!option.value.empty())
                    {
                        data->values.emplace(option.m_InternalId,
                                             option.value);
                    }
                    else if (auto value = iterator.nextValue(); value)
                    {
                        data->values.emplace(option.m_InternalId, *value);
                    }
                    else
                    {
                        // TODO: Display error message about missing argument.
                    }
                    break;
                case ArgumentOperation::CLEAR:
                    data->values.erase(option.m_InternalId);
                    break;
                case ArgumentOperation::NONE:
                    break;
                }
                if (iterator.hasRemainder())
                {
                    // TODO: Display error message about incorrect argument.
                }
            }
            else if (arg->size() <= 1 || (*arg)[0] != '-')
            {
                // TODO: Normal argument
            }
            else if (data->ignoreUndefinedArguments)
            {
                // TODO: Copy argument to a list of some kind.
            }
            else
            {
                // TODO: Display error message about undefined option.
            }
        }
        return ParserResult();
    }
}
