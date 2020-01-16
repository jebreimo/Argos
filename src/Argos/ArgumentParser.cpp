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
//#include "VisitorUtilities.hpp"

namespace Argos
{
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
            //if (it == optionMap.end() && data->allowAbbreviatedOptions)
            //{
            //    it = optionMap.lower_bound(*arg);
            //    //if (it != optionMap.end()
            //    //    && startsWith(it->first, *arg)
            //    //    && next(it) == optionMap.end()
            //    //    || !startsWith(next(it)->first, *arg))
            //}
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
